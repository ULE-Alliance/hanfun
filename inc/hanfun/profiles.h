// =============================================================================
/*!
 * \file       inc/hanfun/profiles.h
 *
 * This file contains the declarations and definitions for the HAN-FUN Profiles.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_PROFILES_H
#define HF_PROFILES_H

#include "common.h"

#include "hanfun/interface.h"
#include "hanfun/interfaces/alert.h"
#include "hanfun/interfaces/on_off.h"
#include "hanfun/interfaces/level_control.h"
#include "hanfun/interfaces/simple_power_meter.h"

// =============================================================================
// API
// =============================================================================

namespace HF
{
   /*!
    * Top level class representing a HAN-FUN Profile.
    *
    * This class is used provide a common interface to all profiles.
    */
   struct IProfile
   {
      enum UID
      {
         // =============================================================================
         // Home Control Unit Types
         // =============================================================================

         //! Allows a unit to receive and interpret on/off requests.
         SIMPLE_ONOFF_SWITCHABLE = 0x0100,

         //! Allows a unit to send on/off requests.
         SIMPLE_ONOFF_SWITCH = 0x0101,

         //! Allows a unit to receive and interpret requests for adjustment of some quantity.
         SIMPLE_LEVEL_CONTROLLABLE = 0x0102,

         //! Allows a unit to send level adjustment requests.
         SIMPLE_LEVEL_CONTROL = 0x0103,

         //! Allows a unit to receive and interpret both on/off and level control requests.
         SIMPLE_LEVEL_CONTROLLABLE_SWITCHABLE = 0x0104,

         //! Allows a unit to send both on/off and level control requests.
         SIMPLE_LEVEL_CONTROL_SWITCH = 0x0105,

         //!  This unit will be acting upon some physical AC switch.
         AC_OUTLET = 0x0106,

         //! This unit will be acting upon some physical AC switch and provides measurements over electric quantities.
         AC_OUTLET_WITH_POWER_METERING = 0x0107,

         //! This unit will be acting upon some physical Light switch.
         SIMPLE_LIGHT = 0x0108,

         //! This unit will be acting upon some dimmable light.
         DIMMABLE_LIGHT = 0x0109,

         //! This unit will be acting upon some dimmable light switch.
         DIMMER_SWITCH = 0x010A,

         //! This unit will be acting upon some physical door lock switch.
         SIMPLE_DOOR_LOCK = 0x010B,

         //! This unit will be acting upon some physical door bell.
         DOOR_BELL = 0x010C,

         //! Allows a unit to do and provide measurements over electric quantities.
         SIMPLE_POWER_METER = 0x010D,

         // =============================================================================
         // Security Unit Types
         // =============================================================================

         //! Use for a generic detector that senses and sends an alert.
         SIMPLE_DETECTOR = 0x0200,

         //! Use for an open/close door detector that senses and sends an alert.
         DOOR_OPEN_CLOSE_DETECTOR = 0x0201,

         //! Use for an open/close window detector that senses and sends an alert.
         WINDOW_OPEN_CLOSE_DETECTOR = 0x0202,

         //! Use for a motion detector that senses and sends an alert.
         MOTION_DETECTOR = 0x0203,

         //! Use for a smoke detector that senses and sends an alert.
         SMOKE_DETECTOR = 0x0204,

         //! Use for a carbon monoxide detector that senses and sends an alert.
         GAS_DETECTOR = 0x0205,

         //! Use for a flood detector that senses and sends an alert.
         FLOOD_DETECTOR = 0x0206,

         //! Use for a glass break detector that senses and sends an alert.
         GLASS_BREAK_DETECTOR = 0x0207,

         //! Use for a vibration detector that senses and sends an alert.
         VIBRATION_DETECTOR = 0x0208,

         //! This unit will be acting upon some physical siren that will sound an alert.
         SIREN = 0x0280,

         // =============================================================================
         // Home care Unit Types
         // =============================================================================

         //! This unit will trigger an alert, much like a panic button.
         SIMPLE_PENDANT = 0x0300,

         // =============================================================================
         // Application Unit Types
         // =============================================================================

         //! User Interface unit (e.g. keypad, remote control)
         USER_INTERFACE = 0x0410,

         //! Logic Application logic unit, (e.g. can be at the concentrator, Portal, user controller)
         GENERIC_APPLICATION = 0x0411,

         // =============================================================================
         // Proprietary Unit Types
         // =============================================================================

         // Use an ID from within the specified range to identify a proprietary unit type.

         //! Proprietary profiles UID start value.
         PROPRIETARY_START = 0xFF00,

         //! Proprietary profiles UID end value.
         PROPRIETARY_END = 0xFFFF,
      };

      /*!
       * Return this profile HAN-FUN UID.
       *
       * @return  the UID associated with this profile. \see IProfile::UID.
       */
      virtual uint16_t uid () const = 0;
   };

   /*!
    * Class template for all profile implementations.
    */
   template<IProfile::UID _uid>
   class AbstractProfile:public IProfile
   {
      public:

      //! \see IProfile::uid()
      uint16_t uid () const
      {
         return _uid;
      }
   };

   /*!
    * Class template for profiles containing only one interface.
    */
   template<IProfile::UID _uid, class Interface>
   struct Profile:public AbstractProfile <_uid>, public Interface
   {
      using AbstractProfile <_uid>::uid;
   };

   /*!
    * Class template for profiles containing two interfaces.
    */
   template<IProfile::UID _uid, class Interface1, class Interface2>
   struct Profile2:public Profile <_uid, Interface1>, public Interface2
   {
      using AbstractProfile <_uid>::uid;

      //! \see Interface::handle()
      virtual bool handle (Protocol::Message &message, ByteArray &payload, size_t offset)
      {
         if (!Interface1::handle (message, payload, offset))
         {
            return Interface2::handle (message, payload, offset);
         }

         return true;
      }
   };

   /*!
    * Class template for Detector type profiles.
    */
   template<IProfile::UID _uid>
   struct Detector:public AbstractProfile <_uid>, protected Interfaces::AlertServer
   {
      Detector()
      {
         AlertServer::disableAll ();
         AlertServer::enable (0);
      }

      using AbstractProfile <_uid>::uid;

      //! \see AlertServer::status()
      void alert (Protocol::Address &addr, bool state)
      {
         AlertServer::state (0, state);
         AlertServer::status (addr, Detector::uid ());
      }
   };

   /*!
    * Namespace for profiles implementations.
    */
   namespace Profiles
   {
      // =============================================================================
      // Home Control Unit Types
      // =============================================================================

      /*!
       * Simple On/Off Switchable profile implementation.
       */
      class SimpleOnOffSwitchable:public Profile <IProfile::SIMPLE_ONOFF_SWITCHABLE, Interfaces::OnOffServer>
      {};

      /*!
       * Simple On/Off Switch profile implementation.
       */
      class SimpleOnOffSwitch:public Profile <IProfile::SIMPLE_ONOFF_SWITCH, Interfaces::OnOffClient>
      {};

      /*!
       * Simple Level Controllable profile implementation.
       */
      class SimpleLevelControllable:
         public Profile <IProfile::SIMPLE_LEVEL_CONTROLLABLE, Interfaces::LevelControlServer>
      {};

      /*!
       * Simple Level Control profile implementation.
       */
      class SimpleLevelControl:public Profile <IProfile::SIMPLE_LEVEL_CONTROL, Interfaces::LevelControlClient>
      {};

      /*!
       * Simple Level Controllable Switchable profile implementation.
       */
      class SimpleLevelControllableSwitchable:
         public Profile2 <IProfile::SIMPLE_LEVEL_CONTROLLABLE_SWITCHABLE, Interfaces::OnOffServer, Interfaces::LevelControlServer>
      {};

      /*!
       * Simple Level Control Switch profile implementation.
       */
      class SimpleLevelControlSwitch:
         public Profile2 <IProfile::SIMPLE_LEVEL_CONTROL_SWITCH, Interfaces::OnOffClient, Interfaces::LevelControlClient>
      {};

      /*!
       * AC Outlet profile implementation.
       */
      class AC_Outlet:public Profile <IProfile::AC_OUTLET, Interfaces::OnOffServer>
      {};

      /*!
       * AC Outlet profile implementation.
       */
      class AC_OutletWithPowerMetering:
         public Profile2 <IProfile::AC_OUTLET_WITH_POWER_METERING, Interfaces::OnOffServer, Interfaces::SimplePowerMeterServer>
      {};

      /*!
       * Simple Light profile implementation.
       */
      class SimpleLight:public Profile <IProfile::SIMPLE_LIGHT, Interfaces::OnOffServer>
      {};

      /*!
       * Dimmable Light profile implementation.
       */
      class DimmableLight:public Profile2 <IProfile::DIMMABLE_LIGHT, Interfaces::OnOffServer, Interfaces::LevelControlClient>
      {};

      /*!
       * Dimmer Switch profile implementation.
       */
      class DimmerSwitch:public Profile2 <IProfile::DIMMER_SWITCH, Interfaces::OnOffServer, Interfaces::LevelControlServer>
      {};

      /*!
       * Simple Door Lock profile implementation.
       */
      class SimpleDoorLock:public Profile <IProfile::SIMPLE_DOOR_LOCK, Interfaces::OnOffServer>
      {};

      /*!
       * Door Bell profile implementation.
       */
      class DoorBell:public Profile <IProfile::DOOR_BELL, Interfaces::AlertServer>
      {};

      /*!
       * Simple Power Meter profile implementation.
       */
      class SimplePowerMeter:public Profile <IProfile::SIMPLE_POWER_METER, Interfaces::SimplePowerMeterServer>
      {};

      // =============================================================================
      // Security Unit Types
      // =============================================================================

      /*!
       * Simple Detector profile implementation.
       */
      class SimpleDetector:public Detector <IProfile::SIMPLE_DETECTOR>
      {};

      /*!
       * Door Open Close Detector profile implementation.
       */
      class DoorOpenCloseDetector:public Detector <IProfile::DOOR_OPEN_CLOSE_DETECTOR>
      {};

      /*!
       * Window Open Close Detector profile implementation.
       */
      class WindowOpenCloseDetector:public Detector <IProfile::WINDOW_OPEN_CLOSE_DETECTOR>
      {};

      /*!
       * Motion Detector profile implementation.
       */
      class MotionDetector:public Detector <IProfile::MOTION_DETECTOR>
      {};

      /*!
       * Smoke Detector profile implementation.
       */
      class SmokeDetector:public Detector <IProfile::SMOKE_DETECTOR>
      {};

      /*!
       * Gas Detector profile implementation.
       */
      class GasDetector:public Detector <IProfile::GAS_DETECTOR>
      {};

      /*!
       * Flood Detector profile implementation.
       */
      class FloodDetector:public Detector <IProfile::FLOOD_DETECTOR>
      {};

      /*!
       * Glass Break Detector profile implementation.
       */
      class GlassBreakDetector:public Detector <IProfile::GLASS_BREAK_DETECTOR>
      {};

      /*!
       * Vibration Detector profile implementation.
       */
      class VibrationDetector:public Detector <IProfile::VIBRATION_DETECTOR>
      {};

      /*!
       * Siren profile implementation.
       */
      class Siren:public Profile <IProfile::SIREN, Interfaces::OnOffServer>
      {};

      // =============================================================================
      // Home care Unit Types
      // =============================================================================

      /*!
       * Simple Pendant profile implementation.
       */
      class SimplePendant:public Detector <IProfile::SIMPLE_PENDANT>
      {};

      // =============================================================================
      // Application Unit Types
      // =============================================================================

      /*!
       * User Interface profile implementation.
       */
      class UserInterface:public AbstractProfile <IProfile::USER_INTERFACE>
      {};

      /*!
       * Generic Application Logic profile implementation.
       */
      class GenericApplicationLogic:public AbstractProfile <IProfile::GENERIC_APPLICATION>
      {};

   }  // namespace Profiles

}  // namespace HF

#endif /* HF_PROFILES_H */
