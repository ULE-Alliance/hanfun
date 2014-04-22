# =============================================================================
#
#   \file src/CMakeLists.txt
#
#   HAN-FUN Library CMakeList.
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version 0.2.0
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

set(HF_COMMON_SRCS common.cpp attributes.cpp devices.cpp)

set(HF_PROTOCOL_SRCS protocol.cpp)
set(HF_INTERFACES_SRCS interfaces.cpp
                       interfaces/alert.cpp
                       interfaces/alert_server.cpp
                       interfaces/alert_client.cpp
                       interfaces/on_off.cpp
                       interfaces/on_off_server.cpp
                       interfaces/on_off_client.cpp
                       interfaces/level_control.cpp
                       interfaces/level_control_server.cpp
                       interfaces/level_control_client.cpp
                       interfaces/simple_power_meter.cpp
                       interfaces/simple_power_meter_server.cpp
                       interfaces/simple_power_meter_client.cpp )

set(HF_CORE_SRCS       core.cpp
                       core/device_management.cpp
                       core/device_management_client.cpp
                       core/device_management_server.cpp )

set(HF_UNITS_SRCS units.cpp)

add_library( hanfun  ${HF_COMMON_SRCS} ${HF_PROTOCOL_SRCS}
                     ${HF_INTERFACES_SRCS} ${HF_CORE_SRCS}
                     ${HF_UNITS_SRCS} )

install(TARGETS hanfun ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../inc/" DESTINATION include
        PATTERN ".git*" EXCLUDE)

file( GLOB_RECURSE CONFIG_HEADER "${CMAKE_BINARY_DIR}/hanfun/*.h")

install(FILES ${CONFIG_HEADER} DESTINATION include/hanfun)
