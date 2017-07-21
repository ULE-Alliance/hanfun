// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_button.cpp
 *
 * This is file contains the unit tests for the Simple Button interface
 * implementation.
 *
 * @version    1.4.3
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
   mock("SimpleButton::Client").expectOneCall("short_press");

   packet.message.itf.member = SimpleButton::SHORT_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

//! @test Long Press support.
TEST(SimpleButtonClient, LongPress)
{
   mock("SimpleButton::Client").expectOneCall("long_press");

   packet.message.itf.member = SimpleButton::LONG_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

//! @test Extra Long Press support.
TEST(SimpleButtonClient, ExtraLongPress)
{
   mock("SimpleButton::Client").expectOneCall("extra_long_press");

   packet.message.itf.member = SimpleButton::EXTRA_LONG_PRESS_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SimpleButton::Client").checkExpectations();
}

//! @test Double Click Press support.
TEST(SimpleButtonClient, DoubleClickPress)
{
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
   struct SimpleButtonServer: public InterfaceHelper<SimpleButton::Server>
   {
      using InterfaceHelper<SimpleButton::Server>::short_press;
      using InterfaceHelper<SimpleButton::Server>::long_press;
      using InterfaceHelper<SimpleButton::Server>::extra_long_press;
      using InterfaceHelper<SimpleButton::Server>::double_click_press;
   };

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

   void check_message(SimpleButton::CMD cmd, const char *file, int line)
   {
      LONGS_EQUAL_LOCATION(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role, NULL, file, line);
      LONGS_EQUAL_LOCATION(server.uid(), server.sendMsg.itf.id, NULL, file, line);
      LONGS_EQUAL_LOCATION(cmd, server.sendMsg.itf.member, NULL, file, line);
      LONGS_EQUAL_LOCATION(Protocol::Message::COMMAND_REQ, server.sendMsg.type, NULL, file, line);
   }
};

#define CHECK_MESSAGE(_cmd)   check_message(_cmd, __FILE__, __LINE__)

//! @test Short Press Max Duration support.
TEST(SimpleButtonServer, ShortPressMaxDuration)
{
   CHECK_ATTRIBUTE(SimpleButtonServer, ShortPressMaxDuration, true,
                   short_press_max_duration, 42, 142);

   mock("Interface").expectNCalls(3, "notify");

   server.extra_long_press_min_duration(200);
   server.short_press_max_duration(242);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(242, server.extra_long_press_min_duration());

   mock("Interface").expectNCalls(2, "notify");

   server.extra_long_press_min_duration(0);
   server.short_press_max_duration(42);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(0, server.extra_long_press_min_duration());
}

//! @test Extra Long Press Min Duration support.
TEST(SimpleButtonServer, ExtraLongPressMinDuration)
{
   CHECK_ATTRIBUTE(SimpleButtonServer, ExtraLongPressMinDuration, true,
                   extra_long_press_min_duration, 42, 142);

   mock("Interface").expectNCalls(2, "notify");

   server.extra_long_press_min_duration(200);

   server.short_press_max_duration(100);

   mock("Interface").checkExpectations();

   // Check lower bound.
   mock("support").expectOneCall("assert").ignoreOtherParameters();

   server.extra_long_press_min_duration(server.short_press_max_duration() - 1);

   mock("support").checkExpectations();

   LONGS_EQUAL(200, server.extra_long_press_min_duration());

   // Disable lower bound.
   mock("Interface").expectOneCall("notify");

   server.extra_long_press_min_duration(0);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(0, server.extra_long_press_min_duration());
}

//! @test Double Click Gap Duration support.
TEST(SimpleButtonServer, DoubleClickGapDuration)
{
   CHECK_OPT_ATTRIBUTE(SimpleButtonServer, DoubleClickGapDuration, true,
                       double_click_gap_duration, 0x100, 0x200);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   server.double_click_gap_duration(Server::DOUBLE_CLICK_GAP_DURATION_MIN_VALUE - 1);

   mock("support").checkExpectations();
}

//! @test Short Press support.
TEST(SimpleButtonServer, ShortPress)
{
   mock("Interface").expectOneCall("send");

   server.short_press(addr);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::SHORT_PRESS_CMD);
}

//! @test Long Press support.
TEST(SimpleButtonServer, LongPress)
{
   mock("Interface").expectOneCall("send");

   server.long_press(addr);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::LONG_PRESS_CMD);
}

//! @test Extra Long Press support.
TEST(SimpleButtonServer, ExtraLongPress)
{
   mock("Interface").expectOneCall("send");

   server.extra_long_press(addr);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::EXTRA_LONG_PRESS_CMD);
}

//! @test Double Click Press support.
TEST(SimpleButtonServer, DoubleClickPress)
{
   mock("Interface").expectOneCall("send");

   server.double_click_press(addr);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::DOUBLE_CLICK_PRESS_CMD);
}

//! @test Button Press handling.
TEST(SimpleButtonServer, PressDetection)
{
   server.short_press_max_duration(500);

   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(200);

   // Should send a short press message.
   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::SHORT_PRESS_CMD);

   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(100 + 500);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::SHORT_PRESS_CMD);

   // Should send a long press message.
   server.extra_long_press_min_duration(0);

   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(100 + 500 + 1);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::LONG_PRESS_CMD);

   server.extra_long_press_min_duration(1000);

   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(100 + 500 + 100);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::LONG_PRESS_CMD);

   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(100 + 1000 - 1);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::LONG_PRESS_CMD);

   // Should send an extra long press.
   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(100 + 1000);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::EXTRA_LONG_PRESS_CMD);

   mock("Interface").expectOneCall("send");

   server.pressed(100);
   server.released(100 + 1000 + 42);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::EXTRA_LONG_PRESS_CMD);
}

//! @test Double Press handling.
TEST(SimpleButtonServer, DoublePressDetection)
{
   server.short_press_max_duration(500);
   server.double_click_gap_duration(300);

   uint16_t timestamp = 100;

   mock("Interface").expectOneCall("send");

   // Short press. #1
   server.pressed(timestamp);
   timestamp += 200;
   server.released(timestamp);

   mock("Interface").checkExpectations();

   CHECK_MESSAGE(SimpleButton::SHORT_PRESS_CMD);

   timestamp += 200;

   mock("Interface").expectOneCall("send");

   // Double click.
   server.pressed(timestamp);

   mock("Interface").checkExpectations();

   timestamp += 200;
   server.released(timestamp);

   CHECK_MESSAGE(SimpleButton::DOUBLE_CLICK_PRESS_CMD);
}
