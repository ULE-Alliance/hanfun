// =============================================================================
/*!
 * \file       tests/core/test_device_management.cpp
 *
 * This file contains the implementation of the Device Management service
 * interface.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#include <string>
#include <algorithm>

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Common;
using namespace HF::Core;

// =============================================================================
// Helper functions.
// =============================================================================

STRING_FROM (DeviceManagement::Unit)

STRING_FROM (DeviceManagement::Device)

// =============================================================================

TEST_GROUP (DeviceManagement)
{};

// =============================================================================
// DeviceManagement::Interface
// =============================================================================

TEST (DeviceManagement, InterfaceServer)
{
   Common::Interface itf;

   size_t size = itf.size ();
   LONGS_EQUAL (sizeof(uint16_t), size);

   ByteArray expected {0x00, 0x00, 0x00,
                       0xFA, 0xAA,        // Optional Interface : Server Role.
                       0x00, 0x00, 0x00};

   ByteArray array (size + 6);

   itf.role = HF::Interface::SERVER_ROLE;
   itf.id   = 0x7AAA;

   size_t wsize = itf.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   memset (&itf, 0xDD, sizeof(itf));

   size_t rsize = itf.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, itf.role);
   LONGS_EQUAL (0x7AAA, itf.id);
}

TEST (DeviceManagement, InterfaceClient)
{
   Common::Interface itf;

   size_t size = itf.size ();
   LONGS_EQUAL (sizeof(uint16_t), size);

   ByteArray expected {0x00, 0x00, 0x00,
                       0x75, 0x55,        // Optional Interface : Client Role.
                       0x00, 0x00, 0x00};

   ByteArray array (size + 6);

   itf.role = HF::Interface::CLIENT_ROLE;
   itf.id   = 0x7555;

   size_t wsize = itf.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   memset (&itf, 0xDD, sizeof(itf));

   size_t rsize = itf.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (HF::Interface::CLIENT_ROLE, itf.role);
   LONGS_EQUAL (0x7555, itf.id);
}

// =============================================================================
// DeviceManagement::Unit
// =============================================================================

TEST (DeviceManagement, Unit_No_Optional_Itf)
{
   DeviceManagement::Unit wunit (0x42, 0x5AA5);
   DeviceManagement::Unit runit;

   size_t size = wunit.size ();
   LONGS_EQUAL (1 + 1 + 2, size);

   ByteArray expected ({0x00, 0x00, 0x00,
                        0x03,       // Unit entry size.
                        0x42,       // Unit ID.
                        0x5A, 0xA5, // Unit UID.
                        0x00, 0x00, 0x00}
                      );
   ByteArray array (size + 6);

   size_t    wsize = wunit.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   size_t rsize = runit.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (wunit.id, runit.id);
   LONGS_EQUAL (wunit.profile, runit.profile);
}

TEST (DeviceManagement, Unit_With_Optional_Itf)
{
   DeviceManagement::Unit wunit (0x42, 0x5AA5);
   DeviceManagement::Unit runit;

   Common::Interface itf1 (0x5432, HF::Interface::SERVER_ROLE);
   Common::Interface itf2 (0x1234, HF::Interface::CLIENT_ROLE);
   Common::Interface itf3 (0x5678, HF::Interface::SERVER_ROLE);

   wunit.opt_ift.push_back (itf1);
   wunit.opt_ift.push_back (itf2);
   wunit.opt_ift.push_back (itf3);

   size_t size = wunit.size ();
   LONGS_EQUAL (1 + 1 + 2 + 1 + itf1.size () + itf2.size () + itf3.size (), size);

   ByteArray expected {0x00, 0x00, 0x00,
                       0x0A,       // Unit entry size.
                       0x42,       // Unit ID.
                       0x5A, 0xA5, // Unit UID.
                       0x03,       // Number of optional interfaces.
                       0xD4, 0x32, // Optional interface 1
                       0x12, 0x34, // Optional interface 2
                       0xD6, 0x78, // Optional interface 3
                       0x00, 0x00, 0x00};

   ByteArray array (size + 6);

   size_t    wsize = wunit.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   size_t rsize = runit.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (wunit.id, runit.id);
   LONGS_EQUAL (wunit.profile, runit.profile);

   LONGS_EQUAL (wunit.opt_ift.size (), runit.opt_ift.size ());

   for (uint8_t i = 0; i < runit.opt_ift.size (); i++)
   {
      CHECK_EQUAL (wunit.opt_ift[i], runit.opt_ift[i]);
   }
}

// =============================================================================
// DeviceManagement::Device
// =============================================================================

TEST (DeviceManagement, Device)
{
   DeviceManagement::Device device;
   DeviceManagement::Unit   unit;

   device.address = 0x3333;

   unit.id        = 0x42;
   unit.profile   = 0x5AA5;

   device.units.push_back (unit);
   device.units.push_back (unit);
   device.units.push_back (unit);

   size_t size = device.size ();
   LONGS_EQUAL (sizeof(uint16_t) + sizeof(uint8_t) + 3 * unit.size (), size);

   ByteArray expected {0x00, 0x00, 0x00,
                       0x33, 0x33,             // Device Address.
                       0x03,                   // Number of units.
                       0x03, 0x42, 0x5A, 0xA5, // Unit 1.
                       0x03, 0x42, 0x5A, 0xA5, // Unit 2.
                       0x03, 0x42, 0x5A, 0xA5, // Unit 3.
                       0x00, 0x00, 0x00};

   ByteArray array (size + 6);

   size_t    wsize = device.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   device = DeviceManagement::Device ();

   LONGS_EQUAL (Protocol::BROADCAST_ADDR, device.address);
   LONGS_EQUAL (0, device.units.size ());

   size_t rsize = device.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (0x3333, device.address);
   LONGS_EQUAL (3, device.units.size ());

   /* *INDENT-OFF* */
   std::for_each (device.units.begin (), device.units.end (),
                  [&unit](const DeviceManagement::Unit &_unit)
   {
      CHECK_EQUAL (unit, _unit);
   });
   /* *INDENT-ON* */
}

// =============================================================================
// DeviceManagement::RegisterMessage
// =============================================================================

TEST_GROUP (DeviceManagement_RegisterMessage)
{
   DeviceManagement::RegisterMessage *message;
   DeviceManagement::Unit unit;

   ByteArray expected;
   UID::IPUI ipui;

   TEST_SETUP ()
   {
      message      = new DeviceManagement::RegisterMessage ();

      unit.id      = 0x42;
      unit.profile = 0x5AA5;

      message->units.push_back (unit);
      message->units.push_back (unit);
      message->units.push_back (unit);

      ipui[0]      = 0x00;
      ipui[1]      = 0x73;
      ipui[2]      = 0x70;
      ipui[3]      = 0xAA;
      ipui[4]      = 0xBB;

      message->uid = HF::UID::UID( &ipui);
   }

   TEST_TEARDOWN ()
   {
      delete message;
   }
};

TEST (DeviceManagement_RegisterMessage, No_EMC)
{
   expected = ByteArray {0x00, 0x00, 0x00,
                         0x02,                          // Discriminator Type.
                         0x05,                          // Size of UID.
                         0x00, 0x73, 0x70, 0xAA, 0xBB,  // IPUI.
                         0x03,                          // Number of units.
                         0x03, 0x42, 0x5A, 0xA5,        // Unit 1.
                         0x03, 0x42, 0x5A, 0xA5,        // Unit 2.
                         0x03, 0x42, 0x5A, 0xA5,        // Unit 3.
                         0x00, 0x00, 0x00};

   size_t size = message->size ();
   LONGS_EQUAL (1 + 1 + 5 + 1 + 3 * unit.size (), size);

   ByteArray array (size + 6);

   size_t    wsize = message->pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   delete message;

   message = new DeviceManagement::RegisterMessage ();

   size_t rsize = message->unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (0x0000, message->emc);

   CHECK_EQUAL (ipui, message->uid);

   CHECK_EQUAL (3, message->units.size ());

   /* *INDENT-OFF* */
   std::for_each (message->units.begin (), message->units.end (),
                  [this](const DeviceManagement::Unit &_unit)
   {
      CHECK_EQUAL (unit, _unit);
   });
   /* *INDENT-ON* */
}

TEST (DeviceManagement_RegisterMessage, No_UID)
{
   expected = ByteArray {0x00, 0x00, 0x00,
                         0x80,                     // Discriminator Type.
                         0x00,                     // Size of UID.
                         0x42, 0x43,               // EMC.
                         0x03,                     // Number of units.
                         0x03, 0x42, 0x5A, 0xA5,   // Unit 1.
                         0x03, 0x42, 0x5A, 0xA5,   // Unit 2.
                         0x03, 0x42, 0x5A, 0xA5,   // Unit 3.
                         0x00, 0x00, 0x00};

   message->emc = 0x4243;

   message->uid = HF::UID::UID();

   size_t size = message->size ();
   LONGS_EQUAL (1 + 1 + 2 + 1 + 3 * unit.size (), size);

   ByteArray array (size + 6);

   size_t    wsize = message->pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);
}

TEST (DeviceManagement_RegisterMessage, EMC)
{
   expected = ByteArray {0x00, 0x00, 0x00,
                         0x82,                          // Discriminator Type.
                         0x05,                          // Size of UID.
                         0x00, 0x73, 0x70, 0xAA, 0xBB,  // IPUI.
                         0x42, 0x43,                    // EMC.
                         0x03,                          // Number of units.
                         0x03, 0x42, 0x5A, 0xA5,        // Unit 1.
                         0x03, 0x42, 0x5A, 0xA5,        // Unit 2.
                         0x03, 0x42, 0x5A, 0xA5,        // Unit 3.
                         0x00, 0x00, 0x00};

   message->emc = 0x4243;

   size_t size = message->size ();
   LONGS_EQUAL (1 + 1 + 5 + 2 + 1 + 3 * unit.size (), size);

   ByteArray array (size + 6);

   size_t    wsize = message->pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   delete message;

   message = new DeviceManagement::RegisterMessage ();

   size_t rsize = message->unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (0x4243, message->emc);

   CHECK_EQUAL (ipui, message->uid);

   CHECK_EQUAL (3, message->units.size ());

   /* *INDENT-OFF* */
   std::for_each (message->units.begin (), message->units.end (),
                  [this](const DeviceManagement::Unit &_unit)
   {
      CHECK_EQUAL (unit, _unit);
   });
   /* *INDENT-ON* */
}

// =============================================================================
// DeviceManagement::RegisterResponse
// =============================================================================

TEST_GROUP (DeviceManagement_RegisterResponse)
{};

TEST (DeviceManagement_RegisterResponse, No_EMC)
{
   ByteArray expected = ByteArray {0x00, 0x00, 0x00,
                                   Result::FAIL_AUTH,  // Response Code.
                                   0x42, 0x43,         // Device Address.
                                   0x00, 0x00, 0x00};

   DeviceManagement::RegisterResponse response;

   response.code    = Result::FAIL_AUTH;
   response.address = 0x4243;

   size_t size = response.size ();
   LONGS_EQUAL (1 + 2, size);

   ByteArray array (size + 6);

   size_t    wsize = response.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   response = DeviceManagement::RegisterResponse ();

   size_t rsize = response.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Result::FAIL_AUTH, response.code);
   LONGS_EQUAL (0x4243, response.address);
}

TEST (DeviceManagement_RegisterResponse, EMC)
{
   ByteArray expected = ByteArray {0x00, 0x00, 0x00,
                                   Result::FAIL_AUTH,  // Responce Code.
                                   0xC2, 0x43,         // Device Address.
                                   0xAA, 0xBB,         // EMC
                                   0x00, 0x00, 0x00};

   DeviceManagement::RegisterResponse response;

   response.code    = Result::FAIL_AUTH;
   response.address = 0x4243;
   response.emc     = 0xAABB;

   size_t size = response.size ();
   LONGS_EQUAL (1 + 2 + 2, size);

   ByteArray array (size + 6);

   size_t    wsize = response.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   response = DeviceManagement::RegisterResponse ();

   LONGS_EQUAL (0, response.code);
   LONGS_EQUAL (0, response.address);
   LONGS_EQUAL (0, response.emc);

   size_t rsize = response.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Result::FAIL_AUTH, response.code);
   LONGS_EQUAL (0x4243, response.address);
   LONGS_EQUAL (0xAABB, response.emc);
}

// =============================================================================
// DeviceManagement::DeregisterMessage
// =============================================================================

TEST (DeviceManagement, DeregisterMessage)
{
   DeviceManagement::DeregisterMessage message;

   size_t size = message.size ();
   LONGS_EQUAL (sizeof(uint16_t), size);

   message.address = 0x4243;

   ByteArray expected ({0x00, 0x00, 0x00,
                        0x42, 0x43, // Device Address.
                        0x00, 0x00, 0x00}
                      );
   ByteArray array (size + 6);

   size_t    wsize = message.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   message.address = 0;

   size_t rsize = message.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (0x4243, message.address);
}

// =============================================================================
// DeviceManagement::StartSessionResponse
// =============================================================================

TEST (DeviceManagement, StartSessionResponse)
{
   DeviceManagement::StartSessionResponse response;

   size_t size = response.size ();
   LONGS_EQUAL (1 + 2, size);

   response.code  = Result::FAIL_READ_SESSION;
   response.count = 0x4243;

   ByteArray expected ({0x00, 0x00, 0x00,
                        Result::FAIL_READ_SESSION, // Response code.
                        0x42, 0x43,                // Device Address.
                        0x00, 0x00, 0x00}
                      );
   ByteArray array (size + 6);

   size_t    wsize = response.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   response.code  = Result::OK;
   response.count = 0;

   size_t rsize = response.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Result::FAIL_READ_SESSION, response.code);
   LONGS_EQUAL (0x4243, response.count);
}

// =============================================================================
// DeviceManagement::GetEntriesMessage
// =============================================================================

TEST (DeviceManagement, GetEntriesMessage)
{
   DeviceManagement::GetEntriesMessage message;

   message.offset = 0x4243;
   message.count  = 0xAA;

   size_t size = message.size ();
   LONGS_EQUAL (2 + 1, size);

   ByteArray expected ({0x00, 0x00, 0x00,
                        0x42, 0x43, // Offset.
                        0xAA,       // Count.
                        0x00, 0x00, 0x00}
                      );
   ByteArray array (size + 6);

   size_t    wsize = message.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   message.offset = message.count = 0;

   size_t rsize = message.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (0x4243, message.offset);
   LONGS_EQUAL (0xAA, message.count);
}

// =============================================================================
// DeviceManagement::GetEntriesResponse
// =============================================================================

TEST (DeviceManagement, GetEntriesResponse)
{
   DeviceManagement::GetEntriesResponse response;

   DeviceManagement::Unit unit;
   DeviceManagement::Device device;

   unit.id        = 0x42;
   unit.profile   = 0x5AA5;

   device.address = 0x4321;
   device.units.push_back (unit);

   response.entries.push_back (device);

   device.address++;
   response.entries.push_back (device);

   device.address++;
   response.entries.push_back (device);

   size_t size = response.size ();
   LONGS_EQUAL (1 + 1 + device.size () * 3, size);

   response.code = Result::FAIL_READ_SESSION;

   ByteArray expected ({0x00, 0x00, 0x00,
                        Result::FAIL_READ_SESSION, // Response code.
                        0x03,                      // Number of entries.
                                                   // Device 1
                        0x43, 0x21,                //  + Device Address.
                        0x01,                      //  + Number of units.
                        0x03, 0x42, 0x5A, 0xA5,    //  + Unit 1.
                                                   // Device 2
                        0x43, 0x22,                //  + Device Address.
                        0x01,                      //  + Number of units.
                        0x03, 0x42, 0x5A, 0xA5,    //  + Unit 1.
                                                   // Device 3
                        0x43, 0x23,                //  + Device Address.
                        0x01,                      //  + Number of units.
                        0x03, 0x42, 0x5A, 0xA5,    //  + Unit 1.
                        0x00, 0x00, 0x00}
                      );
   ByteArray array (size + 6);

   size_t    wsize = response.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   DeviceManagement::GetEntriesResponse other;

   size_t rsize = other.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Result::FAIL_READ_SESSION, other.code);
   LONGS_EQUAL (3, other.entries.size ());

   for (uint8_t i = 0; i < 3; i++)
   {
      CHECK_EQUAL (response.entries[i], other.entries[i]);
   }
}

// =============================================================================
// DeviceManagementClient
// =============================================================================

TEST_GROUP (DeviceManagementClient)
{
   struct TestDeviceManagementClient:public DeviceManagement::Client
   {
      TestDeviceManagementClient(HF::Core::Unit0 &unit):
         DeviceManagement::Client (unit)
      {}

      virtual ~TestDeviceManagementClient()
      {}

      using DeviceManagement::Client::_address;

      void registered (DeviceManagement::RegisterResponse &response)
      {
         mock ("DeviceManagementClient").actualCall ("registered");
         DeviceManagement::Client::registered (response);
      }

      void deregistered (DeviceManagement::DeregisterResponse &response)
      {
         mock ("DeviceManagementClient").actualCall ("deregistered");
         DeviceManagement::Client::deregistered (response);
      }
   };

   Testing::Device *device;
   Testing::Unit   *unit1;
   Testing::Unit   *unit2;
   Testing::Unit   *unit3;

   TestDeviceManagementClient *dev_mgt;

   Protocol::Packet packet;

   TEST_SETUP ()
   {
      device                = new Testing::Device ();

      unit1                 = new Testing::Unit (1, *device);
      unit2                 = new Testing::Unit (2, *device);
      unit3                 = new Testing::Unit (3, *device);

      unit1->_uid           = 0xFF01;
      unit2->_uid           = 0xFF02;
      unit3->_uid           = 0xFF03;

      dev_mgt               = new TestDeviceManagementClient (device->unit0);

      device->unit0.dev_mgt = dev_mgt;

      mock ().ignoreOtherCalls ();

      packet                  = Protocol::Packet ();

      packet.message.type     = Protocol::Message::COMMAND_RES;
      packet.message.itf.role = HF::Interface::SERVER_ROLE;
      packet.message.itf.id   = HF::Interface::DEVICE_MANAGEMENT;
   }

   TEST_TEARDOWN ()
   {
      delete unit1;
      delete unit2;
      delete unit3;

      delete dev_mgt;

      delete device;

      mock ().clear ();
   }

};

TEST (DeviceManagementClient, RegisterMessage)
{
   mock ("AbstractDevice").expectOneCall ("send");

   dev_mgt->register_device ();

   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (1, device->packets.size ());

   Protocol::Packet *packet = device->packets.back ();

   CHECK_TRUE (packet != nullptr);

   LONGS_EQUAL (0, packet->destination.device);
   LONGS_EQUAL (0, packet->destination.unit);
   LONGS_EQUAL (Protocol::Address::DEVICE, packet->destination.mod);

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL (dev_mgt->uid (), packet->message.itf.id);
   LONGS_EQUAL (DeviceManagement::REGISTER_CMD, packet->message.itf.member);

   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, packet->message.type);

   DeviceManagement::RegisterMessage payload;
   payload.unpack (packet->message.payload);

   LONGS_EQUAL (DeviceInformation::EMC, payload.emc);

   LONGS_EQUAL (device->units ().size () - 1, payload.units.size ());

   std::vector <uint8_t> expected;
   std::vector <uint8_t> actual;

   /* *INDENT-OFF* */
   for_each (payload.units.begin (), payload.units.end (), [&actual](DeviceManagement::Unit &unit)
   {
      actual.push_back (unit.id);
   });
   /* *INDENT-ON* */

   // Unit 0 - Should not be present.
   /* *INDENT-OFF* */
   CHECK_TRUE (std::none_of (actual.begin (), actual.end (), [](uint8_t id){ return id == 0U; }));
   /* *INDENT-ON* */

   expected.push_back (unit1->id ());
   expected.push_back (unit2->id ());
   expected.push_back (unit3->id ());

   CHECK_TRUE (std::is_permutation (expected.begin (), expected.end (), actual.begin ()));

   expected.clear ();
   actual.clear ();

   /* *INDENT-OFF* */
   for_each (payload.units.begin (), payload.units.end (), [&actual](DeviceManagement::Unit &unit)
   {
      actual.push_back (unit.profile);
   });
   /* *INDENT-ON* */

   expected.push_back (unit1->uid ());
   expected.push_back (unit2->uid ());
   expected.push_back (unit3->uid ());

   CHECK_TRUE (is_permutation (expected.begin (), expected.end (), actual.begin ()));
}

TEST (DeviceManagementClient, RegisterMessage_EMC)
{
   mock ("AbstractDevice").expectOneCall ("send");

   dev_mgt->register_device ();

   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (1, device->packets.size ());

   Protocol::Packet *packet = device->packets.back ();

   DeviceManagement::RegisterMessage payload;
   payload.unpack (packet->message.payload);

   LONGS_EQUAL (0x1234, payload.emc);
}

TEST (DeviceManagementClient, RegisterResponse_OK)
{
   ByteArray payload ({0x00, 0x00, 0x00,
                       Result::OK, // Response Code.
                       0x42, 0x43, // Device Address.
                       0x00, 0x00, 0x00}
                     );

   packet.message.length     = payload.size ();
   packet.message.itf.member = DeviceManagement::REGISTER_CMD;

   mock ("DeviceManagementClient").expectOneCall ("registered");

   Result result = dev_mgt->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementClient").checkExpectations ();

   LONGS_EQUAL (0x4243, dev_mgt->address ());
}

TEST (DeviceManagementClient, RegisterResponse_FAIL)
{
   ByteArray payload ({0x00, 0x00, 0x00,
                       Result::FAIL_RESOURCES, // Response Code.
                       0x00, 0x00, 0x00}
                     );

   packet.message.length     = payload.size ();
   packet.message.itf.member = DeviceManagement::REGISTER_CMD;

   mock ("DeviceManagementClient").expectOneCall ("registered");

   Result result = dev_mgt->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementClient").checkExpectations ();

   LONGS_EQUAL (Protocol::BROADCAST_ADDR, device->address ());
}

TEST (DeviceManagementClient, DeregisterMessage)
{
   mock ("AbstractDevice").expectOneCall ("send");

   dev_mgt->deregister (0x5A5A);

   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (1, device->packets.size ());

   Protocol::Packet *packet = device->packets.back ();

   CHECK_TRUE (packet != nullptr);

   LONGS_EQUAL (0, packet->destination.device);
   LONGS_EQUAL (0, packet->destination.unit);
   LONGS_EQUAL (Protocol::Address::DEVICE, packet->destination.mod);

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL (dev_mgt->uid (), packet->message.itf.id);
   LONGS_EQUAL (DeviceManagement::DEREGISTER_CMD, packet->message.itf.member);

   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, packet->message.type);

   DeviceManagement::DeregisterMessage payload;
   payload.unpack (packet->message.payload);

   LONGS_EQUAL (0x5A5A, payload.address);
}

TEST (DeviceManagementClient, DeregisterResponse_OK)
{
   ByteArray payload ({0x00, 0x00, 0x00,
                       Result::OK, // Response Code.
                       0x00, 0x00, 0x00}
                     );

   packet.message.length     = payload.size ();
   packet.message.itf.member = DeviceManagement::DEREGISTER_CMD;

   mock ("DeviceManagementClient").expectOneCall ("deregistered");

   Result result = dev_mgt->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementClient").checkExpectations ();

   LONGS_EQUAL (Protocol::BROADCAST_ADDR, device->address ());
}

TEST (DeviceManagementClient, DeregisterResponse_FAIL)
{
   ByteArray payload ({0x00, 0x00, 0x00,
                       Result::FAIL_AUTH, // Response Code.
                       0x00, 0x00, 0x00}
                     );

   packet.message.length     = payload.size ();
   packet.message.itf.member = DeviceManagement::DEREGISTER_CMD;

   dev_mgt->_address         = 0x5A5A;

   mock ("DeviceManagementClient").expectOneCall ("deregistered");

   // This needs to use a temporary variable
   // otherwise the handle method will be called twice.
   Result result = dev_mgt->handle (packet, payload, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementClient").checkExpectations ();

   LONGS_EQUAL (0x5A5A, dev_mgt->address ());
}

// =============================================================================
// DeviceManagementServer
// =============================================================================

#define CHECK_DEVICE_ADDRESS(_expected, _current, _index) \
   check_index <uint16_t>(_expected, _current, _index, "Device Address", __FILE__, __LINE__)

TEST_GROUP (DeviceManagementServer)
{
   Testing::Concentrator *device;

   struct TestDeviceManagementServer:public DeviceManagement::DefaultServer
   {
      TestDeviceManagementServer(HF::Core::Unit0 &unit):
         DeviceManagement::DefaultServer (unit)
      {}

      Result register_device (Protocol::Packet &packet, ByteArray &payload, size_t offset)
      {
         mock ("DeviceManagementServer").actualCall ("register_device");
         return DeviceManagement::DefaultServer::register_device (packet, payload, offset);
      }

      Result deregister_device (Protocol::Packet &packet, ByteArray &payload, size_t offset)
      {
         mock ("DeviceManagementServer").actualCall ("deregister_device");
         return DeviceManagement::DefaultServer::deregister_device (packet, payload, offset);
      }

      using DeviceManagement::DefaultServer::save;
   };

   TestDeviceManagementServer *dev_mgt;

   Protocol::Packet packet;

   Testing::Link    *link;

   TEST_SETUP ()
   {
      device                    = new Testing::Concentrator ();

      dev_mgt                   = new TestDeviceManagementServer (device->unit0);

      device->unit0.dev_mgt     = dev_mgt;

      device->unit0.bind_mgt    = new HF::Core::BindManagement::Server (device->unit0);

      packet.destination.device = 0;
      packet.destination.unit   = 0;

      packet.source.device      = Protocol::BROADCAST_ADDR;
      packet.source.unit        = Protocol::BROADCAST_UNIT;

      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = dev_mgt->uid ();

      UID::UID_T *uid = new UID::URI ("hf://device@example.com");

      link        = new Testing::Link (uid, nullptr);
      packet.link = link;

      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      delete device->unit0.bind_mgt;

      delete dev_mgt;

      delete device;

      delete link;

      mock ().clear ();
   }
};

TEST (DeviceManagementServer, Handle_Register)
{
   ByteArray expected = ByteArray {0x00, 0x00, 0x00,
                                   0x02,                          // Discriminator Type.
                                   0x05,                          // Size of UID.
                                   0x00, 0x73, 0x70, 0xAA, 0xBB,  // IPUI.
                                   0x03,                          // Number of units.
                                   0x03, 0x42, 0x5A, 0xA5,        // Unit 1.
                                   0x03, 0x42, 0x5A, 0xA5,        // Unit 2.
                                   0x03, 0x42, 0x5A, 0xA5,        // Unit 3.
                                   0x00, 0x00, 0x00};

   packet.message.itf.member = DeviceManagement::REGISTER_CMD;
   packet.message.length     = expected.size ();

   mock ("AbstractDevice").expectOneCall ("send");
   mock ("DeviceManagementServer").expectOneCall ("register_device");

   Result result = dev_mgt->handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementServer").checkExpectations ();
   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (1, dev_mgt->entries_count ());

   // Check response packet destination address.
   LONGS_EQUAL (1, device->packets.size ());

   Protocol::Packet *response = device->packets.back ();

   CHECK_TRUE (response != nullptr);

   LONGS_EQUAL (1, response->destination.device);
   LONGS_EQUAL (0, response->destination.unit);
   LONGS_EQUAL (Protocol::Address::DEVICE, response->destination.mod);

   // Should not add entry for same device.
   mock ("AbstractDevice").expectOneCall ("send");
   mock ("DeviceManagementServer").expectOneCall ("register_device");

   result = dev_mgt->handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementServer").checkExpectations ();
   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (1, dev_mgt->entries_count ());

   // Should add entry for other device.

   delete link;

   UID::UID_T *uid = new UID::URI ("hf://device2@example.com");

   link        = new Testing::Link (uid, nullptr);
   packet.link = link;

   mock ("AbstractDevice").expectOneCall ("send");
   mock ("DeviceManagementServer").expectOneCall ("register_device");

   result = dev_mgt->handle (packet, expected, 3);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementServer").checkExpectations ();
   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (2, dev_mgt->entries_count ());
}

TEST (DeviceManagementServer, Handle_Deregister)
{
   for (uint8_t i = 0; i < 20; i++)
   {
      DeviceManagement::Device *dev = new DeviceManagement::Device ();
      dev->address = 0x5A50 + i;
      std::ostringstream uri;
      uri << "hf://device" << i << "@example.com";
      dev->uid = new UID::URI (uri.str ());
      dev_mgt->save (dev);
   }

   size_t size = dev_mgt->entries_count ();

   packet.source.device = 0x5A51;

   DeviceManagement::DeregisterMessage message (0x5A5A);

   ByteArray expected (message.size ());

   message.pack (expected);

   packet.message.itf.member = DeviceManagement::DEREGISTER_CMD;
   packet.message.length     = expected.size ();

   mock ("DeviceManagementServer").expectOneCall ("deregister_device");

   Result result = dev_mgt->handle (packet, expected, 0);
   CHECK_EQUAL (Result::FAIL_AUTH, result);

   mock ("DeviceManagementServer").checkExpectations ();

   LONGS_EQUAL (size, dev_mgt->entries_count ());

   packet.source.device = 0x5A5A;

   mock ("AbstractDevice").expectOneCall ("send");
   mock ("DeviceManagementServer").expectOneCall ("deregister_device");

   result = dev_mgt->handle (packet, expected, 0);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementServer").checkExpectations ();
   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (size - 1, dev_mgt->entries_count ());
}

TEST (DeviceManagementServer, Handle_Deregister_With_Bindings)
{
   // Set-up device entries.
   for (uint8_t i = 0; i < 20; i++)
   {
      DeviceManagement::Device *dev = new DeviceManagement::Device ();
      dev->address = 0x5A50 + i;
      std::ostringstream uri;
      uri << "hf://device" << i << "@example.com";
      dev->uid = new UID::URI (uri.str ());
      uint16_t profile = (uint16_t) (i % 2 == 0 ? Profiles::SIMPLE_ONOFF_SWITCH : Profiles::SIMPLE_ONOFF_SWITCHABLE);
      dev->units.push_back (DeviceManagement::Unit (i + 1, profile));
      dev_mgt->save (dev);
   }

   // == Set-up some bindings. ==
   /*
    * This setups 4 binding entries, where 3 are for the device we are unregistering.
    */

   Protocol::Address src (0x5A52, 3);
   Common::Interface itf (HF::Interface::ON_OFF, HF::Interface::SERVER_ROLE);
   Protocol::Address dst (0x5A53, 4);

   auto bind_res = device->unit0.bind_management ()->add (src, dst, itf);

   CHECK_EQUAL (Result::OK, bind_res.first);

   dst      = Protocol::Address (0x5A55, 6);

   bind_res = device->unit0.bind_management ()->add (src, dst, itf);

   CHECK_EQUAL (Result::OK, bind_res.first);

   dst      = Protocol::Address (0x5A57, 8);

   bind_res = device->unit0.bind_management ()->add (src, dst, itf);

   CHECK_EQUAL (Result::OK, bind_res.first);

   src      = Protocol::Address (0x5A54, 5);
   dst      = Protocol::Address (0x5A53, 4);

   bind_res = device->unit0.bind_management ()->add (src, dst, itf);

   CHECK_EQUAL (Result::OK, bind_res.first);

   LONGS_EQUAL (4, device->unit0.bind_management ()->entries.size ());

   // == De-register the device.

   size_t size = dev_mgt->entries_count ();

   packet.source.device = 0x5A51;

   DeviceManagement::DeregisterMessage message (0x5A52);

   ByteArray expected (message.size ());

   message.pack (expected);

   packet.message.itf.member = DeviceManagement::DEREGISTER_CMD;
   packet.message.length     = expected.size ();

   mock ("DeviceManagementServer").expectOneCall ("deregister_device");

   Result result = dev_mgt->handle (packet, expected, 0);
   CHECK_EQUAL (Result::FAIL_AUTH, result);

   mock ("DeviceManagementServer").checkExpectations ();

   LONGS_EQUAL (4, device->unit0.bind_management ()->entries.size ());

   LONGS_EQUAL (size, dev_mgt->entries_count ());

   packet.source.device = 0x5A52;

   mock ("AbstractDevice").expectOneCall ("send");
   mock ("DeviceManagementServer").expectOneCall ("deregister_device");

   result = dev_mgt->handle (packet, expected, 0);
   CHECK_EQUAL (Result::OK, result);

   mock ("DeviceManagementServer").checkExpectations ();
   mock ("AbstractDevice").checkExpectations ();

   LONGS_EQUAL (size - 1, dev_mgt->entries_count ());

   LONGS_EQUAL (1, device->unit0.bind_management ()->entries.size ());
}

TEST (DeviceManagementServer, Entries)
{
   UID::IPUI ipui;

   ipui[0] = 0x12;
   ipui[1] = 0x34;
   ipui[2] = 0x56;
   ipui[3] = 0x78;
   ipui[4] = 0x90;

   for (int i = 0; i < 20; i++)
   {
      DeviceManagement::Device *dev = new DeviceManagement::Device ();
      dev->address = i + 1;

      UID::IPUI *temp = new UID::IPUI (ipui);
      (*temp)[4] += i;

      dev->uid    = temp;

      dev_mgt->save (dev);
   }

   LONGS_EQUAL (20, dev_mgt->entries_count ());

   std::vector <DeviceManagement::Device *> entries = dev_mgt->entries ();

   LONGS_EQUAL (dev_mgt->entries_count (), entries.size ());

   entries = dev_mgt->entries (14);

   LONGS_EQUAL (6, entries.size ());

   entries = dev_mgt->entries (16, 10);

   LONGS_EQUAL (4, entries.size ());

   entries = dev_mgt->entries (5, 10);

   LONGS_EQUAL (10, entries.size ());

   LONGS_EQUAL (entries.front ()->address + 10 - 1, entries.back ()->address);

   for (uint16_t i = 0; i < entries.size (); i++)
   {
      CHECK_DEVICE_ADDRESS (6 + i, entries[i]->address, i);
   }
}
