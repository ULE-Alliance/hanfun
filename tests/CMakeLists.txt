# =============================================================================
#
#   \file tests/CMakeLists.txt
#
#   HAN-FUN Library Tests CMakeList.
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version x.x.x
#
#   Copyright (c) 2012 Bithium S.A.
#
# =============================================================================

# ==============================================================================
# Configuration
# ==============================================================================

set(CPPUTEST_TESTS OFF CACHE BOOL "Disable CppUTest internal tests" FORCE)
set(CPPUTEST_COVERAGE OFF CACHE BOOL "Enable CppUTest code coverage." FORCE)

add_subdirectory(cpputest)

find_package(CppUTest REQUIRED HINTS ${CMAKE_BINARY_DIR} )

include_directories(
    "${CPPUTEST_INCLUDE_DIRS}"
    "."
)

enable_testing()

# ==============================================================================
# Tests Build
# ==============================================================================
