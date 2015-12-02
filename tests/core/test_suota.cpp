// =============================================================================
/*!
 * @file       tests/core/test_suota.cpp
 *
 * This is file contains the unit tests for the SUOTA interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/suota.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::SUOTA;

// =============================================================================
// SUOTA
// =============================================================================

//! Test Group for SUOTA interface parent class.
TEST_GROUP(SUOTA)
{
   class SUOTABase: public InterfaceParentHelper<SUOTA::Base>
   {};

   SUOTABase interface;

   TEST_SETUP()
   {
      interface = SUOTABase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test SUOTA::uid should return @c HF::Interface::SUOTA.
TEST(SUOTA, UID)
{
   LONGS_EQUAL(HF::Interface::SUOTA, interface.uid());
}

// =============================================================================
// SUOTA Client
// =============================================================================

//! Test Group for SUOTA Client interface class.
TEST_GROUP(SUOTAClient)
{
   // TODO Add required unit tests.
   struct SUOTAClient: public InterfaceHelper<SUOTA::Client>
   {
      void new_version_available(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SUOTA::Client").actualCall("new_version_available");
      }

   };

   SUOTAClient client;
   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      client                    = SUOTAClient();
      addr                      = Protocol::Address(42);

      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
      packet.message.itf.id     = client.uid();
      packet.message.itf.member = 0xFF;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test New Version Available support.
TEST(SUOTAClient, NewVersionAvailable)
{
   // FIXME Generated Stub.
   mock("SUOTA::Client").expectOneCall("new_version_available");

   packet.message.itf.member = SUOTA::NEW_VERSION_AVAILABLE_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("SUOTA::Client").checkExpectations();
}

//! @test Check Version support.
TEST(SUOTAClient, CheckVersion)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.check_version(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SUOTA::CHECK_VERSION_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Upgrade Complete support.
TEST(SUOTAClient, UpgradeComplete)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.upgrade_complete(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SUOTA::UPGRADE_COMPLETE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// SUOTA Server
// =============================================================================

//! Test Group for SUOTA Server interface class.
TEST_GROUP(SUOTAServer)
{
   // TODO Add required unit tests.
   struct SUOTAServer: public InterfaceHelper<SUOTA::Server>
   {
      void check_version(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SUOTA::Server").actualCall("check_version");
      }

      void upgrade_complete(const Protocol::Address &addr)
      {
         UNUSED(addr);
         mock("SUOTA::Server").actualCall("upgrade_complete");
      }

   };

   SUOTAServer server;
   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      server                    = SUOTAServer();
      addr                      = Protocol::Address(42);

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

//! @test New Version Available support.
TEST(SUOTAServer, NewVersionAvailable)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   server.new_version_available(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SUOTA::NEW_VERSION_AVAILABLE_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);
}

//! @test Check Version support.
TEST(SUOTAServer, CheckVersion)
{
   // FIXME Generated Stub.
   mock("SUOTA::Server").expectOneCall("check_version");

   packet.message.itf.member = SUOTA::CHECK_VERSION_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SUOTA::Server").checkExpectations();
}

//! @test Upgrade Complete support.
TEST(SUOTAServer, UpgradeComplete)
{
   // FIXME Generated Stub.
   mock("SUOTA::Server").expectOneCall("upgrade_complete");

   packet.message.itf.member = SUOTA::UPGRADE_COMPLETE_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SUOTA::Server").checkExpectations();
}
