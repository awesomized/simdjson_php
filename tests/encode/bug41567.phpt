--TEST--
Bug #41567 (json_encode() double conversion is inconsistent with PHP)
--INI--
serialize_precision=-1
--FILE--
<?php

$a = simdjson_encode(123456789.12345);
var_dump(simdjson_decode($a));

echo "Done\n";
?>
--EXPECT--
float(123456789.12345)
Done