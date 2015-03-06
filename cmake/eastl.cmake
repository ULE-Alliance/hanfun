# =============================================================================
#
#   @file cmake/eastl.cmake
#
#   HAN-FUN Library - EASTL () library support
#
#   @version    x.x.x
#
#   @copyright  Copyright (c) 2015  ULE Alliance
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
#
#   Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

if(NOT HF_BUILD_EASTL)
  return ()
endif()

include(ExternalProject)

set (EASTL_PREFIX "${CMAKE_BINARY_DIR}/EASTL")

find_program(MAKE_BIN "make")

ExternalProject_Add(eastl
    PREFIX ${EASTL_PREFIX}
    GIT_REPOSITORY "https://github.com/ULE-Alliance/EASTL.git"
    GIT_TAG "community"
    INSTALL_COMMAND ""
)

set(EASTL_LIBRARY       "${EASTL_PREFIX}/src/eastl-build/lib/libEASTL.a")
set(EASTL_INCLUDE_DIRS  "${EASTL_PREFIX}/src/eastl/include/")

include_directories(BEFORE ${EASTL_INCLUDE_DIRS})
add_definitions(-DHF_USE_EASTL)
