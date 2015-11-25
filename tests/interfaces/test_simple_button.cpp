// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_button.cpp
 *
 * This is file contains the unit tests for the Simple Button interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_button.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::SimpleButton;

// =============================================================================
// Simple Button
// =============================================================================

//! Test Group for SimpleButton interface parent class.
TEST_GROUP(SimpleButton)
{
   class SimpleButtonBase: public InterfaceParentHelper<SimpleButton::Base>
   {};

   SimpleButtonBase interface;

   TEST_SETUP()
   {
      interface = SimpleButtonBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test SimpleButton::uid should return @c HF::Interface::SIMPLE_BUTTON.
TEST(SimpleButton, UID)
{
   LONGS_EQUAL(HF::Interface::SIMPLE_BUTTON, interface.uid());
}

// =============================================================================
// Simple Button Client
// =============================================================================

//! Test Group for Simple Button Client interface class.
TEST_GROUP(SimpleButtonClient)
{
   // TODO Add required unit tests.
   struct SimpleButtonClient: public InterfaceHelper<SimpleButton::Client>
   {
      void short_press(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleButton::Client").actualCall("short_press");
      }

      void long_press(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleButton::Client").actualCall("long_press");
      }

      void extra_long_press(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleButton::Client").actualCall("extra_long_press");
      }

      void double_click_press(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleButton::Client").actualCall("double_click_press");
      }

   };

   SimpleButtonClient client;
   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      client                    = SimpleButtonClient();
      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
      packet.message.itf.id     = client.uid();
      packet.message.itf.member = 0xFF;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Short Press support.
TEST(SimpleButtonClient, ShortPress)
{
   // FIXME Generated Stub.
   mock("SimpleButton::Client").expectOneCall("short_press");

   packet.message.itf.member = SimpleButton::SHORT_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

//! @test Long Press support.
TEST(SimpleButtonClient, LongPress)
{
   // FIXME Generated Stub.
   mock("SimpleButton::Client").expectOneCall("long_press");

   packet.message.itf.member = SimpleButton::LONG_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

//! @test Extra Long Press support.
TEST(SimpleButtonClient, ExtraLongPress)
{
   // FIXME Generated Stub.
   mock("SimpleButton::Client").expectOneCall("extra_long_press");

   packet.message.itf.member = SimpleButton::EXTRA_LONG_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

//! @test Double Click Press support.
TEST(SimpleButtonClient, DoubleClickPress)
{
   // FIXME Generated Stub.
   mock("SimpleButton::Client").expectOneCall("double_click_press");

   packet.message.itf.member = SimpleButton::DOUBLE_CLICK_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

// =============================================================================
// Simple Button Server
// =============================================================================

//! Test Group for Simple Button Server interface class.
TEST_GROUP(SimpleButtonServer)
{
   // TODO Add required unit tests.
   struct SimpleButtonServer: public InterfaceHelper<SimpleButton::Server>
   {};

   SimpleButtonServer server;
   Protocol::Address addr;


   TEST_SETUP()
   {
      server = SimpleButtonServer();
      addr   = Protocol::Address(42);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Short Press Max Duration support.
TEST(SimpleButtonServer, ShortPressMaxDuration)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleButtonServer, ShortPressMaxDuration, true, short_press_max_duration, 42,
                   -42);
}

//! @test Extra Long Press Min Duration support.
TEST(SimpleButtonServer, ExtraLongPressMinDuration)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleButtonServer, ExtraLongPressMinDuration, true,
                   extra_long_press_min_duration, 42, -42);
}

//! @test Double Click Gap Duration support.
TEST(SimpleButtonServer, DoubleClickGapDuration)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(SimpleButtonServer, DoubleClickGapDuration, true, double_click_gap_duration,
                       42, -42);
}

//! @test Short Press support.
TEST(SimpleButtonServer, ShortPress)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   server.short_press(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SimpleButton::SHORT_PRESS_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);
}

//! @test Long Press support.
TEST(SimpleButtonServer, LongPress)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   server.long_press(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SimpleButton::LONG_PRESS_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);
}

//! @test Extra Long Press support.
TEST(SimpleButtonServer, ExtraLongPress)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   server.extra_long_press(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SimpleButton::EXTRA_LONG_PRESS_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);
}

//! @test Double Click Press support.
TEST(SimpleButtonServer, DoubleClickPress)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   server.double_click_press(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SimpleButton::DOUBLE_CLICK_PRESS_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);
}
