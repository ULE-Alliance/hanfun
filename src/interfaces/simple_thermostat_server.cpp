// =============================================================================
/*!
 * @file       src/interfaces/simple_thermostat_server.cpp
 *
 * This file contains the implementation of the On-Off interface : Server role.
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
// Defines
// =============================================================================

#define SETTER_HELPER(_Type, _name, _value) \
   {                                        \
      int16_t old = this->_name;            \
                                            \
      this->_name = _value;                 \
                                            \
      _Type old_attr(old, this);            \
      _Type new_attr(this->_name, this);    \
                                            \
      notify(old_attr, new_attr);           \
   }

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleThermostat;

// =============================================================================
// Simple Thermostat Interface : Server Role
// =============================================================================

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::UIDS Server::attributes(uint8_t pack_id) const
{
   UNUSED (pack_id);

   return HF::Attributes::UIDS({SUPPORTED_MODES_ATTR});
}

// =============================================================================

// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::supported_modes
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::supported_modes() const
{
   return _supported_modes;
}

// =============================================================================
// Server::supported_modes
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::supported_modes(uint8_t __supported_modes)
{
   SETTER_HELPER(SupportedModes, _supported_modes, __supported_modes);
}
