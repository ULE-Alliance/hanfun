// =============================================================================
/*!
 * @file       inc/hanfun.h
 *
 * This is the top level include file for the HAN-FUN library.
 *
 * @version    1.4.1
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#ifndef HANFUN_H
#define HANFUN_H

// =============================================================================
// Configuration
// =============================================================================

#include "hanfun/version.h"
#include "hanfun/config.h"

// =============================================================================
// Common
// =============================================================================

#include "hanfun/common.h"
#include "hanfun/uids.h"

// =============================================================================
// Protocol
// =============================================================================

#include "hanfun/protocol.h"

// =============================================================================
// Interfaces
// =============================================================================

#include "hanfun/interface.h"
#include "hanfun/interfaces/alert.h"
#include "hanfun/interfaces/on_off.h"
#include "hanfun/interfaces/level_control.h"
#include "hanfun/interfaces/simple_power_meter.h"
#include "hanfun/interfaces/simple_thermostat.h"
#include "hanfun/interfaces/simple_button.h"
#include "hanfun/interfaces/simple_visual_effects.h"
#include "hanfun/interfaces/simple_air_pressure.h"

// =============================================================================
// Core Services & Interfaces.
// =============================================================================

#include "hanfun/core.h"
#include "hanfun/core/device_management.h"
#include "hanfun/core/device_information.h"
#include "hanfun/core/attribute_reporting.h"
#include "hanfun/core/bind_management.h"
#include "hanfun/core/rssi.h"
#include "hanfun/core/suota.h"

// =============================================================================
// Profiles
// =============================================================================

#include "hanfun/profiles.h"

// =============================================================================
// Units
// =============================================================================

#include "hanfun/units.h"

// =============================================================================
// Device
// =============================================================================

#include "hanfun/devices.h"

#endif /* HANFUN_H */
