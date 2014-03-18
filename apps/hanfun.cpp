// =============================================================================
/*!
 * \file       apps/hanfun.cpp
 *
 * This file contains the an example application for using the HAN-FUN library.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#include <iostream>
#include <deque>
#include <tuple>

#include "hanfun.h"
#include "hanfun/transport.h"

#include "base.h"
#include "transport.h"

using namespace std;

// =============================================================================
// Classes
// =============================================================================

class Device:public HF::Devices::Node::Base <HF::Devices::Node::DefaultUnit0>
{
   void send (HF::Protocol::Packet &packet)
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Devices::Node::Base <HF::Devices::Node::DefaultUnit0>::send (packet);
   }

   void receive (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset)
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Devices::Node::Base <HF::Devices::Node::DefaultUnit0>::receive (packet, payload, offset);
   }
};

class SimpleLight:public HF::Units::Unit <HF::Profiles::SimpleLight>
{
   public:

   SimpleLight(uint8_t index, HF::IDevice *device):
      HF::Units::Unit <HF::Profiles::SimpleLight>(index, device)
   {}

   void on ()
   {
      cout << __PRETTY_FUNCTION__ << endl;
   }

   void off ()
   {
      cout << __PRETTY_FUNCTION__ << endl;
   }

   void toggle ()
   {
      cout << __PRETTY_FUNCTION__ << endl;
   }
};

class SimpleSwitch:public HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>
{
   public:

   SimpleSwitch(uint8_t index, HF::IDevice *device):
      HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>(index, device)
   {}

   void on ()
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>::on ();
   }

   void off ()
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>::off ();
   }

   void toggle ()
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>::toggle ();
   }
};

// =============================================================================
// Global Variables
// =============================================================================

Base base ("hf://i_am_a_base");

Device dev_switch;
SimpleSwitch wall_switch (1, &dev_switch);

Device dev_light;
SimpleLight light (1, &dev_light);

Transport   transport;

// =============================================================================
// Main
// =============================================================================

int main (int argc, char **argv)
{
   UNUSED (argc);
   UNUSED (argv);

   transport.initialize (&base);

   transport.add ((HF::Transport::Endpoint *) &dev_switch, new HF::UID::URI ("hf://i_am_a_simple_switch"));
   transport.add ((HF::Transport::Endpoint *) &dev_light, new HF::UID::URI ("hf://i_am_a_simple_light"));

   dev_switch.unit0.management ()->register_device ();

   cout << "Switch received address : " << dev_switch.address () << endl;

   dev_light.unit0.management ()->register_device ();

   cout << "Light received address : " << dev_light.address () << endl;

   wall_switch.toggle ();

   return 0;
}
