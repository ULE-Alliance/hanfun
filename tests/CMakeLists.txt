# =============================================================================
#
#   @file tests/CMakeLists.txt
#
#   HAN-FUN Library Tests CMakeList.
#
#   @version 1.5.0
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

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-c++14-compat" )

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

# Add existing tests executable to test list.
foreach(_test ${HF_TESTS})
   add_test (NAME "${_test}" COMMAND ${_test})
 endforeach()

macro(_add_test _name)
   string (TOLOWER "${_name}" __name)

   # Construct source file.
   if(__name MATCHES "^(.+/)(.+)")
     set(_target_srcs "${CMAKE_MATCH_1}test_${CMAKE_MATCH_2}.cpp")
   else()
     set(_target_srcs "test_${__name}.cpp")
   endif()

   # Executable name.
   string (REGEX REPLACE "/" "_" _target ${__name})
   set(_target "test_${_target}")

   add_executable (${_target} ${_target_srcs} $<TARGET_OBJECTS:tests_common>)
   target_link_libraries(${_target} hanfun ${CppUTest_LIBRARIES})
   if (Backtrace_FOUND)
     target_link_libraries(${_target} ${Backtrace_LIBRARIES} )
   endif()

   add_test (NAME "${_name}" COMMAND ${_target})
   list(APPEND HF_TESTS ${_target})

   # Cleanup
   unset(_name)
   unset(_target)
   unset(_target_srcs)
   unset(__name)
endmacro()

## Common ##
set(TESTS_HELPER_SRCS test_helper.cpp)
add_library(tests_common OBJECT ${TESTS_HELPER_SRCS})

_add_test("Common")
_add_test("Protocol")

## Interfaces ##
_add_test("Interfaces")
_add_test("Interfaces/Alert")
_add_test("Interfaces/Colour_Control")
_add_test("Interfaces/Level_Control")
_add_test("Interfaces/On_Off")
_add_test("Interfaces/Simple_Air_Pressure")
_add_test("Interfaces/Simple_Button")
_add_test("Interfaces/Simple_Humidity")
_add_test("Interfaces/Simple_Keypad")
_add_test("Interfaces/Simple_Light_Sensor")
_add_test("Interfaces/Simple_Power_meter")
_add_test("Interfaces/Simple_Temperature")
_add_test("Interfaces/Simple_Thermostat")
_add_test("Interfaces/Simple_Visual_Effects")

## Profiles ##
_add_test("Profiles")

## Devices ##
_add_test("Devices")

## Core Services & Interfaces ##
_add_test("Core")
_add_test("Core/Attribute_Reporting")
_add_test("Core/Batch_Program_Management")
_add_test("Core/Bind_Management")
_add_test("Core/Device_Information")
_add_test("Core/Device_Management")
_add_test("Core/Event_Scheduling")
_add_test("Core/Group_Management")
_add_test("Core/Group_Table")
_add_test("Core/RSSI")
_add_test("Core/SUOTA")
_add_test("Core/Scheduling")
_add_test("Core/Session_Management")
_add_test("Core/Time")
_add_test("Core/Weekly_Scheduling")

## Transport Layer ##
_add_test("Transport")

## Unit ##
_add_test("Units")

# =======================
# Check & Coverage Target
# =======================

if(NOT CMAKE_CROSSCOMPILING)
  set (ARGS --output-on-failure -j "\$(CTEST_ARGS)")

  add_custom_target (check ALL COMMAND ${CMAKE_CTEST_COMMAND} ${ARGS}
                               DEPENDS ${HF_TESTS} VERBATIM)

  if (HF_CODE_COVERAGE)
    generate_code_coverage_report(${HF_TESTS})
  endif()
endif()
