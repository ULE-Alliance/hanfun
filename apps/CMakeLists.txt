# =============================================================================
#
#   \file apps/CMakeLists.txt
#
#   HAN-FUN Library Examples CMakeList
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version 0.1.0
#
#   Copyright (c) 2014 Bithium S.A.
#
# =============================================================================

set(BASE_APP_SRCS   "base.h" "base.cpp" "base_app.cpp")

configure_file(application.h ${CMAKE_BINARY_DIR}/hanfun/application.h COPYONLY)

add_library( hf_base_app ${BASE_APP_SRCS})
target_link_libraries(hf_base_app hanfun)
