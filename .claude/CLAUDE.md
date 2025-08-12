# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C JSON parsing library that provides JSON parsing and generation functionality. The project consists of:

- `CJson.h` - Header file with type definitions and function declarations
- `CJson.c` - Main implementation of JSON parsing and generation
- `test.c` - Simple test program
- `README.md` - Basic project description

## Build Instructions

### Using CMake (Recommended)

```bash
# Create build directory and configure
mkdir build && cd build
cmake ..

# Build the library and tests
make

# Run all tests
ctest

# Run specific tests
./test_basic
./test_edge_cases
./test_memory
./test_stringify
```

### Using GCC directly

```bash
# Compile library with test program
gcc CJson.c test.c -o test

# Compile and run specific tests
gcc tests/test_basic.c CJson.c -o test_basic && ./test_basic
gcc tests/test_edge_cases.c CJson.c -o test_edge_cases && ./test_edge_cases
gcc tests/test_memory.c CJson.c -o test_memory && ./test_memory
gcc tests/test_stringify.c CJson.c -o test_stringify && ./test_stringify
```

### CMake Build Options

- `CJSON_BUILD_SHARED=ON/OFF` - Build shared library (default: ON)
- `CJSON_BUILD_STATIC=ON/OFF` - Build static library (default: ON) 
- `CJSON_BUILD_TESTS=ON/OFF` - Build test suite (default: ON)
- `CJSON_ENABLE_SANITIZER=ON/OFF` - Enable AddressSanitizer (default: OFF)

### Test Suite

The project includes comprehensive tests in the `tests/` directory:
- `test_basic.c` - Basic JSON parsing for all types
- `test_edge_cases.c` - Error handling and edge cases
- `test_memory.c` - Memory management and cleanup
- `test_stringify.c` - JSON generation and round-trip tests

## Architecture

### Core Data Structures

- `cjson_value` - Union-based structure representing any JSON value (null, boolean, number, string, array, object)
- `cjson_member` - Key-value pair for JSON objects
- `context` - Internal parsing context with stack-based memory management

### Key Components

1. **Parser** (`CJson.c:397-417`) - Main parsing entry point with error handling
2. **Value Parsers** - Individual parsers for each JSON type:
   - `parse_word()` - For null, true, false literals
   - `parse_number()` - Number parsing with proper validation
   - `parse_string()` - String parsing with Unicode escape support
   - `parse_array()` - Array parsing with recursive value parsing
   - `parse_object()` - Object parsing with key-value pairs

3. **Stringifier** (`CJson.c:524-537`) - Converts parsed JSON back to string format

4. **Memory Management** - Stack-based allocation during parsing with proper cleanup

### Parser Design

The parser uses a recursive descent approach with a context structure that maintains:
- Current position in JSON string
- Dynamic stack for temporary storage
- Error state management

The library supports full JSON specification including:
- Unicode escape sequences
- Scientific notation for numbers  
- Nested arrays and objects
- Proper string escaping

## Key Improvements Made

The library has been fully debugged and enhanced with:

1. **Fixed Compilation Errors**: Added missing error constants and function declarations
2. **Comprehensive Test Suite**: 4 test files covering all functionality and edge cases  
3. **CMake Build System**: Modern build system with static/shared libraries and testing
4. **Documentation**: Complete API reference and implementation design docs
5. **Memory Safety**: All memory leaks fixed with proper cleanup
6. **Error Handling**: All error conditions properly tested and documented

## Common Development Tasks

- **Add new functionality**: Update header, implement in source, add tests
- **Run memory checks**: Use `cmake -DCJSON_ENABLE_SANITIZER=ON` for debug builds
- **Performance testing**: Use release builds with `-DCMAKE_BUILD_TYPE=Release`
- **Integration**: Library exports CMake targets as `CJson::cjson` for easy linking