#include "../CJson.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

void test_memory_cleanup() {
    cjson_value v;
    
    // Test string cleanup
    cjson_init(&v);
    assert(cjson_parse(&v, "\"hello world\"") == CJSON_PARSE_OK);
    cjson_free(&v); // Should not crash
    
    // Test array cleanup
    cjson_init(&v);
    assert(cjson_parse(&v, "[1, \"hello\", true, null]") == CJSON_PARSE_OK);
    cjson_free(&v); // Should not crash
    
    // Test object cleanup
    cjson_init(&v);
    assert(cjson_parse(&v, "{\"name\": \"John\", \"age\": 30, \"active\": true}") == CJSON_PARSE_OK);
    cjson_free(&v); // Should not crash
    
    // Test nested structure cleanup
    cjson_init(&v);
    assert(cjson_parse(&v, "{\"users\": [{\"name\": \"John\"}, {\"name\": \"Jane\"}]}") == CJSON_PARSE_OK);
    cjson_free(&v); // Should not crash
    
    printf("✓ test_memory_cleanup passed\n");
}

void test_set_operations() {
    cjson_value v;
    
    // Test set boolean
    cjson_init(&v);
    cjson_set_boolean(&v, 1);
    assert(v.type == CJSON_TRUE);
    assert(cjson_get_boolean(&v) == 1);
    cjson_free(&v);
    
    cjson_init(&v);
    cjson_set_boolean(&v, 0);
    assert(v.type == CJSON_FALSE);
    assert(cjson_get_boolean(&v) == 0);
    cjson_free(&v);
    
    // Test set number
    cjson_init(&v);
    cjson_set_number(&v, 42.5);
    assert(v.type == CJSON_NUMBER);
    assert(cjson_get_number(&v) == 42.5);
    cjson_free(&v);
    
    // Test set string
    cjson_init(&v);
    cjson_set_string(&v, "test string", 11);
    assert(v.type == CJSON_STRING);
    assert(strcmp(cjson_get_string(&v), "test string") == 0);
    assert(cjson_get_string_length(&v) == 11);
    cjson_free(&v);
    
    // Test set empty string
    cjson_init(&v);
    cjson_set_string(&v, "", 0);
    assert(v.type == CJSON_STRING);
    assert(cjson_get_string_length(&v) == 0);
    cjson_free(&v);
    
    // Test set null string
    cjson_init(&v);
    cjson_set_string(&v, NULL, 0);
    assert(v.type == CJSON_STRING);
    assert(cjson_get_string(&v) == NULL);
    cjson_free(&v);
    
    // Test set array
    cjson_init(&v);
    cjson_set_array(&v, 5);
    assert(v.type == CJSON_ARRAY);
    assert(cjson_get_array_size(&v) == 0);
    assert(cjson_get_array_capacity(&v) == 5);
    cjson_free(&v);
    
    // Test set object
    cjson_init(&v);
    cjson_set_object(&v, 3);
    assert(v.type == CJSON_OBJECT);
    cjson_free(&v);
    
    printf("✓ test_set_operations passed\n");
}

void test_multiple_operations() {
    cjson_value v;
    
    // Test overwriting values (should free previous)
    cjson_init(&v);
    cjson_set_string(&v, "first", 5);
    cjson_set_string(&v, "second", 6);
    assert(strcmp(cjson_get_string(&v), "second") == 0);
    cjson_free(&v);
    
    // Test changing type
    cjson_init(&v);
    cjson_set_number(&v, 123);
    cjson_set_string(&v, "changed", 7);
    assert(v.type == CJSON_STRING);
    assert(strcmp(cjson_get_string(&v), "changed") == 0);
    cjson_free(&v);
    
    printf("✓ test_multiple_operations passed\n");
}

int main() {
    printf("Running memory management tests...\n\n");
    
    test_memory_cleanup();
    test_set_operations();
    test_multiple_operations();
    
    printf("\n✅ All memory tests passed!\n");
    return 0;
}