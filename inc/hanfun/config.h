#ifndef HF_CONFIG_H
#define HF_CONFIG_H

// =============================================================================
// Common
// =============================================================================

#define OFF   0
#define ON   1

// =============================================================================
// SimplePowerMeter Configuration
// =============================================================================

// In testing mode support all attributes.

#define HF_ITF_SPM_ENERGY_ATTR               OFF
#define HF_ITF_SPM_POWER_ATTR                OFF
#define HF_ITF_SPM_ENERGY_AT_RESET_ATTR      OFF
#define HF_ITF_SPM_TIME_AT_RESET_ATTR        OFF
#define HF_ITF_SPM_AVG_POWER_ATTR            OFF
#define HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR   OFF
#define HF_ITF_SPM_VOLTAGE_ATTR              OFF
#define HF_ITF_SPM_CURRENT_ATTR              OFF
#define HF_ITF_SPM_FREQUENCY_ATTR            OFF
#define HF_ITF_SPM_POWER_FACTOR_ATTR         OFF
#define HF_ITF_SPM_REPORT_INTERVAL_ATTR      OFF

// In testing mode support all commands.

#define HF_ITF_SPM_REPORT_CMD                OFF
#define HF_ITF_SPM_RESET_CMD                 OFF

// =============================================================================

#define HF_PROTOCOL_FILTER_REPEATED_MAX_SIZE    10

#endif  /* HF_CONFIG_H */
