cmake_minimum_required(VERSION 3.15)
project(CJson VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build options
option(CJSON_BUILD_SHARED "Build shared library" ON)
option(CJSON_BUILD_STATIC "Build static library" ON)
option(CJSON_BUILD_TESTS "Build tests" ON)
option(CJSON_ENABLE_SANITIZER "Enable AddressSanitizer in debug builds" OFF)

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Enable AddressSanitizer for debug builds if requested
if(CJSON_ENABLE_SANITIZER AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

# Library source files
set(CJSON_SOURCES
    CJson.c
)

set(CJSON_HEADERS
    CJson.h
)

# Build shared library
if(CJSON_BUILD_SHARED)
    add_library(cjson_shared SHARED ${CJSON_SOURCES})
    set_target_properties(cjson_shared PROPERTIES
        OUTPUT_NAME cjson
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "${CJSON_HEADERS}"
    )
    target_include_directories(cjson_shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
endif()

# Build static library
if(CJSON_BUILD_STATIC)
    add_library(cjson_static STATIC ${CJSON_SOURCES})
    set_target_properties(cjson_static PROPERTIES
        OUTPUT_NAME cjson
        PUBLIC_HEADER "${CJSON_HEADERS}"
    )
    target_include_directories(cjson_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
endif()

# Create alias targets
if(CJSON_BUILD_SHARED)
    add_library(CJson::cjson ALIAS cjson_shared)
elseif(CJSON_BUILD_STATIC)
    add_library(CJson::cjson ALIAS cjson_static)
endif()

# Build tests
if(CJSON_BUILD_TESTS)
    enable_testing()
    
    # Test executable function
    function(add_cjson_test test_name)
        add_executable(${test_name} tests/${test_name}.c ${CJSON_SOURCES})
        target_include_directories(${test_name} PRIVATE .)
        add_test(NAME ${test_name} COMMAND ${test_name})
        
        # Enable sanitizer for tests if requested
        if(CJSON_ENABLE_SANITIZER AND CMAKE_BUILD_TYPE STREQUAL "Debug")
            set_target_properties(${test_name} PROPERTIES
                COMPILE_FLAGS "-fsanitize=address -fno-omit-frame-pointer"
                LINK_FLAGS "-fsanitize=address"
            )
        endif()
    endfunction()
    
    # Add all tests
    add_cjson_test(test_basic)
    add_cjson_test(test_edge_cases)
    add_cjson_test(test_memory)
    add_cjson_test(test_stringify)

endif()

# Installation
include(GNUInstallDirs)

if(CJSON_BUILD_SHARED)
    install(TARGETS cjson_shared
        EXPORT CJsonTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

if(CJSON_BUILD_STATIC)
    install(TARGETS cjson_static
        EXPORT CJsonTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

# Export targets
install(EXPORT CJsonTargets
    FILE CJsonTargets.cmake
    NAMESPACE CJson::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CJson
)

# Generate config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CJsonConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CJsonConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CJson
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CJsonConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CJsonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CJsonConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CJson
)

# Generate pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cjson.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cjson.pc"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cjson.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# Print build configuration
message(STATUS "CJson ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build shared: ${CJSON_BUILD_SHARED}")
message(STATUS "Build static: ${CJSON_BUILD_STATIC}")
message(STATUS "Build tests: ${CJSON_BUILD_TESTS}")
message(STATUS "Enable sanitizer: ${CJSON_ENABLE_SANITIZER}")