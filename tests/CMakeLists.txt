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

set(TESTS_HELPER_SRCS test_helper.cpp)

set(TESTS_COMMON_SRCS test_common.cpp)
set(TESTS_PROTOCOL_SRCS test_protocol.cpp)

add_executable( test_hanfun ${TESTS_HELPER_SRCS} ${TESTS_COMMON_SRCS}
                            ${TESTS_PROTOCOL_SRCS} )

target_link_libraries( test_hanfun hanfun ${CPPUTEST_LIBRARIES} )

add_custom_command(TARGET test_hanfun POST_BUILD COMMAND test_hanfun)
