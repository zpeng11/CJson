# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- GitHub Actions CI/CD pipeline with cross-platform testing
- Comprehensive test suite with 4 test categories
- CMake build system with static/shared library support
- Code coverage reporting via Codecov
- Static analysis with Cppcheck
- AddressSanitizer support for memory safety testing
- Professional README with badges and documentation
- MIT License
- Automated release workflow

### Fixed
- Missing error constants in enum
- CJSONS_STRING typo corrected to CJSON_STRING
- Missing function declarations in header
- Assignment operator precedence bug in parse_object function
- Memory management and cleanup issues

### Changed
- Updated build system from simple GCC to modern CMake
- Enhanced error handling with comprehensive error codes
- Improved documentation structure

## [1.0.0] - Initial Release

### Added
- JSON parsing functionality
- JSON generation (stringify) functionality
- Support for all JSON types (null, boolean, number, string, array, object)
- Unicode support with UTF-8 encoding
- Recursive descent parser implementation
- Memory management with proper cleanup
- C99 standard compliance