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
set(HF_INTERFACES_SRCS interfaces/alert.cpp
                       interfaces/alert_server.cpp
                       interfaces/alert_client.cpp
                       interfaces/on_off_server.cpp
                       interfaces/on_off_client.cpp
                       interfaces/level_control_server.cpp
                       interfaces/level_control_client.cpp
                       interfaces/simple_power_meter.cpp
                       interfaces/simple_power_meter_server.cpp
                       interfaces/simple_power_meter_client.cpp )

set(HF_CORE_SRCS       core.cpp
                       core/device_management.cpp
                       core/device_management_client.cpp
                       core/device_management_server.cpp )

add_library( hanfun  ${HF_COMMON_SRCS} ${HF_PROTOCOL_SRCS}
                     ${HF_INTERFACES_SRCS} ${HF_CORE_SRCS} )
