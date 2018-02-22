// =============================================================================
/*!
 * @file       tests/core/test_rssi.cpp
 *
 * This is file contains the unit tests for the RSSI interface
 * implementation.
 *
 * @version    1.5.1
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/rssi.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::RSSI;

// =============================================================================
// RSSI
// =============================================================================

//! Test Group for RSSI interface parent class.
TEST_GROUP(RSSI)
{
   class RSSIBase: public InterfaceParentHelper<RSSI::Base>
   {};

   RSSIBase interface;

   TEST_SETUP()
   {
      interface = RSSIBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test RSSI::uid should return @c HF::Interface::RSSI.
TEST(RSSI, UID)
{
   LONGS_EQUAL(HF::Interface::RSSI, interface.uid());
}

// =============================================================================
// RSSI Client
// =============================================================================

//! Test Group for RSSI Client interface class.
TEST_GROUP(RSSIClient)
{
   struct RSSIClient: public InterfaceHelper<RSSI::Client>
   {};

   RSSIClient client;

   TEST_SETUP()
   {
      client = RSSIClient();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

// =============================================================================
// RSSI Server
// =============================================================================

//! Test Group for RSSI Server interface class.
TEST_GROUP(RSSIServer)
{
   struct RSSIServer: public InterfaceHelper<RSSI::Server>
   {};

   RSSIServer server;

   TEST_SETUP()
   {
      server = RSSIServer();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test RSSI support.
TEST(RSSIServer, Value)
{
   CHECK_ATTRIBUTE(RSSIServer, Value, false, value, 42, -42);
}
