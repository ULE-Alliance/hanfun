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
