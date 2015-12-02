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
// Version Message
// =============================================================================

TEST(SUOTA, Version_Size)
{
   Version version;
   LONGS_EQUAL(2 * sizeof(uint8_t), Version::min_size);
   LONGS_EQUAL(2 * sizeof(uint8_t), version.size());

   version = Version("v1.2.3");

   LONGS_EQUAL((1 + 6 + 1) * sizeof(uint8_t), version.size());

   version = Version("", "revA");

   LONGS_EQUAL((1 + 1 + 4) * sizeof(uint8_t), version.size());

   version = Version("", "", "http");

   LONGS_EQUAL((1 + 1 + 1 + 4) * sizeof(uint8_t), version.size());

   version = Version("v1.2.3", "revA");

   LONGS_EQUAL((1 + 6 + 1 + 4) * sizeof(uint8_t), version.size());

   version = Version("v1.2.3", "revA", "https");

   LONGS_EQUAL((1 + 6 + 1 + 4 + 1 + 5) * sizeof(uint8_t), version.size());
}

TEST(SUOTA, Version_Pack)
{
   const Common::ByteArray expected({
      0x00, 0x00, 0x00,
      0x06, 'v', '1', '.', '2', '.', '3', // Software version string.
      0x04, 'r', 'e', 'v', 'A',           // Hardware version string.
      0x05, 'h', 't', 't', 'p', 's',      // URL string.
      0x00, 0x00, 0x00,
   });

   Version version("v1.2.3", "revA", "https");

   Common::ByteArray payload(3 + version.size() + 3);

   LONGS_EQUAL(version.size(), version.pack(payload, 3));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, version.pack(payload, payload.size()));

   mock("support").checkExpectations();
}

TEST(SUOTA, Version_Unpack)
{
   const Common::ByteArray payload({
      0x00, 0x00, 0x00,
      0x06, 'v', '1', '.', '2', '.', '3', // Software version string.
      0x04, 'r', 'e', 'v', 'A',           // Hardware version string.
      0x05, 'h', 't', 't', 'p', 's',      // URL string.
      0x00, 0x00, 0x00,
   });

   Version version;

   STRCMP_EQUAL("", version.sw_version.c_str());
   STRCMP_EQUAL("", version.hw_version.c_str());
   STRCMP_EQUAL("", version.url.c_str());

   LONGS_EQUAL(18, version.unpack(payload, 3));

   STRCMP_EQUAL("v1.2.3", version.sw_version.c_str());
   STRCMP_EQUAL("revA", version.hw_version.c_str());
   STRCMP_EQUAL("https", version.url.c_str());

   mock("support").expectOneCall("assert").ignoreOtherParameters();
   version = Version();

   LONGS_EQUAL(0, version.unpack(payload, payload.size()));

   mock("support").checkExpectations();

   STRCMP_EQUAL("", version.sw_version.c_str());
   STRCMP_EQUAL("", version.hw_version.c_str());
   STRCMP_EQUAL("", version.url.c_str());
}

// =============================================================================
// SUOTA Client
// =============================================================================

//! Test Group for SUOTA Client interface class.
TEST_GROUP(SUOTAClient)
{
   struct SUOTAClient: public InterfaceHelper<SUOTA::Client>
   {
      NewVersionResponse new_version_available(const Protocol::Address &addr,
                                               const Version &version)
      {
         UNUSED(addr);
         return (NewVersionResponse) mock("SUOTA::Client").actualCall("new_version_available")
                   .withParameter("sw", version.sw_version.c_str())
                   .withParameter("hw", version.hw_version.c_str())
                   .withParameter("url", version.url.c_str())
                   .returnIntValueOrDefault(NewVersionResponse::FAIL_UNKNOWN);
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

      packet.source.device      = 1;
      packet.source.unit        = 2;
      packet.destination.device = 3;
      packet.destination.unit   = 4;

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
   mock().strictOrder();
   mock("Interface").expectOneCall("send");
   mock("SUOTA::Client").expectOneCall("new_version_available")
      .withParameter("sw", "v1.2.3")
      .withParameter("hw", "revA")
      .withParameter("url", "https")
      .andReturnValue(NewVersionResponse::UNSUPPORTED_HARDWARE);

   packet.message.itf.member = SUOTA::NEW_VERSION_AVAILABLE_CMD;

   payload                   = Common::ByteArray({
      0x00, 0x00, 0x00,
      0x06, 'v', '1', '.', '2', '.', '3', // Software version string.
      0x04, 'r', 'e', 'v', 'A',           // Hardware version string.
      0x05, 'h', 't', 't', 'p', 's',      // URL string.
      0x00, 0x00, 0x00,
   });

   auto res = client.handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, res);

   mock("Interface").checkExpectations();
   mock("SUOTA::Client").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(SUOTA::NEW_VERSION_AVAILABLE_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_RES, client.sendMsg.type);

   LONGS_EQUAL(1, client.addr.device);
   LONGS_EQUAL(2, client.addr.unit);

   Protocol::Response response;
   response.unpack(client.sendMsg.payload);

   LONGS_EQUAL(NewVersionResponse::UNSUPPORTED_HARDWARE, response.code);

   mock("SUOTA::Client").expectOneCall("new_version_available").ignoreOtherParameters();
   mock("support").expectNCalls(2, "assert").ignoreOtherParameters();
   mock("Interface").expectOneCall("send");

   res = client.handle(packet, payload, payload.size());
   CHECK_EQUAL(Common::Result::FAIL_ARG, res);

   mock("Interface").checkExpectations();
   mock("support").checkExpectations();
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
   struct SUOTAServer: public InterfaceHelper<SUOTA::Server>
   {
      using InterfaceHelper<SUOTA::Server>::new_version_available;

      void new_version_available(const Protocol::Address &addr, NewVersionResponse result)
      {
         UNUSED(addr);
         mock("SUOTA::Server").actualCall("new_version_available").withParameter("result", (int) result);
      }

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
   mock("Interface").expectOneCall("send");

   Version expected("v1.2.3", "revA", "https");
   server.new_version_available(addr, expected);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(SUOTA::NEW_VERSION_AVAILABLE_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);

   Version version;
   LONGS_EQUAL(18, version.unpack(server.sendMsg.payload));

   STRCMP_EQUAL("v1.2.3", version.sw_version.c_str());
   STRCMP_EQUAL("revA", version.hw_version.c_str());
   STRCMP_EQUAL("https", version.url.c_str());
}

//! @test New Version Available response support.
TEST(SUOTAServer, NewVersionAvailableResponse)
{
   mock("SUOTA::Server").expectOneCall("new_version_available")
      .withParameter("result", NewVersionResponse::INVALID_SOFTWARE);

   packet.message.itf.member = SUOTA::NEW_VERSION_AVAILABLE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.itf.role   = Interface::CLIENT_ROLE;

   payload                   = Common::ByteArray({0x00, 0x00, 0x00,
                                                  NewVersionResponse::INVALID_SOFTWARE,
                                                  0x00, 0x00, 0x00});

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("SUOTA::Server").checkExpectations();

   mock("support").expectOneCall("assert").ignoreOtherParameters();
   auto res = server.handle(packet, payload, payload.size());
   CHECK_EQUAL(Common::Result::FAIL_ARG, res);
   mock("support").checkExpectations();
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
