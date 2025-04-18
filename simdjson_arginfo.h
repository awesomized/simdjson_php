/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 1593e3c33138c464eaf0df92264ac9ee21bf0278 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_validate, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

#define arginfo_simdjson_is_valid arginfo_simdjson_validate

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_decode, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_decode_from_stream, 0, 1, IS_MIXED, 0)
	ZEND_ARG_INFO(0, res)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_key_value, 0, 2, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, associative, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_key_count, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, throw_if_uncountable, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_key_exists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, json, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_cleanup, 0, 0, IS_TRUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_is_valid_utf8, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_encode, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_simdjson_encode_to_stream, 0, 2, IS_TRUE, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_INFO(0, res)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, depth, IS_LONG, 0, "512")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimdJsonBase64Encode___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, base64url, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SimdJsonBase64Encode_jsonSerialize, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SimdJsonBase64Encode___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(simdjson_validate);
ZEND_FUNCTION(simdjson_decode);
ZEND_FUNCTION(simdjson_decode_from_stream);
ZEND_FUNCTION(simdjson_key_value);
ZEND_FUNCTION(simdjson_key_count);
ZEND_FUNCTION(simdjson_key_exists);
ZEND_FUNCTION(simdjson_cleanup);
ZEND_FUNCTION(simdjson_is_valid_utf8);
ZEND_FUNCTION(simdjson_encode);
ZEND_FUNCTION(simdjson_encode_to_stream);
ZEND_METHOD(SimdJsonBase64Encode, __construct);
ZEND_METHOD(SimdJsonBase64Encode, jsonSerialize);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(simdjson_validate, arginfo_simdjson_validate)
	ZEND_FALIAS(simdjson_is_valid, simdjson_validate, arginfo_simdjson_is_valid)
	ZEND_FE(simdjson_decode, arginfo_simdjson_decode)
	ZEND_FE(simdjson_decode_from_stream, arginfo_simdjson_decode_from_stream)
	ZEND_FE(simdjson_key_value, arginfo_simdjson_key_value)
	ZEND_FE(simdjson_key_count, arginfo_simdjson_key_count)
	ZEND_FE(simdjson_key_exists, arginfo_simdjson_key_exists)
	ZEND_FE(simdjson_cleanup, arginfo_simdjson_cleanup)
	ZEND_FE(simdjson_is_valid_utf8, arginfo_simdjson_is_valid_utf8)
	ZEND_FE(simdjson_encode, arginfo_simdjson_encode)
	ZEND_FE(simdjson_encode_to_stream, arginfo_simdjson_encode_to_stream)
	ZEND_FE_END
};


static const zend_function_entry class_SimdJsonException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_SimdJsonDecoderException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_SimdJsonEncoderException_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_SimdJsonBase64Encode_methods[] = {
	ZEND_ME(SimdJsonBase64Encode, __construct, arginfo_class_SimdJsonBase64Encode___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SimdJsonBase64Encode, jsonSerialize, arginfo_class_SimdJsonBase64Encode_jsonSerialize, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SimdJsonBase64Encode, __toString, jsonSerialize, arginfo_class_SimdJsonBase64Encode___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_simdjson_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("SIMDJSON_PRETTY_PRINT", SIMDJSON_PRETTY_PRINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_APPEND_NEWLINE", SIMDJSON_APPEND_NEWLINE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_INVALID_UTF8_IGNORE", SIMDJSON_INVALID_UTF8_IGNORE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_INVALID_UTF8_SUBSTITUTE", SIMDJSON_INVALID_UTF8_SUBSTITUTE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_NONE", SIMDJSON_ERROR_NONE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_DEPTH", SIMDJSON_ERROR_DEPTH, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_UTF8", SIMDJSON_ERROR_UTF8, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_RECURSION", SIMDJSON_ERROR_RECURSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_INF_OR_NAN", SIMDJSON_ERROR_INF_OR_NAN, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_UNSUPPORTED_TYPE", SIMDJSON_ERROR_UNSUPPORTED_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_INVALID_PROPERTY_NAME", SIMDJSON_ERROR_INVALID_PROPERTY_NAME, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERROR_NON_BACKED_ENUM", SIMDJSON_ERROR_NON_BACKED_ENUM, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_CAPACITY", simdjson::CAPACITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_TAPE_ERROR", simdjson::TAPE_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_DEPTH_ERROR", simdjson::DEPTH_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_STRING_ERROR", simdjson::STRING_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_T_ATOM_ERROR", simdjson::T_ATOM_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_F_ATOM_ERROR", simdjson::F_ATOM_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_N_ATOM_ERROR", simdjson::N_ATOM_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_NUMBER_ERROR", simdjson::NUMBER_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_UTF8_ERROR", simdjson::UTF8_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_UNINITIALIZED", simdjson::UNINITIALIZED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_EMPTY", simdjson::EMPTY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_UNESCAPED_CHARS", simdjson::UNESCAPED_CHARS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_UNCLOSED_STRING", simdjson::UNCLOSED_STRING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_UNSUPPORTED_ARCHITECTURE", simdjson::UNSUPPORTED_ARCHITECTURE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INCORRECT_TYPE", simdjson::INCORRECT_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_NUMBER_OUT_OF_RANGE", simdjson::NUMBER_OUT_OF_RANGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INDEX_OUT_OF_BOUNDS", simdjson::INDEX_OUT_OF_BOUNDS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_NO_SUCH_FIELD", simdjson::NO_SUCH_FIELD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_IO_ERROR", simdjson::IO_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INVALID_JSON_POINTER", simdjson::INVALID_JSON_POINTER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INVALID_URI_FRAGMENT", simdjson::INVALID_URI_FRAGMENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_UNEXPECTED_ERROR", simdjson::UNEXPECTED_ERROR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_PARSER_IN_USE", simdjson::PARSER_IN_USE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_OUT_OF_ORDER_ITERATION", simdjson::OUT_OF_ORDER_ITERATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INSUFFICIENT_PADDING", simdjson::INSUFFICIENT_PADDING, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INCOMPLETE_ARRAY_OR_OBJECT", simdjson::INCOMPLETE_ARRAY_OR_OBJECT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_SCALAR_DOCUMENT_AS_VALUE", simdjson::SCALAR_DOCUMENT_AS_VALUE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_OUT_OF_BOUNDS", simdjson::OUT_OF_BOUNDS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_TRAILING_CONTENT", simdjson::TRAILING_CONTENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_KEY_COUNT_NOT_COUNTABLE", SIMDJSON_PHP_ERR_KEY_COUNT_NOT_COUNTABLE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMDJSON_ERR_INVALID_PROPERTY", SIMDJSON_PHP_ERR_INVALID_PHP_PROPERTY, CONST_PERSISTENT);
}

static zend_class_entry *register_class_SimdJsonException(zend_class_entry *class_entry_RuntimeException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SimdJsonException", class_SimdJsonException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_RuntimeException);
	class_entry->ce_flags |= ZEND_ACC_ABSTRACT;

	return class_entry;
}

static zend_class_entry *register_class_SimdJsonDecoderException(zend_class_entry *class_entry_SimdJsonException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SimdJsonDecoderException", class_SimdJsonDecoderException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SimdJsonException);

	return class_entry;
}

static zend_class_entry *register_class_SimdJsonEncoderException(zend_class_entry *class_entry_SimdJsonException)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SimdJsonEncoderException", class_SimdJsonEncoderException_methods);
	class_entry = zend_register_internal_class_ex(&ce, class_entry_SimdJsonException);

	return class_entry;
}

static zend_class_entry *register_class_SimdJsonBase64Encode(zend_class_entry *class_entry_JsonSerializable)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "SimdJsonBase64Encode", class_SimdJsonBase64Encode_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL|ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(class_entry, 1, class_entry_JsonSerializable);

	zval property_string_default_value;
	ZVAL_UNDEF(&property_string_default_value);
	zend_string *property_string_name = zend_string_init("string", sizeof("string") - 1, 1);
	zend_declare_typed_property(class_entry, property_string_name, &property_string_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_STRING));
	zend_string_release(property_string_name);

	zval property_base64url_default_value;
	ZVAL_FALSE(&property_base64url_default_value);
	zend_string *property_base64url_name = zend_string_init("base64url", sizeof("base64url") - 1, 1);
	zend_declare_typed_property(class_entry, property_base64url_name, &property_base64url_default_value, ZEND_ACC_PRIVATE, NULL, (zend_type) ZEND_TYPE_INIT_MASK(MAY_BE_BOOL));
	zend_string_release(property_base64url_name);

	return class_entry;
}
