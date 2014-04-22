# =============================================================================
#
#   \file tests/CMakeLists.txt
#
#   HAN-FUN Library Tests CMakeList.
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version 0.2.0
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
    ${CPPUTEST_INCLUDE_DIRS}
    "."
)

enable_testing()

# ==============================================================================
# Tests Build
# ==============================================================================

set(TESTS_HELPER_SRCS test_helper.cpp)

set(TESTS_COMMON_SRCS test_common.cpp)
set(TESTS_PROTOCOL_SRCS test_protocol.cpp)

set(TESTS_INTERFACES_SRCS "test_interfaces.cpp"
                          "interfaces/test_alert.cpp"
                          "interfaces/test_on_off.cpp"
                          "interfaces/test_level_control.cpp"
                          "interfaces/test_simple_power_meter.cpp")

set(TESTS_PROFILES_SRCS  "test_profiles.cpp")

set(TESTS_DEVICES_SRCS   "test_devices.cpp")

set(TESTS_CORE_SRCS      "core/test_device_management.cpp")

set(TESTS_TRANSPORT_SRCS "test_transport.cpp")

add_executable( test_hanfun ${TESTS_HELPER_SRCS} ${TESTS_COMMON_SRCS}
                            ${TESTS_PROTOCOL_SRCS}
                            ${TESTS_INTERFACES_SRCS}
                            ${TESTS_PROFILES_SRCS}
                            ${TESTS_DEVICES_SRCS}
                            ${TESTS_CORE_SRCS}
                            ${TESTS_TRANSPORT_SRCS} )

target_link_libraries( test_hanfun hanfun ${CPPUTEST_LIBRARIES} )

add_custom_command(TARGET test_hanfun POST_BUILD COMMAND test_hanfun)
