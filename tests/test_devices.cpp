// =============================================================================
/*!
 * \file       tests/test_devices.cpp
 *
 * This file contains the implementation of the tests for the Device API.
 *
 * \version    1.0.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/devices.h"
#include "hanfun/profiles.h"
#include "hanfun/units.h"

#include "hanfun/transport.h"

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

      IDevice::IUnit0 *unit0 () const
      {
         return nullptr;
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

   /* *INDENT-OFF* */
   std::for_each (types.begin (), types.end (),
         [this, &packet, &payload](Protocol::Message::Type type)
         {
            packet.message.type = type;
            device.receive (packet, payload, 0);
         });
   /* *INDENT-ON* */

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

   /* *INDENT-OFF* */
   std::for_each (types.begin (), types.end (),
                  [this, &packet, &payload](Protocol::Message::Type type)
   {
      packet.message.type = type;
      device.receive (packet, payload, 0);
   });
   /* *INDENT-ON* */

   mock ("Link").checkExpectations ();
}

// =============================================================================
// Concentrator Tests
// =============================================================================

namespace
{
   struct SimpleDetector:public HF::Units::Unit <HF::Profiles::SimpleDetector>
   {
      SimpleDetector(uint8_t index, IDevice &device):
         HF::Units::Unit <HF::Profiles::SimpleDetector>(index, device)
      {}
   };

   struct Alertable:public HF::Units::Unit <HF::Profiles::Alertable>
   {
      Alertable(uint8_t index, IDevice &device):
         HF::Units::Unit <HF::Profiles::Alertable>(index, device)
      {}

      void status (HF::Interfaces::Alert::Message &message)
      {
         UNUSED (message);
         mock ("Alertable").actualCall ("status").onObject (this);
      }
   };

   struct AbstractDeviceHelper:public HF::Devices::Node::Abstract <HF::Devices::Node::DefaultUnit0>
   {
      virtual ~AbstractDeviceHelper()
      {}
   };

   struct DeviceHelper:public AbstractDeviceHelper
   {
      SimpleDetector unit;

      DeviceHelper():unit (SimpleDetector (1, *this))
      {}

      virtual ~DeviceHelper()
      {}

   };

   struct DeviceHelper2:public AbstractDeviceHelper
   {
      Alertable unit;

      DeviceHelper2():unit (Alertable (1, *this))
      {}

      virtual ~DeviceHelper2()
      {}
   };

   struct BaseHelper:public HF::Devices::Concentrator::Abstract <HF::Devices::Concentrator::DefaultUnit0>
   {
      Alertable unit;

      BaseHelper():unit (Alertable (1, *this))
      {}
   };

   struct Link:public HF::Transport::AbstractLink
   {
      HF::Transport::AbstractLayer &recv_tsp;

      HF::UID::UID_T               *_uid;
      HF::Transport::Layer         *tsp;

      Link                         *other;

      Link(HF::Transport::AbstractLayer &_recv_tsp,
           HF::UID::UID_T *uid = new HF::UID::NONE (),
           HF::Transport::Layer *tsp = nullptr):
         recv_tsp (_recv_tsp), _uid (uid), tsp (tsp), other (nullptr)
      {}

      virtual ~Link()
      {
         delete _uid;
      }

      void send (Common::ByteArray &array)
      {
         recv_tsp.receive (other, array);
      }

      const HF::UID::UID uid () const
      {
         return HF::UID::UID(_uid);
      }

      HF::Transport::Layer const *transport () const
      {
         return tsp;
      }
   };

   struct TransportHelper
   {
      HF::Devices::Concentrator::Transport         base_tsp;

      std::vector <HF::Devices::Node::Transport *> devices_tsp;

      TransportHelper(BaseHelper &_base)
      {
         base_tsp.add (&_base);
      }

      ~TransportHelper()
      {
         for (auto tsp : devices_tsp)
         {
            tsp->destroy ();
            delete tsp;
         }
      }

      void add (AbstractDeviceHelper &device, std::string id)
      {
         HF::Devices::Node::Transport *dev_tsp = new HF::Devices::Node::Transport ();
         dev_tsp->add (&device);

         Link *dev_link  = new Link (base_tsp, new HF::UID::URI ("base"));
         Link *base_link = new Link (*dev_tsp, new HF::UID::URI (id));

         dev_link->other  = base_link;
         base_link->other = dev_link;

         dev_tsp->add (dev_link);
         base_tsp.add (base_link);

         devices_tsp.push_back (dev_tsp);
      }
   };

}  // namespace

TEST_GROUP (Concentrator)
{
   DeviceHelper *device1;
   DeviceHelper *device2;
   DeviceHelper2 *device3;

   BaseHelper    *base;

   TransportHelper *transport;

   TEST_SETUP ()
   {
      mock ().ignoreOtherCalls ();

      base      = new BaseHelper ();

      device1   = new DeviceHelper ();
      device2   = new DeviceHelper ();
      device3   = new DeviceHelper2 ();

      transport = new TransportHelper (*base);
      transport->add (*device1, "1");
      device1->unit0 ()->device_management ()->register_device ();
      // LONGS_EQUAL (1, device1->address());

      transport->add (*device2, "2");
      device2->unit0 ()->device_management ()->register_device ();
      // LONGS_EQUAL (2, device2->address());

      transport->add (*device3, "3");
      device3->unit0 ()->device_management ()->register_device ();
      // LONGS_EQUAL (3, device3->address());
   }

   TEST_TEARDOWN ()
   {
      delete transport;

      delete device3;
      delete device2;
      delete device1;

      delete base;

      mock ().clear ();
   }
};

TEST (Concentrator, PacketToDevice)
{
   mock ("Alertable").expectOneCall ("status").onObject (&(device3->unit));

   Protocol::Address dest (3, 1);
   device1->unit.alert (dest, true);

   mock ().checkExpectations ();
}

TEST (Concentrator, PacketToBase)
{
   mock ("Alertable").expectOneCall ("status").onObject (&(base->unit));

   Protocol::Address dest (0, 1);
   device1->unit.alert (dest, true);

   mock ().checkExpectations ();
}

TEST (Concentrator, BroadcastToDevice)
{
   Protocol::Address src (1, 1);
   Protocol::Address dst (3, 1);

   Common::Interface itf (Interface::ALERT, Interface::CLIENT_ROLE);

   auto res = base->unit0 ()->bind_management ()->add (src, dst, itf);
   LONGS_EQUAL (Common::Result::OK, res.first);

   src = Protocol::Address (2, 1);
   res = base->unit0 ()->bind_management ()->add (src, dst, itf);
   LONGS_EQUAL (Common::Result::OK, res.first);

   mock ("Alertable").expectNCalls (2, "status").onObject (&(device3->unit));

   Protocol::Address dest;
   device1->unit.alert (dest, true);
   device2->unit.alert (dest, true);

   mock ().checkExpectations ();
}

TEST (Concentrator, PacketToAny)
{
   Protocol::Address src;
   Protocol::Address dst (0, 1);
   Common::Interface itf (Interface::ALERT, Interface::CLIENT_ROLE);

   auto res = base->unit0 ()->bind_management ()->add (src, dst, itf);

   LONGS_EQUAL (Common::Result::OK, res.first);

   mock ("Alertable").expectNCalls (2, "status").onObject (&(base->unit));

   Protocol::Address dest;
   device1->unit.alert (dest, true);
   device2->unit.alert (dest, true);

   mock ().checkExpectations ();
}
