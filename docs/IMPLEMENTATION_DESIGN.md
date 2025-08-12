# CJson Implementation Design Documentation

## Overview

CJson is a lightweight, recursive descent JSON parser written in C99. It provides full JSON parsing and generation capabilities with careful memory management and comprehensive error handling.

## Architecture

### Core Design Principles

1. **Recursive Descent Parsing**: Uses a top-down parsing approach that naturally matches JSON's recursive grammar
2. **Union-based Value Storage**: Efficient memory usage through tagged unions
3. **Stack-based Memory Management**: Dynamic stack allocation during parsing for temporary storage
4. **Zero-copy String Handling**: Strings are copied only when necessary
5. **Complete Error Reporting**: Comprehensive error codes for all failure modes

### Data Structures

#### cjson_value
The central data structure representing any JSON value:

```c
struct cjson_value {
    union {
        struct { cjson_member *m; size_t size; size_t capacity; } o;  // object
        struct { cjson_value *a; size_t size; size_t capacity; } a;   // array
        struct { char *s; size_t len; } s;                           // string
        double n;                                                    // number
    } u;
    cjson_type type;
};
```

**Design Rationale:**
- Union minimizes memory footprint (only stores data for current type)
- Separate capacity tracking enables efficient array/object growth
- Explicit length tracking allows null characters in strings
- Type tag ensures safe union access

#### cjson_member
Represents key-value pairs in JSON objects:

```c
struct cjson_member {
    char *key;          // Object key (null-terminated)
    size_t len;         // Key length
    cjson_value v;      // Associated value
};
```

#### context
Internal parsing state management:

```c
typedef struct context {
    const char *json;   // Current position in input
    char *stack;        // Dynamic memory stack
    size_t top;         // Stack top pointer
    size_t capacity;    // Stack capacity
} context;
```

## Parsing Algorithm

### Recursive Descent Implementation

The parser uses mutually recursive functions following JSON grammar:

```
value    := object | array | string | number | "true" | "false" | "null"
object   := "{" [member ("," member)*] "}"
array    := "[" [value ("," value)*] "]"
member   := string ":" value
```

### Key Functions

#### `parse_value(context *c, cjson_value *v)`
Main dispatch function that determines value type and calls appropriate parser.

**Algorithm:**
1. Skip whitespace
2. Examine first character to determine type
3. Call specialized parser
4. Return parse result

#### `parse_string(context *c, cjson_value *v)`
Handles string parsing with full Unicode support.

**Features:**
- Standard escape sequences (`\"`, `\\`, `\/`, `\b`, `\f`, `\n`, `\r`, `\t`)
- Unicode escape sequences (`\uXXXX`)
- UTF-16 surrogate pairs
- Proper UTF-8 encoding

**Algorithm:**
1. Validate opening quote
2. Process characters until closing quote:
   - Regular characters: copy directly
   - Escape sequences: decode and copy
   - Unicode escapes: decode to UTF-8
   - Surrogate pairs: combine and encode
3. Validate and return

#### `parse_number(context *c, cjson_value *v)`
RFC 7159 compliant number parsing.

**Supported Formats:**
- Integers: `123`, `-456`
- Decimals: `123.456`, `-0.789`
- Scientific: `1e10`, `2.5e-3`, `1.23E+10`

**Algorithm:**
1. Parse optional minus sign
2. Parse integer part (zero or non-zero digit sequence)
3. Parse optional fractional part
4. Parse optional exponent
5. Convert using `strtod()` with overflow checking

#### `parse_array(context *c, cjson_value *v)`
Dynamic array parsing with stack-based temporary storage.

**Algorithm:**
1. Skip opening bracket
2. Handle empty array case
3. Parse elements in loop:
   - Parse value recursively
   - Push to temporary stack
   - Check for comma or closing bracket
4. Allocate final array and copy elements
5. Clean up on error

#### `parse_object(context *c, cjson_value *v)`
Object parsing with key-value pair handling.

**Algorithm:**
1. Skip opening brace
2. Handle empty object case
3. Parse members in loop:
   - Parse key string
   - Expect colon separator
   - Parse value recursively
   - Push member to stack
   - Check for comma or closing brace
4. Allocate final object and copy members

## Memory Management

### Stack-based Allocation

The parser uses a dynamic stack for temporary storage during parsing:

- **Automatic Growth**: Stack doubles in size when full
- **Efficient Cleanup**: Single free() call cleans entire stack
- **Exception Safety**: Stack automatically cleaned on parse failure

### Memory Lifecycle

1. **Parse Phase**: Temporary data on parser stack
2. **Success**: Data copied to final structures, stack freed
3. **Failure**: Stack freed, partial structures cleaned

### Free Function

`cjson_free()` recursively frees all allocated memory:
- Strings: Free character buffer
- Arrays: Recursively free elements, then array
- Objects: Free keys and recursively free values, then object

## Error Handling

### Error Codes

Comprehensive error reporting for all failure modes:

```c
enum {
    CJSON_PARSE_OK = 0,                          // Success
    CJSON_EXPECT_VALUE,                          // Expected value
    CJSON_INVALID_VALUE,                         // Invalid value
    CJSON_ROOT_NOT_SINGULAR,                     // Extra characters after value
    CJSON_NUMBER_TOO_BIG,                        // Number overflow
    CJSON_INVALID_STRING_CHAR,                   // Invalid character in string
    CJSON_INVALID_STRING_ESCAPE,                 // Invalid escape sequence
    CJSON_INVALID_STRING_MISS_QUOTATION,         // Missing closing quote
    CJSON_INVALID_UNICODE_HEX,                   // Invalid Unicode hex digits
    CJSON_INVALID_UNICODE_SURROGATE,             // Invalid surrogate pair
    CJSON_MISS_KEY,                             // Missing object key
    CJSON_MISS_COLON,                           // Missing colon in object
    CJSON_MISS_COMMA_OR_SQUARE_BRACKET,         // Missing comma or ]
    CJSON_MISS_COMMA_OR_CURLY_BRACKET           // Missing comma or }
};
```

### Error Recovery

- **Local Recovery**: Errors bubble up through recursive calls
- **Stack Cleanup**: Parser stack always cleaned on error
- **Memory Safety**: Partial structures properly freed
- **State Reset**: Value set to NULL type on error

## JSON Generation

### Stringify Algorithm

The `cjson_stringify()` function converts parsed JSON back to string format:

1. **Recursive Traversal**: Visit all nodes in tree
2. **Type-based Formatting**: Different formatting for each type
3. **Proper Escaping**: Escape special characters in strings
4. **Compact Output**: No unnecessary whitespace

### String Escaping

Follows JSON specification:
- Control characters (0x00-0x1F): Unicode escapes (`\u00XX`)
- Quote character: `\"`
- Backslash: `\\`
- Standard escapes: `\b`, `\f`, `\n`, `\r`, `\t`

## Unicode Support

### UTF-8 Encoding

All strings are stored as UTF-8:
- Input validation ensures valid UTF-8
- Unicode escapes decoded to UTF-8
- Surrogate pairs properly handled

### Surrogate Pair Processing

For characters outside Basic Multilingual Plane:
1. Parse high surrogate (`\uD800-\uDBFF`)
2. Expect low surrogate (`\uDC00-\uDFFF`)
3. Combine to single Unicode code point
4. Encode as UTF-8 (1-4 bytes)

## Performance Characteristics

### Time Complexity
- **Parsing**: O(n) where n is input length
- **Memory allocation**: Amortized O(1) for stack operations
- **String operations**: O(length) for copying/escaping

### Space Complexity
- **Parser stack**: O(d) where d is maximum nesting depth
- **Final structure**: O(n) where n is number of values
- **Temporary storage**: O(1) additional space

### Optimization Techniques
- **Single-pass parsing**: No tokenization phase
- **Minimal copying**: Direct pointer manipulation where possible
- **Stack reuse**: Same stack for all temporary allocations
- **Efficient growth**: Exponential stack expansion

## API Design

### Consistent Naming
- Prefix: `cjson_` for all public functions
- Getters: `cjson_get_*`
- Setters: `cjson_set_*`
- Types: `cjson_*`

### Memory Ownership
- **Parse results**: User owns returned structures
- **Stringify results**: User must free returned strings
- **Set functions**: Library takes ownership of input data

### Thread Safety
- **Stateless**: No global state
- **Reentrant**: Safe for concurrent use with separate contexts
- **Non-atomic**: Individual operations not thread-safe

## Testing Strategy

### Test Categories

1. **Basic Functionality**: All JSON types and operations
2. **Edge Cases**: Empty values, boundary conditions
3. **Error Handling**: All error conditions
4. **Memory Management**: Leak detection, cleanup verification
5. **Unicode**: Escape sequences, surrogate pairs
6. **Round-trip**: Parse → Stringify → Parse consistency

### Quality Assurance
- **Static Analysis**: Compiler warnings enabled
- **Dynamic Analysis**: AddressSanitizer support
- **Coverage Testing**: All code paths tested
- **Regression Testing**: Automated test suite

This design provides a robust, efficient, and standards-compliant JSON parser suitable for embedded systems and high-performance applications.