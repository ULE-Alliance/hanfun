// =============================================================================
/*!
 * @file       tests/interfaces/test_simple_air_pressure.cpp
 *
 * This is file contains the unit tests for the Simple Air Pressure interface
 * implementation.
 *
 * @version    x.x.x
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
   // TODO Add required unit tests.
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
   // TODO Add required unit tests.
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
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Value, false, value, 42, -42);
}

//! @test Maximum support.
TEST(SimpleAirPressureServer, Maximum)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Maximum, false, maximum, 42, -42);
}

//! @test Minimum support.
TEST(SimpleAirPressureServer, Minimum)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Minimum, false, minimum, 42, -42);
}

//! @test Tolerance support.
TEST(SimpleAirPressureServer, Tolerance)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(SimpleAirPressureServer, Tolerance, false, tolerance, 42, -42);
}
