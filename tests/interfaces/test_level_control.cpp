// =============================================================================
/*!
 * @file       tests/interfaces/test_level_control.cpp
 *
 * This is file contains the unit tests for the Level Control Interface implementation.
 *
 * @version    1.5.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/level_control.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Common;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::LevelControl;

// =============================================================================
// LevelControl
// =============================================================================

TEST_GROUP(LevelControl)
{
   class TestLevelControl: public InterfaceParentHelper<LevelControl::Base>
   {};

   TestLevelControl interface;
};

TEST(LevelControl, UID)
{
   CHECK_EQUAL(HF::Interface::LEVEL_CONTROL, interface.uid());
}

// =============================================================================
// LevelControlClient
// =============================================================================

//! Test Group for LevelControlClient interface class.
TEST_GROUP(LevelControlClient)
{
   class TestLevelControlClient: public InterfaceHelper<LevelControl::Client>
   {};

   TestLevelControlClient client;
   Protocol::Address addr;

   TEST_SETUP()
   {
      mock("support").expectNoCall("assert");
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock("support").checkExpectations();
      mock().clear();
   }
};

//! @test Should send an LevelControl::SET_LEVEL_CMD message.
TEST(LevelControlClient, Level)
{
   mock("Interface").expectOneCall("send");

   client.level(addr, (uint8_t) 0x42);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(LevelControl::SET_LEVEL_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   LevelControl::Level level;

   level.unpack(client.sendMsg.payload);

   BYTES_EQUAL(0x42, level.get());
}

//! @test Should send an LevelControl::INCREASE_LEVEL_CMD message.
TEST(LevelControlClient, Increase_level)
{
   mock("Interface").expectOneCall("send");

   client.increase_level(addr, (uint8_t) 0x42);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(LevelControl::INCREASE_LEVEL_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   LevelControl::Level level;

   level.unpack(client.sendMsg.payload);

   BYTES_EQUAL(0x42, level.get());
}

//! @test Should send an LevelControl::INCREASE_LEVEL_CMD message.
TEST(LevelControlClient, Increase_level_float)
{
   mock("Interface").expectOneCall("send");

   client.increase_level(addr, (float) 10.0);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(LevelControl::INCREASE_LEVEL_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   LevelControl::Level level;

   level.unpack(client.sendMsg.payload);

   BYTES_EQUAL(0x1A, level.get());
}

//! @test Should send an LevelControl::DECREASE_LEVEL_CMD message.
TEST(LevelControlClient, Decrease_level)
{
   mock("Interface").expectOneCall("send");

   client.decrease_level(addr, (uint8_t) 0x42);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(LevelControl::DECREASE_LEVEL_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   LevelControl::Level level;

   level.unpack(client.sendMsg.payload);

   BYTES_EQUAL(0x42, level.get());
}

//! @test Should send an LevelControl::DECREASE_LEVEL_CMD message.
TEST(LevelControlClient, Decrease_level_float)
{
   mock("Interface").expectOneCall("send");

   client.decrease_level(addr, (float) 10.0);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(LevelControl::DECREASE_LEVEL_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   LevelControl::Level level;

   level.unpack(client.sendMsg.payload);

   BYTES_EQUAL(0x1A, level.get());
}

// =============================================================================
// LevelControlServer Tests
// =============================================================================;

TEST_GROUP(LevelControlServer)
{
   class TestLevelControlServer: public InterfaceHelper<LevelControl::Server>
   {
      public:

      void level_change(HF::Protocol::Address &source, uint8_t old_level, uint8_t new_level)
      {
         mock("LevelControlServer").actualCall("level_change");
         LevelControl::Server::level_change(source, old_level, new_level);
      }

      void notify(const HF::Attributes::IAttribute &old_value,
                  const HF::Attributes::IAttribute &new_value) const
      {
         mock("Interface").actualCall("notify")
            .withParameterOfType("IAttribute", "old", &old_value)
            .withParameterOfType("IAttribute", "new", &new_value);
      }
   };

   TestLevelControlServer server;

   Protocol::Packet packet;
   ByteArray expected;

   TEST_SETUP()
   {
      mock("LevelControlServer").ignoreOtherCalls();
      mock("Interface").ignoreOtherCalls();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0xAA,  // Level value.
                            0x00, 0x00, 0x00};

      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server.uid();
      packet.message.itf.member = LevelControl::LEVEL_ATTR;

      packet.message.type       = Protocol::Message::SET_ATTR_REQ;

      packet.message.length     = expected.size();
   }

   TEST_TEARDOWN()
   {
      mock("support").checkExpectations();
      mock().clear();
   }
};

TEST(LevelControlServer, Level)
{
   CHECK_EQUAL(0, server.level());

   Level old_value(0, &server);
   Level new_value(42, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.level((uint8_t) 42);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(42, server.level());
}

TEST(LevelControlServer, Level_Float)
{
   CHECK_EQUAL(0, server.level());

   Level old_value(0, &server);
   Level new_value(128, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.level((float) 50.0);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(128, server.level());
}

TEST(LevelControlServer, Level_Float_Min)
{
   server.level((float) 50.0);

   CHECK_EQUAL(128, server.level());

   Level old_value(128, &server);
   Level new_value(0, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.level((float) -0.01);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(0, server.level());
}

TEST(LevelControlServer, Level_Float_Max)
{
   server.level((float) 50.0);

   CHECK_EQUAL(128, server.level());

   Level old_value(128, &server);
   Level new_value(255, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.level((float) 100.01);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(255, server.level());
}

TEST(LevelControlServer, Increase)
{
   CHECK_EQUAL(0, server.level());

   Level old_value(0, &server);
   Level new_value(200, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.increase((uint8_t) 200);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(200, server.level());
}

TEST(LevelControlServer, Increase_float)
{
   server.level((uint8_t) 0x80);
   CHECK_EQUAL(0x80, server.level());

   Level old_value(0x80, &server);
   Level new_value(0x9A, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.increase((float) 10.0f);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(0x9A, server.level());
}

TEST(LevelControlServer, Increase_more_than_max)
{
   server.level((uint8_t) 200);
   CHECK_EQUAL(200, server.level());

   Level old_value(200, &server);
   Level new_value(255, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.increase((uint8_t) 70);
   mock("Interface").checkExpectations();

   CHECK_EQUAL(255, server.level());
}

TEST(LevelControlServer, Increase_more_when_in_max)
{
   server.level((uint8_t) 255);
   CHECK_EQUAL(255, server.level());

   mock("Interface").expectNoCall("notify");

   server.increase((uint8_t) 70);
   mock("Interface").checkExpectations();

   CHECK_EQUAL(255, server.level());
}

TEST(LevelControlServer, Decrease)
{
   server.level((uint8_t) 255);

   Level old_value(255, &server);
   Level new_value(200, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.decrease((uint8_t) 55);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(200, server.level());
}

TEST(LevelControlServer, Decrease_more_than_min)
{
   server.level((uint8_t) 100);
   CHECK_EQUAL(100, server.level());

   Level old_value(100, &server);
   Level new_value(0, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.decrease((uint8_t) 150);
   mock("Interface").checkExpectations();

   CHECK_EQUAL(0, server.level());
}

TEST(LevelControlServer, Decrease_more_when_in_min)
{
   server.level((uint8_t) 0);
   CHECK_EQUAL(0, server.level());

   mock("Interface").expectNoCall("notify");

   server.decrease((uint8_t) 10);
   mock("Interface").checkExpectations();

   CHECK_EQUAL(0, server.level());
}

TEST(LevelControlServer, Decrease_float)
{
   server.level((uint8_t) 0x80);
   CHECK_EQUAL(0x80, server.level());

   Level old_value(0x80, &server);
   Level new_value(0x66, &server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   server.decrease((float) 10.0f);
   mock("Interface").checkExpectations();
   CHECK_EQUAL(0x66, server.level());
}

//! @test Should handle valid message.
TEST(LevelControlServer, Handle_Valid_Message_SET_LEVEL)
{
   mock("LevelControlServer").expectOneCall("level_change");

   Result result = server.handle(packet, expected, 3);
   CHECK_EQUAL(Result::OK, result);

   LONGS_EQUAL(0xAA, server.level());

   mock("LevelControlServer").checkExpectations();
}

//! @test Should handle valid message.
TEST(LevelControlServer, Handle_Valid_Message_INCREASE_LEVEL)
{
   packet.message.itf.member = LevelControl::INCREASE_LEVEL_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;

   mock("LevelControlServer").expectOneCall("level_change");

   Result result = server.handle(packet, expected, 3);
   CHECK_EQUAL(Result::OK, result);

   LONGS_EQUAL(0xAA, server.level());

   mock("LevelControlServer").checkExpectations();

}

//! @test Should handle valid message.
TEST(LevelControlServer, Handle_Valid_Message_DECREASE_LEVEL)
{
   server.level((uint8_t) 255);

   packet.message.itf.member = LevelControl::DECREASE_LEVEL_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;

   mock("LevelControlServer").expectOneCall("level_change");

   Result result = server.handle(packet, expected, 3);
   CHECK_EQUAL(Result::OK, result);

   LONGS_EQUAL(255 - 0xAA, server.level());

   mock("LevelControlServer").checkExpectations();
}

//! @test Should not handle message from invalid role.
TEST(LevelControlServer, Handle_Invalid_Role)
{
   packet.message.itf.role = HF::Interface::CLIENT_ROLE;

   CHECK_EQUAL(Result::FAIL_SUPPORT, server.handle(packet, expected, 3));
}

//! @test Should not handle message from invalid interface UID.
TEST(LevelControlServer, Handle_Invalid_UID)
{
   packet.message.itf.id = server.uid() + 1;

   CHECK_EQUAL(Result::FAIL_ARG, server.handle(packet, expected, 3));
}

//! @test Should not handle message with invalid payload size.
TEST(LevelControlServer, Handle_Invalid_Payload_Size)
{
   LevelControl::Level level_attr;
   packet.message.length = level_attr.size() - 1;

   CHECK_EQUAL(Result::FAIL_ARG, server.handle(packet, expected, 3));
}

//! @test Should not handle message with not enough payload / offset.
TEST(LevelControlServer, Handle_Invalid_Payload)
{
   CHECK_EQUAL(Result::FAIL_ARG, server.handle(packet, expected, 10));
}

//! @test Should return attribute.
TEST(LevelControlServer, Attribute)
{
   HF::Attributes::IAttribute *attr = server.attribute(LevelControl::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);

   attr = server.attribute(LevelControl::LEVEL_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(LevelControl::LEVEL_ATTR, attr->uid());
   CHECK_TRUE(attr->isWritable());

   LONGS_EQUAL(server.uid(), attr->interface());

   delete attr;
}
