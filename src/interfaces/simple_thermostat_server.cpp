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
   HF::Attributes::UIDS result({SUPPORTED_MODES_ATTR, HEAT_COOL_MODE_ATTR});

   if (pack_id == HF::Attributes::ALL)
   {
#if HF_ITF_STS_FAN_MODE
      result.push_back(FAN_MODE_ATTR);
#endif
#if HF_ITF_STS_HEAT_MODE
      result.push_back(HEAT_MODE_TEMP_ATTR);
#endif
#if HF_ITF_STS_COOL_MODE
      result.push_back(COOL_MODE_TEMP_ATTR);
#endif
   }

   return std::move(result);
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

// =============================================================================
// Server::mode
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::mode() const
{
   return _mode;
}

// =============================================================================
// Server::mode
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::mode(uint8_t __mode)
{
   SETTER_HELPER(HeatCoolMode, _mode, __mode);
}

#if HF_ITF_STS_FAN_MODE
// =============================================================================
// Server::fan_mode
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::fan_mode() const
{
   return _fan_mode;
}

// =============================================================================
// Server::fan_mode
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::fan_mode(uint8_t __fan_mode)
{
   SETTER_HELPER(FanMode, _fan_mode, __fan_mode);
}
#endif

#if HF_ITF_STS_HEAT_MODE

// =============================================================================
// Server::heat_mode_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
int16_t Server::heat_mode_temperature() const
{
   return _heat_mode_temperature;
}

// =============================================================================
// Server::heat_mode_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::heat_mode_temperature(int16_t __temperature)
{
   SETTER_HELPER(HeatModeTemperature, _heat_mode_temperature, __temperature);
}
#endif

#if HF_ITF_STS_COOL_MODE
// =============================================================================
// Server::cool_mode_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
int16_t Server::cool_mode_temperature() const
{
   return _cool_mode_temperature;
}

// =============================================================================
// Server::cool_mode_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::cool_mode_temperature(int16_t __temperature)
{
   SETTER_HELPER(HeatModeTemperature, _cool_mode_temperature, __temperature);
}
#endif
