# =============================================================================
#
#   \file tests/CMakeLists.txt
#
#   HAN-FUN Library Tests CMakeList.
#
#   \version 0.3.2
#
#   Copyright (c) 2014 Bithium S.A.
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include ${CMAKE_CURRENT_SOURCE_DIR}/helpers/memory.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${CMAKE_CURRENT_SOURCE_DIR}/helpers/memory.h")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/hanfun/config.h )

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

set(TESTS_CORE_SRCS      "core/test_device_management.cpp"
                         "core/test_bind_management.cpp")

set(TESTS_TRANSPORT_SRCS "test_transport.cpp")

add_executable( test_hanfun ${TESTS_HELPER_SRCS} ${TESTS_COMMON_SRCS}
                            ${TESTS_PROTOCOL_SRCS}
                            ${TESTS_INTERFACES_SRCS}
                            ${TESTS_PROFILES_SRCS}
                            ${TESTS_DEVICES_SRCS}
                            ${TESTS_CORE_SRCS}
                            ${TESTS_TRANSPORT_SRCS} )

target_link_libraries( test_hanfun hanfun ${CPPUTEST_LIBRARIES} )

if(NOT CMAKE_CROSSCOMPILING)
  add_custom_target (check ALL DEPENDS test_hanfun COMMAND test_hanfun)
endif()

# add_test(hanfun_all test_hanfun)
