// =============================================================================
/*!
 * @file       tests/interfaces/test_colour_control.cpp
 *
 * This is file contains the unit tests for the Colour Control interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/colour_control.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;
using namespace HF::Common;

using namespace HF::Interfaces::ColourControl;

// =============================================================================
// Color Control
// =============================================================================
// Colour Control
// =============================================================================

//! Test Group for ColourControl interface parent class.
TEST_GROUP(ColourControl)
{
   struct ColourControlBase: public InterfaceParentHelper<ColourControl::Base>
   {};

   ColourControlBase interface;

   TEST_SETUP()
   {
      interface = ColourControlBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test ColourControl::uid should return @c HF::Interface::COLOUR_CONTROL.
TEST(ColourControl, UID)
{
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, interface.uid());
}

//! @test Supported support.
TEST(ColourControl, Supported)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(ColourControl::SUPPORTED_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Supported::ID, attr->uid());
   CHECK_EQUAL(Supported::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Mode support.
TEST(ColourControl, Mode)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(ColourControl::MODE_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Mode::ID, attr->uid());
   CHECK_EQUAL(Mode::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Hue And Saturation support.
TEST(ColourControl, HueAndSaturation)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(
      ColourControl::HUE_AND_SATURATION_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(HueAndSaturation::ID, attr->uid());
   CHECK_EQUAL(HueAndSaturation::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test XY support.
TEST(ColourControl, Xy)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(ColourControl::XY_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Xy::ID, attr->uid());
   CHECK_EQUAL(Xy::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Colour Temperature support.
TEST(ColourControl, ColourTemperature)
{
   HF::Attributes::IAttribute *attr = ColourControl::create_attribute(
      ColourControl::COLOUR_TEMPERATURE_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(ColourTemperature::ID, attr->uid());
   CHECK_EQUAL(ColourTemperature::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::COLOUR_CONTROL, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(ColourControl, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr =
      ColourControl::create_attribute(ColourControl::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// Colour Control Client
// =============================================================================

//! Test Group for Colour Control Client interface class.
TEST_GROUP(ColourControlClient)
{
   // TODO Add required unit tests.
   struct ColourControlClient: public InterfaceHelper<ColourControl::Client>
   {};

   ColourControlClient client;

   Protocol::Address addr;


   TEST_SETUP()
   {
      client = ColourControlClient();

      addr   = Protocol::Address(42);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Move To Hue support.
TEST(ColourControlClient, MoveToHue)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_hue(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move Hue support.
TEST(ColourControlClient, MoveHue)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_hue(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Step Hue support.
TEST(ColourControlClient, StepHue)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.step_hue(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_HUE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Saturation support.
TEST(ColourControlClient, MoveToSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move Saturation support.
TEST(ColourControlClient, MoveSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Step Saturation support.
TEST(ColourControlClient, StepSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.step_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Hue And Saturation support.
TEST(ColourControlClient, MoveToHueAndSaturation)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_hue_and_saturation(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Xy support.
TEST(ColourControlClient, MoveToXy)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_xy(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_XY_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move Xy support.
TEST(ColourControlClient, MoveXy)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_xy(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_XY_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Step Xy support.
TEST(ColourControlClient, StepXy)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.step_xy(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STEP_XY_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Move To Colour Temperature support.
TEST(ColourControlClient, MoveToColourTemperature)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.move_to_colour_temperature(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Stop support.
TEST(ColourControlClient, Stop)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.stop(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(ColourControl::STOP_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// Colour Control Server
// =============================================================================

//! Test Group for Colour Control Server interface class.
TEST_GROUP(ColourControlServer)
{
   // TODO Add required unit tests.
   struct ColourControlServer: public InterfaceHelper<ColourControl::Server>
   {
      void move_to_hue(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_hue");
         InterfaceHelper<ColourControl::Server>::move_to_hue(addr);
      }

      void move_hue(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_hue");
         InterfaceHelper<ColourControl::Server>::move_hue(addr);
      }

      void step_hue(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("step_hue");
         InterfaceHelper<ColourControl::Server>::step_hue(addr);
      }

      void move_to_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_saturation");
         InterfaceHelper<ColourControl::Server>::move_to_saturation(addr);
      }

      void move_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_saturation");
         InterfaceHelper<ColourControl::Server>::move_saturation(addr);
      }

      void step_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("step_saturation");
         InterfaceHelper<ColourControl::Server>::step_saturation(addr);
      }

      void move_to_hue_and_saturation(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_hue_and_saturation");
         InterfaceHelper<ColourControl::Server>::move_to_hue_and_saturation(addr);
      }

      void move_to_xy(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_xy");
         InterfaceHelper<ColourControl::Server>::move_to_xy(addr);
      }

      void move_xy(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_xy");
         InterfaceHelper<ColourControl::Server>::move_xy(addr);
      }

      void step_xy(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("step_xy");
         InterfaceHelper<ColourControl::Server>::step_xy(addr);
      }

      void move_to_colour_temperature(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("move_to_colour_temperature");
         InterfaceHelper<ColourControl::Server>::move_to_colour_temperature(addr);
      }

      void stop(const Protocol::Address &addr) override
      {
         mock("ColourControl::Server").actualCall("stop");
         InterfaceHelper<ColourControl::Server>::stop(addr);
      }

   };

   ColourControlServer server;


   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      server                    = ColourControlServer();

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

//! @test Supported support.
TEST(ColourControlServer, Supported)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(ColourControlServer, Supported, false, supported, 42, 142);
}

//! @test Mode support.
TEST(ColourControlServer, Mode)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(ColourControlServer, Mode, false, mode, 42, 142);
}

//! @test Hue And Saturation support.
TEST(ColourControlServer, HueAndSaturation)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, HueAndSaturation, false, hue_and_saturation, 42, 142);
}

//! @test XY support.
TEST(ColourControlServer, Xy)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, Xy, false, xy, 42, 142);
}

//! @test Colour Temperature support.
TEST(ColourControlServer, ColourTemperature)
{
   // FIXME Generated Stub.
   CHECK_OPT_ATTRIBUTE(ColourControlServer, ColourTemperature, false, colour_temperature, 42, 142);
}

//! @test Move To Hue support.
TEST(ColourControlServer, MoveToHue)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_hue");

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move Hue support.
TEST(ColourControlServer, MoveHue)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_hue");

   packet.message.itf.member = ColourControl::MOVE_HUE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Step Hue support.
TEST(ColourControlServer, StepHue)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("step_hue");

   packet.message.itf.member = ColourControl::STEP_HUE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Saturation support.
TEST(ColourControlServer, MoveToSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_saturation");

   packet.message.itf.member = ColourControl::MOVE_TO_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move Saturation support.
TEST(ColourControlServer, MoveSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_saturation");

   packet.message.itf.member = ColourControl::MOVE_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Step Saturation support.
TEST(ColourControlServer, StepSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("step_saturation");

   packet.message.itf.member = ColourControl::STEP_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Hue And Saturation support.
TEST(ColourControlServer, MoveToHueAndSaturation)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_hue_and_saturation");

   packet.message.itf.member = ColourControl::MOVE_TO_HUE_AND_SATURATION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Xy support.
TEST(ColourControlServer, MoveToXy)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_xy");

   packet.message.itf.member = ColourControl::MOVE_TO_XY_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move Xy support.
TEST(ColourControlServer, MoveXy)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_xy");

   packet.message.itf.member = ColourControl::MOVE_XY_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Step Xy support.
TEST(ColourControlServer, StepXy)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("step_xy");

   packet.message.itf.member = ColourControl::STEP_XY_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Move To Colour Temperature support.
TEST(ColourControlServer, MoveToColourTemperature)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("move_to_colour_temperature");

   packet.message.itf.member = ColourControl::MOVE_TO_COLOUR_TEMPERATURE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}

//! @test Stop support.
TEST(ColourControlServer, Stop)
{
   // FIXME Generated Stub.
   mock("ColourControl::Server").expectOneCall("stop");

   packet.message.itf.member = ColourControl::STOP_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("ColourControl::Server").checkExpectations();
}
