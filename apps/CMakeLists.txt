# =============================================================================
#
#   \file apps/CMakeLists.txt
#
#   HAN-FUN Library Examples CMakeList
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version 0.3.0
#
#   Copyright (c) 2014 Bithium S.A.
#
# =============================================================================

configure_file(application.h ${CMAKE_BINARY_DIR}/hanfun/application.h COPYONLY)

# =============================================================================
# Libuv
# =============================================================================

include(ExternalProject)

set (LIBUV_PREFIX      "${CMAKE_CURRENT_BINARY_DIR}/libuv")
set (LIBUV_INSTALL_DIR "${LIBUV_PREFIX}/src/libuv")

find_program(MAKE_BIN "make")

ExternalProject_Add(libuv
    PREFIX ${LIBUV_PREFIX}
    URL "http://libuv.org/dist/v0.10.27/libuv-v0.10.27.tar.gz"
    URL_HASH MD5=8bc14dc254363092bd36919ea05cc9ab
    CONFIGURE_COMMAND ""
    BUILD_COMMAND CC=${CMAKE_C_COMPILER} ${MAKE_BIN}
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
)

set (LIBUV_LIBRARY "${LIBUV_PREFIX}/src/libuv/libuv.a")
set (LIBUV_LIBRARIES ${LIBUV_LIBRARY} -lrt -ldl -lm -pthread)

include_directories( ${LIBUV_INSTALL_DIR}/include )

if (HF_APP_EXT_REG)
    add_definitions( -DHF_APP_EXT_REG )
endif()

# =============================================================================
# Base Application Example
# =============================================================================

set(BASE_APP_SRCS   "base.h" "common.cpp" "base.cpp" "base_app.cpp")

add_library(hf_base_app ${BASE_APP_SRCS})
target_link_libraries(hf_base_app hanfun)

add_executable(base "main.cpp" "transport.cpp")
add_dependencies(base libuv)
set_target_properties(base PROPERTIES COMPILE_DEFINITIONS "HF_BASE_APP")
target_link_libraries(base hf_base_app hanfun  ${LIBUV_LIBRARIES} )

# =============================================================================
# Node Applications Example
# =============================================================================

set(NODE_APP_SRCS   "node.h" "common.cpp" "node.cpp" "node_app.cpp")

add_library( hf_node_app ${NODE_APP_SRCS})
target_link_libraries(hf_node_app hanfun)

add_executable(node "main.cpp" "transport.cpp")
add_dependencies(node libuv)
set_target_properties(node PROPERTIES COMPILE_DEFINITIONS "HF_NODE_APP;HF_APP_SIMPLE_LIGHT")
target_link_libraries(node hf_node_app hanfun ${LIBUV_LIBRARIES})
