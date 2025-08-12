#include "../CJson.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_null() {
    cjson_value v;
    cjson_init(&v);
    assert(cjson_parse(&v, "null") == CJSON_PARSE_OK);
    assert(v.type == CJSON_NULL);
    cjson_free(&v);
    printf("✓ test_null passed\n");
}

void test_boolean() {
    cjson_value v;
    cjson_init(&v);
    
    assert(cjson_parse(&v, "true") == CJSON_PARSE_OK);
    assert(v.type == CJSON_TRUE);
    assert(cjson_get_boolean(&v) == 1);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "false") == CJSON_PARSE_OK);
    assert(v.type == CJSON_FALSE);
    assert(cjson_get_boolean(&v) == 0);
    cjson_free(&v);
    
    printf("✓ test_boolean passed\n");
}

void test_number() {
    cjson_value v;
    cjson_init(&v);
    
    assert(cjson_parse(&v, "123") == CJSON_PARSE_OK);
    assert(v.type == CJSON_NUMBER);
    assert(cjson_get_number(&v) == 123.0);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "-456.789") == CJSON_PARSE_OK);
    assert(v.type == CJSON_NUMBER);
    assert(cjson_get_number(&v) == -456.789);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "1.23e10") == CJSON_PARSE_OK);
    assert(v.type == CJSON_NUMBER);
    assert(cjson_get_number(&v) == 1.23e10);
    cjson_free(&v);
    
    printf("✓ test_number passed\n");
}

void test_string() {
    cjson_value v;
    cjson_init(&v);
    
    assert(cjson_parse(&v, "\"hello\"") == CJSON_PARSE_OK);
    assert(v.type == CJSON_STRING);
    assert(strcmp(cjson_get_string(&v), "hello") == 0);
    assert(cjson_get_string_length(&v) == 5);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "\"\"") == CJSON_PARSE_OK);
    assert(v.type == CJSON_STRING);
    assert(cjson_get_string_length(&v) == 0);
    cjson_free(&v);
    
    printf("✓ test_string passed\n");
}

void test_array() {
    cjson_value v;
    cjson_init(&v);
    
    assert(cjson_parse(&v, "[]") == CJSON_PARSE_OK);
    assert(v.type == CJSON_ARRAY);
    assert(cjson_get_array_size(&v) == 0);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "[1, 2, 3]") == CJSON_PARSE_OK);
    assert(v.type == CJSON_ARRAY);
    assert(cjson_get_array_size(&v) == 3);
    assert(cjson_get_array_element(&v, 0)->type == CJSON_NUMBER);
    assert(cjson_get_number(cjson_get_array_element(&v, 0)) == 1.0);
    assert(cjson_get_number(cjson_get_array_element(&v, 1)) == 2.0);
    assert(cjson_get_number(cjson_get_array_element(&v, 2)) == 3.0);
    cjson_free(&v);
    
    printf("✓ test_array passed\n");
}

void test_object() {
    cjson_value v;
    cjson_init(&v);
    
    assert(cjson_parse(&v, "{}") == CJSON_PARSE_OK);
    assert(v.type == CJSON_OBJECT);
    cjson_free(&v);
    
    cjson_init(&v);
    assert(cjson_parse(&v, "{\"name\": \"John\", \"age\": 30}") == CJSON_PARSE_OK);
    assert(v.type == CJSON_OBJECT);
    cjson_free(&v);
    
    printf("✓ test_object passed\n");
}

int main() {
    printf("Running basic JSON parsing tests...\n\n");
    
    test_null();
    test_boolean();
    test_number();
    test_string();
    test_array();
    test_object();
    
    printf("\n✅ All basic tests passed!\n");
    return 0;
}