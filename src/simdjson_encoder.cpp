/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Omar Kilani <omar@php.net>                                   |
  |         Jakub Zelenka <bukka@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "zend_smart_str.h"
#include "zend_portability.h"
#include <zend_exceptions.h>
#include "ext/json/php_json.h" /* For php_json_serializable_ce */
#if PHP_VERSION_ID >= 80400
#include "zend_property_hooks.h"
#include "zend_lazy_objects.h"
#endif

#include "simdjson_encoder.h"
#include "countlut.h"
#include "simdjson_vector8.h"
#include "simdjson.h"
#include "simdjson_compatibility.h"

static zend_always_inline bool simdjson_check_stack_limit(void)
{
#ifdef ZEND_CHECK_STACK_LIMIT
	return zend_call_stack_overflowed(EG(stack_limit));
#else
	return false;
#endif
}

static inline void simdjson_pretty_print_colon(smart_str *buf, int options)
{
	ZEND_ASSERT(buf->s);

	if (options & SIMDJSON_PRETTY_PRINT) {
		smart_str_appendl(buf, ": ", 2);
	} else {
		smart_str_appendc(buf, ':');
    }
}

static inline void simdjson_pretty_print_nl_ident(smart_str *buf, int options, simdjson_encoder *encoder)
{
  	char *next;
	const char *whitespace = "\n                                ";

	ZEND_ASSERT(buf->s);

	if (options & SIMDJSON_PRETTY_PRINT) {
		next = smart_str_extend(buf, 4 * encoder->depth + 1);
        if (EXPECTED(encoder->depth < 8)) {
        	memcpy(next, whitespace, encoder->depth * 4 + 1);
        } else {
        	next[0] = '\n';
        	next++;
			for (size_t i = 0; i < encoder->depth; ++i) {
	            memcpy(next, "    ", 4);
	            next += 4;
			}
        }
	}
}

static inline void simdjson_append_double(smart_str *buf, double d)
{
	size_t len;
	char num[PHP_DOUBLE_MAX_LENGTH];

	ZEND_ASSERT(buf->s);
	php_gcvt(d, (int)PG(serialize_precision), '.', 'e', num);
	len = strlen(num);
	smart_str_appendl(buf, num, len);
}

static inline void simdjson_append_long(smart_str *buf, zend_long number)
{
	ZEND_ASSERT(buf->s);

	smart_str_alloc(buf, sizeof("-9223372036854775807") - 1, 0);
	unsigned chars = simdjson_i64toa_countlut(number, ZSTR_VAL(buf->s) + ZSTR_LEN(buf->s));
    ZSTR_LEN(buf->s) += chars;
}

static zend_always_inline void simdjson_smart_str_appendc_unsafe(smart_str *dest, char ch) {
	ZSTR_VAL(dest->s)[ZSTR_LEN(dest->s)++] = ch;
}

static zend_always_inline void simdjson_smart_str_appendl_unsafe(smart_str *dest, const char* str, size_t len) {
	memcpy(ZSTR_VAL(dest->s) + ZSTR_LEN(dest->s), str, len);
	ZSTR_LEN(dest->s) += len;
}

#define SIMDJSON_HASH_PROTECT_RECURSION(_tmp_ht) \
	do { \
		if (EXPECTED(_tmp_ht)) { \
			GC_TRY_PROTECT_RECURSION(_tmp_ht); \
		} \
	} while (0)

#define SIMDJSON_HASH_UNPROTECT_RECURSION(_tmp_ht) \
	do { \
		if (EXPECTED(_tmp_ht)) { \
			GC_TRY_UNPROTECT_RECURSION(_tmp_ht); \
		} \
	} while (0)

// Specific implementation for faster encoding packed arrays
static zend_result simdjson_encode_packed_array(smart_str *buf, HashTable *table, int options, simdjson_encoder *encoder)
{
  	zval* data;
	zend_refcounted *recursion_rc = (zend_refcounted *)table;

    ZEND_ASSERT(recursion_rc != NULL);
	ZEND_ASSERT(buf->s);

	if (GC_IS_RECURSIVE(recursion_rc)) {
		encoder->error_code = SIMDJSON_ERROR_RECURSION;
		return FAILURE;
	}

	SIMDJSON_HASH_PROTECT_RECURSION(recursion_rc);

	smart_str_appendc(buf, '[');
	++encoder->depth;

	ZEND_HASH_PACKED_FOREACH_VAL(table, data) {
		simdjson_pretty_print_nl_ident(buf, options, encoder);
		if (UNEXPECTED(simdjson_encode_zval(buf, data, options, encoder) == FAILURE)) {
			SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);
        	return FAILURE;
        }
		smart_str_appendc(buf, ',');
    } ZEND_HASH_FOREACH_END();

    ZSTR_LEN(buf->s)--; // remove last comma

	SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);

	if (UNEXPECTED(encoder->depth > encoder->max_depth)) {
		encoder->error_code = SIMDJSON_ERROR_DEPTH;
		return FAILURE;
	}
	--encoder->depth;

	simdjson_pretty_print_nl_ident(buf, options, encoder);
	smart_str_appendc(buf, ']');

    return SUCCESS;
}

static zend_result simdjson_encode_mixed_array(smart_str *buf, HashTable *table, int options, simdjson_encoder *encoder)
{
	int need_comma = 0;
	zend_string *key;
	zval *data;
	zend_ulong index;
	zend_refcounted *recursion_rc = (zend_refcounted *)table;

    ZEND_ASSERT(recursion_rc != NULL);
	ZEND_ASSERT(buf->s);

	if (GC_IS_RECURSIVE(recursion_rc)) {
		encoder->error_code = SIMDJSON_ERROR_RECURSION;
		return FAILURE;
	}

	SIMDJSON_HASH_PROTECT_RECURSION(recursion_rc);

	smart_str_appendc(buf, '{');
	++encoder->depth;

	ZEND_HASH_FOREACH_KEY_VAL_IND(table, index, key, data) {
		if (need_comma) {
			smart_str_appendc(buf, ',');
		} else {
			need_comma = 1;
		}

		simdjson_pretty_print_nl_ident(buf, options, encoder);

		if (EXPECTED(key)) {
			if (UNEXPECTED(simdjson_escape_string(buf, key, encoder) == FAILURE)) {
				SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);
				return FAILURE;
			}
		} else {
			smart_str_appendc(buf, '"');
			simdjson_append_long(buf, (zend_long) index);
			smart_str_appendc(buf, '"');
		}

		simdjson_pretty_print_colon(buf, options);

		if (UNEXPECTED(simdjson_encode_zval(buf, data, options, encoder) == FAILURE)) {
			SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);

	if (UNEXPECTED(encoder->depth > encoder->max_depth)) {
		encoder->error_code = SIMDJSON_ERROR_DEPTH;
		return FAILURE;
	}
	--encoder->depth;

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		simdjson_pretty_print_nl_ident(buf, options, encoder);
	}
	smart_str_appendc(buf, '}');

	return SUCCESS;
}

static zend_result simdjson_encode_simple_object(smart_str *buf, zval *val, int options, simdjson_encoder *encoder)
{
	int need_comma = 0;

	/* Optimized version without rebuilding properties HashTable */
	zend_object *obj = Z_OBJ_P(val);
	zend_class_entry *ce = obj->ce;
	zend_property_info *prop_info;
	zval *prop;

	if (GC_IS_RECURSIVE(obj)) {
		encoder->error_code = SIMDJSON_ERROR_RECURSION;
		return FAILURE;
	}

	SIMDJSON_HASH_PROTECT_RECURSION(obj);

	smart_str_appendc(buf, '{');
	++encoder->depth;

	for (int i = 0; i < ce->default_properties_count; i++) {
		prop_info = ce->properties_info_table[i];
		if (!prop_info) {
			continue;
		}
		if (ZSTR_VAL(prop_info->name)[0] == '\0' && ZSTR_LEN(prop_info->name) > 0) {
			/* Skip protected and private members. */
			continue;
		}
		prop = OBJ_PROP(obj, prop_info->offset);
		if (Z_TYPE_P(prop) == IS_UNDEF) {
			continue;
		}

		if (need_comma) {
			smart_str_appendc(buf, ',');
		} else {
			need_comma = 1;
		}

		simdjson_pretty_print_nl_ident(buf, options, encoder);

		if (simdjson_escape_string(buf, prop_info->name, encoder) == FAILURE) {
			SIMDJSON_HASH_UNPROTECT_RECURSION(obj);
			return FAILURE;
		}

		simdjson_pretty_print_colon(buf, options);

		if (simdjson_encode_zval(buf, prop, options, encoder) == FAILURE) {
			SIMDJSON_HASH_UNPROTECT_RECURSION(obj);
			return FAILURE;
		}
	}

	SIMDJSON_HASH_UNPROTECT_RECURSION(obj);
	if (encoder->depth > encoder->max_depth) {
		encoder->error_code = SIMDJSON_ERROR_DEPTH;
		return FAILURE;
	}
	--encoder->depth;

	if (need_comma) {
		simdjson_pretty_print_nl_ident(buf, options, encoder);
	}
	smart_str_appendc(buf, '}');
	return SUCCESS;
}

static zend_always_inline bool simdjson_is_simple_object(zval *val)
{
	return Z_OBJ_P(val)->properties == NULL
		&& Z_OBJ_HT_P(val)->get_properties_for == NULL
		&& Z_OBJ_HT_P(val)->get_properties == zend_std_get_properties
#if PHP_VERSION_ID >= 80400
		&& Z_OBJ_P(val)->ce->num_hooked_props == 0
 		&& !zend_object_is_lazy(Z_OBJ_P(val))
#endif
        ;
}

static zend_result simdjson_encode_array(smart_str *buf, zval *val, int options, simdjson_encoder *encoder)
{
	int need_comma = 0;
	HashTable *myht;
	zend_refcounted *recursion_rc;

	if (simdjson_check_stack_limit()) {
		encoder->error_code = SIMDJSON_ERROR_DEPTH;
		return FAILURE;
	}

	if (Z_TYPE_P(val) == IS_ARRAY) {
		myht = Z_ARRVAL_P(val);
        // Array is empty
		if (zend_hash_num_elements(myht) == 0) {
			smart_str_appendl(buf, "[]", 2);
			return SUCCESS;
		}
        if (zend_array_is_list(myht)) {
        	return simdjson_encode_packed_array(buf, myht, options, encoder);
        } else {
        	return simdjson_encode_mixed_array(buf, myht, options, encoder);
        }
	}

    if (simdjson_is_simple_object(val)) {
		return simdjson_encode_simple_object(buf, val, options, encoder);
	}

	zend_object *obj = Z_OBJ_P(val);
	myht = zend_get_properties_for(val, ZEND_PROP_PURPOSE_JSON);
#if PHP_VERSION_ID >= 80400
	if (obj->ce->num_hooked_props == 0) {
		recursion_rc = (zend_refcounted *)myht;
	} else {
		/* Protecting the object itself is fine here because myht is temporary and can't be
		 * referenced from a different place in the object graph. */
		recursion_rc = (zend_refcounted *)obj;
	}
#else
    recursion_rc = (zend_refcounted *)myht;
#endif

	if (recursion_rc && GC_IS_RECURSIVE(recursion_rc)) {
		encoder->error_code = SIMDJSON_ERROR_RECURSION;
		zend_release_properties(myht);
		return FAILURE;
	}

	SIMDJSON_HASH_PROTECT_RECURSION(recursion_rc);

	smart_str_appendc(buf, '{');

	++encoder->depth;

	uint32_t i = zend_hash_num_elements(myht);

	if (i > 0) {
		zend_string *key;
		zval *data;
		zend_ulong index;

		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, data) {
			zval tmp;
			ZVAL_UNDEF(&tmp);

			if (key) {
				if (ZSTR_VAL(key)[0] == '\0' && ZSTR_LEN(key) > 0 && Z_TYPE_P(val) == IS_OBJECT) {
					/* Skip protected and private members. */
					continue;
				}

#if PHP_VERSION_ID >= 80400
				/* data is IS_PTR for properties with hooks. */
				if (UNEXPECTED(Z_TYPE_P(data) == IS_PTR)) {
					zend_property_info *prop_info = (zend_property_info*)Z_PTR_P(data);
					if ((prop_info->flags & ZEND_ACC_VIRTUAL) && !prop_info->hooks[ZEND_PROPERTY_HOOK_GET]) {
						continue;
					}
					zend_read_property_ex(prop_info->ce, Z_OBJ_P(val), prop_info->name, /* silent */ true, &tmp);
					if (EG(exception)) {
						SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);
						zend_release_properties(myht);
						return FAILURE;
					}
					data = &tmp;
				}
#endif

				if (need_comma) {
					smart_str_appendc(buf, ',');
				} else {
					need_comma = 1;
				}

				simdjson_pretty_print_nl_ident(buf, options, encoder);

				if (simdjson_escape_string(buf, key, encoder) == FAILURE) {
					SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);
					zend_release_properties(myht);
					return FAILURE;
				}
			} else {
				if (need_comma) {
					smart_str_appendc(buf, ',');
				} else {
					need_comma = 1;
				}

				simdjson_pretty_print_nl_ident(buf, options, encoder);

				smart_str_appendc(buf, '"');
				simdjson_append_long(buf, (zend_long) index);
				smart_str_appendc(buf, '"');
			}

			simdjson_pretty_print_colon(buf, options);

			if (simdjson_encode_zval(buf, data, options, encoder) == FAILURE) {
				SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);
				zend_release_properties(myht);
				zval_ptr_dtor(&tmp);
				return FAILURE;
			}
			zval_ptr_dtor(&tmp);
		} ZEND_HASH_FOREACH_END();
	}

	SIMDJSON_HASH_UNPROTECT_RECURSION(recursion_rc);

	if (encoder->depth > encoder->max_depth) {
		encoder->error_code = SIMDJSON_ERROR_DEPTH;
		zend_release_properties(myht);
		return FAILURE;
	}
	--encoder->depth;

	/* Only keep closing bracket on same line for empty arrays/objects */
	if (need_comma) {
		simdjson_pretty_print_nl_ident(buf, options, encoder);
	}

	smart_str_appendc(buf, '}');

	zend_release_properties(myht);
	return SUCCESS;
}

static zend_always_inline void simdjson_append_escape_char_unsafe(smart_str *buf, char c) {
	char *dst;

	switch (c) {
		case '"':
			simdjson_smart_str_appendl_unsafe(buf, "\\\"", 2);
			break;

		case '\\':
			simdjson_smart_str_appendl_unsafe(buf, "\\\\", 2);
			break;

		case '\b':
			simdjson_smart_str_appendl_unsafe(buf, "\\b", 2);
			break;

		case '\f':
			simdjson_smart_str_appendl_unsafe(buf, "\\f", 2);
			break;

		case '\n':
			simdjson_smart_str_appendl_unsafe(buf, "\\n", 2);
			break;

		case '\r':
			simdjson_smart_str_appendl_unsafe(buf, "\\r", 2);
			break;

		case '\t':
			simdjson_smart_str_appendl_unsafe(buf, "\\t", 2);
			break;

		default:
			simdjson_smart_str_appendl_unsafe(buf, control_chars[c], 6);
			break;
	}
}

#if defined(__SSE2__) || defined(__aarch64__) || defined(_M_ARM64)
static zend_always_inline void simdjson_escape_long_string(smart_str *buf, const char *s, size_t len) {
	size_t copypos = 0;
	size_t i = 0;
	simdjson_vector8 chunk;

    // vlen = len - (len % sizeof(simdjson_vector8))
	size_t vlen = len & (int) (~(sizeof(simdjson_vector8) - 1));

	ZEND_ASSERT(buf->s);

	smart_str_alloc(buf, len + 2, 0);
	simdjson_smart_str_appendc_unsafe(buf, '"');

	for (;;) {
        // Check chunk if contains char that needs to be escaped
		for (; i < vlen; i += sizeof(simdjson_vector8)) {
			simdjson_vector8_load(&chunk, (const uint8_t *) &s[i]);
			if (UNEXPECTED(simdjson_vector8_has_le(chunk, (unsigned char) 0x1F) ||
				simdjson_vector8_has(chunk, (unsigned char) '"') ||
				simdjson_vector8_has(chunk, (unsigned char) '\\'))) {
				break;
            }
		}

		if (copypos < i) {
			smart_str_appendl(buf, &s[copypos], i - copypos);
			copypos = i;
		}

        // Ensure that buf contains enoug space that we can call unsafe methods
		smart_str_alloc(buf, sizeof(simdjson_vector8) * 6 + 1, 0);

		for (int b = 0; b < sizeof(simdjson_vector8); b++) {
			if (UNEXPECTED(i == len)) {
				simdjson_smart_str_appendc_unsafe(buf, '"');
                return;
			}
			char c = s[i++];
			if (EXPECTED(need_escaping[c] == 0)) {
				simdjson_smart_str_appendc_unsafe(buf, c);
			} else {
				simdjson_append_escape_char_unsafe(buf, c);
			}
		}
		copypos = i;
	}

	simdjson_smart_str_appendc_unsafe(buf, '"');
}
#endif

zend_result simdjson_escape_string(smart_str *buf, zend_string *str, simdjson_encoder *encoder)
{
	size_t pos, len = ZSTR_LEN(str);
    const char *s = ZSTR_VAL(str);

	ZEND_ASSERT(buf->s);

	if (len == 0) {
		smart_str_appendl(buf, "\"\"", 2);
		return SUCCESS;
	}

	// Check if string is valid UTF-8 string
	if (UNEXPECTED(!ZSTR_IS_VALID_UTF8(str) && !simdjson::validate_utf8(s, len))) {
		encoder->error_code = SIMDJSON_ERROR_UTF8;
		return FAILURE;
    }

    // Mark string as valid UTF-8
	GC_ADD_FLAGS(str, IS_STR_VALID_UTF8);

#if defined(__SSE2__) || defined(__aarch64__) || defined(_M_ARM64)
    if (len >= sizeof(simdjson_vector8)) {
    	simdjson_escape_long_string(buf, s, len);
        return SUCCESS;
    }
#endif

    // For short strings allocate maximum possible string length, so we can use
    // unsafe methods for string copying
    smart_str_alloc(buf, len * 6 + 2, 0);
    simdjson_smart_str_appendc_unsafe(buf, '"');

    size_t start = 0;
    for (pos = 0; pos < len; ++pos) {
    	char c = s[pos];
    	if (EXPECTED(need_escaping[c] == 0)) {
    		continue;
    	}

    	simdjson_smart_str_appendl_unsafe(buf, s + start, pos - start);
    	simdjson_append_escape_char_unsafe(buf, c);

    	start = pos + 1;
    }

    if (start != len) {
    	simdjson_smart_str_appendl_unsafe(buf, s + start, len - start);
    }

    simdjson_smart_str_appendc_unsafe(buf, '"');

    return SUCCESS;
}

static zend_result simdjson_encode_serializable_object(smart_str *buf, zval *val, int options, simdjson_encoder *encoder) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	zval retval, fname;
	zend_result return_code;

#if PHP_VERSION_ID >= 80300
	zend_object *obj = Z_OBJ_P(val);
	uint32_t *guard = zend_get_recursion_guard(obj);
	ZEND_ASSERT(guard != NULL);

	if (ZEND_GUARD_IS_RECURSIVE(guard, JSON)) {
		encoder->error_code = SIMDJSON_ERROR_RECURSION;
		return FAILURE;
	}

	ZEND_GUARD_PROTECT_RECURSION(guard, JSON);
#else
	HashTable* myht = Z_OBJPROP_P(val);

	if (myht && GC_IS_RECURSIVE(myht)) {
		encoder->error_code = SIMDJSON_ERROR_RECURSION;
		return FAILURE;
	}

	SIMDJSON_HASH_PROTECT_RECURSION(myht);
#endif

	ZVAL_STRING(&fname, "jsonSerialize");

	if (FAILURE == call_user_function(NULL, val, &fname, &retval, 0, NULL) || Z_TYPE(retval) == IS_UNDEF) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0, "Failed calling %s::jsonSerialize()", ZSTR_VAL(ce->name));
		}
		zval_ptr_dtor(&fname);
#if PHP_VERSION_ID >= 80300
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
#else
		SIMDJSON_HASH_UNPROTECT_RECURSION(myht);
#endif
		return FAILURE;
	}

	if (EG(exception)) {
		/* Error already raised */
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&fname);
#if PHP_VERSION_ID >= 80300
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
#else
		SIMDJSON_HASH_UNPROTECT_RECURSION(myht);
#endif
		return FAILURE;
	}

	if ((Z_TYPE(retval) == IS_OBJECT) &&
		(Z_OBJ(retval) == Z_OBJ_P(val))) {
		/* Handle the case where jsonSerialize does: return $this; by going straight to encode array */
#if PHP_VERSION_ID >= 80300
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
#else
		SIMDJSON_HASH_UNPROTECT_RECURSION(myht);
#endif
		return_code = simdjson_encode_array(buf, &retval, options, encoder);
	} else {
		/* All other types, encode as normal */
		return_code = simdjson_encode_zval(buf, &retval, options, encoder);
#if PHP_VERSION_ID >= 80300
		ZEND_GUARD_UNPROTECT_RECURSION(guard, JSON);
#else
		SIMDJSON_HASH_UNPROTECT_RECURSION(myht);
#endif
	}

	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&fname);

	return return_code;
}

#if PHP_VERSION_ID >= 80100
// Copy of zend_enum_fetch_case_value
static zend_always_inline zval *simdjson_zend_enum_fetch_case_value(zend_object *zobj)
{
	ZEND_ASSERT(zobj->ce->ce_flags & ZEND_ACC_ENUM);
	ZEND_ASSERT(zobj->ce->enum_backing_type != IS_UNDEF);
	return OBJ_PROP_NUM(zobj, 1);
}

static zend_result simdjson_encode_serializable_enum(smart_str *buf, zval *val, int options, simdjson_encoder *encoder)
{
	zend_class_entry *ce = Z_OBJCE_P(val);
	if (ce->enum_backing_type == IS_UNDEF) {
		encoder->error_code = SIMDJSON_ERROR_NON_BACKED_ENUM;
		return FAILURE;
	}

	zval *value_zv = simdjson_zend_enum_fetch_case_value(Z_OBJ_P(val));
	return simdjson_encode_zval(buf, value_zv, options, encoder);
}
#endif

zend_result simdjson_encode_zval(smart_str *buf, zval *val, int options, simdjson_encoder *encoder)
{
	ZEND_ASSERT(buf->s);
again:
	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			smart_str_appendl(buf, "null", 4);
			break;

		case IS_TRUE:
			smart_str_appendl(buf, "true", 4);
			break;
		case IS_FALSE:
			smart_str_appendl(buf, "false", 5);
			break;

		case IS_LONG:
			simdjson_append_long(buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			if (EXPECTED(!zend_isinf(Z_DVAL_P(val)) && !zend_isnan(Z_DVAL_P(val)))) {
				simdjson_append_double(buf, Z_DVAL_P(val));
			} else {
				encoder->error_code = SIMDJSON_ERROR_INF_OR_NAN;
                return FAILURE;
			}
			break;

		case IS_STRING:
			return simdjson_escape_string(buf, Z_STR_P(val), encoder);

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_json_serializable_ce)) {
				return simdjson_encode_serializable_object(buf, val, options, encoder);
			}
#if PHP_VERSION_ID >= 80100
			if (Z_OBJCE_P(val)->ce_flags & ZEND_ACC_ENUM) {
				return simdjson_encode_serializable_enum(buf, val, options, encoder);
			}
#endif
			/* fallthrough -- Non-serializable object */
			ZEND_FALLTHROUGH;
		case IS_ARRAY: {
			/* Avoid modifications (and potential freeing) of the array through a reference when a
			 * jsonSerialize() method is invoked. */
			zval zv;
			zend_result res;
			ZVAL_COPY(&zv, val);
			res = simdjson_encode_array(buf, &zv, options, encoder);
			zval_ptr_dtor_nogc(&zv);
			return res;
		}

		case IS_REFERENCE:
			val = Z_REFVAL_P(val);
			goto again;

		default:
			encoder->error_code = SIMDJSON_ERROR_UNSUPPORTED_TYPE;
			return FAILURE;
	}

    // For simdjson_encode_to_stream method, write data to stream if buffer is larger than 4096 bytes
    if (UNEXPECTED(encoder->stream != NULL && ZSTR_LEN(buf->s) >= 4 * 1024)) {
    	if (simdjson_encode_write_stream(buf, encoder) == FAILURE) {
          	return FAILURE;
    	}
    }

	return SUCCESS;
}

zend_result simdjson_encode_write_stream(smart_str *buf, simdjson_encoder* encoder) {
	ssize_t numbytes = php_stream_write(encoder->stream, ZSTR_VAL(buf->s), ZSTR_LEN(buf->s));
	if (UNEXPECTED(numbytes < 0)) {
		encoder->error_code = SIMDJSON_ERROR_STREAM_WRITE;
		return FAILURE;
	}
	if (UNEXPECTED(numbytes != ZSTR_LEN(buf->s))) {
		php_error_docref(NULL, E_WARNING, "Only %zd of %zd bytes written, possibly out of free disk space", numbytes, ZSTR_LEN(buf->s));
		encoder->error_code = SIMDJSON_ERROR_STREAM_WRITE;
		return FAILURE;
    }
	ZSTR_LEN(buf->s) = 0; // clenaup buffer
    return SUCCESS;
}