// =============================================================================
/*!
 * \file       tests/test_profiles.cpp
 *
 * This file contains the implementation of the tests for the profiles.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/profiles.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Testing;

// =============================================================================
// Defines
// =============================================================================

/* *INDENT-OFF* */
#define HELPER_CLASS(_name) \
   class _name:public InterfaceHelper<Profiles::_name>
/* *INDENT-ON* */

// =============================================================================
// Helper Test Classes
// =============================================================================

namespace HF
{
   namespace Testing
   {
      HELPER_CLASS (SimpleOnOffSwitchable) {};
      HELPER_CLASS (SimpleOnOffSwitch) {};
      HELPER_CLASS (SimpleLevelControllable) {};
      HELPER_CLASS (SimpleLevelControl) {};
      HELPER_CLASS (SimpleLevelControllableSwitchable) {};
      HELPER_CLASS (SimpleLevelControlSwitch) {};
      HELPER_CLASS (AC_Outlet) {};
      HELPER_CLASS (AC_OutletWithPowerMetering) {};
      HELPER_CLASS (SimpleLight) {};
      HELPER_CLASS (DimmableLight) {};
      HELPER_CLASS (DimmerSwitch) {};
      HELPER_CLASS (SimpleDoorLock) {};

      HELPER_CLASS (DoorBell) {};
      HELPER_CLASS (SimplePowerMeter) {};

      HELPER_CLASS (SimpleDetector) {};
      HELPER_CLASS (DoorOpenCloseDetector) {};
      HELPER_CLASS (WindowOpenCloseDetector) {};
      HELPER_CLASS (MotionDetector) {};
      HELPER_CLASS (SmokeDetector) {};
      HELPER_CLASS (GasDetector) {};
      HELPER_CLASS (FloodDetector) {};
      HELPER_CLASS (GlassBreakDetector) {};
      HELPER_CLASS (VibrationDetector) {};
      HELPER_CLASS (Siren) {};

      HELPER_CLASS (SimplePendant) {};

      HELPER_CLASS (UserInterface) {};

      HELPER_CLASS (GenericApplicationLogic) {};

   }  // namespace Testing

}  // namespace HF

// =============================================================================
// Profiles UIDs Tests
// =============================================================================

TEST_GROUP (Profiles)
{
   TEST_SETUP ()
   {
      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();
   }
};

TEST (Profiles, UIDs)
{
   IProfile *profile = NULL;

   // =============================================================================
   // Home Control Unit Types
   // =============================================================================

   profile = new SimpleOnOffSwitchable ();
   CHECK_EQUAL (IProfile::SIMPLE_ONOFF_SWITCHABLE, profile->uid ());
   delete profile;

   profile = new SimpleOnOffSwitch ();
   CHECK_EQUAL (IProfile::SIMPLE_ONOFF_SWITCH, profile->uid ());
   delete profile;

   profile = new SimpleLevelControllable ();
   CHECK_EQUAL (IProfile::SIMPLE_LEVEL_CONTROLLABLE, profile->uid ());
   delete profile;

   profile = new SimpleLevelControl ();
   CHECK_EQUAL (IProfile::SIMPLE_LEVEL_CONTROL, profile->uid ());
   delete profile;

   profile = new SimpleLevelControllableSwitchable ();
   CHECK_EQUAL (IProfile::SIMPLE_LEVEL_CONTROLLABLE_SWITCHABLE, profile->uid ());
   delete profile;

   profile = new SimpleLevelControlSwitch ();
   CHECK_EQUAL (IProfile::SIMPLE_LEVEL_CONTROL_SWITCH, profile->uid ());
   delete profile;

   profile = new AC_Outlet ();
   CHECK_EQUAL (IProfile::AC_OUTLET, profile->uid ());
   delete profile;

   profile = new AC_OutletWithPowerMetering ();
   CHECK_EQUAL (IProfile::AC_OUTLET_WITH_POWER_METERING, profile->uid ());
   delete profile;

   profile = new SimpleLight ();
   CHECK_EQUAL (IProfile::SIMPLE_LIGHT, profile->uid ());
   delete profile;

   profile = new DimmableLight ();
   CHECK_EQUAL (IProfile::DIMMABLE_LIGHT, profile->uid ());
   delete profile;

   profile = new DimmerSwitch ();
   CHECK_EQUAL (IProfile::DIMMER_SWITCH, profile->uid ());
   delete profile;

   profile = new SimpleDoorLock ();
   CHECK_EQUAL (IProfile::SIMPLE_DOOR_LOCK, profile->uid ());
   delete profile;

   profile = new DoorBell ();
   CHECK_EQUAL (IProfile::DOOR_BELL, profile->uid ());
   delete profile;

   profile = new Testing::SimplePowerMeter ();
   CHECK_EQUAL (IProfile::SIMPLE_POWER_METER, profile->uid ());
   delete profile;

   // =============================================================================
   // Security Unit Types
   // =============================================================================
   profile = new SimpleDetector ();
   CHECK_EQUAL (IProfile::SIMPLE_DETECTOR, profile->uid ());
   delete profile;

   profile = new DoorOpenCloseDetector ();
   CHECK_EQUAL (IProfile::DOOR_OPEN_CLOSE_DETECTOR, profile->uid ());
   delete profile;

   profile = new WindowOpenCloseDetector ();
   CHECK_EQUAL (IProfile::WINDOW_OPEN_CLOSE_DETECTOR, profile->uid ());
   delete profile;

   profile = new MotionDetector ();
   CHECK_EQUAL (IProfile::MOTION_DETECTOR, profile->uid ());
   delete profile;

   profile = new SmokeDetector ();
   CHECK_EQUAL (IProfile::SMOKE_DETECTOR, profile->uid ());
   delete profile;

   profile = new GasDetector ();
   CHECK_EQUAL (IProfile::GAS_DETECTOR, profile->uid ());
   delete profile;

   profile = new FloodDetector ();
   CHECK_EQUAL (IProfile::FLOOD_DETECTOR, profile->uid ());
   delete profile;

   profile = new GlassBreakDetector ();
   CHECK_EQUAL (IProfile::GLASS_BREAK_DETECTOR, profile->uid ());
   delete profile;

   profile = new VibrationDetector ();
   CHECK_EQUAL (IProfile::VIBRATION_DETECTOR, profile->uid ());
   delete profile;

   profile = new Siren ();
   CHECK_EQUAL (IProfile::SIREN, profile->uid ());
   delete profile;

   // =============================================================================
   // Home care Unit Types
   // =============================================================================

   profile = new SimplePendant ();
   CHECK_EQUAL (IProfile::SIMPLE_PENDANT, profile->uid ());
   delete profile;

   // =============================================================================
   // Application Unit Types
   // =============================================================================

   profile = new UserInterface ();
   CHECK_EQUAL (IProfile::USER_INTERFACE, profile->uid ());
   delete profile;

   profile = new GenericApplicationLogic ();
   CHECK_EQUAL (IProfile::GENERIC_APPLICATION, profile->uid ());
   delete profile;
}

TEST (Profiles, Detector)
{
   Protocol::Address addr;
   SimpleDetector    detector;

   addr.mod    = Protocol::Address::DEVICE_ADDR;
   addr.device = 42;
   addr.unit   = 33;

   mock ("Interface").expectOneCall ("sendMessage");

   detector.alert (addr, true);

   mock ("Interface").checkExpectations ();

   CHECK (detector.sendMsg.payload != nullptr);

   Interfaces::Alert::Message *status_msg = static_cast <Interfaces::Alert::Message *>(detector.sendMsg.payload);

   CHECK_EQUAL (1, status_msg->state);
   CHECK_EQUAL (detector.uid (), status_msg->type);

   LONGS_EQUAL (Protocol::Address::DEVICE_ADDR, detector.addr.mod);
   LONGS_EQUAL (42, detector.addr.device);
   LONGS_EQUAL (33, detector.addr.unit);
}
