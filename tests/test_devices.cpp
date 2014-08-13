// =============================================================================
/*!
 * \file       tests/test_devices.cpp
 *
 * This file contains the implementation of the tests for the Device API.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/devices.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Testing;

TEST_GROUP (Devices)
{
   struct Device:public HF::Devices::AbstractDevice
   {
      uint16_t            _address;

      HF::Transport::Link *_link;

      uint16_t address () const
      {
         return _address;
      }

      void connected (HF::Transport::Link *_link)
      {
         this->_link = _link;
         mock ("Device").actualCall ("connected");
      }

      void disconnected (HF::Transport::Link *_link)
      {
         UNUSED (_link);

         this->_link = nullptr;
         mock ("Device").actualCall ("disconnected");
      }

      Transport::Link *link (uint16_t addr) const
      {
         mock ("Device").actualCall ("link").withIntParameter ("address", addr);
         return _link;
      }
   };

   Device device;

   Testing::Link link;

   TEST_SETUP ()
   {
      device._address = 0x5A5A;
      link.address (0x7AA5);

      mock ().ignoreOtherCalls ();

      device.connected (&link);
   }

   TEST_TEARDOWN ()
   {
      mock ().clear ();
   }
};

TEST (Devices, NoResponseRequired)
{
   Protocol::Packet  packet;
   Common::ByteArray payload;

   packet.destination.device = 0x7AA5;
   packet.destination.unit   = 0x78;

   packet.source.device      = device.address ();
   packet.source.unit        = 0x23;

   std::vector <Protocol::Message::Type> types {
      Protocol::Message::COMMAND_REQ,
      Protocol::Message::GET_ATTR_REQ,
      Protocol::Message::SET_ATTR_REQ,
      Protocol::Message::GET_ATTR_PACK_REQ,
      Protocol::Message::SET_ATTR_PACK_REQ,
      Protocol::Message::ATOMIC_SET_ATTR_PACK_REQ,
   };

   mock ("Link").expectNCalls (0, "send");

   std::for_each (types.begin (), types.end (),
                  [this, &packet, &payload](Protocol::Message::Type type)
                  {
                     packet.message.type = type;
                     device.receive (packet, payload, 0);
                  }
                 );

   mock ("Link").checkExpectations ();
}

TEST (Devices, ResponseRequired)
{
   Protocol::Packet  packet;
   Common::ByteArray payload;

   packet.destination.device = 0x7AA5;
   packet.destination.unit   = 0x78;

   packet.source.device      = device.address ();
   packet.source.unit        = 0x23;

   std::vector <Protocol::Message::Type> types {
      Protocol::Message::COMMAND_RESP_REQ,
      Protocol::Message::SET_ATTR_RESP_REQ,
      Protocol::Message::SET_ATTR_PACK_RESP_REQ,
      Protocol::Message::ATOMIC_SET_ATTR_PACK_RESP_REQ,
   };

   mock ("Link").expectNCalls (types.size (), "send");

   std::for_each (types.begin (), types.end (),
                  [this, &packet, &payload](Protocol::Message::Type type)
                  {
                     packet.message.type = type;
                     device.receive (packet, payload, 0);
                  }
                 );

   mock ("Link").checkExpectations ();
}
