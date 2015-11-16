# =============================================================================
#
#   @file tests/CMakeLists.txt
#
#   HAN-FUN Library Tests CMakeList.
#
#   @version 1.3.0
#
#   @copyright  Copyright (c) 2014  ULE Alliance
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
#
#   Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

# ==============================================================================
# Configuration
# ==============================================================================

set(CPPUTEST_TESTS OFF CACHE BOOL "Disable CppUTest internal tests" FORCE)
set(CPPUTEST_COVERAGE OFF CACHE BOOL "Enable CppUTest code coverage." FORCE)
set(CPPUTEST_C++11 ON CACHE BOOL "Enable CppUTest C++11 support." FORCE)

add_subdirectory(cpputest)

find_package(CppUTest REQUIRED HINTS ${CMAKE_BINARY_DIR} )

include_directories(
    ${CppUTest_INCLUDE_DIRS}
    "."
)

find_package("Backtrace")
if (Backtrace_FOUND)
  include_directories(${Backtrace_INCLUDE_DIR})
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include ${CMAKE_CURRENT_SOURCE_DIR}/helpers/memory.h")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${CMAKE_CURRENT_SOURCE_DIR}/helpers/memory.h")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/hanfun/config.h )

enable_testing()

# ==============================================================================
# Tests Build
# ==============================================================================

## Common ##
set(TESTS_HELPER_SRCS test_helper.cpp)

set(TESTS_COMMON_SRCS test_common.cpp)
set(TESTS_PROTOCOL_SRCS test_protocol.cpp)

## Interfaces ##
set(TESTS_INTERFACES_SRCS "test_interfaces.cpp")

macro(add_interface __name)
   list(APPEND TESTS_INTERFACES_SRCS "interfaces/test_${__name}.cpp")
endmacro()

add_interface("alert")
add_interface("on_off")
add_interface("level_control")
add_interface("simple_power_meter")
add_interface("simple_temperature")
add_interface("simple_humidity")
add_interface("simple_thermostat")

## Profiles ##
set(TESTS_PROFILES_SRCS  "test_profiles.cpp")

## Devices ##
set(TESTS_DEVICES_SRCS   "test_devices.cpp")

## Core Services & Interfaces ##
set(TESTS_CORE_SRCS      "core/test_device_management.cpp"
                         "core/test_bind_management.cpp"
                         "core/test_device_information.cpp"
                         "core/test_attribute_reporting.cpp"
                         "core/test_session_management.cpp")

## Transport Layer ##
set(TESTS_TRANSPORT_SRCS "test_transport.cpp")

## Unit ##
set(TESTS_UNITS_SRCS     "test_units.cpp")

## Test Executable ##
add_executable( test_hanfun ${TESTS_HELPER_SRCS} ${TESTS_COMMON_SRCS}
                            ${TESTS_PROTOCOL_SRCS}
                            ${TESTS_INTERFACES_SRCS}
                            ${TESTS_PROFILES_SRCS}
                            ${TESTS_DEVICES_SRCS}
                            ${TESTS_CORE_SRCS}
                            ${TESTS_TRANSPORT_SRCS}
                            ${TESTS_UNITS_SRCS} )

target_link_libraries( test_hanfun hanfun ${CppUTest_LIBRARIES} )
if (Backtrace_FOUND)
  target_link_libraries( test_hanfun ${Backtrace_LIBRARIES} )
endif()

if(NOT CMAKE_CROSSCOMPILING)
  add_custom_target (check ALL DEPENDS test_hanfun COMMAND test_hanfun)
endif()
