// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_keypad.cpp
 *
 * This is file contains the unit tests for the Simple Keypad interface
 * implementation.
 *
 * @version    x.x.x
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

// =============================================================================
// Simple Keypad Client
// =============================================================================

//! Test Group for Simple Keypad Client interface class.
TEST_GROUP(SimpleKeypadClient)
{
   // TODO Add required unit tests.
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
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.keypressed(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleKeypad::KEYPRESSED_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// Simple Keypad Server
// =============================================================================

//! Test Group for Simple Keypad Server interface class.
TEST_GROUP(SimpleKeypadServer)
{
   // TODO Add required unit tests.
   struct SimpleKeypadServer: public InterfaceHelper<SimpleKeypad::Server>
   {
      void keypressed(const Protocol::Address &addr) override
      {
         mock("SimpleKeypad::Server").actualCall("keypressed");
         InterfaceHelper<SimpleKeypad::Server>::keypressed(addr);
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
   // FIXME Generated Stub.
   mock("SimpleKeypad::Server").expectOneCall("keypressed");

   packet.message.itf.member = SimpleKeypad::KEYPRESSED_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleKeypad::Server").checkExpectations();
}
