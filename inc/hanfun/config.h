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

#define HF_ITF_SPM_ENERGY_ATTR               ON
#define HF_ITF_SPM_POWER_ATTR                ON
#define HF_ITF_SPM_ENERGY_AT_RESET_ATTR      ON
#define HF_ITF_SPM_TIME_AT_RESET_ATTR        ON
#define HF_ITF_SPM_AVG_POWER_ATTR            ON
#define HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR   ON
#define HF_ITF_SPM_VOLTAGE_ATTR              ON
#define HF_ITF_SPM_CURRENT_ATTR              ON
#define HF_ITF_SPM_FREQUENCY_ATTR            ON
#define HF_ITF_SPM_POWER_FACTOR_ATTR         ON
#define HF_ITF_SPM_REPORT_INTERVAL_ATTR      ON

// In testing mode support all commands.

#define HF_ITF_SPM_REPORT_CMD                ON
#define HF_ITF_SPM_RESET_CMD                 ON

// =============================================================================

#define HF_PROTOCOL_FILTER_REPEATED_MAX_SIZE    5

#endif  /* HF_CONFIG_H */
