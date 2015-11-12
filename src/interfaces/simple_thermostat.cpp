// =============================================================================
/*!
 * @file       src/interfaces/simple_thermostat.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Thermostat interface.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/simple_thermostat.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleThermostat;

// =============================================================================
// SimpleThermostat::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimpleThermostat::create_attribute(uint8_t uid)
{
   return Interfaces::create_attribute((SimpleThermostat::Server *) nullptr, uid);
}
