// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_keypad.cpp
 *
 * This is file contains the unit tests for the Simple Keypad interface
 * implementation.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_keypad.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::SimpleKeypad;

// =============================================================================
// Simple Keypad
// =============================================================================

//! Test Group for SimpleKeypad interface parent class.
TEST_GROUP(SimpleKeypad)
{
   struct SimpleKeypadBase: public InterfaceParentHelper<SimpleKeypad::Base>
   {};

   SimpleKeypadBase interface;

   TEST_SETUP()
   {
      interface = SimpleKeypadBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test SimpleKeypad::uid should return @c HF::Interface::SIMPLE_KEYPAD.
TEST(SimpleKeypad, UID)
{
   LONGS_EQUAL(HF::Interface::SIMPLE_KEYPAD, interface.uid());
}

//! Test Group for SimpleKeypad message class.
TEST_GROUP(SimpleKeypadMessages)
{
   Common::ByteArray expected;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      expected = Common::ByteArray();
      payload  = Common::ByteArray();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test KeyPressed::size.
TEST(SimpleKeypadMessages, KeyPressed_size)
{
   KeyPressed key;

   expected = Common::ByteArray({
      0x00, 0x00, 0x00, 0x00
   });

   UNSIGNED_LONGS_EQUAL(expected.size(), key.size());
   UNSIGNED_LONGS_EQUAL(key.min_size, key.size());
   UNSIGNED_LONGS_EQUAL(4, key.size());
}

//! @test KeyPressed::pack.
TEST(SimpleKeypadMessages, KeyPressed_pack)
{
   KeyPressed key(0x01234567);

   payload  = Common::ByteArray(key.size());

   expected = Common::ByteArray({
      0x01, 0x23, 0x45, 0x67
   });

   LONGS_EQUAL(expected.size(), key.pack(payload));
   CHECK_EQUAL(expected, payload);
}

//! @test KeyPressed::pack. Should fail because the payload container doesn't have the correct size.
TEST(SimpleKeypadMessages, KeyPressed_pack_fail_size)
{
   KeyPressed key(0x01234567);

   payload = Common::ByteArray(key.size() - 1);

   LONGS_EQUAL(0, key.pack(payload));
}

//! @test KeyPressed::unpack.
TEST(SimpleKeypadMessages, KeyPressed_unpack)
{
   KeyPressed key;

   payload = Common::ByteArray({
      0x01, 0x23, 0x45, 0x67
   });

   LONGS_EQUAL(payload.size(), key.unpack(payload));
   CHECK_EQUAL(0x01234567, key.key_id);
}

//! @test KeyPressed::unpack. Missing one octet from the message.
TEST(SimpleKeypadMessages, KeyPressed_unpack_fail_missing_octet)
{
   KeyPressed key;

   payload = Common::ByteArray({
      0x01, 0x23, 0x45
   });

   LONGS_EQUAL(0, key.unpack(payload));
   CHECK_EQUAL(0x00000000, key.key_id);
}

// =============================================================================
// Simple Keypad Client
// =============================================================================

//! Test Group for Simple Keypad Client interface class.
TEST_GROUP(SimpleKeypadClient)
{
   struct SimpleKeypadClient: public InterfaceHelper<SimpleKeypad::Client>
   {};

   SimpleKeypadClient client;

   Protocol::Address addr;


   TEST_SETUP()
   {
      client = SimpleKeypadClient();

      addr   = Protocol::Address(42);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Keypressed support.
TEST(SimpleKeypadClient, Keypressed)
{
   mock("Interface").expectOneCall("send");

   client.key_pressed(addr, 0x00001111);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleKeypad::KEYPRESSED_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   KeyPressed sent;
   sent.unpack(client.sendMsg.payload);
   UNSIGNED_LONGS_EQUAL(0x00001111, sent.key_id);
}

// =============================================================================
// Simple Keypad Server
// =============================================================================

//! Test Group for Simple Keypad Server interface class.
TEST_GROUP(SimpleKeypadServer)
{
   struct SimpleKeypadServer: public InterfaceHelper<SimpleKeypad::Server>
   {
      void key_pressed(const Protocol::Address &addr, KeyPressed &msg) override
      {
         mock("SimpleKeypad::Server").actualCall("key_pressed");
         InterfaceHelper<SimpleKeypad::Server>::key_pressed(addr, msg);
      }

      void key_received(const uint32_t key_id) override
      {
         mock("SimpleKeypad::Server")
            .actualCall("key_received")
            .withParameter("key_id", key_id);
      }
   };

   SimpleKeypadServer server;


   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      server                    = SimpleKeypadServer();

      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server.uid();
      packet.message.itf.member = 0xFF;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Keypressed support.
TEST(SimpleKeypadServer, Keypressed)
{
   KeyPressed received(0x00001111);

   payload = Common::ByteArray(received.size());
   received.pack(payload);

   mock("SimpleKeypad::Server").expectOneCall("key_pressed");
   mock("SimpleKeypad::Server").expectOneCall("key_received")
         .withParameter("key_id", 0x00001111);

   packet.message.itf.member = SimpleKeypad::KEYPRESSED_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 0));

   mock("SimpleKeypad::Server").checkExpectations();
}
