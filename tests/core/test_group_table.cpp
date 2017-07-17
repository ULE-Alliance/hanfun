// =============================================================================
/*!
 * @file       tests/core/test_group_table.cpp
 *
 * This is file contains the unit tests for the Group Table service
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/group_table.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::GroupTable;

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

// =============================================================================
// Group Table Client
// =============================================================================

//! Test Group for Group Table Client interface class.
TEST_GROUP(GroupTableClient)
{
   // TODO Add required unit tests.
   struct GroupTableClient: public InterfaceHelper<GroupTable::Client>
   {
      GroupTableClient(HF::Core::Unit0 &unit): InterfaceHelper<GroupTable::Client>(unit) {}

   };

   Testing::Device *device;
   GroupTableClient *client;

   Protocol::Address addr;


   TEST_SETUP()
   {
      device = new Testing::Device();
      client = new GroupTableClient(*(device->unit0()));

      addr   = Protocol::Address(42, 0);

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
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->add(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupTable::ADD_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Remove support.
TEST(GroupTableClient, Remove)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->remove(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Remove All support.
TEST(GroupTableClient, RemoveAll)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->remove_all(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupTable::REMOVE_ALL_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Read Entries support.
TEST(GroupTableClient, ReadEntries)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->read_entries(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupTable::READ_ENTRIES_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

// =============================================================================
// Group Table Server
// =============================================================================

//! Test Group for Group Table Server interface class.
TEST_GROUP(GroupTableServer)
{
   // TODO Add required unit tests.
   struct GroupTableServer: public InterfaceHelper<GroupTable::Server>
   {
      GroupTableServer(HF::Core::Unit0 &unit): InterfaceHelper<GroupTable::Server>(unit) {}

      void add(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("add");
         InterfaceHelper<GroupTable::Server>::add(addr);
      }

      void remove(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("remove");
         InterfaceHelper<GroupTable::Server>::remove(addr);
      }

      void remove_all(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("remove_all");
         InterfaceHelper<GroupTable::Server>::remove_all(addr);
      }

      void read_entries(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("read_entries");
         InterfaceHelper<GroupTable::Server>::read_entries(addr);
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
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(GroupTableServer, NumberOfEntries, false, number_of_entries, 42, 142);
}

//! @test Number Of Max Entries support.
TEST(GroupTableServer, NumberOfMaxEntries)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(GroupTableServer, NumberOfMaxEntries, false, number_of_max_entries, 42, 142);
}

//! @test Add support.
TEST(GroupTableServer, Add)
{
   // FIXME Generated Stub.
   mock("GroupTable::Server").expectOneCall("add");

   packet.message.itf.member = GroupTable::ADD_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
}

//! @test Remove support.
TEST(GroupTableServer, Remove)
{
   // FIXME Generated Stub.
   mock("GroupTable::Server").expectOneCall("remove");

   packet.message.itf.member = GroupTable::REMOVE_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
}

//! @test Remove All support.
TEST(GroupTableServer, RemoveAll)
{
   // FIXME Generated Stub.
   mock("GroupTable::Server").expectOneCall("remove_all");

   packet.message.itf.member = GroupTable::REMOVE_ALL_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
}

//! @test Read Entries support.
TEST(GroupTableServer, ReadEntries)
{
   // FIXME Generated Stub.
   mock("GroupTable::Server").expectOneCall("read_entries");

   packet.message.itf.member = GroupTable::READ_ENTRIES_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupTable::Server").checkExpectations();
}
