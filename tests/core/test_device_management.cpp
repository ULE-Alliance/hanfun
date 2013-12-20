// =============================================================================
/*!
 * \file			tests/core/test_device_management.cpp
 *
 * This file contains the implementation of the Simple Power Meter interface.
 *
 * \author		Filipe Alves <filipe.alves@bithium.com>
 *
 * \version		x.x.x
 *
 * \copyright	Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/core/device_management.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

TEST_GROUP (DeviceManagement)
{};

// =============================================================================
// DeviceManagement::Interface
// =============================================================================

TEST (DeviceManagement, InterfaceServer)
{
   DeviceManagement::Interface itf;

   size_t size = itf.size ();
   LONGS_EQUAL (sizeof(uint16_t), size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0xFA, 0xAA, // Optional Interface : Server Role.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   itf.role = Interface::SERVER_ROLE;
   itf.uid  = 0x7AAA;

   size_t wsize = itf.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   memset (&itf, 0xDD, sizeof(itf));

   size_t rsize = itf.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Interface::SERVER_ROLE, itf.role);
   LONGS_EQUAL (0x7AAA, itf.uid);
};

TEST (DeviceManagement, InterfaceClient)
{
   DeviceManagement::Interface itf;

   size_t size = itf.size ();
   LONGS_EQUAL (sizeof(uint16_t), size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x75, 0x55, // Optional Interface : Client Role.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   itf.role = Interface::CLIENT_ROLE;
   itf.uid  = 0x7555;

   size_t wsize = itf.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   memset (&itf, 0xDD, sizeof(itf));

   size_t rsize = itf.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Interface::CLIENT_ROLE, itf.role);
   LONGS_EQUAL (0x7555, itf.uid);
};

// =============================================================================
// DeviceManagement::Unit
// =============================================================================

TEST (DeviceManagement, Unit_No_Optional_Itf)
{
   DeviceManagement::Unit wunit (0x42, 0x5AA5);
   DeviceManagement::Unit runit;

   size_t size = wunit.size ();
   LONGS_EQUAL (1 + 1 + 2, size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x03,       // Unit entry size.
                     0x42,       // Unit ID.
                     0x5A, 0xA5, // Unit UID.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
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

   DeviceManagement::Interface itf1 (HF::Interface::SERVER_ROLE, 0x5432);
   DeviceManagement::Interface itf2 (HF::Interface::CLIENT_ROLE, 0x1234);
   DeviceManagement::Interface itf3 (HF::Interface::SERVER_ROLE, 0x5678);

   wunit.opt_ift.push_back (itf1);
   wunit.opt_ift.push_back (itf2);
   wunit.opt_ift.push_back (itf3);

   size_t size = wunit.size ();
   LONGS_EQUAL (1 + 1 + 2 + 1 + itf1.size () + itf2.size () + itf3.size (), size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x0A,       // Unit entry size.
                     0x42,       // Unit ID.
                     0x5A, 0xA5, // Unit UID.
                     0x03,       // Number of optional interfaces.
                     0xD4, 0x32, // Optional interface 1
                     0x12, 0x34, // Optional interface 2
                     0xD6, 0x78, // Optional interface 3
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
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

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x33, 0x33,            // Device Address.
                     0x03,                  // Number of units.
                     0x03, 0x42, 0x5A,0xA5, // Unit 1.
                     0x03, 0x42, 0x5A,0xA5, // Unit 2.
                     0x03, 0x42, 0x5A,0xA5, // Unit 3.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   size_t    wsize = device.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   device = DeviceManagement::Device ();

   LONGS_EQUAL (0x0, device.address);
   LONGS_EQUAL (0, device.units.size ());

   size_t rsize = device.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (0x3333, device.address);
   LONGS_EQUAL (3, device.units.size ());

   for (auto _unit : device.units)
   {
      CHECK_EQUAL (unit, _unit);
   }
}

// =============================================================================
// DeviceManagement::RegisterMessage
// =============================================================================

TEST_GROUP (DeviceManagement_RegisterMessage)
{
   DeviceManagement::RegisterMessage *message;
   DeviceManagement::Unit unit;

   ByteArray expected;
   IPUI *ipui;

   TEST_SETUP ()
   {
      message      = new DeviceManagement::RegisterMessage ();

      unit.id      = 0x42;
      unit.profile = 0x5AA5;

      message->units.push_back (unit);
      message->units.push_back (unit);
      message->units.push_back (unit);

      ipui           = new IPUI ();

      ipui->value[0] = 0x00;
      ipui->value[1] = 0x73;
      ipui->value[2] = 0x70;
      ipui->value[3] = 0xAA;
      ipui->value[4] = 0xBB;

      message->uid (ipui);
   }

   TEST_TEARDOWN ()
   {
      delete message;
      delete ipui;
   }
};

TEST (DeviceManagement_RegisterMessage, No_EMC)
{
   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x01,                         // Discriminator Type.
                     0x05,                         // Size of UID.
                     0x00, 0x73, 0x70,0xAA,  0xBB, // IPUI.
                     0x03,                         // Number of units.
                     0x03, 0x42, 0x5A,0xA5,        // Unit 1.
                     0x03, 0x42, 0x5A,0xA5,        // Unit 2.
                     0x03, 0x42, 0x5A,0xA5,        // Unit 3.
                     0x00, 0x00, 0x00};

   expected = ByteArray (data, sizeof(data));

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

   CHECK_EQUAL (*ipui, *message->uid ());

   CHECK_EQUAL (3, message->units.size ());

   for (auto _unit : message->units)
   {
      CHECK_EQUAL (unit, _unit);
   }
}

TEST (DeviceManagement_RegisterMessage, EMC)
{
   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x81,                         // Discriminator Type.
                     0x05,                         // Size of UID.
                     0x00, 0x73, 0x70,0xAA,  0xBB, // IPUI.
                     0x42, 0x43,                   // EMC.
                     0x03,                         // Number of units.
                     0x03, 0x42, 0x5A,0xA5,        // Unit 1.
                     0x03, 0x42, 0x5A,0xA5,        // Unit 2.
                     0x03, 0x42, 0x5A,0xA5,        // Unit 3.
                     0x00, 0x00, 0x00};

   expected     = ByteArray (data, sizeof(data));

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

   CHECK_EQUAL (*ipui, *message->uid ());

   CHECK_EQUAL (3, message->units.size ());

   for (auto _unit : message->units)
   {
      CHECK_EQUAL (unit, _unit);
   }
}

// =============================================================================
// DeviceManagement::RegisterResponce
// =============================================================================

TEST_GROUP (DeviceManagement_RegisterResponce)
{};

TEST (DeviceManagement_RegisterResponce, No_EMC)
{
   uint8_t data[] = {0x00, 0x00, 0x00,
                     Protocol::Response::FAIL_AUTH, // Responce Code.
                     0x42, 0x43,                    // Device Address.
                     0x00, 0x00, 0x00};

   ByteArray expected = ByteArray (data, sizeof(data));

   DeviceManagement::RegisterResponse response;

   response.code    = Protocol::Response::FAIL_AUTH;
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

   LONGS_EQUAL (Protocol::Response::FAIL_AUTH, response.code);
   LONGS_EQUAL (0x4243, response.address);
}

TEST (DeviceManagement_RegisterResponce, EMC)
{
   uint8_t data[] = {0x00, 0x00, 0x00,
                     Protocol::Response::FAIL_AUTH, // Responce Code.
                     0xC2, 0x43,                    // Device Address.
                     0xAA, 0xBB,                    // EMC
                     0x00, 0x00, 0x00};

   ByteArray expected = ByteArray (data, sizeof(data));

   DeviceManagement::RegisterResponse response;

   response.code    = Protocol::Response::FAIL_AUTH;
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

   LONGS_EQUAL (Protocol::Response::FAIL_AUTH, response.code);
   LONGS_EQUAL (0x4243, response.address);
   LONGS_EQUAL (0xAABB, response.emc);
};


// =============================================================================
// DeviceManagement::DeregisterMessage
// =============================================================================

TEST (DeviceManagement, DeregisterMessage)
{
   DeviceManagement::DeregisterMessage message;

   size_t size = message.size ();
   LONGS_EQUAL (sizeof(uint16_t), size);

   message.address = 0x4243;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x42, 0x43, // Device Address.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
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

   response.code  = Protocol::Response::FAIL_READ_SESSION;
   response.count = 0x4243;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     Protocol::Response::FAIL_READ_SESSION, // Response code.
                     0x42, 0x43,                            // Device Address.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   size_t    wsize = response.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   response.code  = Protocol::Response::OK;
   response.count = 0;

   size_t rsize = response.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Protocol::Response::FAIL_READ_SESSION, response.code);
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

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x42, 0x43, // Offset.
                     0xAA,       // Count.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
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

   response.code = Protocol::Response::FAIL_READ_SESSION;

   uint8_t data[] = {0x00, 0x00, 0x00,
                     Protocol::Response::FAIL_READ_SESSION, // Response code.
                     0x03,                                  // Number of entries.
                                                            // Device 1
                     0x43, 0x21,                            //  + Device Address.
                     0x01,                                  //  + Number of units.
                     0x03, 0x42, 0x5A,0xA5,                 //  + Unit 1.
                                                            // Device 2
                     0x43, 0x22,                            //  + Device Address.
                     0x01,                                  //  + Number of units.
                     0x03, 0x42, 0x5A,0xA5,                 //  + Unit 1.
                                                            // Device 3
                     0x43, 0x23,                            //  + Device Address.
                     0x01,                                  //  + Number of units.
                     0x03, 0x42, 0x5A,0xA5,                 //  + Unit 1.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   size_t    wsize = response.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   DeviceManagement::GetEntriesResponse other;

   size_t rsize = other.unpack (expected, 3);
   LONGS_EQUAL (size, rsize);

   LONGS_EQUAL (Protocol::Response::FAIL_READ_SESSION, other.code);
   LONGS_EQUAL (3, other.entries.size ());

   for (uint8_t i = 0; i < 3; i++)
   {
      CHECK_EQUAL (response.entries[i], other.entries[i]);
   }
}