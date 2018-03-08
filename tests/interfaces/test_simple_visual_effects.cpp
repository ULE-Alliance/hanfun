// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_visual_effects.cpp
 *
 * This is file contains the unit tests for the Simple Visual Effects interface
 * implementation.
 *
 * @version    1.5.2
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

// =============================================================================
// On Effect
// =============================================================================

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
// Blink Effect
// =============================================================================

TEST(SimpleVisualEffects, BlinkEffect_Size)
{
   BlinkEffect effect;

   LONGS_EQUAL(3 * sizeof(uint16_t), BlinkEffect::min_size);
   LONGS_EQUAL(3 * sizeof(uint16_t), effect.size());
}

TEST(SimpleVisualEffects, BlinkEffect_Pack)
{
   const Common::ByteArray expected({0x00, 0x00, 0x00,
                                     0x12, 0x34, // Duty cycle ON.
                                     0x56, 0x78, // Duty cycle OFF.
                                     0x9A, 0xBC, // Number of cycles.
                                     0x00, 0x00, 0x00});

   Common::ByteArray payload(3 + BlinkEffect::min_size + 3);

   BlinkEffect effect(0x1234, 0x5678, 0x9ABC);
   LONGS_EQUAL(BlinkEffect::min_size, effect.pack(payload, 3));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, effect.pack(payload, payload.size()));

   mock("support").checkExpectations();

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect.number_of_cycles = 0;
   LONGS_EQUAL(0, effect.pack(payload, 3));

   mock("support").checkExpectations();
}

TEST(SimpleVisualEffects, BlinkEffect_Unpack)
{
   Common::ByteArray payload({0x00, 0x00, 0x00,
                              0x12, 0x34,           // Duty cycle ON.
                              0x56, 0x78,           // Duty cycle OFF.
                              0x9A, 0xBC,           // Number of cycles.
                              0x00, 0x00, 0x00});

   BlinkEffect effect;
   LONGS_EQUAL(0, effect.duty_cycle_on);
   LONGS_EQUAL(0, effect.duty_cycle_off);
   LONGS_EQUAL(1, effect.number_of_cycles);

   LONGS_EQUAL(BlinkEffect::min_size, effect.unpack(payload, 3));

   LONGS_EQUAL(0x1234, effect.duty_cycle_on);
   LONGS_EQUAL(0x5678, effect.duty_cycle_off);
   LONGS_EQUAL(0x9ABC, effect.number_of_cycles);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect = BlinkEffect();
   LONGS_EQUAL(0, effect.unpack(payload, payload.size()));

   mock("support").checkExpectations();

   LONGS_EQUAL(0, effect.duty_cycle_on);
   LONGS_EQUAL(0, effect.duty_cycle_off);
   LONGS_EQUAL(1, effect.number_of_cycles);

   // Number of cycles can't be zero.
   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect                 = BlinkEffect();
   payload[3 + 2 + 2]     = 0x00;
   payload[3 + 2 + 2 + 1] = 0x00;

   LONGS_EQUAL(0, effect.unpack(payload, 3));

   mock("support").checkExpectations();
}

// =============================================================================
// Fade Effect
// =============================================================================

TEST(SimpleVisualEffects, FadeEffect_Size)
{
   FadeEffect effect;

   LONGS_EQUAL(2 * sizeof(uint8_t) + sizeof(uint16_t), FadeEffect::min_size);
   LONGS_EQUAL(2 * sizeof(uint8_t) + sizeof(uint16_t), effect.size());
}

TEST(SimpleVisualEffects, FadeEffect_Pack)
{
   const Common::ByteArray expected({0x00, 0x00, 0x00,
                                     0x12,         // Start brightness.
                                     0x34,         // End brightness
                                     0x56, 0x78,   // Duration.
                                     0x00, 0x00, 0x00});

   Common::ByteArray payload(3 + FadeEffect::min_size + 3);

   FadeEffect effect(0x12, 0x34, 0x5678);
   LONGS_EQUAL(FadeEffect::min_size, effect.pack(payload, 3));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, effect.pack(payload, payload.size()));

   mock("support").checkExpectations();
}

TEST(SimpleVisualEffects, FadeEffect_Unpack)
{
   const Common::ByteArray payload({0x00, 0x00, 0x00,
                                    0x12,          // Start brightness.
                                    0x34,          // End brightness
                                    0x56, 0x78,    // Duration.
                                    0x00, 0x00, 0x00});

   FadeEffect effect;

   LONGS_EQUAL(0, effect.start);
   LONGS_EQUAL(0, effect.end);
   LONGS_EQUAL(0, effect.duration);

   LONGS_EQUAL(FadeEffect::min_size, effect.unpack(payload, 3));

   LONGS_EQUAL(0x12, effect.start);
   LONGS_EQUAL(0x34, effect.end);
   LONGS_EQUAL(0x5678, effect.duration);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect = FadeEffect();
   LONGS_EQUAL(0, effect.unpack(payload, payload.size()));

   mock("support").checkExpectations();

   LONGS_EQUAL(0, effect.start);
   LONGS_EQUAL(0, effect.end);
   LONGS_EQUAL(0, effect.duration);
}

// =============================================================================
// Breathe Effect
// =============================================================================

TEST(SimpleVisualEffects, BreatheEffect_Size)
{
   BreatheEffect effect;

   LONGS_EQUAL(2 * sizeof(uint8_t) + 5 * sizeof(uint16_t), BreatheEffect::min_size);
   LONGS_EQUAL(2 * sizeof(uint8_t) + 5 * sizeof(uint16_t), effect.size());
}

TEST(SimpleVisualEffects, BreatheEffect_Pack)
{
   const Common::ByteArray expected({0x00, 0x00, 0x00,
                                     0x12,       // Start brightness.
                                     0x34, 0x56, // Start brightness hold time.
                                     0x78, 0x9A, // Start to End time.

                                     0xBC,       // End brightness.
                                     0xDE, 0xF1, // End brightness hold time.
                                     0x23, 0x45, // End to Start time.

                                     0x67, 0x89, // Number of cycles.
                                     0x00, 0x00, 0x00});

   Common::ByteArray payload(3 + BreatheEffect::min_size + 3);

   BreatheEffect effect(0x12, 0x3456, 0x789A, 0xBC, 0xDEF1, 0x2345, 0x6789);
   LONGS_EQUAL(BreatheEffect::min_size, effect.pack(payload, 3));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, effect.pack(payload, payload.size()));

   mock("support").checkExpectations();

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect.number_of_cycles = 0;
   LONGS_EQUAL(0, effect.pack(payload, 3));

   mock("support").checkExpectations();
}

TEST(SimpleVisualEffects, BreatheEffect_Unpack)
{
   Common::ByteArray payload({0x00, 0x00, 0x00,
                              0x12,             // Start brightness.
                              0x34, 0x56,       // Start brightness hold time.
                              0x78, 0x9A,       // Start to End time.

                              0xBC,             // End brightness.
                              0xDE, 0xF1,       // End brightness hold time.
                              0x23, 0x45,       // End to Start time.

                              0x67, 0x89,       // Number of cycles.
                              0x00, 0x00, 0x00});

   BreatheEffect effect;
   LONGS_EQUAL(0, effect.start);
   LONGS_EQUAL(0, effect.start_hold);
   LONGS_EQUAL(0, effect.ste_duration);
   LONGS_EQUAL(0, effect.end);
   LONGS_EQUAL(0, effect.end_hold);
   LONGS_EQUAL(0, effect.ets_duration);
   LONGS_EQUAL(1, effect.number_of_cycles);

   LONGS_EQUAL(BreatheEffect::min_size, effect.unpack(payload, 3));

   LONGS_EQUAL(0x12, effect.start);
   LONGS_EQUAL(0x3456, effect.start_hold);
   LONGS_EQUAL(0x789A, effect.ste_duration);

   LONGS_EQUAL(0xBC, effect.end);
   LONGS_EQUAL(0xDEF1, effect.end_hold);
   LONGS_EQUAL(0x2345, effect.ets_duration);

   LONGS_EQUAL(0x6789, effect.number_of_cycles);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect = BreatheEffect();
   LONGS_EQUAL(0, effect.unpack(payload, payload.size()));

   mock("support").checkExpectations();

   // Number of cycles can't be zero.
   effect = BreatheEffect();
   mock("support").expectOneCall("assert").ignoreOtherParameters();

   effect                           = BreatheEffect();
   payload[3 + 2 * (1 + 2 + 2)]     = 0x00;
   payload[3 + 2 * (1 + 2 + 2) + 1] = 0x00;

   LONGS_EQUAL(0, effect.unpack(payload, 3));

   mock("support").checkExpectations();
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
   mock("Interface").expectOneCall("send");

   BlinkEffect expected(0x1234, 0x5678, 0x9ABC);
   client.blink(addr, expected);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::BLINK_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   BlinkEffect effect;
   effect.unpack(client.sendMsg.payload);

   LONGS_EQUAL(0x1234, effect.duty_cycle_on);
   LONGS_EQUAL(0x5678, effect.duty_cycle_off);
   LONGS_EQUAL(0x9ABC, effect.number_of_cycles);
}

//! @test Fade support.
TEST(SimpleVisualEffectsClient, Fade)
{
   mock("Interface").expectOneCall("send");

   FadeEffect expected(0x12, 0x34, 0x5678);
   client.fade(addr, expected);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::FADE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   FadeEffect effect;
   effect.unpack(client.sendMsg.payload);

   LONGS_EQUAL(0x12, effect.start);
   LONGS_EQUAL(0x34, effect.end);
   LONGS_EQUAL(0x5678, effect.duration);
}

//! @test Breathe support.
TEST(SimpleVisualEffectsClient, Breathe)
{
   mock("Interface").expectOneCall("send");

   BreatheEffect expected(0x12, 0x3456, 0x789A, 0xBC, 0xDEF1, 0x2345, 0x6789);
   client.breathe(addr, expected);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleVisualEffects::BREATHE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   BreatheEffect effect;
   effect.unpack(client.sendMsg.payload);

   LONGS_EQUAL(0x12, effect.start);
   LONGS_EQUAL(0x3456, effect.start_hold);
   LONGS_EQUAL(0x789A, effect.ste_duration);

   LONGS_EQUAL(0xBC, effect.end);
   LONGS_EQUAL(0xDEF1, effect.end_hold);
   LONGS_EQUAL(0x2345, effect.ets_duration);

   LONGS_EQUAL(0x6789, effect.number_of_cycles);
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
         mock("SimpleVisualEffects::Server").actualCall("on")
            .withParameter("duration", effect.duration);
      }

      void off(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("off");
      }

      void blink(const Protocol::Address &addr, const BlinkEffect &effect)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("blink")
            .withParameter("duty_cycle_on", effect.duty_cycle_on)
            .withParameter("duty_cycle_off", effect.duty_cycle_off)
            .withParameter("number_of_cycles", effect.number_of_cycles);
      }

      void fade(const Protocol::Address &addr, const FadeEffect &effect)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("fade")
            .withParameter("start", effect.start)
            .withParameter("end", effect.end)
            .withParameter("duration", effect.duration);
      }

      void breathe(const Protocol::Address &addr, const BreatheEffect &effect)
      {
         UNUSED(addr);
         mock("SimpleVisualEffects::Server").actualCall("breathe")
            .withParameter("start", effect.start)
            .withParameter("start_hold", effect.start_hold)
            .withParameter("ste_duration", effect.ste_duration)
            .withParameter("end", effect.end)
            .withParameter("end_hold", effect.end_hold)
            .withParameter("ets_duration", effect.ets_duration)
            .withParameter("number_of_cycles", effect.number_of_cycles);
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

   payload = Common::ByteArray({0x00, 0x00, 0x00,
                                0x5A, 0xA5,  // Duration value.
                                0x00, 0x00, 0x00});

   packet.message.itf.member = SimpleVisualEffects::ON_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   payload = Common::ByteArray({0x00, 0x00, 0x00});

   CHECK_EQUAL(Common::Result::FAIL_ARG, server.handle(packet, payload, 2));

   mock("support").checkExpectations();
}

//! @test Off support.
TEST(SimpleVisualEffectsServer, Off)
{
   mock("SimpleVisualEffects::Server").expectOneCall("off");

   packet.message.itf.member = SimpleVisualEffects::OFF_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();
}

//! @test Blink support.
TEST(SimpleVisualEffectsServer, Blink)
{
   mock("SimpleVisualEffects::Server").expectOneCall("blink")
      .withParameter("duty_cycle_on", 0x1234)
      .withParameter("duty_cycle_off", 0x5678)
      .withParameter("number_of_cycles", 0x9ABC);

   payload = Common::ByteArray({0x00, 0x00, 0x00,
                                0x12, 0x34,     // Duty cycle ON.
                                0x56, 0x78,     // Duty cycle OFF.
                                0x9A, 0xBC,     // Number of cycles.
                                0x00, 0x00, 0x00});

   packet.message.itf.member = SimpleVisualEffects::BLINK_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();

   payload = Common::ByteArray({0x00, 0x00, 0x00});

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   CHECK_EQUAL(Common::Result::FAIL_ARG, server.handle(packet, payload, 2));

   mock("support").checkExpectations();
}

//! @test Fade support.
TEST(SimpleVisualEffectsServer, Fade)
{
   mock("SimpleVisualEffects::Server").expectOneCall("fade")
      .withParameter("start", 0x12)
      .withParameter("end", 0x34)
      .withParameter("duration", 0x5678);

   payload = Common::ByteArray({0x00, 0x00, 0x00,
                                0x12,        // Start brightness.
                                0x34,        // End brightness.
                                0x56, 0x78,  // Duration.
                                0x00, 0x00, 0x00});

   packet.message.itf.member = SimpleVisualEffects::FADE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();
}

//! @test Breathe support.
TEST(SimpleVisualEffectsServer, Breathe)
{
   mock("SimpleVisualEffects::Server").expectOneCall("breathe")
      .withParameter("start", 0x12)
      .withParameter("start_hold", 0x3456)
      .withParameter("ste_duration", 0x789A)
      .withParameter("end", 0xBC)
      .withParameter("end_hold", 0xDEF1)
      .withParameter("ets_duration", 0x2345)
      .withParameter("number_of_cycles", 0x6789);

   payload = Common::ByteArray({0x00, 0x00, 0x00,
                                0x12,        // Start brightness.
                                0x34, 0x56,  // Start brightness hold time.
                                0x78, 0x9A,  // Start to End time.

                                0xBC,        // End brightness.
                                0xDE, 0xF1,  // End brightness hold time.
                                0x23, 0x45,  // End to Start time.

                                0x67, 0x89,  // Number of cycles.
                                0x00, 0x00, 0x00});

   packet.message.itf.member = SimpleVisualEffects::BREATHE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SimpleVisualEffects::Server").checkExpectations();

   payload = Common::ByteArray({0x00, 0x00, 0x00});

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   CHECK_EQUAL(Common::Result::FAIL_ARG, server.handle(packet, payload, 2));

   mock("support").checkExpectations();
}
