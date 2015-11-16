# =============================================================================
#
#   @file src/CMakeLists.txt
#
#   HAN-FUN Library CMakeList.
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

# ==============================================================================
# Build
# ==============================================================================

## Common ##
set(HF_COMMON_SRCS common.cpp attributes.cpp devices.cpp profiles.cpp uids.cpp debug.cpp)

## Protocol ##
set(HF_PROTOCOL_SRCS protocol.cpp)

## Interfaces ##
set(HF_INTERFACES_SRCS interfaces.cpp)

macro(add_interface __name)
   if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interfaces/${__name}.cpp")
     list(APPEND HF_INTERFACES_SRCS "interfaces/${__name}.cpp")
   endif ()
   list(APPEND HF_INTERFACES_SRCS "interfaces/${__name}_server.cpp"
                                  "interfaces/${__name}_client.cpp")
endmacro()

add_interface("alert")
add_interface("on_off")
add_interface("level_control")
add_interface("simple_power_meter")
add_interface("simple_temperature")
add_interface("simple_humidity")
add_interface("simple_thermostat")

## Core Services & Interfaces
set(HF_CORE_SRCS       core/device_management.cpp
                       core/device_management_client.cpp
                       core/device_management_server.cpp
                       core/bind_management.cpp
                       core/bind_management_client.cpp
                       core/bind_management_server.cpp
                       core/device_information.cpp
                       core/attribute_reporting.cpp
                       core/attribute_reporting_server.cpp
                       core/attribute_reporting_client.cpp
                       core/session_management_server.cpp
                       core/session_management_client.cpp)

## Units ##
set(HF_UNITS_SRCS units.cpp)

## Library ##
add_library(hanfun  ${HF_COMMON_SRCS} ${HF_PROTOCOL_SRCS}
                    ${HF_INTERFACES_SRCS} ${HF_CORE_SRCS}
                    ${HF_UNITS_SRCS})

install(TARGETS hanfun EXPORT ${PACKAGE_NAME}-targets
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev)

set(HF_TARGETS ${HF_TARGETS} hanfun PARENT_SCOPE)
