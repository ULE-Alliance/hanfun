# =============================================================================
#
#   \file src/CMakeLists.txt
#
#   HAN-FUN Library CMakeList.
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

include_directories( "${PROJECT_INCLUDE_DIR}/hanfun" )

# ==============================================================================
# Build
# ==============================================================================

set(HF_COMMON_SRCS common.cpp)
set(HF_PROTOCOL_SRCS protocol.cpp)

add_library( hanfun  ${HF_COMMON_SRCS} ${HF_PROTOCOL_SRCS} )
