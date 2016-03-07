// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_air_pressure.cpp
 *
 * This is file contains the unit tests for the Simple Air Pressure interface
 * implementation.
 *
 * @version    1.4.3
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/simple_air_pressure.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

using namespace HF::Testing;

using namespace HF::Interfaces::SimpleAirPressure;

// =============================================================================
// Simple Air Pressure
// =============================================================================

//! Test Group for SimpleAirPressure interface parent class.
TEST_GROUP(SimpleAirPressure)
{
   class SimpleAirPressureBase: public InterfaceParentHelper<SimpleAirPressure::Base>
   {};

   SimpleAirPressureBase interface;

   TEST_SETUP()
   {
      interface = SimpleAirPressureBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test SimpleAirPressure::uid should return @c HF::Interface::SIMPLE_AIR_PRESSURE.
TEST(SimpleAirPressure, UID)
{
   LONGS_EQUAL(HF::Interface::SIMPLE_AIR_PRESSURE, interface.uid());
}

// =============================================================================
// Simple Air Pressure Client
// =============================================================================

//! Test Group for Simple Air Pressure Client interface class.
TEST_GROUP(SimpleAirPressureClient)
{
   struct SimpleAirPressureClient: public InterfaceHelper<SimpleAirPressure::Client>
   {};

   SimpleAirPressureClient client;

   TEST_SETUP()
   {
      client = SimpleAirPressureClient();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

// =============================================================================
// Simple Air Pressure Server
// =============================================================================

//! Test Group for Simple Air Pressure Server interface class.
TEST_GROUP(SimpleAirPressureServer)
{
   struct SimpleAirPressureServer: public InterfaceHelper<SimpleAirPressure::Server>
   {};

   SimpleAirPressureServer server;


   TEST_SETUP()
   {
      server = SimpleAirPressureServer();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Value support.
TEST(SimpleAirPressureServer, Value)
{
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Value, false, value, 42, 142);
}

//! @test Maximum support.
TEST(SimpleAirPressureServer, Maximum)
{
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Maximum, false, maximum, 42, 142);
}

//! @test Minimum support.
TEST(SimpleAirPressureServer, Minimum)
{
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Minimum, false, minimum, 42, 142);
}

//! @test Tolerance support.
TEST(SimpleAirPressureServer, Tolerance)
{
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Tolerance, false, tolerance, 42, 142);
}
