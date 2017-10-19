// =============================================================================
/*!
 * @file       tests/core/test_suota.cpp
 *
 * This is file contains the unit tests for the SUOTA interface
 * implementation.
 *
 * @version    1.4.3
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
// Helper Functions
// =============================================================================

static const Version g_version("v1.2.3", "revA", "https");

static const Common::ByteArray g_version_payload({
   0x00, 0x00, 0x00,
   0x06, 'v', '1', '.', '2', '.', '3', // Software version string.
   0x04, 'r', 'e', 'v', 'A',           // Hardware version string.
   0x05, 'h', 't', 't', 'p', 's',      // URL string.
   0x00, 0x00, 0x00,
});

static void check_version(const Version &version, const char *file, int line)
{
   STRCMP_EQUAL_LOCATION("v1.2.3", version.sw_version.c_str(), NULL, file, line);
   STRCMP_EQUAL_LOCATION("revA", version.hw_version.c_str(), NULL, file, line);
   STRCMP_EQUAL_LOCATION("https", version.url.c_str(), file, NULL, line);
}

static void check_version(const HF::Common::ByteArray &payload, const char *file, int line,
                          uint16_t offset = 0)
{
   Version version;

   LONGS_EQUAL_LOCATION(18, version.unpack(payload, offset), NULL, file, line);

   check_version(version, file, line);
}

#define CHECK_VERSION(_arg, ...) \
   check_version(_arg, __FILE__, __LINE__,##__VA_ARGS__)

// =============================================================================
// SUOTA
// =============================================================================

//! Test Group for SUOTA interface parent class.
TEST_GROUP(SUOTA)
{
   struct SOUTABase: public Base
   {
      SOUTABase(HF::Core::Unit0 &unit): Base(unit) {}

      Interface::Role role() const
      {
         return Interface::SERVER_ROLE;
      }
   };

   Testing::Device *device;
   SOUTABase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new SOUTABase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;
      mock().clear();
   }
};

//! @test SUOTA::uid should return @c HF::Interface::SUOTA.
TEST(SUOTA, UID)
{
   LONGS_EQUAL(HF::Interface::SUOTA, service->uid());
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
   Version version("v1.2.3", "revA", "https");

   Common::ByteArray payload(3 + version.size() + 3);

   LONGS_EQUAL(version.size(), version.pack(payload, 3));

   CHECK_EQUAL(g_version_payload, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, version.pack(payload, payload.size()));

   mock("support").checkExpectations();
}

TEST(SUOTA, Version_Unpack)
{
   Version version;

   STRCMP_EQUAL("", version.sw_version.c_str());
   STRCMP_EQUAL("", version.hw_version.c_str());
   STRCMP_EQUAL("", version.url.c_str());

   CHECK_VERSION(g_version_payload, 3);

   mock("support").expectOneCall("assert").ignoreOtherParameters();
   version = Version();

   LONGS_EQUAL(0, version.unpack(g_version_payload, g_version_payload.size()));

   mock("support").checkExpectations();

   STRCMP_EQUAL("", version.sw_version.c_str());
   STRCMP_EQUAL("", version.hw_version.c_str());
   STRCMP_EQUAL("", version.url.c_str());
}

// =============================================================================
// Check Version Response Message
// =============================================================================

TEST(SUOTA, CheckVersionResponse_Size)
{
   CheckVersionResponse response(CheckVersionResponse::FAIL_INFRASTUCTURE);

   LONGS_EQUAL(3 * sizeof(uint8_t), CheckVersionResponse::min_size);
   LONGS_EQUAL(3 * sizeof(uint8_t), response.size());

   response = CheckVersionResponse(CheckVersionResponse::FAIL_INFRASTUCTURE, "v1.2.3");

   LONGS_EQUAL((1 + 1 + 6 + 1) * sizeof(uint8_t), response.size());

   response = CheckVersionResponse(CheckVersionResponse::FAIL_INFRASTUCTURE, "v1.2.3", "revA",
                                   "https");

   LONGS_EQUAL((1 + 1 + 6 + 1 + 4 + 1 + 5) * sizeof(uint8_t), response.size());
}

TEST(SUOTA, CheckVersionResponse_Pack)
{
   CheckVersionResponse response(CheckVersionResponse::FAIL_INFRASTUCTURE, "v1.2.3", "revA",
                                 "https");

   Common::ByteArray expected = g_version_payload;
   expected[2] = CheckVersionResponse::FAIL_INFRASTUCTURE;

   Common::ByteArray payload(2 + response.size() + 3);

   LONGS_EQUAL(response.size(), response.pack(payload, 2));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, response.pack(payload, payload.size()));

   mock("support").checkExpectations();
}

TEST(SUOTA, CheckVersionResponse_Unpack)
{
   Common::ByteArray payload = g_version_payload;
   payload[2] = CheckVersionResponse::FAIL_INFRASTUCTURE;

   CheckVersionResponse response;
   LONGS_EQUAL(response.size(), response.unpack(payload, 2));

   LONGS_EQUAL(CheckVersionResponse::FAIL_INFRASTUCTURE, response.code);
   CHECK_VERSION(response);

   mock("support").expectOneCall("assert").ignoreOtherParameters();
   response = CheckVersionResponse();

   LONGS_EQUAL(0, response.unpack(payload, payload.size()));

   mock("support").checkExpectations();

   LONGS_EQUAL(CheckVersionResponse::FAIL_UNKNOWN, response.code);
   STRCMP_EQUAL("", response.sw_version.c_str());
   STRCMP_EQUAL("", response.hw_version.c_str());
   STRCMP_EQUAL("", response.url.c_str());
}

// =============================================================================
// Upgrade Status
// =============================================================================

TEST(SUOTA, UpgradeStatus_Size)
{
   UpgradeStatus status;

   LONGS_EQUAL(sizeof(uint8_t), UpgradeStatus::min_size);
   LONGS_EQUAL(sizeof(uint8_t), status.size());

   status = UpgradeStatus(UpgradeStatus::GMEP_SESSION_ERROR, "v1.2.3");

   LONGS_EQUAL((1 + 1 + 6) * sizeof(uint8_t), status.size());
}

TEST(SUOTA, UpgradeStatus_Pack)
{
   UpgradeStatus status(UpgradeStatus::GMEP_SESSION_ERROR, "v1.2.3");

   const Common::ByteArray expected({
      0x00, 0x00, 0x00,
      UpgradeStatus::GMEP_SESSION_ERROR,
      0x06, 'v', '1', '.', '2', '.', '3',
      0x00, 0x00, 0x00,
   });

   Common::ByteArray payload(3 + status.size() + 3);

   LONGS_EQUAL(status.size(), status.pack(payload, 3));

   CHECK_EQUAL(expected, payload);

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, status.pack(payload, payload.size()));

   mock("support").checkExpectations();
}

TEST(SUOTA, UpgradeStatus_Unpack)
{
   Common::ByteArray payload({
      0x00, 0x00, 0x00,
      UpgradeStatus::GMEP_SESSION_ERROR,
   });

   UpgradeStatus status;
   LONGS_EQUAL(status.size(), status.unpack(payload, 3));

   LONGS_EQUAL(UpgradeStatus::GMEP_SESSION_ERROR, status.code);
   CHECK_TRUE(status.sw_version.empty());

   payload = Common::ByteArray({
      0x00, 0x00, 0x00,
      UpgradeStatus::GMEP_SESSION_ERROR,
      0x06, 'v', '1', '.', '2', '.', '3',    // Software version string.
      0x00, 0x00, 0x00,
   });

   status = UpgradeStatus();
   LONGS_EQUAL(UpgradeStatus::min_size + 7 * sizeof(uint8_t), status.unpack(payload, 3));

   LONGS_EQUAL(UpgradeStatus::GMEP_SESSION_ERROR, status.code);
   CHECK_FALSE(status.sw_version.empty());

   STRCMP_EQUAL("v1.2.3", status.sw_version.c_str());

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, status.unpack(payload, payload.size()));

   mock("support").checkExpectations();
}

// =============================================================================
// SUOTA Client
// =============================================================================

//! Test Group for SUOTA Client interface class.
TEST_GROUP(SUOTAClient)
{
   struct SUOTAClient: public SUOTA::Client
   {
      SUOTAClient(HF::Core::Unit0 &unit): SUOTA::Client(unit) {}

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

      using SUOTA::Client::check_version;

      void check_version(const Protocol::Address &addr, const CheckVersionResponse &response)
      {
         UNUSED(addr);
         UNUSED(response);
         mock("SUOTA::Client").actualCall("check_version")
            .withParameter("code", (int) response.code)
            .withParameter("sw", response.sw_version.c_str())
            .withParameter("hw", response.hw_version.c_str())
            .withParameter("url", response.url.c_str());
      }
   };

   Testing::Device *device;
   SUOTAClient *client;
   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      device                    = new Testing::Device();

      client                    = new SUOTAClient(*(device->unit0()));
      addr                      = Protocol::Address(42);

      packet                    = Protocol::Packet();

      packet.source.device      = 1;
      packet.source.unit        = 2;
      packet.destination.device = 3;
      packet.destination.unit   = 4;

      packet.message.itf.role   = HF::Interface::CLIENT_ROLE;
      packet.message.itf.id     = client->uid();
      packet.message.itf.member = 0xFF;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;
      delete device;

      mock().clear();
   }
};

//! @test New Version Available support.
TEST(SUOTAClient, NewVersionAvailable)
{
   mock("AbstractDevice").expectOneCall("send");
   mock("SUOTA::Client").expectOneCall("new_version_available")
      .withParameter("sw", "v1.2.3")
      .withParameter("hw", "revA")
      .withParameter("url", "https")
      .andReturnValue(NewVersionResponse::UNSUPPORTED_HARDWARE);

   packet.message.itf.member = SUOTA::NEW_VERSION_AVAILABLE_CMD;

   payload                   = g_version_payload;

   auto res = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, res);

   mock("AbstractDevice").checkExpectations();
   mock("SUOTA::Client").checkExpectations();

   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *packet_response = device->packets.back();

   CHECK_TRUE(packet_response != nullptr);

   LONGS_EQUAL(1, packet_response->destination.device);
   LONGS_EQUAL(2, packet_response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, packet_response->destination.mod);

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, packet_response->message.itf.role);
   LONGS_EQUAL(client->uid(), packet_response->message.itf.id);
   LONGS_EQUAL(SUOTA::NEW_VERSION_AVAILABLE_CMD, packet_response->message.itf.member);

   LONGS_EQUAL(Protocol::Message::COMMAND_RES, packet_response->message.type);

   Protocol::Response response;
   response.unpack(packet_response->message.payload);

   LONGS_EQUAL(NewVersionResponse::UNSUPPORTED_HARDWARE, response.code);

   mock("SUOTA::Client").expectOneCall("new_version_available").ignoreOtherParameters();
   mock("support").expectNCalls(2, "assert").ignoreOtherParameters();
   mock("AbstractDevice").expectOneCall("send");

   res = client->handle(packet, payload, payload.size());
   CHECK_EQUAL(Common::Result::FAIL_ARG, res);

   mock("AbstractDevice").checkExpectations();
   mock("support").checkExpectations();
   mock("SUOTA::Client").checkExpectations();
}

//! @test Check Version command support.
TEST(SUOTAClient, CheckVersion)
{
   mock("AbstractDevice").expectOneCall("send");

   client->check_version(addr, g_version);

   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(SUOTA::CHECK_VERSION_CMD, packet->message.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);

   CHECK_VERSION(packet->message.payload);
}

//! @test Check Version response support.
TEST(SUOTAClient, CheckVersionResponse)
{
   Common::ByteArray payload = g_version_payload;
   payload[2]              = CheckVersionResponse::FAIL_INFRASTUCTURE;

   packet.message.itf.role = HF::Interface::SERVER_ROLE;

   auto res = client->handle(packet, payload, 2);
   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, res);

   mock("SUOTA::Client").expectOneCall("check_version")
      .withParameter("code", CheckVersionResponse::FAIL_INFRASTUCTURE)
      .withParameter("sw", "v1.2.3")
      .withParameter("hw", "revA")
      .withParameter("url", "https");

   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.itf.member = CHECK_VERSION_CMD;
   packet.message.itf.role   = HF::Interface::SERVER_ROLE;

   res                       = client->handle(packet, payload, 2);
   LONGS_EQUAL(Common::Result::OK, res);

   mock("SUOTA::Client").checkExpectations();

   mock("support").expectNCalls(2, "assert").ignoreOtherParameters();

   res = client->handle(packet, payload, payload.size());
   LONGS_EQUAL(Common::Result::FAIL_ARG, res);

   mock("support").checkExpectations();
}

//! @test Upgrade Complete support.
TEST(SUOTAClient, UpgradeComplete)
{
   mock("AbstractDevice").expectOneCall("send");

   client->upgrade_complete(addr, UpgradeStatus::INVALID_IMAGE, "v1.2.3");

   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(SUOTA::UPGRADE_COMPLETE_CMD, packet->message.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);

   UpgradeStatus upgrade;
   CHECK_TRUE(upgrade.unpack(packet->message.payload) != 0);

   LONGS_EQUAL(UpgradeStatus::INVALID_IMAGE, upgrade.code);

   STRCMP_EQUAL("v1.2.3", upgrade.sw_version.c_str());
}

// =============================================================================
// SUOTA Server
// =============================================================================

//! Test Group for SUOTA Server interface class.
TEST_GROUP(SUOTAServer)
{
   struct SUOTAServer: public SUOTA::Server
   {
      SUOTAServer(HF::Core::Unit0 &unit): SUOTA::Server(unit) {}

      using SUOTA::Server::new_version_available;

      void new_version_available(const Protocol::Address &addr, NewVersionResponse result)
      {
         UNUSED(addr);
         mock("SUOTA::Server").actualCall("new_version_available").withParameter("result",
                                                                                 (int) result);
      }

      CheckVersionResponse check_version(const Protocol::Address &addr, const Version &version)
      {
         UNUSED(addr);
         return *((CheckVersionResponse *) mock("SUOTA::Server").actualCall("check_version")
                     .withParameter("sw", version.sw_version.c_str())
                     .withParameter("hw", version.hw_version.c_str())
                     .withParameter("url", version.url.c_str())
                     .returnPointerValue());
      }

      void upgrade_complete(const Protocol::Address &addr, const UpgradeStatus &status)
      {
         UNUSED(addr);
         mock("SUOTA::Server").actualCall("upgrade_complete")
            .withParameter("status", status.code)
            .withParameter("sw_version", status.sw_version.c_str());
      }
   };

   Testing::Device *device;
   SUOTAServer *server;
   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      server                    = new SUOTAServer(*(device->unit0()));
      addr                      = Protocol::Address(42);

      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server->uid();
      packet.message.itf.member = 0xFF;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete device;
      mock().clear();
   }
};

//! @test New Version Available support.
TEST(SUOTAServer, NewVersionAvailable)
{
   mock("AbstractDevice").expectOneCall("send");

   server->new_version_available(addr, g_version);

   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, packet->message.itf.role);
   LONGS_EQUAL(server->uid(), packet->message.itf.id);
   LONGS_EQUAL(SUOTA::NEW_VERSION_AVAILABLE_CMD, packet->message.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);

   CHECK_VERSION(packet->message.payload);
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

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("SUOTA::Server").checkExpectations();

   mock("support").expectOneCall("assert").ignoreOtherParameters();
   auto res = server->handle(packet, payload, payload.size());
   CHECK_EQUAL(Common::Result::FAIL_ARG, res);
   mock("support").checkExpectations();
}

//! @test Check Version support.
TEST(SUOTAServer, CheckVersion)
{
   payload = g_version_payload;

   CheckVersionResponse expected(CheckVersionResponse::FAIL_INFRASTUCTURE, "v3.2.1", "revB", "ftp");

   mock("AbstractDevice").expectOneCall("send");
   mock("SUOTA::Server").expectOneCall("check_version")
      .withParameter("sw", "v1.2.3")
      .withParameter("hw", "revA")
      .withParameter("url", "https")
      .andReturnValue(&expected);

   packet.message.itf.member = SUOTA::CHECK_VERSION_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("SUOTA::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet_response = device->packets.back();
   CHECK_TRUE(packet_response != nullptr);

   CheckVersionResponse actual;
   LONGS_EQUAL(expected.size(), actual.unpack(packet_response->message.payload));

   LONGS_EQUAL(expected.code, actual.code);

   STRCMP_EQUAL(expected.sw_version.c_str(), actual.sw_version.c_str());
   STRCMP_EQUAL(expected.hw_version.c_str(), actual.hw_version.c_str());
   STRCMP_EQUAL(expected.url.c_str(), actual.url.c_str());

   mock("AbstractDevice").expectOneCall("send");

   CHECK_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, payload.size()));

   mock("AbstractDevice").checkExpectations();

   packet_response = device->packets.back();
   CHECK_TRUE(packet_response != nullptr);

   actual = CheckVersionResponse();
   LONGS_EQUAL(CheckVersionResponse::min_size, actual.unpack(packet_response->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, actual.code);
}

//! @test Upgrade Complete support.
TEST(SUOTAServer, UpgradeComplete)
{
   packet.message.itf.member = SUOTA::UPGRADE_COMPLETE_CMD;

   payload                   = Common::ByteArray({
      0x00, 0x00, 0x00,
      UpgradeStatus::GMEP_SESSION_ERROR,
      0x06, 'v', '1', '.', '2', '.', '3',     // Software version string.
      0x00, 0x00, 0x00,
   });

   mock("SUOTA::Server").expectOneCall("upgrade_complete")
      .withParameter("status", UpgradeStatus::GMEP_SESSION_ERROR)
      .withParameter("sw_version", "v1.2.3");

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("SUOTA::Server").checkExpectations();

   mock("support").expectNCalls(2, "assert").ignoreOtherParameters();
   auto res = server->handle(packet, payload, payload.size());
   CHECK_EQUAL(Common::Result::FAIL_ARG, res);
   mock("support").checkExpectations();
}
