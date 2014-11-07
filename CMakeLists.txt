# =============================================================================
#
#   @file CMakeLists.txt
#
#   HAN-FUN Library Top CMakeList
#
#   @version    1.1.1
#
#   @copyright  Copyright (c) 2014  ULE Alliance
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
#
#   Initial development by Bithium S.A. [http://www.bithium.com]
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
  ${PROJECT_BINARY_DIR}
  ${PROJECT_INCLUDE_DIR}
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-rtti")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 ")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-exceptions")

# ==============================================================================
# Configuration
# ==============================================================================

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# The version number.
set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 1)
set(PROJECT_PATCH_VERSION 1)

set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

configure_file( ${CMAKE_SOURCE_DIR}/version.h.in
                ${CMAKE_BINARY_DIR}/hanfun/version.h )

include("cmake/options.cmake")

configure_file( ${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/hanfun/config.h )

# ==============================================================================
# Library
# ==============================================================================

add_subdirectory(src)

# ==============================================================================
# Example Applications
# ==============================================================================

add_subdirectory(apps)

# ==============================================================================
# Tests
# ==============================================================================

if(HF_BUILD_TESTS)
    add_subdirectory(tests)
endif(HF_BUILD_TESTS)

# ==============================================================================
# Documentation
# ==============================================================================

add_subdirectory(doc EXCLUDE_FROM_ALL)

# ==============================================================================
# Package
# ==============================================================================

include("cmake/package.cmake")
