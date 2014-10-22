// =============================================================================
/*!
 * \file       src/attributes.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Attributes API.
 *
 * \version    1.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
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

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"
#include "hanfun/core/bind_management.h"
#include "hanfun/core/attribute_reporting.h"

using namespace HF;
using namespace HF::Attributes;
using namespace HF::Interfaces;
using namespace HF::Core;

// =============================================================================
// Attribute Factories
// =============================================================================

struct Entry
{
   uint16_t            id;
   Attributes::Factory factory;
};

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
};

// =============================================================================
// Attributes::get_factory
// =============================================================================
/*!
 *
 */
// =============================================================================
Attributes::Factory Attributes::get_factory (Common::Interface itf)
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
IAttribute *Interfaces::create_attribute (Alert::Server *server, uint8_t uid)
{
   Alert::Attributes attr = static_cast <Alert::Attributes>(uid);

   uint16_t itf_uid       = Interface::ALERT;

   switch (attr)
   {
      case Alert::STATE_ATTR:
      {
         bool writabble = Alert::State::WRITABBLE;

         if (server != nullptr)
         {
            return new Attribute <uint32_t &>(itf_uid, attr, server, server->_state, writabble);
         }
         else
         {
            return new Attribute <uint32_t>(itf_uid, attr, server, writabble);
         }
      }

      case Alert::ENABLE_ATTR:
      {
         bool writabble = Alert::Enable::WRITABBLE;

         if (server != nullptr)
         {
            return new Attribute <uint32_t &>(itf_uid, attr, server, server->_enabled, writabble);
         }
         else
         {
            return new Attribute <uint32_t>(itf_uid, attr, server, writabble);
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
IAttribute *Interfaces::create_attribute (LevelControl::Server *server, uint8_t uid)
{
   LevelControl::Attributes attr = static_cast <LevelControl::Attributes>(uid);

   uint16_t itf_uid              = Interface::LEVEL_CONTROL;

   switch (attr)
   {
      case LevelControl::LEVEL_ATTR:
      {
         bool writabble = LevelControl::Level::WRITABBLE;

         if (server != nullptr)
         {
            return new Attribute <uint8_t &>(itf_uid, attr, server, server->_level, writabble);
         }
         else
         {
            return new Attribute <uint8_t>(itf_uid, attr, server, writabble);
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
IAttribute *Interfaces::create_attribute (OnOff::Server *server, uint8_t uid)
{
   OnOff::Attributes attr = static_cast <OnOff::Attributes>(uid);

   uint16_t itf_uid       = Interface::ON_OFF;

   switch (attr)
   {
      case OnOff::STATE_ATTR:
      {
         bool writabble = OnOff::State::WRITABBLE;

         if (server != nullptr)
         {
            return new Attribute <bool &>(itf_uid, attr, server, server->_state, writabble);
         }
         else
         {
            return new Attribute <bool>(itf_uid, attr, server, false, writabble);
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
IAttribute *Interfaces::create_attribute (SimplePowerMeter::Server *server, uint8_t uid)
{
   SimplePowerMeter::Attributes attr = static_cast <SimplePowerMeter::Attributes>(uid);

   uint16_t itf_uid                  = Interface::SIMPLE_POWER_METER;
   bool writabble                    = false;

   switch (attr)
   {
      case SimplePowerMeter::ENERGY_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_ENERGY_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_energy, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::ENERGY_AT_RESET_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_ENERGY_AT_RESET_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_last_energy, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::TIME_AT_RESET_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_TIME_AT_RESET_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_last_time, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::POWER_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_POWER_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server, server->_power,
                                                                   writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::AVG_POWER_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_AVG_POWER_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_avg_power, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::AVG_POWER_INTERVAL_ATTR:
      {
         writabble = true;

         if (server != nullptr)
         {
#if HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR
            return new Attribute <uint16_t &>(itf_uid, attr, server,
                                              server->_avg_power_interval, writabble);

#else
            return nullptr;

#endif

         }
         else
         {
            return new Attribute <uint16_t>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::VOLTAGE_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_VOLTAGE_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_voltage, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::CURRENT_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_CURRENT_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_current, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::FREQUENCY_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_FREQUENCY_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server,
                                                                   server->_frequency, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::POWER_FACTOR_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_POWER_FACTOR_ATTR
            return new Attribute <uint8_t &>(itf_uid, attr, server, server->_power_factor, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <uint8_t>(itf_uid, attr, server, writabble);
         }
      }

      case SimplePowerMeter::REPORT_INTERVAL_ATTR:
      {
         writabble = true;

         if (server != nullptr)
         {
#if HF_ITF_SPM_REPORT_INTERVAL_ATTR
            return new Attribute <uint16_t &>(itf_uid, attr, server, server->_report_interval, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <uint16_t>(itf_uid, attr, server, writabble);
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
IAttribute *Core::create_attribute (DeviceInformation::Server *server, uint8_t uid)
{
   DeviceInformation::Attributes attr = static_cast <DeviceInformation::Attributes>(uid);

   switch (attr)
   {
      case DeviceInformation::CORE_VERSION_ATTR:
      {
         uint8_t value = (server != nullptr ? DeviceInformation::CORE_VERSION : 0xFF);
         return new Attribute <uint8_t>(Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case DeviceInformation::PROFILE_VERSION_ATTR:
      {
         uint8_t value = (server != nullptr ? DeviceInformation::PROFILE_VERSION : 0xFF);
         return new Attribute <uint8_t>(Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case DeviceInformation::INTERFACE_VERSION_ATTR:
      {
         uint8_t value = (server != nullptr ? DeviceInformation::INTERFACE_VERSION : 0xFF);
         return new Attribute <uint8_t>(Interface::DEVICE_INFORMATION, attr, server, value);
      }
      case DeviceInformation::UID_ATTR:
      {
         HF::UID::UID value;

         if (server != nullptr)
         {
            value = server->device_uid;
         }

         return new Attribute <HF::UID::UID>(Interface::DEVICE_INFORMATION, attr, server, value);
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
IAttribute *Core::create_attribute (DeviceManagement::IServer *server, uint8_t uid)
{
   DeviceManagement::Attributes attr = static_cast <DeviceManagement::Attributes>(uid);

   switch (attr)
   {
      case DeviceManagement::NUMBER_OF_ENTRIES_ATTR:
      {
         uint16_t value = (server != nullptr ? server->entries ().size () : 0);
         return new Attribute <uint16_t>(Interface::DEVICE_MANAGEMENT, attr, server, value);
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
IAttribute *Core::create_attribute (BindManagement::IServer *server, uint8_t uid)
{
   BindManagement::Attributes attr = static_cast <BindManagement::Attributes>(uid);

   switch (attr)
   {
      case BindManagement::NUMBER_OF_ENTRIES_ATTR:
      {
         uint16_t value = (server != nullptr ? server->entries ().size () : 0);
         return new Attribute <uint16_t>(Interface::DEVICE_MANAGEMENT, attr, server, value);
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
IAttribute *Core::create_attribute (AttributeReporting::Server *server, uint8_t uid)
{
   AttributeReporting::Attributes attr = static_cast <AttributeReporting::Attributes>(uid);

   switch (attr)
   {
      case AttributeReporting::REPORT_COUNT_ATTR:
      {
         uint16_t value = (server != nullptr ?
                           server->count (AttributeReporting::Type::PERIODIC) +
                           server->count (AttributeReporting::Type::EVENT) : 0);
         return new Attribute <uint16_t>(Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      case AttributeReporting::PERIODIC_REPORT_COUNT_ATTR:
      {
         uint16_t value = (server != nullptr ?
                           server->count (AttributeReporting::Type::PERIODIC) : 0);
         return new Attribute <uint16_t>(Interface::DEVICE_MANAGEMENT, attr, server, value);
      }
      case AttributeReporting::EVENT_REPORT_COUNT_ATTR:
      {
         uint16_t value = (server != nullptr ? server->count (AttributeReporting::Type::EVENT) : 0);
         return new Attribute <uint16_t>(Interface::DEVICE_MANAGEMENT, attr, server, value);
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
List Attributes::get (const HF::Interface &itf, uint8_t pack_id, const UIDS &uids)
{
   List result;

   UIDS attr_uids;

   if (pack_id != DYNAMIC)
   {
      attr_uids = itf.attributes (pack_id);
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
