# =============================================================================
#
#   @file cmake/compiler.cmake
#
#   HAN-FUN Library code coverage compiler and enviroment checks and configuration.
#
#   @version    1.5.0
#
#   @copyright  Copyright (c) 2016  ULE Alliance
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
#
#   Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

if (NOT HF_CODE_COVERAGE)
  return()
endif()

if(NOT CMAKE_COMPILER_IS_GNUCXX)
    # Clang version 3.0.0 and greater now supports gcov as well.
    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND
        CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.0.0)
        message(WARNING "Compiler is not GNU gcc or Clang Version >= 3.0.0 !")
        set (HF_CODE_COVERAGE OFF)
    endif()
endif()

find_program(GCOV_PATH gcov)
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)
find_program(FIND_PATH find)
find_program(GIT_PATH git)
find_program(CPPFILT_PATH c++filt)

if(NOT GCOV_PATH)
    message(WARNING "gcov not found!")
    set (HF_CODE_COVERAGE OFF)
endif()

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message( WARNING "Code coverage results with an optimized (non-Debug) build may be misleading" )
endif()

if(NOT HF_CODE_COVERAGE)
  message(WARNING "Code coverage NOT enabled !")
  return ()
endif()

set(CMAKE_COVERAGE_FLAGS "-g --coverage -fprofile-arcs -ftest-coverage")
mark_as_advanced(CMAKE_COVERAGE_FLAGS)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_COVERAGE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_COVERAGE_FLAGS}")

function(generate_code_coverage_report _targets)
  if(NOT HF_CODE_COVERAGE)
    return()
  endif()

  if(NOT LCOV_PATH)
    message(WARNING "lcov NOT FOUND ! Skipping coverage report ...")
    return()
  endif()

  if(NOT GENHTML_PATH)
    message(WARNING "genhtml NOT FOUND ! Skipping coverage report ...")
    return()
  endif()

  if(NOT FIND_PATH)
    message(WARNING "find NOT FOUND ! Skipping coverage report ...")
    return()
  endif()

  if(NOT GIT_PATH)
    message(WARNING "git NOT FOUND ! Skipping coverage report ...")
    return()
  endif()

  set(LCOV_ARGS_COMMON --rc lcov_branch_coverage=1 --directory .)

  set(GENHTML_ARGS --rc lcov_branch_coverage=1 -f -s --legend --ignore-errors source)
  if(CPPFILT_PATH)
    list(APPEND GENHTML_ARGS --demangle-cpp)
  endif()

  add_custom_target(coverage_prepare
                    COMMAND ${LCOV_PATH} ${LCOV_ARGS_COMMON} --zerocounters
                    COMMAND ${LCOV_PATH} ${LCOV_ARGS_COMMON} --capture --initial --output-file hanfun.info
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                    DEPENDS ${_targets}
                    COMMENT "Resetting code coverage counters to zero.")

  execute_process(COMMAND ${GIT_PATH} describe OUTPUT_VARIABLE GIT_DESCRIBE
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

  add_custom_target(coverage_report
                    COMMAND ${LCOV_PATH} ${LCOV_ARGS_COMMON} --capture --output-file hanfun.info
                    COMMAND ${LCOV_PATH} ${LCOV_ARGS_COMMON} --remove hanfun.info '*/cpputest/*' '/usr/*' --output-file hanfun.info
                    COMMAND ${GENHTML_PATH} ${GENHTML_ARGS} --title "HAN-FUN ${GIT_DESCRIBE}" -o coverage hanfun.info
                    COMMAND ${CMAKE_COMMAND} -E remove hanfun.info
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                    DEPENDS check
                    COMMENT "Processing code coverage counters and generating report.")

    # Show info where to find the report
    add_custom_target(coverage
        COMMAND ;
        DEPENDS coverage_prepare check coverage_report
        COMMENT "Open ${CMAKE_BINARY_DIR}/coverage/index.html in your browser to view the coverage report.")

endfunction()
