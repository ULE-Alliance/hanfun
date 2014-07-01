# =============================================================================
#
#   \file cmake/options.cmake
#
#   HAN-FUN Library Options.
#
#   This file contains the configuration options for the HAN-FUN library.
#
#   \author  Filipe Alves <filipe.alves@bithium.com>
#
#   \version x.x.x
#
#   Copyright (c) 2012 Bithium S.A.
#
# =============================================================================

# =============================================================================
# Project Configuration
# =============================================================================

option(HF_BUILD_APPS "Build example applications" OFF)

# =============================================================================
# SimplePowerMeter Configuration
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
