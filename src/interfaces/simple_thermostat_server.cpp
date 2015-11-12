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
#if HF_ITF_STS_AUTO_MODE
      result.push_back(AUTO_MODE_HEAT_TEMP_ATTR);
      result.push_back(AUTO_MODE_COOL_TEMP_ATTR);
#endif
#if HF_ITF_STS_HEAT_MODE && HF_ITF_STS_HEAT_OFFSET_ATTR
      result.push_back(HEAT_MODE_TEMP_OFFSET_ATTR);
#endif
#if HF_ITF_STS_COOL_MODE && HF_ITF_STS_COOL_OFFSET_ATTR
      result.push_back(COOL_MODE_TEMP_OFFSET_ATTR);
#endif
#if HF_ITF_STS_BOOST_CMD
      result.push_back(BOOST_DURATION_ATTR);
#endif
   }

   return std::move(result);
}

#if HF_ITF_STS_BOOST_CMD
// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd               = static_cast<CMD>(packet.message.itf.member);

   Common::Result result = Common::Result::FAIL_SUPPORT;

   if (!packet.message.isCommand())
   {
      return Common::Result::FAIL_SUPPORT;
   }

   switch (cmd)
   {
      case BOOST_START_CMD:
         result = boost_start(packet.source);
         break;
      case BOOST_STOP_CMD:
         result = boost_stop(packet.source);
         break;
      default:
         return Common::Result::FAIL_ARG;
   }

   Protocol::Response resp(result);

   Protocol::Message response(packet.message, resp.size());

   response.itf.role   = SERVER_ROLE;
   response.itf.id     = Server::uid();
   response.itf.member = cmd;

   resp.pack(response.payload);

   send(packet.source, response);

   return Common::OK;
}
#endif

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

#if HF_ITF_STS_AUTO_MODE
// =============================================================================
// Server::auto_mode_heat_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
int16_t Server::auto_mode_heat_temperature() const
{
   return _auto_mode_heat_temperature;
}

// =============================================================================
// Server::auto_mode_heat_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::auto_mode_heat_temperature(int16_t __temperature)
{
   SETTER_HELPER(AutoModeHeatTemperature, _auto_mode_heat_temperature, __temperature);
}

// =============================================================================
// Server::auto_mode_heat_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
int16_t Server::auto_mode_cool_temperature() const
{
   return _auto_mode_cool_temperature;
}

// =============================================================================
// Server::auto_mode_cool_temperature
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::auto_mode_cool_temperature(int16_t __temperature)
{
   SETTER_HELPER(AutoModeCoolTemperature, _auto_mode_cool_temperature, __temperature);
}
#endif

#if HF_ITF_STS_HEAT_MODE && HF_ITF_STS_HEAT_OFFSET_ATTR
// =============================================================================
// Server::heat_mode_temperature_offset
// =============================================================================
/*!
 *
 */
// =============================================================================
int16_t Server::heat_mode_temperature_offset() const
{
   return _heat_mode_temperature_offset;
}

// =============================================================================
// Server::heat_mode_temperature_offset
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::heat_mode_temperature_offset(int16_t __offset)
{
   SETTER_HELPER(HeatModeTemperatureOffset, _heat_mode_temperature_offset, __offset);
}
#endif

#if HF_ITF_STS_COOL_MODE && HF_ITF_STS_COOL_OFFSET_ATTR
// =============================================================================
// Server::cool_mode_temperature_offset
// =============================================================================
/*!
 *
 */
// =============================================================================
int16_t Server::cool_mode_temperature_offset() const
{
   return _cool_mode_temperature_offset;
}

// =============================================================================
// Server::cool_mode_temperature_offset
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::cool_mode_temperature_offset(int16_t __offset)
{
   SETTER_HELPER(CoolModeTemperatureOffset, _cool_mode_temperature_offset, __offset);
}
#endif

#if HF_ITF_STS_BOOST_CMD
// =============================================================================
// Server::boost_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t Server::boost_duration() const
{
   return _boost_duration;
}

// =============================================================================
// Server::boost_duration
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::boost_duration(uint8_t __duration)
{
   SETTER_HELPER(BoostDuration, _boost_duration, __duration);
}
#endif
