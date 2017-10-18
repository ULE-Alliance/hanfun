// =============================================================================
/*!
 * @file       src/attributes.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Attributes API.
 *
 * @version    1.4.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/attributes.h"

#include "hanfun/interfaces/alert.h"
#include "hanfun/interfaces/level_control.h"
#include "hanfun/interfaces/on_off.h"
#include "hanfun/interfaces/simple_power_meter.h"
#include "hanfun/interfaces/simple_temperature.h"
#include "hanfun/interfaces/simple_humidity.h"
#include "hanfun/interfaces/simple_thermostat.h"
#include "hanfun/interfaces/simple_button.h"
#include "hanfun/interfaces/simple_visual_effects.h"
#include "hanfun/interfaces/simple_air_pressure.h"
#include "hanfun/interfaces/simple_light_sensor.h"
#include "hanfun/interfaces/colour_control.h"
#include "hanfun/interfaces/simple_keypad.h"

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"
#include "hanfun/core/bind_management.h"
#include "hanfun/core/attribute_reporting.h"
#include "hanfun/core/group_management.h"
#include "hanfun/core/group_table.h"
#include "hanfun/core/rssi.h"
#include "hanfun/core/suota.h"
#include "hanfun/core/time.h"
#include "hanfun/core/batch_program_management.h"
#include "hanfun/core/event_scheduling.h"
#include "hanfun/core/weekly_scheduling.h"

using namespace HF;
using namespace HF::Attributes;
using namespace HF::Interfaces;
using namespace HF::Core;

// =============================================================================
// Attribute Factories
// =============================================================================

/*!
 * Interface / Attribute Factory database entry.
 */
struct Entry
{
   uint16_t            id;       //!< Interface UID.
   Attributes::Factory factory;  //!< Attribute's factory.
};

/*!
 * Interface / Attribute Factory database.
 */
static const Entry factories[] =
{
   /* Core Services */
   {
      HF::Interface::DEVICE_MANAGEMENT,
      HF::Core::DeviceManagement::create_attribute
   },
   {
      HF::Interface::BIND_MANAGEMENT,
      HF::Core::BindManagement::create_attribute,
   },
   {
      HF::Interface::DEVICE_INFORMATION,
      HF::Core::DeviceInformation::create_attribute
   },
   {
      HF::Interface::ATTRIBUTE_REPORTING,
      HF::Core::AttributeReporting::create_attribute,
   },
   {
      HF::Interface::RSSI,
      HF::Core::RSSI::create_attribute,
   },
#if HF_CORE_SUOTA_SUPPORT
   {
      HF::Interface::SUOTA,
      HF::Core::SUOTA::create_attribute,
   },
#endif
   {
      HF::Interface::TIME,
      HF::Core::Time::create_attribute,
   },
   {
      HF::Interface::GROUP_MANAGEMENT,
      HF::Core::GroupManagement::create_attribute,
   },
   {
      HF::Interface::GROUP_TABLE,
      HF::Core::GroupTable::create_attribute,
   },
   {
      HF::Interface::BATCH_PROGRAM_MANAGEMENT,
      HF::Core::BatchProgramManagement::create_attribute,
   },
   {
      HF::Interface::EVENT_SCHEDULING,
      HF::Core::Scheduling::Event::create_attribute,
   },
   {
      HF::Interface::WEEKLY_SCHEDULING,
      HF::Core::Scheduling::Weekly::create_attribute,
   },
   /* Functional Interfaces. */
   {
      HF::Interface::ALERT,
      HF::Interfaces::Alert::create_attribute,
   },
   {
      HF::Interface::ON_OFF,
      HF::Interfaces::OnOff::create_attribute
   },
   {
      HF::Interface::LEVEL_CONTROL,
      HF::Interfaces::LevelControl::create_attribute
   },
   {
      HF::Interface::SIMPLE_POWER_METER,
      HF::Interfaces::SimplePowerMeter::create_attribute
   },
   {
      HF::Interface::SIMPLE_TEMPERATURE,
      HF::Interfaces::SimpleTemperature::create_attribute
   },
   {
      HF::Interface::SIMPLE_THERMOSTAT,
      HF::Interfaces::SimpleThermostat::create_attribute
   },
   {
      HF::Interface::SIMPLE_BUTTON,
      HF::Interfaces::SimpleButton::create_attribute,
   },
   {
      HF::Interface::SIMPLE_VISUAL_EFFECTS,
      HF::Interfaces::SimpleVisualEffects::create_attribute,
   },
   {
      HF::Interface::SIMPLE_AIR_PRESSURE,
      HF::Interfaces::SimpleAirPressure::create_attribute,
   },
   {
      HF::Interface::SIMPLE_LIGHT_SENSOR,
      HF::Interfaces::SimpleLightSensor::create_attribute,
   },
   {
      HF::Interface::COLOUR_CONTROL,
      HF::Interfaces::ColourControl::create_attribute,
   },
   {
      HF::Interface::SIMPLE_KEYPAD,
      HF::Interfaces::SimpleKeypad::create_attribute,
   },
};

// =============================================================================
// Attributes::get_factory
// =============================================================================
/*!
 *
 */
// =============================================================================
Attributes::Factory Attributes::get_factory(Common::Interface itf)
{
   Attributes::Factory result = nullptr;

   for (uint32_t index = 0; index < (sizeof(factories) / sizeof(*factories)); ++index)
   {
      if (factories[index].id == itf.id)
      {
         result = factories[index].factory;
         break;
      }
   }

   return result;
}

// =============================================================================
// Interfaces
// =============================================================================

// =============================================================================
// HF::Interfaces::create_attribute - Alert
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(Alert::Server *server, uint8_t uid)
{
   using namespace HF::Interfaces::Alert;

   Alert::Attributes attr = static_cast<Alert::Attributes>(uid);

   uint16_t itf_uid       = HF::Interface::ALERT;

   switch (attr)
   {
      case STATE_ATTR:
      {
         bool writabble = State::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (uint32_t (Server::*)(void)) & Server::state;
            auto setter = (void (Server::*)(uint32_t)) & Server::set_state;

            return new Attribute<uint32_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new Attribute<uint32_t>(itf_uid, attr, writabble);
         }
      }

      case ENABLE_ATTR:
      {
         bool writabble = Enable::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (uint32_t (Server::*)(void)) & Server::enabled;
            auto setter = (void (Server::*)(uint32_t)) & Server::set_enabled;

            return new Attribute<uint32_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new Attribute<uint32_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// HF::Interfaces::create_attribute - Level Control
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(LevelControl::Server *server, uint8_t uid)
{
   using namespace HF::Interfaces::LevelControl;

   LevelControl::Attributes attr = static_cast<LevelControl::Attributes>(uid);

   uint16_t itf_uid              = HF::Interface::LEVEL_CONTROL;

   switch (attr)
   {
      case LEVEL_ATTR:
      {
         bool writabble = Level::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (uint8_t (Server::*)(void)) & Server::level;
            auto setter = (void (Server::*)(uint8_t)) & Server::level;

            return new Attribute<uint8_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new Attribute<uint8_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// HF::Interfaces::create_attribute - On-Off
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(OnOff::Server *server, uint8_t uid)
{
   using  namespace HF::Interfaces::OnOff;

   OnOff::Attributes attr = static_cast<OnOff::Attributes>(uid);

   uint16_t itf_uid       = HF::Interface::ON_OFF;

   switch (attr)
   {
      case STATE_ATTR:
      {
         bool writabble = State::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (bool (Server::*)(void)) & Server::state;
            auto setter = (void (Server::*)(bool)) & Server::state;

            return new Attribute<bool, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new Attribute<bool>(itf_uid, attr, false, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Interfaces::create_attribute - Simple Power Meter
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(SimplePowerMeter::Server *server, uint8_t uid)
{
   using namespace HF::Interfaces::SimplePowerMeter;

   SimplePowerMeter::Attributes attr = static_cast<SimplePowerMeter::Attributes>(uid);

   uint16_t itf_uid                  = HF::Interface::SIMPLE_POWER_METER;
   bool writabble                    = false;

   switch (attr)
   {
      case ENERGY_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_ENERGY_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::energy;
            auto setter = (void (Server::*)(Measurement &)) & Server::energy;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case ENERGY_AT_RESET_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_ENERGY_AT_RESET_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::last_energy;
            auto setter = (void (Server::*)(Measurement &)) & Server::last_energy;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case TIME_AT_RESET_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_TIME_AT_RESET_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::last_time;
            auto setter = (void (Server::*)(Measurement &)) & Server::last_time;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case POWER_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_POWER_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::power;
            auto setter = (void (Server::*)(Measurement &)) & Server::power;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case AVG_POWER_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_AVG_POWER_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::avg_power;
            auto setter = (void (Server::*)(Measurement &)) & Server::avg_power;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case AVG_POWER_INTERVAL_ATTR:
      {
         writabble = true;

         if (server != nullptr)
         {
#if HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR
            auto getter = (uint16_t (Server::*)(void)) & Server::avg_power_interval;
            auto setter = (void (Server::*)(uint16_t)) & Server::avg_power_interval;

            return new Attribute<uint16_t, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif

         }
         else
         {
            return new Attribute<uint16_t>(itf_uid, attr, writabble);
         }
      }

      case VOLTAGE_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_VOLTAGE_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::voltage;
            auto setter = (void (Server::*)(Measurement &)) & Server::voltage;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case CURRENT_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_CURRENT_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::current;
            auto setter = (void (Server::*)(Measurement &)) & Server::current;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case FREQUENCY_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_FREQUENCY_ATTR
            auto getter = (Measurement (Server::*)(void)) & Server::frequency;
            auto setter = (void (Server::*)(Measurement &)) & Server::frequency;

            return new Attribute<Measurement &, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<Measurement>(itf_uid, attr, writabble);
         }
      }

      case POWER_FACTOR_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_POWER_FACTOR_ATTR
            auto getter = (uint8_t (Server::*)(void)) & Server::power_factor;
            auto setter = (void (Server::*)(uint8_t)) & Server::power_factor;

            return new Attribute<uint8_t, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<uint8_t>(itf_uid, attr, writabble);
         }
      }

      case REPORT_INTERVAL_ATTR:
      {
         writabble = true;

         if (server != nullptr)
         {
#if HF_ITF_SPM_REPORT_INTERVAL_ATTR
            auto getter = (uint16_t (Server::*)(void)) & Server::report_interval;
            auto setter = (void (Server::*)(uint16_t)) & Server::report_interval;

            return new Attribute<uint16_t, Server>(*server, attr, getter, setter, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute<uint16_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}


// =============================================================================
// Interfaces::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(SimpleTemperature::Server *server, uint8_t uid)
{
   using namespace HF::Interfaces::SimpleTemperature;

   SimpleTemperature::Attributes attr = static_cast<SimpleTemperature::Attributes>(uid);

   uint16_t itf_uid                   = HF::Interface::SIMPLE_TEMPERATURE;

   switch (attr)
   {
      case VALUE_ATTR:
      {
         bool writabble = Temperature::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void)) & Server::temperature;
            auto setter = (void (Server::*)(int16_t)) & Server::temperature;

            return new Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case MINIMUM_ATTR:
      {
         bool writabble = MininumTemperature::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void)) & Server::minimum_temperature;

            return new Attribute<int16_t, Server>(*server, attr, getter, writabble);
         }
         else
         {
            return new Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case MAXIMUM_ATTR:
      {
         bool writabble = MaximumTemperature::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void)) & Server::maximum_temperature;

            return new Attribute<int16_t, Server>(*server, attr, getter, writabble);
         }
         else
         {
            return new Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case TOLERANCE_ATTR:
      {
         bool writabble = Tolerance::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (uint16_t (Server::*)(void)) & Server::tolerance;

            return new Attribute<uint16_t, Server>(*server, attr, getter, writabble);
         }
         else
         {
            return new Attribute<uint16_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Interfaces::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(SimpleHumidity::Server *server, uint8_t uid)
{
   using namespace HF::Interfaces::SimpleHumidity;

   SimpleHumidity::Attributes attr = static_cast<SimpleHumidity::Attributes>(uid);

   uint16_t itf_uid                = HF::Interface::SIMPLE_HUMIDITY;

   switch (attr)
   {
      case VALUE_ATTR:
      {
         bool writabble = Humidity::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (uint16_t (Server::*)(void)) & Server::humidity;
            auto setter = (void (Server::*)(uint16_t)) & Server::humidity;

            return new Attribute<uint16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new Attribute<uint16_t>(itf_uid, attr, writabble);
         }
      }

      case TOLERANCE_ATTR:
      {
         bool writabble = Tolerance::WRITABBLE;

         if (server != nullptr)
         {
            auto getter = (uint16_t (Server::*)(void)) & Server::tolerance;

            return new Attribute<uint16_t, Server>(*server, attr, getter, writabble);
         }
         else
         {
            return new Attribute<uint16_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Interfaces::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Interfaces::create_attribute(SimpleThermostat::Server *server, uint8_t uid)
{
   using namespace HF::Interfaces::SimpleThermostat;

   SimpleThermostat::Attributes attr = static_cast<SimpleThermostat::Attributes>(uid);

   uint16_t itf_uid                  = HF::Interface::SIMPLE_THERMOSTAT;

   switch (attr)
   {
      case SUPPORTED_MODES_ATTR:
      {
         bool writabble = SupportedModes::WRITABLE;

         if (server != nullptr)
         {
            auto getter = (uint8_t (Server::*)(void) const) & Server::supported_modes;
            auto setter = (void (Server::*)(uint8_t)) & Server::supported_modes;

            return new ::Attribute<uint8_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new ::Attribute<uint8_t>(itf_uid, attr, writabble);
         }
      }

      case HEAT_COOL_MODE_ATTR:
      {
         bool writabble = HeatCoolMode::WRITABLE;

         if (server != nullptr)
         {
            auto getter = (uint8_t (Server::*)(void) const) & Server::mode;
            auto setter = (void (Server::*)(uint8_t)) & Server::mode;

            return new ::Attribute<uint8_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
         {
            return new ::Attribute<uint8_t>(itf_uid, attr, writabble);
         }
      }

      case FAN_MODE_ATTR:
      {
         bool writabble = FanMode::WRITABLE;
#if HF_ITF_STS_FAN_MODE

         if (server != nullptr)
         {
            auto getter = (uint8_t (Server::*)(void) const) & Server::fan_mode;
            auto setter = (void (Server::*)(uint8_t)) & Server::fan_mode;

            return new ::Attribute<uint8_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<uint8_t>(itf_uid, attr, writabble);
         }
      }

      case HEAT_MODE_TEMP_ATTR:
      {
         bool writabble = HeatModeTemperature::WRITABLE;
#if HF_ITF_STS_HEAT_MODE

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void) const) & Server::heat_mode_temperature;
            auto setter = (void (Server::*)(int16_t)) & Server::heat_mode_temperature;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case COOL_MODE_TEMP_ATTR:
      {
         bool writabble = CoolModeTemperature::WRITABLE;
#if HF_ITF_STS_COOL_MODE

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void) const) & Server::cool_mode_temperature;
            auto setter = (void (Server::*)(int16_t)) & Server::cool_mode_temperature;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case AUTO_MODE_HEAT_TEMP_ATTR:
      {
         bool writabble = AutoModeHeatTemperature::WRITABLE;
#if HF_ITF_STS_AUTO_MODE

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void) const) & Server::auto_mode_heat_temperature;
            auto setter = (void (Server::*)(int16_t)) & Server::auto_mode_heat_temperature;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case AUTO_MODE_COOL_TEMP_ATTR:
      {
         bool writabble = AutoModeCoolTemperature::WRITABLE;
#if HF_ITF_STS_AUTO_MODE

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void) const) & Server::auto_mode_cool_temperature;
            auto setter = (void (Server::*)(int16_t)) & Server::auto_mode_cool_temperature;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case HEAT_MODE_TEMP_OFFSET_ATTR:
      {
         bool writabble = HeatModeTemperatureOffset::WRITABLE;
#if HF_ITF_STS_HEAT_MODE && HF_ITF_STS_HEAT_OFFSET_ATTR

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void) const) & Server::heat_mode_temperature_offset;
            auto setter = (void (Server::*)(int16_t)) & Server::heat_mode_temperature_offset;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case COOL_MODE_TEMP_OFFSET_ATTR:
      {
         bool writabble = HeatModeTemperatureOffset::WRITABLE;
#if HF_ITF_STS_COOL_MODE && HF_ITF_STS_COOL_OFFSET_ATTR

         if (server != nullptr)
         {
            auto getter = (int16_t (Server::*)(void) const) & Server::cool_mode_temperature_offset;
            auto setter = (void (Server::*)(int16_t)) & Server::cool_mode_temperature_offset;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      case BOOST_DURATION_ATTR:
      {
         bool writabble = BoostDuration::WRITABLE;
#if HF_ITF_STS_BOOST_CMD

         if (server != nullptr)
         {
            auto getter = (uint8_t (Server::*)(void) const) & Server::boost_duration;
            auto setter = (void (Server::*)(uint8_t)) & Server::boost_duration;

            return new ::Attribute<int16_t, Server>(*server, attr, getter, setter, writabble);
         }
         else
#endif
         {
            return new ::Attribute<int16_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Core Services & Interfaces
// =============================================================================

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Core::create_attribute(DeviceInformation::Server *server, uint8_t uid)
{
   using namespace HF::Core::DeviceInformation;

   DeviceInformation::Attributes attr = static_cast<DeviceInformation::Attributes>(uid);

   switch (attr)
   {
      case CORE_VERSION_ATTR:
      {
         uint8_t value = (server != nullptr ? CORE_VERSION : 0xFF);
         return new Attribute<uint8_t>(HF::Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case PROFILE_VERSION_ATTR:
      {
         uint8_t value = (server != nullptr ? PROFILE_VERSION : 0xFF);
         return new Attribute<uint8_t>(HF::Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case INTERFACE_VERSION_ATTR:
      {
         uint8_t value = (server != nullptr ? INTERFACE_VERSION : 0xFF);
         return new Attribute<uint8_t>(HF::Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case EXTRA_CAP_ATTR:
      {
         if (server != nullptr)
         {
            auto getter = (uint8_t (Server::*)(void)) & Server::capabilities;
            auto setter = (void (Server::*)(uint8_t)) & Server::capabilities;

            return new Attribute<uint8_t, Server>(*server, attr, getter, setter);
         }
         else
         {
            return new Attribute<uint8_t>(HF::Interface::DEVICE_INFORMATION, attr);
         }
      }
      case MIN_SLEEP_TIME_ATTR:
      {
         return new Attribute<uint32_t>(HF::Interface::DEVICE_INFORMATION, attr, (uint32_t) 0);
      }
      case ACTUAL_RESP_TIME_ATTR:
      {
         return new Attribute<uint32_t>(HF::Interface::DEVICE_INFORMATION, attr, (uint32_t) 0);
      }
      case APP_VERSION_ATTR:
      {
#if HF_CORE_DEV_INFO_APP_VERSION_ATTR

         if (server != nullptr)
         {
            auto getter = (const std::string (Server::*)(void)) & Server::application_version;
            auto setter = (void (Server::*)(std::string))nullptr;

            return new Attribute<std::string, Server>(*server, attr, getter, setter);
         }
         else
#endif
         {
            return new Attribute<std::string>(HF::Interface::DEVICE_INFORMATION, attr, "");
         }
      }
      case HW_VERSION_ATTR:
      {
#if HF_CORE_DEV_INFO_HW_VERSION_ATTR

         if (server != nullptr)
         {
            auto getter = (const std::string (Server::*)(void)) & Server::hardware_version;
            auto setter = (void (Server::*)(std::string))nullptr;

            return new Attribute<std::string, Server>(*server, attr, getter, setter);
         }
         else
#endif
         {
            return new Attribute<std::string>(HF::Interface::DEVICE_INFORMATION, attr, "");
         }
      }
      case EMC_ATTR:
      {
         uint16_t value = (server != nullptr ? EMC : 0x0000);
         return new Attribute<uint16_t>(HF::Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case DECT_ID_ATTR:
      {
         std::vector<uint8_t> temp(5);
         return new Attribute<std::vector<uint8_t>>(HF::Interface::DEVICE_INFORMATION, attr, temp);
      }
      case MANUFACTURE_NAME_ATTR:
      {
#if HF_CORE_DEV_INFO_MANUFACTURER_NAME_ATTR

         if (server != nullptr)
         {
            auto getter = (const std::string (Server::*)(void)) & Server::manufacturer_name;
            auto setter = (void (Server::*)(std::string))nullptr;

            return new Attribute<std::string, Server>(*server, attr, getter, setter);
         }
         else
#endif
         {
            return new Attribute<std::string>(HF::Interface::DEVICE_INFORMATION, attr, "");
         }
      }
      case LOCATION_ATTR:
      {
         return new Attribute<std::string>(HF::Interface::DEVICE_INFORMATION, attr, "");
      }
      case ENABLED_ATTR:
      {
         return new Attribute<uint8_t>(HF::Interface::DEVICE_INFORMATION, attr, (uint8_t) 0xFF);
      }
      case FRIENDLY_NAME_ATTR:
      {
         return new Attribute<FriendlyName>(HF::Interface::DEVICE_INFORMATION, attr);
      }
      case UID_ATTR:
      {
         HF::UID::UID value;

         if (server != nullptr)
         {
            value = server->device_uid;
         }

         return new Attribute<HF::UID::UID>(HF::Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case SERIAL_NUMBER_ATTR:
      {
         return new Attribute<std::string>(HF::Interface::DEVICE_INFORMATION, attr, "");
      }
      default:
         return nullptr;
   }
}

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Core::create_attribute(DeviceManagement::IServer *server, uint8_t uid)
{
   DeviceManagement::Attributes attr = static_cast<DeviceManagement::Attributes>(uid);

   switch (attr)
   {
      case DeviceManagement::NUMBER_OF_ENTRIES_ATTR:
      {
         uint16_t value = (server != nullptr ? server->entries().size() : 0);
         return new Attribute<uint16_t>(HF::Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      default:
         return nullptr;
   }
}

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Core::create_attribute(BindManagement::IServer *server, uint8_t uid)
{
   BindManagement::Attributes attr = static_cast<BindManagement::Attributes>(uid);

   switch (attr)
   {
      case BindManagement::NUMBER_OF_ENTRIES_ATTR:
      {
         uint16_t value = (server != nullptr ? server->entries().size() : 0);
         return new Attribute<uint16_t>(HF::Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      default:
         return nullptr;
   }
}

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
IAttribute *Core::create_attribute(AttributeReporting::IServer *server, uint8_t uid)
{
   AttributeReporting::Attributes attr = static_cast<AttributeReporting::Attributes>(uid);

   switch (attr)
   {
      case AttributeReporting::REPORT_COUNT_ATTR:
      {
         uint16_t value = (server != nullptr ?
                           server->count(AttributeReporting::Type::PERIODIC) +
                           server->count(AttributeReporting::Type::EVENT) : 0);
         return new Attribute<uint16_t>(HF::Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      case AttributeReporting::PERIODIC_REPORT_COUNT_ATTR:
      {
         uint16_t value = (server != nullptr ?
                           server->count(AttributeReporting::Type::PERIODIC) : 0);
         return new Attribute<uint16_t>(HF::Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      case AttributeReporting::EVENT_REPORT_COUNT_ATTR:
      {
         uint16_t value = (server != nullptr ? server->count(AttributeReporting::Type::EVENT) : 0);
         return new Attribute<uint16_t>(HF::Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      default:
         return nullptr;
   }
}

// =============================================================================
// Attributes::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
List Attributes::get(const HF::Interface &itf, uint8_t pack_id, const UIDS &uids)
{
   List result;

   UIDS attr_uids;

   if (pack_id != DYNAMIC)
   {
      attr_uids = itf.attributes(pack_id);
   }
   else
   {
      attr_uids = uids;
   }

   /* *INDENT-OFF* */
   std::for_each(attr_uids.begin (), attr_uids.end (), [&result, &itf](uint8_t uid)
   {
      result.push_back(const_cast<HF::Interface &>(itf).attribute(uid));
   });
   /* *INDENT-ON* */

   return result;
}
