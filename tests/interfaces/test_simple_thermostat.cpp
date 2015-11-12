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
// Simple Thermostat
// =============================================================================

//! Test Group for SimpleThermostat interface parent class.
TEST_GROUP(SimpleThermostat)
{
   class TestSimpleThermostat: public InterfaceParentHelper<SimpleThermostat::Base>
   {};

   TestSimpleThermostat interface;

   TEST_SETUP()
   {
      interface = TestSimpleThermostat();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test SimpleThermostat::uid should return @c Interface::ON_OFF.
TEST(SimpleThermostat, UID)
{
   CHECK_EQUAL(Interface::SIMPLE_THERMOSTAT, interface.uid());
}

// =============================================================================
// Simple Thermostat Client
// =============================================================================

//! Test Group for Simple Thermostat Client interface class.
TEST_GROUP(SimpleThermostatClient)
{
   class TestSimpleThermostat: public InterfaceHelper<SimpleThermostat::Client>
   {};

   TestSimpleThermostat client;

   TEST_SETUP()
   {
      client = TestSimpleThermostat();
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
   class TestSimpleThermostat: public InterfaceHelper<SimpleThermostat::Server>
   {};

   TestSimpleThermostat server;

   TEST_SETUP()
   {
      server = TestSimpleThermostat();

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
}
