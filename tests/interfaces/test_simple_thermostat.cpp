// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_thermostat.cpp
 *
 * This is file contains the unit tests for the Simple Thermostat Interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_thermostat.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::SimpleThermostat;

// =============================================================================
// Helper Classes
// =============================================================================

/*!
 * Helper class to test the SimpleThermostat interface - server role.
 */
class SimpleThermostatServer: public InterfaceHelper<SimpleThermostat::Server>
{
   Common::Result boost_start(const HF::Protocol::Address &source)
   {
      UNUSED(source);
      return (Common::Result) mock("SimpleThermostatServer").actualCall("boost_start")
                .returnIntValueOrDefault((int) Common::Result::FAIL_UNKNOWN);
   }

   Common::Result boost_stop(const HF::Protocol::Address &source)
   {
      UNUSED(source);
      return (Common::Result) mock("SimpleThermostatServer").actualCall("boost_stop")
                .returnIntValueOrDefault((int) Common::Result::FAIL_UNKNOWN);
   }
};

/*!
 * Helper class to test the SimpleThermostat interface - client role.
 */
class SimpleThermostatClient: public InterfaceHelper<SimpleThermostat::Client>
{
   public:

   using InterfaceHelper<SimpleThermostat::Client>::boost_start;
   using InterfaceHelper<SimpleThermostat::Client>::boost_stop;

   protected:

   void boost_start(const HF::Protocol::Address &source, const Common::Result result)
   {
      UNUSED(source);
      UNUSED(result);
      mock("SimpleThermostatClient").actualCall("boost_start")
         .withIntParameter("result", (int) result);
   }

   void boost_stop(const HF::Protocol::Address &source, const Common::Result result)
   {
      UNUSED(source);
      UNUSED(result);
      mock("SimpleThermostatClient").actualCall("boost_stop")
         .withIntParameter("result", (int) result);
   }
};

// =============================================================================
// Simple Thermostat
// =============================================================================

//! Test Group for SimpleThermostat interface parent class.
TEST_GROUP(SimpleThermostat)
{
   class SimpleThermostatBase: public InterfaceParentHelper<SimpleThermostat::Base>
   {};

   SimpleThermostatBase interface;
   SimpleThermostatServer server;
   SimpleThermostatClient client;

   TEST_SETUP()
   {
      interface = SimpleThermostatBase();
      server    = SimpleThermostatServer();
      client    = SimpleThermostatClient();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }

   void check_invalid_message(HF::Interface &interface, const Message::Type type,
                              uint8_t uid, const Interface::Role role, const char *file, int lineno)
   {
      Protocol::Packet packet;
      packet.message.itf.role   = role;
      packet.message.itf.id     = uid;
      packet.message.itf.member = 0xFF;
      packet.message.type       = type;

      Common::ByteArray expected;
      CHECK_EQUAL_LOCATION(Common::Result::FAIL_ARG,
                           interface.handle(packet, expected, 3), file, lineno);
   }

   void check_invalid_role(SimpleThermostat::Server &server, Interface::Role role,
                           const char *file, int lineno)
   {
      check_invalid_message(server, Message::Type::COMMAND_REQ, server.uid(), role, file, lineno);
   }

   void check_invalid_role(SimpleThermostat::Client &client, Interface::Role role,
                           const char *file, int lineno)
   {
      check_invalid_message(client, Message::Type::COMMAND_RES, client.uid(), role, file, lineno);
   }

   void check_invalid_uid(SimpleThermostat::Server &server, uint8_t uid, const char *file,
                          int lineno)
   {
      check_invalid_message(server, Message::Type::COMMAND_REQ, uid, Interface::CLIENT_ROLE, file,
                            lineno);
   }

   void check_invalid_uid(SimpleThermostat::Client &client, uint8_t uid, const char *file,
                          int lineno)
   {
      check_invalid_message(client, Message::Type::COMMAND_RES, uid, Interface::SERVER_ROLE, file,
                            lineno);
   }
};

#define CHECK_INVALID_ROLE(_itf, _role) \
   check_invalid_role(_itf, _role, __FILE__, __LINE__)

#define CHECK_INVALID_UID(_itf, _uid) \
   check_invalid_uid(_itf, _uid, __FILE__, __LINE__)

//! @test SimpleThermostat::uid should return @c Interface::SIMPLE_THERMOSTAT.
TEST(SimpleThermostat, UID)
{
   CHECK_EQUAL(Interface::SIMPLE_THERMOSTAT, interface.uid());
}

//! @test Should send a BOOST_START_CMD message/response correctly.
TEST(SimpleThermostat, BoostStart)
{
   mock("Interface").expectOneCall("send");

   Protocol::Address addr;
   client.boost_start(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleThermostat::BOOST_START_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   Protocol::Packet packet;
   Common::ByteArray expected;

   packet.message = client.sendMsg;

   mock("SimpleThermostatServer").expectOneCall("boost_start")
      .andReturnValue((int) Common::Result::FAIL_RESOURCES);

   mock("Interface").expectOneCall("send");

   Common::Result result = server.handle(packet, expected, 42);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("SimpleThermostatServer").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(Interface::SERVER_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SimpleThermostat::BOOST_START_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_RES, server.sendMsg.type);

   packet.message = server.sendMsg;
   expected       = server.sendMsg.payload;

   mock("SimpleThermostatClient").expectOneCall("boost_start")
      .withParameter("result", Common::Result::FAIL_RESOURCES);

   client.handle(packet, expected, 0);

   mock("SimpleThermostatClient").checkExpectations();
}

//! @test Should send a BOOST_START_CMD message/response correctly.
TEST(SimpleThermostat, BoostStop)
{
   mock("Interface").expectOneCall("send");

   Protocol::Address addr;
   client.boost_stop(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SimpleThermostat::BOOST_STOP_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);

   Protocol::Packet packet;
   Common::ByteArray expected;

   packet.message = client.sendMsg;

   mock("SimpleThermostatServer").expectOneCall("boost_stop")
      .andReturnValue((int) Common::Result::FAIL_RESOURCES);

   mock("Interface").expectOneCall("send");

   Common::Result result = server.handle(packet, expected, 42);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("SimpleThermostatServer").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(Interface::SERVER_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SimpleThermostat::BOOST_STOP_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_RES, server.sendMsg.type);

   packet.message = server.sendMsg;
   expected       = server.sendMsg.payload;

   mock("SimpleThermostatClient").expectOneCall("boost_stop")
      .withParameter("result", Common::Result::FAIL_RESOURCES);

   client.handle(packet, expected, 0);

   mock("SimpleThermostatClient").checkExpectations();
}

//! @test Should not handle message from invalid role.
TEST(SimpleThermostat, Handle_Invalid_Role)
{
   CHECK_INVALID_ROLE(server, Interface::SERVER_ROLE);
   CHECK_INVALID_ROLE(client, Interface::CLIENT_ROLE);
}

//! @test Should not handle message from invalid interface UID.
TEST(SimpleThermostat, Handle_Invalid_UID)
{
   CHECK_INVALID_UID(server, server.uid() + 1);
   CHECK_INVALID_UID(client, client.uid() - 1);
}

// =============================================================================
// Simple Thermostat Client
// =============================================================================

//! Test Group for Simple Thermostat Client interface class.
TEST_GROUP(SimpleThermostatClient)
{
   SimpleThermostatClient client;

   TEST_SETUP()
   {
      client = SimpleThermostatClient();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

// =============================================================================
// Simple Thermostat Server
// =============================================================================

//! Test Group for Simple Thermostat Server interface class.
TEST_GROUP(SimpleThermostatServer)
{
   class SimpleThermostatServer: public InterfaceHelper<SimpleThermostat::Server>
   {};

   SimpleThermostatServer server;

   TEST_SETUP()
   {
      server = SimpleThermostatServer();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }

   template<typename Attribute, typename Getter, typename Setter,
            typename Value = typename Attribute::value_type>
   void check_attribute(Value first, Value second, Getter getter, Setter setter, const char *file,
                        int lineno)
   {
      auto attrs = server.attributes(HF::Attributes::Pack::MANDATORY);

      CHECK_TRUE_LOCATION(std::none_of(attrs.begin(), attrs.end(),
                                       [](uint8_t uid) {return uid == Attribute::ID;}),
                          file, lineno);

      attrs = server.attributes(HF::Attributes::Pack::ALL);

      CHECK_TRUE_LOCATION(std::any_of(attrs.begin(), attrs.end(),
                                      [](uint8_t uid) {return uid == Attribute::ID;}),
                          file, lineno);

      mock("Interface").expectNCalls(2, "notify");

      (server.*setter)(first);

      LONGS_EQUAL_LOCATION(first, (server.*getter)(), file, lineno);

      typedef HF::Attributes::Attribute<int16_t, Server> __Attribute;

      auto attr = static_cast<__Attribute *>(server.attribute(Attribute::ID));
      CHECK_TRUE_LOCATION(attr != nullptr, file, lineno);

      CHECK_EQUAL_LOCATION(true, attr->isWritable(), file, lineno);
      POINTERS_EQUAL_LOCATION(&server, attr->owner(), file, lineno);
      LONGS_EQUAL_LOCATION(server.uid(), attr->interface(), file, lineno);

      LONGS_EQUAL_LOCATION(first, attr->value(), file, lineno);

      attr->value(second);
      LONGS_EQUAL_LOCATION(second, attr->value(), file, lineno);

      mock("Interface").checkExpectations();

      delete attr;
   }
};

#define CHECK_ATTRIBUTE(Type, _name, _first, _second)                                \
   check_attribute<Type>(_first, _second,                                            \
                         (Type::value_type (Server::*)(void) const) & Server::_name, \
                         (void (Server::*)(Type::value_type)) & Server::_name,       \
                         __FILE__, __LINE__)

#define CHECK_TEMPERATURE_ATTRIBUTE(Type, _name)   CHECK_ATTRIBUTE(Type, _name, -42, 42)

//! @test Supported Modes support.
TEST(SimpleThermostatServer, SupportedModes)
{
   auto attrs = server.attributes(HF::Attributes::Pack::MANDATORY);

   CHECK_TRUE(std::any_of(attrs.begin(), attrs.end(),
                          [](uint8_t uid) {return uid == SUPPORTED_MODES_ATTR;}));

   LONGS_EQUAL(ALL_MODES_MASK, server.supported_modes());

   mock("Interface").expectNCalls(2, "notify");

   server.supported_modes(HEAT_MODE);

   LONGS_EQUAL(HEAT_MODE, server.supported_modes());

   typedef HF::Attributes::Attribute<uint8_t, Server> SupportedModeAttr;

   auto attr = static_cast<SupportedModeAttr *>(server.attribute(SUPPORTED_MODES_ATTR));
   CHECK_TRUE(attr != nullptr);

   CHECK_FALSE(attr->isWritable());
   POINTERS_EQUAL(&server, attr->owner());
   LONGS_EQUAL(server.uid(), attr->interface());

   LONGS_EQUAL(server.supported_modes(), attr->value());

   attr->value(COOL_MODE);
   LONGS_EQUAL(COOL_MODE, attr->value());

   mock("Interface").checkExpectations();

   delete attr;
}

//! @test Heat/Cool Mode support.
TEST(SimpleThermostatServer, Mode)
{
   auto attrs = server.attributes(HF::Attributes::Pack::MANDATORY);

   CHECK_TRUE(std::any_of(attrs.begin(), attrs.end(),
                          [](uint8_t uid) {return uid == HEAT_COOL_MODE_ATTR;}));

   mock("Interface").expectNCalls(2, "notify");

   server.mode(HEAT_MODE);

   LONGS_EQUAL(HEAT_MODE, server.mode());

   typedef HF::Attributes::Attribute<uint8_t, Server> HeatCoolModeAttr;

   auto attr = static_cast<HeatCoolModeAttr *>(server.attribute(HEAT_COOL_MODE_ATTR));
   CHECK_TRUE(attr != nullptr);

   CHECK_TRUE(attr->isWritable());
   POINTERS_EQUAL(&server, attr->owner());
   LONGS_EQUAL(server.uid(), attr->interface());

   LONGS_EQUAL(server.mode(), attr->value());

   attr->value(COOL_MODE);
   LONGS_EQUAL(COOL_MODE, attr->value());

   mock("Interface").checkExpectations();

   delete attr;
}

//! @test Fan Mode support.
TEST(SimpleThermostatServer, FanMode)
{
   CHECK_ATTRIBUTE(FanMode, fan_mode, FAN_OFF_MODE, FAN_AUTO_MODE);
}

//! @test Check temperature attributes support.
TEST(SimpleThermostatServer, TemperatureAttributes)
{
   CHECK_TEMPERATURE_ATTRIBUTE(HeatModeTemperature, heat_mode_temperature);
   CHECK_TEMPERATURE_ATTRIBUTE(CoolModeTemperature, cool_mode_temperature);
   CHECK_TEMPERATURE_ATTRIBUTE(AutoModeHeatTemperature, auto_mode_heat_temperature);
   CHECK_TEMPERATURE_ATTRIBUTE(AutoModeCoolTemperature, auto_mode_cool_temperature);
}

//! @test Check temperature offset attributes support.
TEST(SimpleThermostatServer, TemperatureOffsetAttributes)
{
   CHECK_TEMPERATURE_ATTRIBUTE(HeatModeTemperatureOffset, heat_mode_temperature_offset);
   CHECK_TEMPERATURE_ATTRIBUTE(CoolModeTemperatureOffset, cool_mode_temperature_offset);
}

//! @test Check boost duration attribute support.
TEST(SimpleThermostatServer, BoostDurationAttribute)
{
   CHECK_ATTRIBUTE(BoostDuration, boost_duration, 42, 142);
}
