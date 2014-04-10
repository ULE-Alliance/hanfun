# =============================================================================
#
#   \file CMakeLists.txt
#
#   HAN-FUN Library Top CMakeList
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version 0.1.0
#
#   Copyright (c) 2012 Bithium S.A.
#
# =============================================================================

cmake_minimum_required (VERSION 2.8)

project (HAN-FUN)

# ==============================================================================
# Default project setup.

## Standard project paths.
set( PROJECT_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/inc" )

# setup default include-directories
include_directories(
  ${PROJECT_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
)

if( ${PROJECT_NAME}_BUILD_TESTS )
    set(BUILD_TESTS true)
else()
    set(BUILD_TESTS false)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-rtti -std=c++0x")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 ")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-exceptions")

# ==============================================================================
# Configuration
# ==============================================================================

# The version number.
set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 1)
set(PROJECT_PATCH_VERSION 0)

set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

configure_file( ${CMAKE_SOURCE_DIR}/version.h.in
                ${CMAKE_BINARY_DIR}/hanfun/version.h )

include("cmake/options.cmake")

if(BUILD_TESTS)
    configure_file( ${CMAKE_SOURCE_DIR}/tests/config.h.in
                    ${CMAKE_BINARY_DIR}/hanfun/config.h )
else()
    configure_file( ${CMAKE_SOURCE_DIR}/config.h.in
                    ${CMAKE_BINARY_DIR}/hanfun/config.h )
endif()

# ==============================================================================
# Library
# ==============================================================================

add_subdirectory(src)

if(HANFUN_BUILD_APPS)
    add_subdirectory(apps)
endif()

# ==============================================================================
# Tests
# ==============================================================================

if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

# ==============================================================================
# Documentation
# ==============================================================================

add_subdirectory(doc EXCLUDE_FROM_ALL)
