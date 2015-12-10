# =============================================================================
#
#   @file cmake/options.cmake
#
#   HAN-FUN Library Options.
#
#   This file contains the configuration options for the HAN-FUN library.
#
#   @version    1.3.0
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

option(HF_BUILD_TESTS "Build project's unit tests.")
option(HF_SHARED_SUPPORT "Build HAN-FUN so that it can be compilied into a shared library." NO)

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
