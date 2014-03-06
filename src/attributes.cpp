// =============================================================================
/*!
 * \file       src/attributes.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Attributes API.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include "hanfun/attributes.h"

#include "hanfun/interfaces/alert.h"
#include "hanfun/interfaces/level_control.h"
#include "hanfun/interfaces/on_off.h"
#include "hanfun/interfaces/simple_power_meter.h"

#include "hanfun/core/device_management.h"

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Core;

// =============================================================================
// Attribute Factories
// =============================================================================

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
IAttribute *Interfaces::create_attribute (AlertServer *server, uint8_t uid)
{
   AlertServer::Attributes attr = static_cast <AlertServer::Attributes>(uid);

   uint16_t itf_uid             = Interface::ALERT;

   switch (attr)
   {
      case AlertServer::STATE_ATTR:
      {
         bool writabble = AlertServer::State::WRITABBLE;

         if (server != nullptr)
         {
            return new Attribute <uint32_t &>(itf_uid, attr, server->_state, writabble);
         }
         else
         {
            return new Attribute <uint32_t>(itf_uid, attr, writabble);
         }
      }

      case AlertServer::ENABLE_ATTR:
      {
         bool writabble = AlertServer::Enable::WRITABBLE;

         if (server != nullptr)
         {
            return new Attribute <uint32_t &>(itf_uid, attr, server->_enabled, writabble);
         }
         else
         {
            return new Attribute <uint32_t>(itf_uid, attr, writabble);
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
IAttribute *Interfaces::create_attribute (LevelControlServer *server, uint8_t uid)
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
            return new Attribute <uint8_t &>(itf_uid, attr, server->_level, writabble);
         }
         else
         {
            return new Attribute <uint8_t>(itf_uid, attr, writabble);
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
IAttribute *Interfaces::create_attribute (OnOffServer *server, uint8_t uid)
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
            return new Attribute <bool &>(itf_uid, attr, server->_state, writabble);
         }
         else
         {
            return new Attribute <bool>(itf_uid, attr, false, writabble);
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
IAttribute *Interfaces::create_attribute (SimplePowerMeterServer *server, uint8_t uid)
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
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr,
                                                                    server->_energy, writabble);
#else
            return nullptr;
#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::ENERGY_AT_RESET_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_ENERGY_AT_RESET_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr,
                                                                   server->_last_energy, writabble);
#else
            return nullptr;
#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::TIME_AT_RESET_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_TIME_AT_RESET_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server->_last_time,
                                                                   writabble);
#else
            return nullptr;
#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::POWER_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_POWER_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server->_power,
                                                                   writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::AVG_POWER_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_AVG_POWER_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server->_avg_power,
                                                                   writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::AVG_POWER_INTERVAL_ATTR:
      {
         writabble = true;

         if (server != nullptr)
         {
#if HF_ITF_SPM_AVG_POWER_INTERVAL_ATTR
            return new Attribute <uint16_t &>(itf_uid, attr, server->_avg_power_interval, writabble);

#else
            return nullptr;

#endif

         }
         else
         {
            return new Attribute <uint16_t>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::VOLTAGE_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_VOLTAGE_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server->_voltage,
                                                                   writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::CURRENT_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_CURRENT_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server->_current,
                                                                   writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::FREQUENCY_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_FREQUENCY_ATTR
            return new Attribute <SimplePowerMeter::Measurement &>(itf_uid, attr, server->_frequency,
                                                                   writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <SimplePowerMeter::Measurement>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::POWER_FACTOR_ATTR:
      {
         if (server != nullptr)
         {
#if HF_ITF_SPM_POWER_FACTOR_ATTR
            return new Attribute <uint8_t &>(itf_uid, attr, server->_power_factor, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <uint8_t>(itf_uid, attr, writabble);
         }
      }

      case SimplePowerMeter::REPORT_INTERVAL_ATTR:
      {
         writabble = true;

         if (server != nullptr)
         {
#if HF_ITF_SPM_REPORT_INTERVAL_ATTR
            return new Attribute <uint16_t &>(itf_uid, attr, server->_report_interval, writabble);

#else
            return nullptr;

#endif
         }
         else
         {
            return new Attribute <uint16_t>(itf_uid, attr, writabble);
         }
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Core Services & Interfaces
// =============================================================================

IAttribute *Core::create_attribute (DeviceManagementServer *server, uint8_t uid)
{
   DeviceManagement::Attributes attr = static_cast <DeviceManagement::Attributes>(uid);

   switch (attr)
   {
      case DeviceManagement::NUMBER_OF_ENTRIES_ATTR:
      {
         uint16_t value = (server != nullptr ? server->entries_count () : 0);
         return new Attribute <uint16_t>(Interface::DEVICE_MANAGEMENT, attr, value);
      }
      default:
         return nullptr;
   }
}
