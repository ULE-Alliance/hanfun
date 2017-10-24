// =============================================================================
/*!
 * @file       tests/core/test_group_table.cpp
 *
 * This is file contains the unit tests for the Group Table service
 * implementation.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include <algorithm>
#include <iostream>
#include <iomanip>

#include "hanfun/core/group_table.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::GroupTable;

// =============================================================================
// Helper Functions
// =============================================================================

std::ostream &operator<<(std::ostream &stream, const HF::Core::GroupTable::Entry &entry)
{
   std::ios_base::fmtflags ff = stream.flags();
   char f                     = stream.fill(' ');

   stream << "G: 0x" << std::hex << entry.group << ", U: 0x" << std::hex << entry.unit;

   stream << std::setfill(f) << std::setiosflags(ff);
   return stream;
}

// =============================================================================
// Group Table
// =============================================================================

//! Test Group for GroupTable service parent class.
TEST_GROUP(GroupTable)
{
   struct GroupTableBase: public InterfaceParentHelper<GroupTable::Base>
   {
      GroupTableBase(HF::Core::Unit0 &unit): InterfaceParentHelper<GroupTable::Base>(unit) {}
   };

   Testing::Device *device;
   GroupTableBase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new GroupTableBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;

      mock().clear();
   }
};

//! @test GroupTable::uid should return @c HF::Interface::GROUP_TABLE.
TEST(GroupTable, UID)
{
   LONGS_EQUAL(HF::Interface::GROUP_TABLE, service->uid());
}

//! @test Number Of Entries support.
TEST(GroupTable, NumberOfEntries)
{
   HF::Attributes::IAttribute *attr = GroupTable::create_attribute(
      GroupTable::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_TABLE, attr->interface());

   delete attr;
}

//! @test Number Of Max Entries support.
TEST(GroupTable, NumberOfMaxEntries)
{
   HF::Attributes::IAttribute *attr = GroupTable::create_attribute(
      GroupTable::NUMBER_OF_MAX_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfMaxEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfMaxEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_TABLE, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(GroupTable, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr = GroupTable::create_attribute(GroupTable::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

TEST(GroupTable, Entry)
{
   HF::Core::GroupTable::Entry entry;

   LONGS_EQUAL(0x0000, entry.group);
   LONGS_EQUAL(0x00, entry.unit);
}

TEST(GroupTable, Entry_Size)
{
   using namespace HF::Core::GroupTable;

   Entry entry;
   LONGS_EQUAL(sizeof(uint16_t) + sizeof(uint8_t), entry.size());
}

TEST(GroupTable, Entry_Pack)
{
   using namespace HF::Core::GroupTable;

   /* *INDENT-OFF* */
   Common::ByteArray expected { 0x00, 0x00, 0x00,
                                0x5A, 0x5A,  // Group Address.
                                0x42,        // Unit ID
                                0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   Entry entry(0x5A5A, 0x42);

   Common::ByteArray data(entry.size() + 6);

   uint16_t size = entry.pack(data, 3);

   LONGS_EQUAL(entry.size(), size);

   CHECK_EQUAL(expected, data);
}

TEST(GroupTable, Entry_Unpack)
{
   /* *INDENT-OFF* */
   Common::ByteArray data { 0x00, 0x00, 0x00,
                            0x5A, 0x5A,  // Group Address.
                            0x42,        // Unit ID
                            0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   Entry entry;

   uint16_t size = entry.unpack(data, 3);

   LONGS_EQUAL(entry.size(), size);

   LONGS_EQUAL(0x5A5A, entry.group);
   LONGS_EQUAL(0x42, entry.unit);
}

TEST(GroupTable, EntryEqual)
{
   GroupTable::Entry lhs;
   GroupTable::Entry rhs;

   lhs.group = 0x5A5A;
   lhs.unit  = 0x42;

   memcpy(&rhs, &lhs, sizeof(lhs));

   CHECK_TRUE(lhs == rhs);
   CHECK_FALSE(lhs != rhs);

   ++rhs.unit;

   CHECK_FALSE(lhs == rhs);
   CHECK_TRUE(lhs != rhs);

   memcpy(&rhs, &lhs, sizeof(lhs));

   CHECK_TRUE(lhs == rhs);

   ++rhs.group;

   CHECK_FALSE(lhs == rhs);
   CHECK_TRUE(lhs != rhs);
}

TEST(GroupTable, Response)
{
   HF::Core::GroupTable::Response response;

   LONGS_EQUAL(Common::Result::FAIL_UNKNOWN, response.code);
   LONGS_EQUAL(0x0000, response.group);
   LONGS_EQUAL(0x00, response.unit);
}

TEST(GroupTable, Response_Size)
{
   HF::Core::GroupTable::Response response;
   LONGS_EQUAL(sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t), response.size());
}

TEST(GroupTable, Response_Pack)
{
   /* *INDENT-OFF* */
   Common::ByteArray expected { 0x00, 0x00, 0x00,
                                Common::Result::FAIL_AUTH,  // Operation result.
                                0x5A, 0x5A,                 // Group Address.
                                0x42,                       // Unit ID
                                0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   HF::Core::GroupTable::Response response(Common::Result::FAIL_AUTH, 0x5A5A, 0x42);

   Common::ByteArray data(response.size() + 6);

   uint16_t size = response.pack(data, 3);

   LONGS_EQUAL(response.size(), size);

   CHECK_EQUAL(expected, data);
}

TEST(GroupTable, Response_Unpack)
{
   /* *INDENT-OFF* */
   Common::ByteArray data { 0x00, 0x00, 0x00,
                            Common::Result::FAIL_AUTH,  // Operation result.
                            0x5A, 0x5A,                 // Group Address.
                            0x42,                       // Unit ID
                            0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   HF::Core::GroupTable::Response response;

   uint16_t size = response.unpack(data, 3);

   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
   LONGS_EQUAL(0x5A5A, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

TEST(GroupTable, ReadEntries)
{
   ReadEntries params;

   LONGS_EQUAL(0, params.start);
   LONGS_EQUAL(1, params.count);
}

TEST(GroupTable, GetEntries_Size)
{
   ReadEntries params;

   LONGS_EQUAL(sizeof(uint8_t) + sizeof(uint8_t), params.size());
   LONGS_EQUAL(sizeof(uint8_t) + sizeof(uint8_t), ReadEntries::min_size);
}

TEST(GroupTable, GetEntries_Pack)
{
   /* *INDENT-OFF* */
   Common::ByteArray expected { 0x00, 0x00, 0x00,
                                0x5A,  // Start Index
                                0x42,  // Number of entries.
                                0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   ReadEntries params(0x5A, 0x42);

   Common::ByteArray data(params.size() + 6);

   uint16_t size = params.pack(data, 3);

   LONGS_EQUAL(params.size(), size);

   CHECK_EQUAL(expected, data);
}

TEST(GroupTable, GetEntries_Unpack)
{
   /* *INDENT-OFF* */
   Common::ByteArray data { 0x00, 0x00, 0x00,
                            0x5A,  // Start Index
                            0x42,  // Number of entries.
                            0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   ReadEntries params;

   uint16_t size = params.unpack(data, 3);

   LONGS_EQUAL(params.size(), size);

   LONGS_EQUAL(0x5A, params.start);
   LONGS_EQUAL(0x42, params.count);
}

TEST(GroupTable, ReadEntriesResponse)
{
   ReadEntriesResponse response;

   LONGS_EQUAL(0, response.start);
   CHECK_TRUE(response.entries.empty());
}

TEST(GroupTable, GetEntriesResponse_Size)
{
   ReadEntriesResponse params;

   // -----------> Result code     + Start Index     + Number of entries.
   uint16_t size = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t);

   LONGS_EQUAL(size, params.size());

   params = ReadEntriesResponse(Common::Result::OK, 0, 10);

   for (int i = 0; i < 10; ++i)
   {
      params.entries.emplace_back(0x5A5A, 42);
   }

   LONGS_EQUAL(size + 10 * Entry::min_size, params.size());
}

TEST(GroupTable, GetEntriesResponse_Pack)
{
   /* *INDENT-OFF* */
   Common::ByteArray expected { 0x00, 0x00, 0x00,
                                Common::Result::FAIL_AUTH,
                                0x22,        // Start Index
                                0x05,        // Number of entries.
                                0x5A, 0x01,  // Entry 1
                                0x71,
                                0x5A, 0x02,  // Entry 2
                                0x72,
                                0x5A, 0x03,  // Entry 3
                                0x73,
                                0x5A, 0x04,  // Entry 4
                                0x74,
                                0x5A, 0x05,  // Entry 5
                                0x75,
                                0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   ReadEntriesResponse response(Common::Result::FAIL_AUTH, 0x22, 5);

   for (int i = 1; i <= 5; ++i)
   {
      response.entries.emplace_back(0x5A00 | i, 0x70 | i);
   }

   Common::ByteArray data(response.size() + 6);

   uint16_t size = response.pack(data, 3);

   LONGS_EQUAL(response.size(), size);

   CHECK_EQUAL(expected, data);
}

TEST(GroupTable, GetEntriesResponse_Unpack)
{
   /* *INDENT-OFF* */
   Common::ByteArray data { 0x00, 0x00, 0x00,
                            Common::Result::FAIL_AUTH,
                            0x22,        // Start Index
                            0x05,        // Number of entries.
                            0x5A, 0x01,  // Entry 1
                            0x71,
                            0x5A, 0x02,  // Entry 2
                            0x72,
                            0x5A, 0x03,  // Entry 3
                            0x73,
                            0x5A, 0x04,  // Entry 4
                            0x74,
                            0x5A, 0x05,  // Entry 5
                            0x75,
                            0x00, 0x00, 0x00};
   /* *INDENT-ON* */

   ReadEntriesResponse response;

   uint16_t size = response.unpack(data, 3);

   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
   LONGS_EQUAL(0x22, response.start);
   LONGS_EQUAL(0x05, response.entries.size());

   for (int i = 1; i <= 5; ++i)
   {
      Entry e(0x5A00 | i, 0x70 | i);
      check_index(e, response.entries[i - 1], i, "Entries", __FILE__, __LINE__);
   }
}

TEST(GroupTable, GetEntriesResponse_Unpack_Fail)
{
   /* *INDENT-OFF* */
   Common::ByteArray data { 0x00, 0x00, 0x00,
                            Common::Result::FAIL_AUTH,
                            0x22,        // Start Index
                            0x05,        // Number of entries.
                            0x5A, 0x01,  // Entry 1
                            0x71,
                            0x5A, 0x02,  // Entry 2
                            0x72};
   /* *INDENT-ON* */

   ReadEntriesResponse response;

   mock("support").expectOneCall("assert").ignoreOtherParameters();

   LONGS_EQUAL(0, response.unpack(data, 3));

   mock("support").checkExpectations();

   LONGS_EQUAL(0x22, response.start);
   LONGS_EQUAL(0, response.entries.size());
}

// =============================================================================
// Group Table Client
// =============================================================================

//! Test Group for Group Table Client interface class.
TEST_GROUP(GroupTableClient)
{
   struct GroupTableClient: public GroupTable::Client
   {
      Protocol::Address                        addr;
      GroupTable::Response                     response;

      GroupTable::ReadEntriesResponse          read_entries_res;

      Common::Result                           code = Common::Result::FAIL_UNKNOWN;

      GroupTableClient(HF::Core::Unit0 &unit): GroupTable::Client(unit) {}

      void added(const Protocol::Address &addr, const GroupTable::Response &response) override
      {
         mock("GroupTable::Client").actualCall("added");

         this->addr     = addr;
         this->response = response;

         GroupTable::Client::added(addr, response);
      }

      void removed(const Protocol::Address &addr, const GroupTable::Response &response) override
      {
         mock("GroupTable::Client").actualCall("removed");

         this->addr     = addr;
         this->response = response;

         GroupTable::Client::removed(addr, response);
      }

      void removed_all(const Protocol::Address &addr, const Protocol::Response &response) override
      {
         mock("GroupTable::Client").actualCall("removed_all");

         this->addr = addr;
         this->code = response.code;

         GroupTable::Client::removed_all(addr, response);
      }

      using GroupTable::Client::read_entries;

      void read_entries(const Protocol::Address &addr, const ReadEntriesResponse &response) override
      {
         mock("GroupTable::Client").actualCall("read_entries");

         this->addr             = addr;
         this->read_entries_res = response;

         GroupTable::Client::removed_all(addr, response);
      }
   };

   Testing::Device *device;
   GroupTableClient *client;

   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      client                    = new GroupTableClient(*(device->unit0()));

      addr                      = Protocol::Address(42, 0);

      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = client->uid();
      packet.message.itf.member = 0xFF;

      packet.source             = addr;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;
      delete device;

      mock().clear();
   }
};

//! @test Add support.
TEST(GroupTableClient, Add)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);

   client->add(addr, expected);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload, 0);

   CHECK_TRUE(expected == entry);
}

//! @test Add support.
TEST(GroupTableClient, Add_2)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);

   client->add(addr, expected.group, expected.unit);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload, 0);

   CHECK_TRUE(expected == entry);
}

TEST(GroupTableClient, Add_3)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);

   client->add(0x4242, expected);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(0x4242, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload, 0);

   CHECK_TRUE(expected == entry);
}

TEST(GroupTableClient, Add_4)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);

   client->add(0x4242, expected.group, expected.unit);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(0x4242, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload, 0);

   CHECK_TRUE(expected == entry);
}

//! @test Added callback support.
TEST(GroupTableClient, Added)
{
   /* *INDENT-OFF* */
   payload =
   {
     0x00, 0x00, 0x00,
     Common::Result::FAIL_AUTH,  // Result code.
     0x5A, 0x5A,                 // Group Address.
     0x42,                       // Unit ID.
     0x00, 0x00, 0x00,
   };
   /* *INDENT-ON* */

   mock("GroupTable::Client").expectOneCall("added");

   packet.message.itf.member = GroupTable::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.length     = GroupTable::Response::min_size;

   LONGS_EQUAL(Common::Result::OK, client->handle(packet, payload, 3));

   mock("GroupTable::Client").checkExpectations();

   LONGS_EQUAL(42, client->addr.device);
   LONGS_EQUAL(0, client->addr.unit);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, client->response.code);
   LONGS_EQUAL(0x5A5A, client->response.group);
   LONGS_EQUAL(0x42, client->response.unit);
}

//! @test Remove support.
TEST(GroupTableClient, Remove)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);
   client->remove(addr, expected);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload);

   CHECK_TRUE(expected == expected);
}

//! @test Remove support.
TEST(GroupTableClient, Remove_2)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);
   client->remove(addr, expected.group, expected.unit);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload);

   CHECK_TRUE(expected == expected);
}

TEST(GroupTableClient, Remove_3)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);

   client->remove(0x4242, expected);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(0x4242, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload, 0);

   CHECK_TRUE(expected == entry);
}

TEST(GroupTableClient, Remove_4)
{
   mock("AbstractDevice").expectOneCall("send");

   Entry expected(0x5A5A, 0x42);

   client->remove(0x4242, expected.group, expected.unit);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(0x4242, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);

   Entry entry;
   entry.unpack(packet->message.payload);

   CHECK_TRUE(expected == entry);
}

//! @test Removed callback support.
TEST(GroupTableClient, Removed)
{
   /* *INDENT-OFF* */
   payload =
   {
     0x00, 0x00, 0x00,
     Common::Result::FAIL_AUTH,  // Result code.
     0x5A, 0x5A,                 // Group Address.
     0x42,                       // Unit ID.
     0x00, 0x00, 0x00,
   };
   /* *INDENT-ON* */

   mock("GroupTable::Client").expectOneCall("removed");

   packet.message.itf.member = GroupTable::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.length     = GroupTable::Response::min_size;

   LONGS_EQUAL(Common::Result::OK, client->handle(packet, payload, 3));

   mock("GroupTable::Client").checkExpectations();

   LONGS_EQUAL(42, client->addr.device);
   LONGS_EQUAL(0, client->addr.unit);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, client->response.code);
   LONGS_EQUAL(0x5A5A, client->response.group);
   LONGS_EQUAL(0x42, client->response.unit);
}

//! @test Remove All support.
TEST(GroupTableClient, RemoveAll)
{
   mock("AbstractDevice").expectOneCall("send");

   client->remove_all(addr);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_ALL_CMD, packet->message.itf.member);
}

//! @test Remove All support.
TEST(GroupTableClient, RemoveAll_2)
{
   mock("AbstractDevice").expectOneCall("send");

   client->remove_all(0x4242);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(0x4242, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_ALL_CMD, packet->message.itf.member);
}

//! @test Removed All callback support.
TEST(GroupTableClient, RemovedAll)
{
   /* *INDENT-OFF* */
   payload =
   {
     0x00, 0x00, 0x00,
     Common::Result::FAIL_AUTH,  // Result code.
   };
   /* *INDENT-ON* */

   mock("GroupTable::Client").expectOneCall("removed_all");

   packet.message.itf.member = GroupTable::REMOVE_ALL_CMD;
   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.length     = Protocol::Response::min_size;

   LONGS_EQUAL(Common::Result::OK, client->handle(packet, payload, 3));

   mock("GroupTable::Client").checkExpectations();

   LONGS_EQUAL(42, client->addr.device);
   LONGS_EQUAL(0, client->addr.unit);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, client->code);
}

//! @test Read Entries support.
TEST(GroupTableClient, ReadEntries)
{
   mock("AbstractDevice").expectOneCall("send");

   ReadEntries expected(42, 24);
   client->read_entries(addr, expected);

   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, packet->message.type);
   LONGS_EQUAL(HF::Interface::SERVER_ROLE, packet->message.itf.role);
   LONGS_EQUAL(client->uid(), packet->message.itf.id);
   LONGS_EQUAL(GroupTable::READ_ENTRIES_CMD, packet->message.itf.member);

   ReadEntries params;
   params.unpack(packet->message.payload);

   LONGS_EQUAL(42, params.start);
   LONGS_EQUAL(24, params.count);
}

//! @test Read entries callback support.
TEST(GroupTableClient, ReadEntries_Callback)
{
   /* *INDENT-OFF* */
   payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x22,        // Start Index
       0x05,        // Number of entries.
       0x5A, 0x01,  // Entry 1
       0x71,
       0x5A, 0x02,  // Entry 2
       0x72,
       0x5A, 0x03,  // Entry 3
       0x73,
       0x5A, 0x04,  // Entry 4
       0x74,
       0x5A, 0x05,  // Entry 5
       0x75,
       0x00, 0x00, 0x00
   };
   /* *INDENT-ON* */

   mock("GroupTable::Client").expectOneCall("read_entries");

   packet.message.itf.member = GroupTable::READ_ENTRIES_CMD;
   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.length     = payload.size() - 6;

   LONGS_EQUAL(Common::Result::OK, client->handle(packet, payload, 3));

   mock("GroupTable::Client").checkExpectations();

   LONGS_EQUAL(42, client->addr.device);
   LONGS_EQUAL(0, client->addr.unit);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, client->read_entries_res.code);
   LONGS_EQUAL(0x22, client->read_entries_res.start);

   LONGS_EQUAL(0x05, client->read_entries_res.entries.size());

   for (int i = 1; i <= 5; ++i)
   {
      Entry e(0x5A00 | i, 0x70 | i);
      check_index(e, client->read_entries_res.entries[i - 1], i, "Entries", __FILE__, __LINE__);
   }
}

//! @test Read entries callback support.
TEST(GroupTableClient, ReadEntries_Callback_Fail)
{
   /* *INDENT-OFF* */
   payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x22,        // Start Index
       0x05,        // Number of entries.
       0x5A, 0x01,  // Entry 1
       0x71,
       0x5A, 0x02,  // Entry 2
       0x72,
       0x00, 0x00, 0x00
   };
   /* *INDENT-ON* */

   mock("GroupTable::Client").expectNoCall("read_entries");
   mock("support").expectNCalls(2, "assert").ignoreOtherParameters();

   packet.message.itf.member = GroupTable::READ_ENTRIES_CMD;
   packet.message.type       = Protocol::Message::COMMAND_RES;
   packet.message.length     = payload.size() - 6;

   LONGS_EQUAL(Common::Result::FAIL_ARG, client->handle(packet, payload, 3));

   mock("GroupTable::Client").checkExpectations();
   mock("support").checkExpectations();
}

// =============================================================================
// Group Table Server
// =============================================================================

//! Test Group for Group Table Server interface class.
TEST_GROUP(GroupTableServer)
{
   // TODO Add required unit tests.
   struct GroupTableServer: public GroupTable::DefaultServer
   {
      GroupTableServer(HF::Core::Unit0 &unit):
         GroupTable::DefaultServer(unit) {}

      Common::Result add(const Protocol::Address &addr, const Entry &entry) override
      {
         mock("GroupTable::Server").actualCall("add");
         return GroupTable::DefaultServer::add(addr, entry);
      }

      Common::Result remove(const Protocol::Address &addr, const Entry &entry) override
      {
         mock("GroupTable::Server").actualCall("remove");
         return GroupTable::DefaultServer::remove(addr, entry);
      }

      Common::Result remove_all(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("remove_all");
         return GroupTable::DefaultServer::remove_all(addr);
      }

      ReadEntriesResponse read_entries(const Protocol::Address &addr,
                                       const ReadEntries &params) override
      {
         mock("GroupTable::Server").actualCall("read_entries");
         return GroupTable::DefaultServer::read_entries(addr, params);
      }

      void notify(const HF::Attributes::IAttribute &old_value,
                  const HF::Attributes::IAttribute &new_value) const
      {
         UNUSED(old_value);
         UNUSED(new_value);

         mock("Interface").actualCall("notify");
      }
   };

   Testing::Device *device;
   GroupTableServer *server;


   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      server                    = new GroupTableServer(*(device->unit0()));

      packet                    = Protocol::Packet();
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server->uid();
      packet.message.itf.member = 0xFF;

      packet.source             = Protocol::Address(0, 0);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete device;

      mock().clear();
   }
};

//! @test Number Of Entries support.
TEST(GroupTableServer, NumberOfEntries)
{
   auto attr = GroupTable::create_attribute(GroupTable::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_TABLE, attr->interface());

   delete attr;

   attr = Core::create_attribute((GroupTable::IServer *) nullptr, NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_TABLE, attr->interface());

   POINTERS_EQUAL(nullptr, attr->owner());

   delete attr;

   CHECK_ATTRIBUTE_ALLOC(NumberOfEntries);
}

//! @test Number Of Max Entries support.
TEST(GroupTableServer, NumberOfMaxEntries)
{
   CHECK_ATTRIBUTE(GroupTableServer, NumberOfMaxEntries, false, number_of_max_entries, 42, 142);
}

//! @test Add support.
TEST(GroupTableServer, Add_OK)
{
   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, 0x5A, // Group Address.
      0x42,       // Unit ID.
      0x00, 0x00, 0x00,
   };

   mock("GroupTable::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::ADD_CMD;
   packet.message.length     = payload.size();

   LONGS_EQUAL(0, server->number_of_entries());

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(1, server->number_of_entries());

   int count = 0;
   server->entries().for_each(0x5A5A, [&count](const Entry &e)
   {
      UNUSED(e);
      ++count;
   });

   LONGS_EQUAL(1, count);

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(0, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::OK, response.code);
   LONGS_EQUAL(0x5A5A, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

//! @test Add support - Max Entries.
TEST(GroupTableServer, Add_Fail_Max_Entries)
{
   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, 0x5A, // Group Address.
      0x42,       // Unit ID.
      0x00, 0x00, 0x00,
   };

   mock("GroupTable::Server").expectOneCall("add");
   mock("AbstractDevice").actualCall("send");

   packet.message.itf.member = GroupTable::ADD_CMD;
   packet.message.length     = payload.size();

   server->number_of_max_entries(2);

   Entry e1(0x5A5A, 0x42);
   LONGS_EQUAL(Common::Result::OK, server->entries().save(e1));
   Entry e2(0x5A5A, 0x43);
   LONGS_EQUAL(Common::Result::OK, server->entries().save(e2));

   int size = server->entries().size();
   LONGS_EQUAL(2, size);

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(0, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, response.code);
   LONGS_EQUAL(0x5A5A, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

//! @test Add support - Command source.
TEST(GroupTableServer, Add_Source)
{
   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, 0x5A, // Group Address.
      0x42,       // Unit ID.
      0x00, 0x00, 0x00,
   };

   mock("GroupTable::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::ADD_CMD;
   packet.message.length     = payload.size();

   packet.source             = Protocol::Address(42, 0);

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
   LONGS_EQUAL(0x5A5A, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

//! @test Remove support.
TEST(GroupTableServer, Remove_OK)
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<uint8_t> dist(0, 9);

   uint8_t index = dist(gen);

   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, (uint8_t) (0x5A + index), // Group Address.
      0x42,                           // Unit ID.
      0x00, 0x00, 0x00,
   };

   // Fill in some entries
   for (int i = 0; i < 10; ++i)
   {
      Entry e(0x5A5A + i, 0x42);
      server->entries().save(e);
   }

   LONGS_EQUAL(10, server->number_of_entries());

   mock("GroupTable::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::REMOVE_CMD;
   packet.message.length     = payload.size();

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(9, server->number_of_entries());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(0, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::OK, response.code);
   LONGS_EQUAL(0x5A5A + index, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

//! @test Remove support - No group entry (Group Address).
TEST(GroupTableServer, Remove_Fail_No_Entry_Group)
{
   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, 0x80, // Group Address.
      0x42,       // Unit ID.
      0x00, 0x00, 0x00,
   };

   // Fill in some entries
   for (int i = 0; i < 10; ++i)
   {
      Entry e(0x5A5A + i, 0x42);
      server->entries().save(e);
   }

   LONGS_EQUAL(10, server->number_of_entries());

   mock("GroupTable::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::REMOVE_CMD;
   packet.message.length     = payload.size();

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(10, server->number_of_entries());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(0, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
   LONGS_EQUAL(0x5A80, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

//! @test Remove support - No group entry (Group Address).
TEST(GroupTableServer, Remove_Fail_No_Entry_Unit_ID)
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<uint8_t> dist(0, 9);

   uint8_t index = dist(gen);

   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, (uint8_t) (0x5A + index), // Group Address.
      0x24,                           // Unit ID.
      0x00, 0x00, 0x00,
   };

   // Fill in some entries
   for (int i = 0; i < 10; ++i)
   {
      Entry e(0x5A5A + i, 0x42);
      server->entries().save(e);
   }

   LONGS_EQUAL(10, server->number_of_entries());

   mock("GroupTable::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::REMOVE_CMD;
   packet.message.length     = payload.size();

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(10, server->number_of_entries());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(0, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
   LONGS_EQUAL(0x5A5A + index, response.group);
   LONGS_EQUAL(0x24, response.unit);
}

//! @test Remove support - Command source.
TEST(GroupTableServer, Remove_Source)
{
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<uint8_t> dist(0, 9);

   uint8_t index = dist(gen);

   payload =
   {
      0x00, 0x00, 0x00,
      0x5A, (uint8_t) (0x5A + index), // Group Address.
      0x42,                           // Unit ID.
      0x00, 0x00, 0x00,
   };

   // Fill in some entries
   for (int i = 0; i < 10; ++i)
   {
      Entry e(0x5A5A + i, 0x42);
      server->entries().save(e);
   }

   LONGS_EQUAL(10, server->number_of_entries());

   mock("GroupTable::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::REMOVE_CMD;
   packet.message.length     = payload.size();

   packet.source             = Protocol::Address(42, 0);

   LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(10, server->number_of_entries());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   GroupTable::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
   LONGS_EQUAL(0x5A5A + index, response.group);
   LONGS_EQUAL(0x42, response.unit);
}

//! @test Remove All support.
TEST(GroupTableServer, RemoveAll)
{
   // Fill in some entries
   for (int i = 0; i < 10; ++i)
   {
      Entry e(0x5A5A + i, 0x42);
      server->entries().save(e);
   }

   LONGS_EQUAL(10, server->number_of_entries());

   mock("GroupTable::Server").expectOneCall("remove_all");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::REMOVE_ALL_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(0, server->number_of_entries());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(0, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_ALL_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   Protocol::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::OK, response.code);
}

//! @test Remove All support - Command Source.
TEST(GroupTableServer, RemoveAll_Source)
{
   // Fill in some entries
   for (int i = 0; i < 10; ++i)
   {
      Entry e(0x5A5A + i, 0x42);
      server->entries().save(e);
   }

   LONGS_EQUAL(10, server->number_of_entries());

   mock("GroupTable::Server").expectOneCall("remove_all");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::REMOVE_ALL_CMD;
   packet.source             = Protocol::Address(42, 0);

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   LONGS_EQUAL(10, server->number_of_entries());

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_ALL_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   Protocol::Response response;
   LONGS_EQUAL(response.size(), packet->message.payload.size());

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
}

//! @test Read Entries support.
TEST(GroupTableServer, ReadEntries_OK)
{
   // Fill in some entries
   for (int i = 1; i <= 10; ++i)
   {
      Entry e(0x5A00 | i, 0x70 | i);
      server->entries().save(e);
   }

   payload =
   {
      0x00, 0x00, 0x00,
      0x02, // Read Offset.
      0x05, // Number of entries to read.
      0x00, 0x00, 0x00,
   };

   mock("GroupTable::Server").expectOneCall("read_entries");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::READ_ENTRIES_CMD;
   packet.source             = Protocol::Address(42, 0);
   packet.message.length     = ReadEntries::min_size;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::READ_ENTRIES_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   ReadEntriesResponse response;

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::OK, response.code);

   LONGS_EQUAL(0x02, response.start);

   LONGS_EQUAL(0x05, response.entries.size());

   for (int i = 0; i < 5; ++i)
   {
      Entry e(0x5A00 | (i + 3), 0x70 | (i + 3));
      check_index(e, response.entries[i], i, "Entries", __FILE__, __LINE__);
   }
}

//! @test Read Entries support.
TEST(GroupTableServer, ReadEntries_Bad_Index)
{
   // Fill in some entries
   for (int i = 1; i <= 10; ++i)
   {
      Entry e(0x5A00 | i, 0x70 | i);
      server->entries().save(e);
   }

   payload =
   {
      0x00, 0x00, 0x00,
      0x22, // Read Offset.
      0x01, // Number of entries to read.
      0x00, 0x00, 0x00,
   };

   mock("GroupTable::Server").expectOneCall("read_entries");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::READ_ENTRIES_CMD;
   packet.source             = Protocol::Address(42, 0);
   packet.message.length     = ReadEntries::min_size;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::READ_ENTRIES_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   ReadEntriesResponse response;

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);

   LONGS_EQUAL(0x22, response.start);

   LONGS_EQUAL(0x00, response.entries.size());
}

//! @test Read Entries support.
TEST(GroupTableServer, ReadEntries_Count)
{
   // Fill in some entries
   for (int i = 1; i <= 10; ++i)
   {
      Entry e(0x5A00 | i, 0x70 | i);
      server->entries().save(e);
   }

   payload =
   {
      0x00, 0x00, 0x00,
      0x02, // Read Offset.
      0x5A, // Number of entries to read.
      0x00, 0x00, 0x00,
   };

   mock("GroupTable::Server").expectOneCall("read_entries");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = GroupTable::READ_ENTRIES_CMD;
   packet.source             = Protocol::Address(42, 0);
   packet.message.length     = ReadEntries::min_size;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   auto packet = device->packets.back();

   CHECK_TRUE(packet != nullptr);

   LONGS_EQUAL(42, packet->destination.device);
   LONGS_EQUAL(0, packet->destination.unit);

   LONGS_EQUAL(Interface::GROUP_TABLE, packet->message.itf.id);
   LONGS_EQUAL(GroupTable::READ_ENTRIES_CMD, packet->message.itf.member);
   LONGS_EQUAL(Interface::SERVER_ROLE, packet->message.itf.role);

   ReadEntriesResponse response;

   uint16_t size = response.unpack(packet->message.payload);
   LONGS_EQUAL(response.size(), size);

   LONGS_EQUAL(Common::Result::OK, response.code);

   LONGS_EQUAL(0x02, response.start);

   LONGS_EQUAL(0x08, response.entries.size());

   for (int i = 0; i < 8; ++i)
   {
      Entry e(0x5A00 | (i + 3), 0x70 | (i + 3));
      check_index(e, response.entries[i], i, "Entries", __FILE__, __LINE__);
   }
}

// =============================================================================
// BindManagement - Entries
// =============================================================================

TEST_GROUP(GroupTableEntries)
{
   struct TestEntries: public GroupTable::Entries
   {
      GroupTable::Entries::Container &data()
      {
         return db;
      }
   };

   TestEntries entries;

   uint16_t group = 0;
   uint8_t unit   = 0;

   TEST_SETUP()
   {
      group = 0x5A5A;
      unit  = 0x42;
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }

   void should_create(uint16_t group, uint8_t unit, const char *file, int line)
   {
      uint16_t size = entries.size();

      auto res      = entries.save(GroupTable::Entry(group, unit));

      LONGS_EQUAL_LOCATION(size + 1, entries.size(), NULL, file, line);
      LONGS_EQUAL_LOCATION(Common::Result::OK, res, NULL, file, line);
   }

   void should_not_create(uint16_t group, uint8_t unit, const char *file, int line,
                          Common::Result expected)
   {
      uint16_t size = entries.size();

      auto res      = entries.save(GroupTable::Entry(group, unit));

      LONGS_EQUAL_LOCATION(size, entries.size(), NULL, file, line);
      LONGS_EQUAL_LOCATION(expected, res, NULL, file, line);
   }

   std::vector<GroupTable::Entry> create_entries(uint8_t groups = 1, uint8_t units = 1)
   {
      std::vector<GroupTable::Entry> result;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<uint16_t> dist1(1, 0x7FFF);
      std::uniform_int_distribution<uint8_t> dist2(1, 0xFF);

      auto g_group = std::bind(dist1, gen);
      auto g_unit  = std::bind(dist2, gen);

      for (int i = 0; i < groups; ++i)
      {
         uint16_t group = g_group();

         if (std::any_of(result.begin(), result.end(),
                         [group, &i](const GroupTable::Entry &e)
         {
            return e.group == group;
         }))
         {
            --i;
            continue;
         }

         for (int j = 0; j < units; ++j)
         {
            uint8_t unit = g_unit();

            if (std::any_of(result.begin(), result.end(),
                            [group, unit, &j](const GroupTable::Entry &e)
            {
               return e.group == group && e.unit == unit;
            }))
            {
               --j;
               continue;
            }

            entries.data().emplace_back(group, unit);
            result.emplace_back(group, unit);
         }
      }

      return std::move(result);
   }

   GroupTable::Entry setup_entries(uint8_t groups = 1, uint8_t units = 1)
   {
      auto entries = create_entries(groups, units);

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<uint8_t> dist(0, entries.size() - 1);

      auto select = std::bind(dist, gen);

      return entries.at(select());
   }
};

#define SHOULD_CREATE(group, unit) \
   should_create(group, unit, __FILE__, __LINE__)

#define SHOULD_NOT_CREATE(group, unit, ...) \
   should_not_create(group, unit, __FILE__, __LINE__,##__VA_ARGS__)

TEST(GroupTableEntries, Initialization)
{
   // Should be empty.
   LONGS_EQUAL(0, entries.size());
}

TEST(GroupTableEntries, Save)
{
   // Should create a new entry.
   SHOULD_CREATE(group, unit);
}

TEST(GroupTableEntries, Save_Equal)
{
   // Should create a new entry.
   SHOULD_CREATE(group, unit);

   // Should NOT create a entry with the same parameters.
   SHOULD_NOT_CREATE(group, unit, Common::Result::OK);
}

TEST(GroupTableEntries, Destroy_OK)
{
   auto entry   = setup_entries(10, 20);

   uint8_t size = entries.size();

   LONGS_EQUAL(Common::Result::OK, entries.destroy(entry));
   LONGS_EQUAL(size - 1, entries.size());
}

TEST(GroupTableEntries, Destroy_Fail)
{
   auto entry   = setup_entries(10, 20);

   uint8_t size = entries.size();

   LONGS_EQUAL(Common::Result::OK, entries.destroy(entry));
   LONGS_EQUAL(size - 1, entries.size());

   LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(entry));
   LONGS_EQUAL(size - 1, entries.size());
}

TEST(GroupTableEntries, Clear)
{
   setup_entries(10, 20);

   LONGS_EQUAL(10 * 20, entries.size());

   entries.clear();

   LONGS_EQUAL(0, entries.size());
}

TEST(GroupTableEntries, ForEach)
{
   // Create a multiple entries for multiple groups.
   for (uint8_t groups = 1; groups <= 10; ++groups)
   {
      for (int units = 0; units < 20; ++units)
      {
         entries.save(GroupTable::Entry(groups, units));
      }
   }

   mock("GroupTableEntries").expectNCalls(20, "for_each_predicate");

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<uint8_t> dist(1, 10);

   auto group = std::bind(dist, gen);

   /* *INDENT-OFF* */
   entries.for_each(group(), [](const Entry &e)
   {
      UNUSED(e);
      mock("GroupTableEntries").actualCall("for_each_predicate");
   });
   /* *INDENT-ON* */

   mock("GroupTableEntries").checkExpectations();
}
