# =============================================================================
#
#   \file apps/CMakeLists.txt
#
#   HAN-FUN Library Examples CMakeList
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version x.x.x
#
#   Copyright (c) 2014 Bithium S.A.
#
# =============================================================================

add_executable( app_hanfun "hanfun.cpp")
target_link_libraries(app_hanfun hanfun)
