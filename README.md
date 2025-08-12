# CJson

[![CI](https://github.com/eleven/CJson/actions/workflows/ci.yml/badge.svg)](https://github.com/eleven/CJson/actions/workflows/ci.yml)
[![Release](https://github.com/eleven/CJson/actions/workflows/release.yml/badge.svg)](https://github.com/eleven/CJson/actions/workflows/release.yml)
[![GitHub release](https://img.shields.io/github/v/release/eleven/CJson)](https://github.com/eleven/CJson/releases)
[![codecov](https://codecov.io/gh/eleven/CJson/branch/master/graph/badge.svg)](https://codecov.io/gh/eleven/CJson)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)](#building)

A lightweight, high-performance JSON parsing and generation library written in C99. CJson provides a complete JSON implementation with comprehensive error handling, Unicode support, and efficient memory management.

## Features

- **Complete JSON Support**: Parse and generate all JSON types (null, boolean, number, string, array, object)
- **RFC 7159 Compliant**: Follows the JSON specification precisely
- **Unicode Support**: Full UTF-8 support with proper escape sequence handling
- **Memory Safe**: Careful memory management with comprehensive cleanup
- **Error Reporting**: Detailed error codes for all failure modes
- **Zero Dependencies**: Pure C99 implementation with no external dependencies
- **Thread Safe**: Reentrant design suitable for multi-threaded applications
- **High Performance**: Single-pass recursive descent parser

## Installation

### Download Pre-built Releases

The easiest way to get CJson is to download pre-built releases from GitHub:

1. Go to the [Releases page](https://github.com/eleven/CJson/releases)
2. Download the archive for your platform:
   - **Linux**: `cjson-linux-vX.X.X.tar.gz`
   - **macOS**: `cjson-macos-vX.X.X.tar.gz`
   - **Windows**: `cjson-windows-vX.X.X.zip`
3. Extract and include `CJson.h` in your project
4. Link against the provided library

### Package Managers

```bash
# Using vcpkg
vcpkg install cjson

# Using Conan (if available)
conan install cjson/1.0.0@
```

## Building

### Using CMake (Recommended)

```bash
mkdir build
cd build
cmake ..
make

# Run tests
ctest
```

### Using GCC directly

```bash
# Compile library with your program
gcc your_program.c CJson.c -o your_program

# Run tests
gcc tests/test_basic.c CJson.c -o test_basic && ./test_basic
```

### CMake Options

- `CJSON_BUILD_SHARED=ON/OFF` - Build shared library (default: ON)
- `CJSON_BUILD_STATIC=ON/OFF` - Build static library (default: ON) 
- `CJSON_BUILD_TESTS=ON/OFF` - Build test suite (default: ON)
- `CJSON_ENABLE_SANITIZER=ON/OFF` - Enable AddressSanitizer for debug builds (default: OFF)

Example:
```bash
cmake -DCJSON_BUILD_SHARED=OFF -DCJSON_ENABLE_SANITIZER=ON ..
```

## Quick Start

```c
#include "CJson.h"
#include <stdio.h>

int main() {
    const char *json = "{\"name\": \"Alice\", \"age\": 25}";
    cjson_value root;
    
    // Parse JSON
    cjson_init(&root);
    int result = cjson_parse(&root, json);
    
    if (result == CJSON_PARSE_OK) {
        printf("Parse successful!\n");
        
        // Generate JSON string
        char *output = cjson_stringify(&root, NULL);
        printf("Generated: %s\n", output);
        free(output);
    } else {
        printf("Parse failed: %d\n", result);
    }
    
    // Always cleanup
    cjson_free(&root);
    return 0;
}
```

## API Overview

### Core Functions
- `cjson_parse()` - Parse JSON string
- `cjson_stringify()` - Generate JSON string
- `cjson_init()` - Initialize value
- `cjson_free()` - Free memory

### Type-specific Functions
- `cjson_get_boolean()` / `cjson_set_boolean()`
- `cjson_get_number()` / `cjson_set_number()`
- `cjson_get_string()` / `cjson_set_string()`
- Array and object manipulation functions

## Documentation

- [API Reference](docs/API_REFERENCE.md) - Complete API documentation
- [Implementation Design](docs/IMPLEMENTATION_DESIGN.md) - Architecture and design details
- [CLAUDE.md](CLAUDE.md) - Developer quick reference

## Testing

The library includes comprehensive tests covering:

- Basic JSON parsing and generation
- Edge cases and error conditions
- Memory management and leak detection  
- Unicode and escape sequence handling
- Round-trip parsing consistency

### Local Testing
```bash
# Using CMake
make test

# Or run individual tests
./tests/test_basic
./tests/test_edge_cases  
./tests/test_memory
./tests/test_stringify
```

### Continuous Integration

The project uses GitHub Actions for continuous integration with:

- **Cross-platform testing**: Linux (Ubuntu), macOS, and Windows
- **Multiple compilers**: GCC, Clang, and MSVC
- **Memory safety**: AddressSanitizer integration
- **Code coverage**: Automated coverage reporting via Codecov
- **Static analysis**: Cppcheck for code quality

All tests must pass on all platforms before merging changes.

## License

This project is provided as-is for educational and development purposes.

## Contributing

1. Ensure all tests pass
2. Follow existing code style
3. Add tests for new features
4. Update documentation as needed
