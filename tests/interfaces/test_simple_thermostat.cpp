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
};

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
