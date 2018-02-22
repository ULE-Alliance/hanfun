# =============================================================================
#
#   @file cmake/options.cmake
#
#   HAN-FUN Library Options.
#
#   This file contains the configuration options for the HAN-FUN library.
#
#   @version    1.5.1
#
#   @copyright  Copyright (c) 2014  ULE Alliance
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
#
#   Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

# =============================================================================
# Project Configuration
# =============================================================================

include(CMakeDependentOption)

option(HF_BUILD_TESTS "Build project's unit tests." NO)
option(HF_SHARED_SUPPORT "Build HAN-FUN so that it can be compilied into a shared library." NO)
option(HF_CODE_COVERAGE "Build HAN-FUN with code coverage support" NO)

#
# Package configuration options.
#

set(PACKAGE_NAME "hanfun")

# Offer the user the choice of overriding the installation directories
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for runtime binaries")

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for headers")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
  set(DEF_INSTALL_DATA_DIR data)
else()
  set(DEF_INSTALL_DATA_DIR  "share/${PACKAGE_NAME}")
  set(DEF_INSTALL_CMAKE_DIR "share/${PACKAGE_NAME}/cmake")
endif()

set(INSTALL_DATA_DIR ${DEF_INSTALL_DATA_DIR}
    CACHE PATH "Installation directory for data files")
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR}
    CACHE PATH "Installation directory for CMake files")

# =============================================================================
# Example Application Configuration
# =============================================================================

option (HF_BASE_APP "Build base example application." OFF)
option (HF_NODE_APP "Build node example application." OFF)

option (HF_BASE_LIB "Build base example application library." OFF)
option (HF_NODE_LIB "Build node example application library." OFF)

if (HF_BASE_APP OR BUILD_TESTS)
    set(HF_BASE_LIB ON CACHE BOOL "Build base example application library." FORCE)
endif()

if (HF_NODE_APP OR BUILD_TESTS)
    set(HF_NODE_LIB ON CACHE BOOL "Build node example application library." FORCE)
endif()

option(HF_APP_EXT_REG "Build example application with external registration support." OFF)

# =============================================================================
# Library Configuration
# =============================================================================

option(HF_GROUP_SUPPORT "General - Group Support")

# =============================================================================
# Core Services & Interfaces Configuration
# =============================================================================

# =============================================================================
# Device Information Service
# =============================================================================

option(HF_CORE_DEV_INFO_APP_VERSION_ATTR        "Service - Device Information - Application Version Attribute Support")
option(HF_CORE_DEV_INFO_HW_VERSION_ATTR         "Service - Device Information - Hardware Version Attribute Support")
option(HF_CORE_DEV_INFO_MANUFACTURER_NAME_ATTR  "Service - Device Information - Manufacture Attribute Support")

# =============================================================================
# SUOTA Configuration
# =============================================================================

option(HF_CORE_SUOTA_SUPPORT "Service - SUOTA support")
cmake_dependent_option(HF_CORE_SUOTA_CHECK_VERSION_CMD "Service - SUOTA - Check Version"
                       OFF "HF_CORE_SUOTA_SUPPORT" OFF)

# =============================================================================
# Group Management Configuration
# =============================================================================

option(HF_CORE_GROUP_MANAGEMENT_GET_INFO_CMD          "Service - Group Management - Get Info")

# =============================================================================
# Batch Program Management Interface Configuration
# =============================================================================

option(HF_CORE_BATCH_PROGRAM_MANAGEMENT_GET_PROGRAM_ACTIONS_CMD        "Interface - Batch Program Management Interface - Get Program Actions")

# =============================================================================
# Event Scheduling Configuration
# =============================================================================

option(HF_CORE_EVENT_SCHEDULING_UPDATE_EVENT_STATUS_CMD        "Service - Event Scheduling - Update Event Status")
option(HF_CORE_EVENT_SCHEDULING_GET_EVENT_ENTRY_CMD            "Service - Event Scheduling - Get Event Entry")
option(HF_CORE_EVENT_SCHEDULING_DELETE_ALL_EVENTS_CMD          "Service - Event Scheduling - Delete All Events")

# =============================================================================
# Weekly Scheduling Configuration
# =============================================================================

option(HF_CORE_WEEKLY_SCHEDULING_UPDATE_EVENT_STATUS_CMD        "Service - Weekly Scheduling - Update Event Status")
option(HF_CORE_WEEKLY_SCHEDULING_GET_EVENT_ENTRY_CMD            "Service - Weekly Scheduling - Get Event Entry")
option(HF_CORE_WEEKLY_SCHEDULING_DELETE_ALL_EVENTS_CMD          "Service - Weekly Scheduling - Delete All Events")

# =============================================================================
# Interfaces Configuration
# =============================================================================

# =============================================================================
# Simple Power Meter Configuration
# =============================================================================

option(HF_ITF_SPM_ENERGY_ATTR              "Interface - Simple Power Meter - Energy Attribute Support")
option(HF_ITF_SPM_POWER_ATTR               "Interface - Simple Power Meter - Power Attribute Support")
option(HF_ITF_SPM_ENERGY_AT_RESET_ATTR     "Interface - Simple Power Meter - Energy at Last Reset Attribute Support")
option(HF_ITF_SPM_TIME_AT_RESET_ATTR       "Interface - Simple Power Meter - Time at Last Reset Attribute Support")
option(HF_ITF_SPM_AVG_POWER_ATTR           "Interface - Simple Power Meter - Average Power Attribute Support")
option(HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR  "Interface - Simple Power Meter - Average Power Interval Attribute Support")
option(HF_ITF_SPM_VOLTAGE_ATTR             "Interface - Simple Power Meter - Voltage Attribute Support")
option(HF_ITF_SPM_CURRENT_ATTR             "Interface - Simple Power Meter - Current Attribute Support")
option(HF_ITF_SPM_FREQUENCY_ATTR           "Interface - Simple Power Meter - Frequency Attribute Support")
option(HF_ITF_SPM_POWER_FACTOR_ATTR        "Interface - Simple Power Meter - Power Factor Attribute Support")
option(HF_ITF_SPM_REPORT_INTERVAL_ATTR     "Interface - Simple Power Meter - Report Interval Attribute Support")

option(HF_ITF_SPM_REPORT_CMD               "Interface - Simple Power Meter - Periodic Report Command Support")
option(HF_ITF_SPM_RESET_CMD                "Interface - Simple Power Meter - Reading Values Reset Command Support")

# =============================================================================
# Simple Thermostat Configuration
# =============================================================================

option(HF_ITF_STS_FAN_MODE      "Interface - Simple Thermostat - Fan Mode Support")
option(HF_ITF_STS_HEAT_MODE     "Interface - Simple Thermostat - Heat Mode Support")
option(HF_ITF_STS_COOL_MODE     "Interface - Simple Thermostat - Cool Mode Support")
option(HF_ITF_STS_AUTO_MODE     "Interface - Simple Thermostat - Heat/Cool Mode Support")

cmake_dependent_option(HF_ITF_STS_HEAT_OFFSET_ATTR
                       "Interface - Simple Thermostat - Heat Mode Temperature Offset Attribute Support"
                       OFF
                       "HF_ITF_STS_HEAT_MODE"
                       OFF)

cmake_dependent_option(HF_ITF_STS_COOL_OFFSET_ATTR
                       "Interface - Simple Thermostat - Cool Mode Temperature Offset Attribute Support"
                       OFF
                       "HF_ITF_STS_COOL_MODE"
                       OFF)

option(HF_ITF_STS_BOOST_CMD "Interface - Simple Thermostat - Boost Command Support")

# =============================================================================
# Simple Button Configuration
# =============================================================================

option(HF_ITF_SIMPLE_BUTTON_SHORT_PRESS_CMD        "Interface - Simple Button - Short Press")
option(HF_ITF_SIMPLE_BUTTON_LONG_PRESS_CMD         "Interface - Simple Button - Long Press")
option(HF_ITF_SIMPLE_BUTTON_EXTRA_LONG_PRESS_CMD   "Interface - Simple Button - Extra Long Press")
option(HF_ITF_SIMPLE_BUTTON_DOUBLE_CLICK_PRESS_CMD "Interface - Simple Button - Double Click Press")

# =============================================================================
# Simple Visual Effects Configuration
# =============================================================================

option(HF_ITF_SIMPLE_VISUAL_EFFECTS_ON_CMD      "Interface - Simple Visual Effects - On")
option(HF_ITF_SIMPLE_VISUAL_EFFECTS_OFF_CMD     "Interface - Simple Visual Effects - Off")
option(HF_ITF_SIMPLE_VISUAL_EFFECTS_BLINK_CMD   "Interface - Simple Visual Effects - Blink")
option(HF_ITF_SIMPLE_VISUAL_EFFECTS_FADE_CMD    "Interface - Simple Visual Effects - Fade")
option(HF_ITF_SIMPLE_VISUAL_EFFECTS_BREATHE_CMD "Interface - Simple Visual Effects - Breathe")

# =============================================================================
# Level Control Configuration
# =============================================================================

option(HF_ITF_LEVEL_CONTROL_SET_LEVEL_CMD       "Interface - Level Control - Set Level")
option(HF_ITF_LEVEL_CONTROL_INCREASE_LEVEL_CMD  "Interface - Level Control - Increase Level")
option(HF_ITF_LEVEL_CONTROL_DECREASE_LEVEL_CMD  "Interface - Level Control - Decrease Level")

# =============================================================================
# Colour Control Configuration
# =============================================================================

option(HF_ITF_COLOUR_CONTROL_STOP_CMD                       "Interface - Colour Control - Stop")

option(HF_ITF_COLOUR_CONTROL_HUE_AND_SATURATION_ATTR        "Interface - Colour Control - Hue And Saturation")
option(HF_ITF_COLOUR_CONTROL_XY_ATTR                        "Interface - Colour Control - XY")
option(HF_ITF_COLOUR_CONTROL_COLOUR_TEMPERATURE_ATTR         "Interface - Colour Control - Colour Temperature")

# =============================================================================
# Dependecies
# =============================================================================

if(HF_BUILD_TESTS)
    set(HF_CORE_SUOTA_SUPPORT ON)
endif()

## SUOTA Service
if(HF_CORE_SUOTA_SUPPORT)
    set(HF_CORE_DEV_INFO_APP_VERSION_ATTR        ON)
    set(HF_CORE_DEV_INFO_HW_VERSION_ATTR         ON)
    set(HF_CORE_DEV_INFO_MANUFACTURER_NAME_ATTR  ON)
endif()
