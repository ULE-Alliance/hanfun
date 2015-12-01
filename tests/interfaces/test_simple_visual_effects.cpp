// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_visual_effects.cpp
 *
 * This is file contains the unit tests for the Simple Visual Effects interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_visual_effects.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::SimpleVisualEffects;

// =============================================================================
// Simple Visual Effects
// =============================================================================

//! Test Group for SimpleVisualEffects interface parent class.
TEST_GROUP(SimpleVisualEffects)
{
   class SimpleVisualEffectsBase: public InterfaceParentHelper<SimpleVisualEffects::Base>
   {};

   SimpleVisualEffectsBase interface;

   TEST_SETUP()
   {
      interface = SimpleVisualEffectsBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test SimpleVisualEffects::uid should return @c HF::Interface::SIMPLE_VISUAL_EFFECTS.
TEST(SimpleVisualEffects, UID)
{
   LONGS_EQUAL(HF::Interface::SIMPLE_VISUAL_EFFECTS, interface.uid());
}

TEST(SimpleVisualEffects, OnEffect_Size)
{
   OnEffect effect;
   LONGS_EQUAL(sizeof(uint16_t), OnEffect::min_size);
   LONGS_EQUAL(sizeof(uint16_t), effect.size());
}

TEST(SimpleVisualEffects, OnEffect_Pack)
{
   const Common::ByteArray expected({
      0x00, 0x00, 0x00,
      0x5A, 0x5A,
      0x00, 0x00, 0x00,
   });

   Common::ByteArray payload(3 + OnEffect::min_size + 3);

   OnEffect effect(0x5A5A);
   LONGS_EQUAL(OnEffect::min_size, effect.pack(payload, 3));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, effect.pack(payload, payload.size()));

   mock("support").checkExpectations();
}

TEST(SimpleVisualEffects, OnEffect_Unpack)
{
   const Common::ByteArray payload({
      0x00, 0x00, 0x00,
      0x5A, 0x5A,
      0x00, 0x00, 0x00,
   });

   OnEffect effect;
   LONGS_EQUAL(0, effect.duration);

   LONGS_EQUAL(OnEffect::min_size, effect.unpack(payload, 3));

   LONGS_EQUAL(0x5A5A, effect.duration);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect.duration = 0;
   LONGS_EQUAL(0, effect.unpack(payload, payload.size()));

   mock("support").checkExpectations();

   LONGS_EQUAL(0, effect.duration);
}

// =============================================================================
// Simple Visual Effects Client
// =============================================================================

//! Test Group for Simple Visual Effects Client interface class.
TEST_GROUP(SimpleVisualEffectsClient)
{
   struct SimpleVisualEffectsClient: public InterfaceHelper<SimpleVisualEffects::Client>
   {};

   SimpleVisualEffectsClient client;
   Protocol::Address addr;

   TEST_SETUP()
   {
      client = SimpleVisualEffectsClient();
      addr   = Protocol::Address(42);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test On support.
TEST(SimpleVisualEffectsClient, On)
{
   mock("Interface").expectOneCall("send");

   client.on(addr, 42);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::ON_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   OnEffect effect;
   effect.unpack(client.sendMsg.payload);

   LONGS_EQUAL(42, effect.duration);
}

//! @test Off support.
TEST(SimpleVisualEffectsClient, Off)
{
   mock("Interface").expectOneCall("send");

   client.off(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::OFF_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Blink support.
TEST(SimpleVisualEffectsClient, Blink)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.blink(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::BLINK_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Fade support.
TEST(SimpleVisualEffectsClient, Fade)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.fade(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::FADE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Breathe support.
TEST(SimpleVisualEffectsClient, Breathe)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.breathe(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::BREATHE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// Simple Visual Effects Server
// =============================================================================

//! Test Group for Simple Visual Effects Server interface class.
TEST_GROUP(SimpleVisualEffectsServer)
{
   struct SimpleVisualEffectsServer: public InterfaceHelper<SimpleVisualEffects::Server>
   {
      void on(const Protocol::Address &addr, const OnEffect &effect)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("on").withParameter("duration", effect.duration);
      }

      void off(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("off");
      }

      void blink(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("blink");
      }

      void fade(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("fade");
      }

      void breathe(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("breathe");
      }

   };

   SimpleVisualEffectsServer server;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      server                    = SimpleVisualEffectsServer();
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

//! @test On support.
TEST(SimpleVisualEffectsServer, On)
{
   mock("SimpleVisualEffects::Server").expectOneCall("on").withParameter("duration", 0x5AA5);

   payload = Common::ByteArray({ 0x00, 0x00, 0x00,
                                 0x5A, 0xA5, // Duration value.
                                 0x00, 0x00, 0x00});

   packet.message.itf.member = SimpleVisualEffects::ON_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   payload = Common::ByteArray({ 0x00, 0x00, 0x00});

   CHECK_EQUAL(Common::Result::FAIL_ARG, server.handle(packet, payload, 2));

   mock("support").checkExpectations();
}

//! @test Off support.
TEST(SimpleVisualEffectsServer, Off)
{
   // FIXME Generated Stub.
   mock("SimpleVisualEffects::Server").expectOneCall("off");

   packet.message.itf.member = SimpleVisualEffects::OFF_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();
}

//! @test Blink support.
TEST(SimpleVisualEffectsServer, Blink)
{
   // FIXME Generated Stub.
   mock("SimpleVisualEffects::Server").expectOneCall("blink");

   packet.message.itf.member = SimpleVisualEffects::BLINK_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();
}

//! @test Fade support.
TEST(SimpleVisualEffectsServer, Fade)
{
   // FIXME Generated Stub.
   mock("SimpleVisualEffects::Server").expectOneCall("fade");

   packet.message.itf.member = SimpleVisualEffects::FADE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();
}

//! @test Breathe support.
TEST(SimpleVisualEffectsServer, Breathe)
{
   // FIXME Generated Stub.
   mock("SimpleVisualEffects::Server").expectOneCall("breathe");

   packet.message.itf.member = SimpleVisualEffects::BREATHE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();
}
