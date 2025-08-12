#include "../CJson.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_error_handling() {
    cjson_value v;
    cjson_init(&v);
    
    // Invalid value
    assert(cjson_parse(&v, "invalid") == CJSON_INVALID_VALUE);
    cjson_free(&v);
    
    // Root not singular
    cjson_init(&v);
    assert(cjson_parse(&v, "123 456") == CJSON_ROOT_NOT_SINGULAR);
    cjson_free(&v);
    
    // Missing quotation
    cjson_init(&v);
    assert(cjson_parse(&v, "\"hello") == CJSON_INVALID_STRING_MISS_QUOTATION);
    cjson_free(&v);
    
    // Invalid escape
    cjson_init(&v);
    assert(cjson_parse(&v, "\"\\x\"") == CJSON_INVALID_STRING_ESCAPE);
    cjson_free(&v);
    
    // Missing key in object
    cjson_init(&v);
    assert(cjson_parse(&v, "{123: \"value\"}") == CJSON_MISS_KEY);
    cjson_free(&v);
    
    // Missing colon in object
    cjson_init(&v);
    assert(cjson_parse(&v, "{\"key\" \"value\"}") == CJSON_MISS_COLON);
    cjson_free(&v);
    
    printf("✓ test_error_handling passed\n");
}

void test_escape_sequences() {
    cjson_value v;
    cjson_init(&v);
    
    assert(cjson_parse(&v, "\"\\\"\"") == CJSON_PARSE_OK);
    assert(strcmp(cjson_get_string(&v), "\"") == 0);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "\"\\\\\"") == CJSON_PARSE_OK);
    assert(strcmp(cjson_get_string(&v), "\\") == 0);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "\"\\/\"") == CJSON_PARSE_OK);
    assert(strcmp(cjson_get_string(&v), "/") == 0);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "\"\\b\\f\\n\\r\\t\"") == CJSON_PARSE_OK);
    assert(strcmp(cjson_get_string(&v), "\b\f\n\r\t") == 0);
    cjson_free(&v);
    
    printf("✓ test_escape_sequences passed\n");
}

void test_unicode() {
    cjson_value v;
    cjson_init(&v);
    
    // Basic Unicode escape
    assert(cjson_parse(&v, "\"\\u0041\"") == CJSON_PARSE_OK);
    assert(strcmp(cjson_get_string(&v), "A") == 0);
    cjson_free(&v);
    
    // Null character
    cjson_init(&v);
    assert(cjson_parse(&v, "\"\\u0000\"") == CJSON_PARSE_OK);
    assert(cjson_get_string_length(&v) == 1);
    assert(cjson_get_string(&v)[0] == '\0');
    cjson_free(&v);
    
    printf("✓ test_unicode passed\n");
}

void test_nested_structures() {
    cjson_value v;
    cjson_init(&v);
    
    // Nested arrays
    assert(cjson_parse(&v, "[[1, 2], [3, 4]]") == CJSON_PARSE_OK);
    assert(v.type == CJSON_ARRAY);
    assert(cjson_get_array_size(&v) == 2);
    cjson_value *nested = cjson_get_array_element(&v, 0);
    assert(nested->type == CJSON_ARRAY);
    assert(cjson_get_array_size(nested) == 2);
    cjson_free(&v);
    
    // Nested objects
    cjson_init(&v);
    assert(cjson_parse(&v, "{\"person\": {\"name\": \"John\", \"age\": 30}}") == CJSON_PARSE_OK);
    assert(v.type == CJSON_OBJECT);
    cjson_free(&v);
    
    printf("✓ test_nested_structures passed\n");
}

void test_whitespace() {
    cjson_value v;
    cjson_init(&v);
    
    // Whitespace around values
    assert(cjson_parse(&v, "  123  ") == CJSON_PARSE_OK);
    assert(v.type == CJSON_NUMBER);
    assert(cjson_get_number(&v) == 123.0);
    cjson_free(&v);
    
    // Whitespace in arrays
    cjson_init(&v);
    assert(cjson_parse(&v, "[ 1 , 2 , 3 ]") == CJSON_PARSE_OK);
    assert(v.type == CJSON_ARRAY);
    assert(cjson_get_array_size(&v) == 3);
    cjson_free(&v);
    
    // Whitespace in objects
    cjson_init(&v);
    assert(cjson_parse(&v, "{ \"key\" : \"value\" }") == CJSON_PARSE_OK);
    assert(v.type == CJSON_OBJECT);
    cjson_free(&v);
    
    printf("✓ test_whitespace passed\n");
}

int main() {
    printf("Running edge case tests...\n\n");
    
    test_error_handling();
    test_escape_sequences();
    test_unicode();
    test_nested_structures();
    test_whitespace();
    
    printf("\n✅ All edge case tests passed!\n");
    return 0;
}