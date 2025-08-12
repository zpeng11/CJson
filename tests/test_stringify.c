#include "../CJson.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void test_stringify_basic() {
    cjson_value v;
    char *json_str;
    size_t length;
    
    // Test null
    cjson_init(&v);
    json_str = cjson_stringify(&v, &length);
    assert(strcmp(json_str, "null") == 0);
    assert(length == 4);
    free(json_str);
    cjson_free(&v);
    
    // Test boolean true
    cjson_init(&v);
    cjson_set_boolean(&v, 1);
    json_str = cjson_stringify(&v, &length);
    assert(strcmp(json_str, "true") == 0);
    assert(length == 4);
    free(json_str);
    cjson_free(&v);
    
    // Test boolean false
    cjson_init(&v);
    cjson_set_boolean(&v, 0);
    json_str = cjson_stringify(&v, &length);
    assert(strcmp(json_str, "false") == 0);
    assert(length == 5);
    free(json_str);
    cjson_free(&v);
    
    // Test number
    cjson_init(&v);
    cjson_set_number(&v, 123.456);
    json_str = cjson_stringify(&v, NULL);
    assert(strstr(json_str, "123.456") != NULL);
    free(json_str);
    cjson_free(&v);
    
    printf("✓ test_stringify_basic passed\n");
}

void test_stringify_string() {
    cjson_value v;
    char *json_str;
    
    // Test simple string
    cjson_init(&v);
    cjson_set_string(&v, "hello", 5);
    json_str = cjson_stringify(&v, NULL);
    assert(strcmp(json_str, "\"hello\"") == 0);
    free(json_str);
    cjson_free(&v);
    
    // Test string with escapes
    cjson_init(&v);
    cjson_set_string(&v, "hello\nworld", 11);
    json_str = cjson_stringify(&v, NULL);
    assert(strcmp(json_str, "\"hello\\nworld\"") == 0);
    free(json_str);
    cjson_free(&v);
    
    // Test string with quotes
    cjson_init(&v);
    cjson_set_string(&v, "say \"hello\"", 11);
    json_str = cjson_stringify(&v, NULL);
    assert(strcmp(json_str, "\"say \\\"hello\\\"\"") == 0);
    free(json_str);
    cjson_free(&v);
    
    printf("✓ test_stringify_string passed\n");
}

void test_round_trip() {
    cjson_value original, parsed;
    char *json_str;
    
    // Test round-trip with number
    cjson_init(&original);
    cjson_set_number(&original, 42.5);
    json_str = cjson_stringify(&original, NULL);
    
    cjson_init(&parsed);
    assert(cjson_parse(&parsed, json_str) == CJSON_PARSE_OK);
    assert(parsed.type == CJSON_NUMBER);
    assert(cjson_get_number(&parsed) == cjson_get_number(&original));
    
    free(json_str);
    cjson_free(&original);
    cjson_free(&parsed);
    
    // Test round-trip with string
    cjson_init(&original);
    cjson_set_string(&original, "test string", 11);
    json_str = cjson_stringify(&original, NULL);
    
    cjson_init(&parsed);
    assert(cjson_parse(&parsed, json_str) == CJSON_PARSE_OK);
    assert(parsed.type == CJSON_STRING);
    assert(strcmp(cjson_get_string(&parsed), cjson_get_string(&original)) == 0);
    
    free(json_str);
    cjson_free(&original);
    cjson_free(&parsed);
    
    printf("✓ test_round_trip passed\n");
}

int main() {
    printf("Running stringify tests...\n\n");
    
    test_stringify_basic();
    test_stringify_string();
    test_round_trip();
    
    printf("\n✅ All stringify tests passed!\n");
    return 0;
}