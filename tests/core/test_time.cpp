// =============================================================================
/*!
 * @file       tests/core/test_time.cpp
 *
 * This is file contains the unit tests for the Time service implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2016 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/time.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::Time;

// =============================================================================
// Time
// =============================================================================

//! Test Group for Time service parent class.
TEST_GROUP(Time)
{
   struct TimeBase: public Time::Base
   {
      TimeBase(HF::Core::Unit0 &unit): Time::Base(unit) {}

      Interface::Role role() const
      {
         return Interface::SERVER_ROLE;
      }
   };

   Testing::Device *device;
   TimeBase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new TimeBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;
      mock().clear();
   }
};

//! @test Time::uid should return @c HF::Interface::TIME.
TEST(Time, UID)
{
   LONGS_EQUAL(HF::Interface::TIME, service->uid());
}

// =============================================================================
// Time Client
// =============================================================================

//! Test Group for Time Client service class.
TEST_GROUP(TimeClient)
{
   struct TimeClient: public Time::Client
   {
      TimeClient(HF::Core::Unit0 &unit):
         Time::Client(unit)
      {}

   };

   Testing::Device *device;
   TimeClient *client;

   TEST_SETUP()
   {
      device = new Testing::Device();
      client = new TimeClient(*(device->unit0()));
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;
      delete device;
      mock().clear();
   }
};

// =============================================================================
// Time Server
// =============================================================================

//! Test Group for Time Server service class.
TEST_GROUP(TimeServer)
{
   struct TimeServer: public InterfaceHelper<Time::Server>
   {
      TimeServer(HF::Core::Unit0 &unit):
         InterfaceHelper<Time::Server>(unit)
      {}

   };

   Testing::Device *device;
   TimeServer *server;

   TEST_SETUP()
   {
      device = new Testing::Device();
      server = new TimeServer(*(device->unit0()));
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete device;
      mock().clear();
   }
};

//! @test Time support.
TEST(TimeServer, Time)
{
   CHECK_ATTRIBUTE(TimeServer, Value, true, time, 42, 142);
}
