# CJson API Reference

## Overview

CJson provides a complete JSON parsing and generation API for C programs. All functions use the `cjson_` prefix and follow consistent naming conventions.

## Data Types

### cjson_value

The main structure representing any JSON value:

```c
typedef struct cjson_value cjson_value;
```

### cjson_type

Enumeration of JSON value types:

```c
typedef enum {
    CJSON_NULL,     // null
    CJSON_TRUE,     // true
    CJSON_FALSE,    // false  
    CJSON_NUMBER,   // number
    CJSON_STRING,   // string
    CJSON_ARRAY,    // array
    CJSON_OBJECT    // object
} cjson_type;
```

### Error Codes

```c
enum {
    CJSON_PARSE_OK = 0,                          // Success
    CJSON_EXPECT_VALUE,                          // Expected a value
    CJSON_INVALID_VALUE,                         // Invalid value
    CJSON_ROOT_NOT_SINGULAR,                     // Extra content after JSON
    CJSON_NUMBER_TOO_BIG,                        // Number overflow
    CJSON_INVALID_STRING_CHAR,                   // Invalid character in string
    CJSON_INVALID_STRING_ESCAPE,                 // Invalid escape sequence
    CJSON_INVALID_STRING_MISS_QUOTATION,         // Missing closing quote
    CJSON_INVALID_UNICODE_HEX,                   // Invalid Unicode hex
    CJSON_INVALID_UNICODE_SURROGATE,             // Invalid surrogate pair
    CJSON_MISS_KEY,                             // Missing object key
    CJSON_MISS_COLON,                           // Missing colon
    CJSON_MISS_COMMA_OR_SQUARE_BRACKET,         // Missing , or ]
    CJSON_MISS_COMMA_OR_CURLY_BRACKET           // Missing , or }
};
```

## Core Functions

### Initialization and Cleanup

#### cjson_init()

```c
#define cjson_init(v) do { (v)->type = CJSON_NULL; } while(0)
```

Initializes a JSON value to null type.

**Parameters:**
- `v`: Pointer to cjson_value to initialize

**Example:**
```c
cjson_value v;
cjson_init(&v);
```

#### cjson_free()

```c
void cjson_free(cjson_value *v);
```

Recursively frees all memory associated with a JSON value and resets it to null type.

**Parameters:**
- `v`: Pointer to cjson_value to free

**Example:**
```c
cjson_free(&v);  // Frees all memory, sets type to CJSON_NULL
```

### Parsing

#### cjson_parse()

```c
int cjson_parse(cjson_value *v, const char *json_str);
```

Parses a JSON string and populates the value structure.

**Parameters:**
- `v`: Pointer to cjson_value to store result
- `json_str`: Null-terminated JSON string to parse

**Returns:**
- `CJSON_PARSE_OK` on success
- Error code on failure (see Error Codes section)

**Example:**
```c
cjson_value v;
cjson_init(&v);
int result = cjson_parse(&v, "{\"name\": \"John\", \"age\": 30}");
if (result == CJSON_PARSE_OK) {
    // Parse successful
} else {
    // Handle error
}
cjson_free(&v);
```

### String Generation

#### cjson_stringify()

```c
char *cjson_stringify(const cjson_value *v, size_t *length);
```

Converts a JSON value to its string representation.

**Parameters:**
- `v`: Pointer to cjson_value to stringify
- `length`: Optional pointer to store resulting string length (can be NULL)

**Returns:**
- Dynamically allocated string (must be freed by caller)
- NULL on error

**Example:**
```c
size_t len;
char *json_str = cjson_stringify(&v, &len);
if (json_str) {
    printf("JSON: %s (length: %zu)\n", json_str, len);
    free(json_str);
}
```

## Boolean Functions

#### cjson_get_boolean()

```c
int cjson_get_boolean(cjson_value *v);
```

Gets the boolean value (for true/false types).

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_TRUE or CJSON_FALSE

**Returns:**
- 1 for true, 0 for false

**Precondition:** `v->type` must be `CJSON_TRUE` or `CJSON_FALSE`

#### cjson_set_boolean()

```c
void cjson_set_boolean(cjson_value *v, int bool_val);
```

Sets a boolean value.

**Parameters:**
- `v`: Pointer to cjson_value
- `bool_val`: Non-zero for true, zero for false

**Example:**
```c
cjson_set_boolean(&v, 1);  // Sets to true
cjson_set_boolean(&v, 0);  // Sets to false
```

## Number Functions

#### cjson_get_number()

```c
double cjson_get_number(cjson_value *v);
```

Gets the numeric value.

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_NUMBER

**Returns:**
- The numeric value as double

**Precondition:** `v->type` must be `CJSON_NUMBER`

#### cjson_set_number()

```c
void cjson_set_number(cjson_value *v, double n);
```

Sets a numeric value.

**Parameters:**
- `v`: Pointer to cjson_value
- `n`: Numeric value to set

**Example:**
```c
cjson_set_number(&v, 42.5);
```

## String Functions

#### cjson_get_string()

```c
const char *cjson_get_string(cjson_value *v);
```

Gets the string value.

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_STRING

**Returns:**
- Null-terminated string (do not modify or free)
- NULL if string was set to NULL

**Precondition:** `v->type` must be `CJSON_STRING`

#### cjson_get_string_length()

```c
size_t cjson_get_string_length(cjson_value *v);
```

Gets the string length (not including null terminator).

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_STRING

**Returns:**
- String length in bytes

**Precondition:** `v->type` must be `CJSON_STRING`

#### cjson_set_string()

```c
void cjson_set_string(cjson_value *v, const char *str, size_t str_len);
```

Sets a string value. The string is copied internally.

**Parameters:**
- `v`: Pointer to cjson_value
- `str`: String to copy (can be NULL)
- `str_len`: Length of string in bytes

**Example:**
```c
cjson_set_string(&v, "hello", 5);
cjson_set_string(&v, "world\0test", 10);  // Can contain null bytes
cjson_set_string(&v, NULL, 0);           // NULL string
```

## Array Functions

#### cjson_set_array()

```c
void cjson_set_array(cjson_value *v, size_t capacity);
```

Initializes a value as an array with given capacity.

**Parameters:**
- `v`: Pointer to cjson_value
- `capacity`: Initial capacity (can be 0)

#### cjson_get_array_size()

```c
size_t cjson_get_array_size(cjson_value *v);
```

Gets the number of elements in an array.

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_ARRAY

**Returns:**
- Number of elements

**Precondition:** `v->type` must be `CJSON_ARRAY`

#### cjson_get_array_capacity()

```c
size_t cjson_get_array_capacity(cjson_value *v);
```

Gets the allocated capacity of an array.

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_ARRAY

**Returns:**
- Array capacity

**Precondition:** `v->type` must be `CJSON_ARRAY`

#### cjson_get_array_element()

```c
cjson_value *cjson_get_array_element(cjson_value *v, size_t index);
```

Gets a pointer to an array element.

**Parameters:**
- `v`: Pointer to cjson_value of type CJSON_ARRAY
- `index`: Zero-based element index

**Returns:**
- Pointer to array element (do not free)

**Precondition:** 
- `v->type` must be `CJSON_ARRAY`
- `index` must be less than array size

## Object Functions

#### cjson_set_object()

```c
void cjson_set_object(cjson_value *v, size_t capacity);
```

Initializes a value as an object with given capacity.

**Parameters:**
- `v`: Pointer to cjson_value
- `capacity`: Initial capacity for key-value pairs (can be 0)

## Usage Examples

### Basic Parsing

```c
#include "CJson.h"
#include <stdio.h>

int main() {
    const char *json = "{\"name\": \"Alice\", \"age\": 25, \"active\": true}";
    cjson_value root;
    
    cjson_init(&root);
    
    int result = cjson_parse(&root, json);
    if (result == CJSON_PARSE_OK) {
        printf("Parse successful!\n");
        printf("Type: %d\n", root.type);  // Should be CJSON_OBJECT
    } else {
        printf("Parse failed with error code: %d\n", result);
    }
    
    cjson_free(&root);
    return 0;
}
```

### Creating JSON

```c
cjson_value root;
cjson_init(&root);

// Create an object
cjson_set_object(&root, 0);

// Note: The current API doesn't provide functions to add members to objects
// or elements to arrays. This would be a logical extension.

char *json_str = cjson_stringify(&root, NULL);
printf("Generated JSON: %s\n", json_str);
free(json_str);

cjson_free(&root);
```

### Working with Arrays

```c
const char *json = "[1, 2.5, \"hello\", true, null]";
cjson_value root;

cjson_init(&root);
if (cjson_parse(&root, json) == CJSON_PARSE_OK) {
    if (root.type == CJSON_ARRAY) {
        size_t size = cjson_get_array_size(&root);
        printf("Array has %zu elements:\n", size);
        
        for (size_t i = 0; i < size; i++) {
            cjson_value *element = cjson_get_array_element(&root, i);
            switch (element->type) {
                case CJSON_NUMBER:
                    printf("  [%zu]: %f\n", i, cjson_get_number(element));
                    break;
                case CJSON_STRING:
                    printf("  [%zu]: \"%s\"\n", i, cjson_get_string(element));
                    break;
                case CJSON_TRUE:
                    printf("  [%zu]: true\n", i);
                    break;
                case CJSON_FALSE:
                    printf("  [%zu]: false\n", i);
                    break;
                case CJSON_NULL:
                    printf("  [%zu]: null\n", i);
                    break;
                default:
                    printf("  [%zu]: unknown type\n", i);
            }
        }
    }
}
cjson_free(&root);
```

### Error Handling

```c
const char *invalid_json = "{\"key\": }";  // Missing value
cjson_value root;

cjson_init(&root);
int result = cjson_parse(&root, invalid_json);

switch (result) {
    case CJSON_PARSE_OK:
        printf("Parse successful\n");
        break;
    case CJSON_INVALID_VALUE:
        printf("Invalid value in JSON\n");
        break;
    case CJSON_MISS_KEY:
        printf("Missing key in object\n");
        break;
    case CJSON_MISS_COLON:
        printf("Missing colon in object\n");
        break;
    default:
        printf("Parse error: %d\n", result);
}

cjson_free(&root);  // Always safe to call
```

## Memory Management Notes

1. **Ownership**: After parsing, the user owns all memory in the cjson_value structure
2. **Cleanup**: Always call `cjson_free()` to prevent memory leaks
3. **Strings**: Returned strings from `cjson_get_string()` should not be modified or freed
4. **Stringify**: The string returned by `cjson_stringify()` must be freed by the caller
5. **Safety**: `cjson_free()` is always safe to call and handles NULL/uninitialized values

## Thread Safety

- Individual `cjson_value` structures are not thread-safe
- Multiple threads can safely use the library with separate `cjson_value` instances
- No global state is used, making the library reentrant