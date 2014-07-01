// =============================================================================
/*!
 * \file       inc/hanfun/profiles.h
 *
 * This file contains the declarations and definitions for the HAN-FUN Profiles.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
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
    * Namespace for profiles implementations.
    */
   namespace Profiles
   {
      //! Profiles UID's
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
       * This function returns a pointer to a entry on a static const array,
       * containing the interfaces the profile with the \c profile UID.
       *
       * The number of interfaces is given by \c count. You can get the addicional
       * interfaces by incrementing the returned pointer.
       *
       * \warning The returned pointer MUST NOT be used with free/delete.
       *
       * @param [in]  profile  the profile UID to retrieve the interfaces for.
       * @param [out] count    the number of interfaces this profile has.
       *
       * @return  a pointer to structure containing the interface information for the
       *          profile or \c nullptr if the profile is not known or does not use any
       *          of the official interfaces.
       */
      Common::Interface const *interfaces (uint16_t profile, uint16_t &count);

      /*!
       * Top level class representing a HAN-FUN Profile.
       *
       * This class is used provide a common interface to all profiles.
       */
      struct IProfile
      {
         virtual ~IProfile() {}

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
      template<uint16_t _uid>
      struct AbstractProfile:public IProfile
      {
         virtual ~AbstractProfile()
         {}

         //! \see IProfile::uid()
         uint16_t uid () const
         {
            return _uid;
         }
      };

      /*!
       * Class template for profiles containing only one interface.
       */
      template<uint16_t _uid, class Interface>
      struct Profile:public AbstractProfile <_uid>, public Interface
      {
         virtual ~Profile()
         {}

         using AbstractProfile <_uid>::uid;
      };

      /*!
       * Class template for profiles containing two interfaces.
       */
      template<uint16_t _uid, typename Interface1, typename Interface2>
      struct Profile2:public AbstractProfile <_uid>
      {
         virtual ~Profile2()
         {}
         typedef Profile2 <_uid, Interface1, Interface2> profile_t;

         template<typename Itf>
         struct InterfaceProxy:public Itf
         {
            InterfaceProxy(profile_t &proxy):proxy (proxy)
            {}

            void sendMessage (Protocol::Address &addr, Protocol::Message &message)
            {
               proxy.sendMessage (addr, message);
            }

            private:

            profile_t &proxy;
         };

         typedef InterfaceProxy <Interface1> first_itf_t;
         typedef InterfaceProxy <Interface2> second_itf_t;

         static_assert (is_base_of <Interfaces::AbstractInterface, Interface1>::value &&
                        is_base_of <Interfaces::AbstractInterface, Interface2>::value,
                        "Interface1 and Interface 2 MUST be of type HF::AbstractInterface !");

         //! \see Interface::handle
         virtual Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            Common::Result result = interfaces.first.handle (packet, payload, offset);

            if (result == Common::Result::OK || result == Common::Result::FAIL_SUPPORT)
            {
               return result;
            }
            else
            {
               return interfaces.second.handle (packet, payload, offset);
            }
         }

         /*!
          * Pointer to the first interface instance.
          *
          * @return  pointer to the interface instance.
          */
         Interface1 *first ()
         {
            return static_cast <Interface1 *>(&(this->interfaces.first));
         }

         /*!
          * Pointer to the second interface instance.
          *
          * @return  pointer to the interface instance.
          */
         Interface2 *second ()
         {
            return static_cast <Interface2 *>(&(this->interfaces.second));
         }

         //! \see AbstractInterface::sendMessage
         virtual void sendMessage (Protocol::Address &addr, Protocol::Message &message) = 0;

         protected:

         Profile2():interfaces (first_itf_t (*this), second_itf_t (*this))
         {}

         pair <first_itf_t, second_itf_t> interfaces;
      };

      /*!
       * Class template for Detector type profiles.
       */
      template<Profiles::UID _uid>
      struct Detector:public AbstractProfile <_uid>, protected Interfaces::Alert::Server
      {
         Detector()
         {
            Interfaces::Alert::Server::disableAll ();
            Interfaces::Alert::Server::enable (0);
         }

         virtual ~Detector()
         {}

         using AbstractProfile <_uid>::uid;

         //! \see Alert::Server::status()
         void alert (Protocol::Address &addr, bool state)
         {
            Interfaces::Alert::Server::state (0, state);
            Interfaces::Alert::Server::status (addr, Detector::uid ());
         }
      };

      // =============================================================================
      // Home Control Unit Types
      // =============================================================================

      /*!
       * Simple On/Off Switchable profile implementation.
       */
      class SimpleOnOffSwitchable:public Profile <SIMPLE_ONOFF_SWITCHABLE, Interfaces::OnOff::Server>
      {
         public:

         virtual ~SimpleOnOffSwitchable() {}
      };

      /*!
       * Simple On/Off Switch profile implementation.
       */
      class SimpleOnOffSwitch:public Profile <SIMPLE_ONOFF_SWITCH, Interfaces::OnOff::Client>
      {
         public:

         virtual ~SimpleOnOffSwitch() {}
      };

      /*!
       * Simple Level Controllable profile implementation.
       */
      class SimpleLevelControllable:
         public Profile <SIMPLE_LEVEL_CONTROLLABLE, Interfaces::LevelControl::Server>
      {
         public:

         virtual ~SimpleLevelControllable() {}
      };

      /*!
       * Simple Level Control profile implementation.
       */
      class SimpleLevelControl:public Profile <SIMPLE_LEVEL_CONTROL, Interfaces::LevelControl::Client>
      {
         public:

         virtual ~SimpleLevelControl() {}
      };

      /*!
       * Simple Level Controllable Switchable profile implementation.
       */
      template<typename OnOffServer        = Interfaces::OnOff::Server,
               typename LevelControlServer = Interfaces::LevelControl::Server>
      class SimpleLevelControllableSwitchable:
         public Profile2 <SIMPLE_LEVEL_CONTROLLABLE_SWITCHABLE, OnOffServer, LevelControlServer>
      {
         static_assert (is_base_of <Interfaces::OnOff::Server, OnOffServer>::value,
                        "OnOff::Server MUST be of type Interfaces::OnOff::Server !");
         static_assert (is_base_of <Interfaces::LevelControl::Server, LevelControlServer>::value,
                        "LevelControl::Server MUST be of type Interfaces::LevelControl::Server !");

         public:

         virtual ~SimpleLevelControllableSwitchable() {}

         Interfaces::OnOff::Server *on_off ()
         {
            return this->first ();
         }

         Interfaces::LevelControl::Server *level_control ()
         {
            return this->second ();
         }
      };

      /*!
       * Simple Level Control Switch profile implementation.
       */
      template<typename OnOffClient        = Interfaces::OnOff::Client,
               typename LevelControlClient = Interfaces::LevelControl::Client>
      class SimpleLevelControlSwitch:
         public Profile2 <SIMPLE_LEVEL_CONTROL_SWITCH, OnOffClient, LevelControlClient>
      {
         static_assert (is_base_of <Interfaces::OnOff::Client, OnOffClient>::value,
                        "OnOff::Client MUST be of type Interfaces::OnOff::Client !");
         static_assert (is_base_of <Interfaces::LevelControl::Client, LevelControlClient>::value,
                        "LevelControl::Client MUST be of type Interfaces::LevelControl::Client !");
         public:

         virtual ~SimpleLevelControlSwitch() {}

         Interfaces::OnOff::Client *on_off ()
         {
            return this->first ();
         }

         Interfaces::LevelControl::Client *level_control ()
         {
            return this->second ();
         }
      };

      /*!
       * AC Outlet profile implementation.
       */
      class AC_Outlet:public Profile <AC_OUTLET, Interfaces::OnOff::Server>
      {
         public:

         virtual ~AC_Outlet() {}
      };

      /*!
       * AC Outlet profile implementation.
       */
      template<typename OnOffServer            = Interfaces::OnOff::Server,
               typename SimplePowerMeterServer = Interfaces::SimplePowerMeter::Server>
      class AC_OutletWithPowerMetering:
         public Profile2 <AC_OUTLET_WITH_POWER_METERING, OnOffServer, SimplePowerMeterServer>
      {
         static_assert (is_base_of <Interfaces::OnOff::Server, OnOffServer>::value,
                        "OnOff::Server MUST be of type Interfaces::OnOff::Server !");
         static_assert (is_base_of <Interfaces::SimplePowerMeter::Server, SimplePowerMeterServer>::value,
                        "SimplePowerMeterServer MUST be of type Interfaces::SimplePowerMeter::Server !");

         public:

         virtual ~AC_OutletWithPowerMetering() {}

         Interfaces::OnOff::Server *on_off ()
         {
            return this->first ();
         }

         Interfaces::SimplePowerMeter::Server *power_meter ()
         {
            return this->second ();
         }
      };

      /*!
       * Simple Light profile implementation.
       */
      class SimpleLight:public Profile <SIMPLE_LIGHT, Interfaces::OnOff::Server>
      {
         public:

         virtual ~SimpleLight() {}
      };

      /*!
       * Dimmable Light profile implementation.
       */
      template<typename OnOffServer        = Interfaces::OnOff::Server,
               typename LevelControlServer = Interfaces::LevelControl::Server>
      class DimmableLight:public Profile2 <DIMMABLE_LIGHT, OnOffServer, LevelControlServer>
      {
         static_assert (is_base_of <Interfaces::OnOff::Server, OnOffServer>::value,
                        "OnOff::Server MUST be of type Interfaces::OnOff::Server !");
         static_assert (is_base_of <Interfaces::LevelControl::Server, LevelControlServer>::value,
                        "LevelControl::Client MUST be of type Interfaces::LevelControl::Server !");

         public:

         virtual ~DimmableLight() {}

         Interfaces::OnOff::Server *on_off ()
         {
            return this->first ();
         }

         Interfaces::LevelControl::Server *level_control ()
         {
            return this->second ();
         }
      };

      /*!
       * Dimmer Switch profile implementation.
       */
      template<typename OnOffClient        = Interfaces::OnOff::Client,
               typename LevelControlClient = Interfaces::LevelControl::Client>
      class DimmerSwitch:public Profile2 <DIMMER_SWITCH, OnOffClient, LevelControlClient>
      {
         static_assert (is_base_of <Interfaces::OnOff::Client, OnOffClient>::value,
                        "OnOff::Server MUST be of type Interfaces::OnOff::Client !");
         static_assert (is_base_of <Interfaces::LevelControl::Client, LevelControlClient>::value,
                        "LevelControl::Server MUST be of type Interfaces::LevelControl::Client !");

         public:

         virtual ~DimmerSwitch() {}

         Interfaces::OnOff::Client *on_off ()
         {
            return this->first ();
         }

         Interfaces::LevelControl::Client *level_control ()
         {
            return this->second ();
         }
      };

      /*!
       * Simple Door Lock profile implementation.
       */
      class SimpleDoorLock:public Profile <SIMPLE_DOOR_LOCK, Interfaces::OnOff::Server>
      {
         public:

         virtual ~SimpleDoorLock() {}
      };

      /*!
       * Door Bell profile implementation.
       */
      class DoorBell:public Profile <DOOR_BELL, Interfaces::Alert::Server>
      {
         public:

         virtual ~DoorBell() {}
      };

      /*!
       * Simple Power Meter profile implementation.
       */
      class SimplePowerMeter:public Profile <SIMPLE_POWER_METER, Interfaces::SimplePowerMeter::Server>
      {
         public:

         virtual ~SimplePowerMeter() {}
      };

      // =============================================================================
      // Security Unit Types
      // =============================================================================

      /*!
       * Simple Detector profile implementation.
       */
      class SimpleDetector:public Detector <SIMPLE_DETECTOR>
      {
         public:

         virtual ~SimpleDetector() {}
      };

      /*!
       * Door Open Close Detector profile implementation.
       */
      class DoorOpenCloseDetector:public Detector <DOOR_OPEN_CLOSE_DETECTOR>
      {
         public:

         virtual ~DoorOpenCloseDetector() {}
      };

      /*!
       * Window Open Close Detector profile implementation.
       */
      class WindowOpenCloseDetector:public Detector <WINDOW_OPEN_CLOSE_DETECTOR>
      {
         public:

         virtual ~WindowOpenCloseDetector() {}
      };

      /*!
       * Motion Detector profile implementation.
       */
      class MotionDetector:public Detector <MOTION_DETECTOR>
      {
         public:

         virtual ~MotionDetector() {}
      };

      /*!
       * Smoke Detector profile implementation.
       */
      class SmokeDetector:public Detector <SMOKE_DETECTOR>
      {
         public:

         virtual ~SmokeDetector() {}
      };

      /*!
       * Gas Detector profile implementation.
       */
      class GasDetector:public Detector <GAS_DETECTOR>
      {
         public:

         virtual ~GasDetector() {}
      };

      /*!
       * Flood Detector profile implementation.
       */
      class FloodDetector:public Detector <FLOOD_DETECTOR>
      {
         public:

         virtual ~FloodDetector() {}
      };

      /*!
       * Glass Break Detector profile implementation.
       */
      class GlassBreakDetector:public Detector <GLASS_BREAK_DETECTOR>
      {
         public:

         virtual ~GlassBreakDetector() {}
      };

      /*!
       * Vibration Detector profile implementation.
       */
      class VibrationDetector:public Detector <VIBRATION_DETECTOR>
      {
         public:

         virtual ~VibrationDetector() {}
      };

      /*!
       * Siren profile implementation.
       */
      class Siren:public Profile <SIREN, Interfaces::OnOff::Server>
      {
         public:

         virtual ~Siren() {}
      };

      // =============================================================================
      // Home care Unit Types
      // =============================================================================

      /*!
       * Simple Pendant profile implementation.
       */
      class SimplePendant:public Detector <SIMPLE_PENDANT>
      {
         public:

         virtual ~SimplePendant() {}
      };

      // =============================================================================
      // Application Unit Types
      // =============================================================================

      /*!
       * User Interface profile implementation.
       */
      class UserInterface:public AbstractProfile <USER_INTERFACE>
      {
         public:

         virtual ~UserInterface() {}
      };

      /*!
       * Generic Application Logic profile implementation.
       */
      class GenericApplicationLogic:public AbstractProfile <GENERIC_APPLICATION>
      {
         public:

         virtual ~GenericApplicationLogic() {}
      };

   }  // namespace Profiles

}  // namespace HF

#endif /* HF_PROFILES_H */
