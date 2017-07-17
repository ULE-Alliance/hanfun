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
   struct GroupTableServer: public GroupTable::DefaultServer
   {
      GroupTableServer(HF::Core::Unit0 &unit):
         GroupTable::DefaultServer(unit) {}

      void add(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("add");
         GroupTable::DefaultServer::add(addr);
      }

      void remove(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("remove");
         GroupTable::DefaultServer::remove(addr);
      }

      void remove_all(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("remove_all");
         GroupTable::DefaultServer::remove_all(addr);
      }

      void read_entries(const Protocol::Address &addr) override
      {
         mock("GroupTable::Server").actualCall("read_entries");
         GroupTable::DefaultServer::read_entries(addr);
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
IGNORE_TEST(GroupTableServer, NumberOfEntries)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(GroupTableServer, NumberOfEntries, false, number_of_entries, 42, 142);
}

//! @test Number Of Max Entries support.
TEST(GroupTableServer, NumberOfMaxEntries)
{
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
   uint8_t  unit  = 0;

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

      for(int i = 0; i < groups; ++i)
      {
         for(int j = 0; j < units; ++j)
         {
            uint16_t group = g_group();
            uint8_t   unit = g_unit();
            entries.data().emplace_back(group, unit);
            result.emplace_back(group, unit);;
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

      auto select  = std::bind(dist, gen);

      return entries.at(select());
   }
};

#define SHOULD_CREATE(group, unit) \
   should_create(group, unit, __FILE__, __LINE__)

#define SHOULD_NOT_CREATE(group, unit, ...) \
   should_not_create(group, unit, __FILE__, __LINE__, ## __VA_ARGS__)

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
   auto entry = setup_entries(10, 20);

   uint8_t size = entries.size();

   LONGS_EQUAL (Common::Result::OK, entries.destroy(entry));
   LONGS_EQUAL (size - 1, entries.size());
}

TEST(GroupTableEntries, Destroy_Fail)
{
   auto entry = setup_entries(10, 20);

   uint8_t size = entries.size();

   LONGS_EQUAL (Common::Result::OK, entries.destroy(entry));
   LONGS_EQUAL (size - 1, entries.size());

   LONGS_EQUAL (Common::Result::FAIL_ARG, entries.destroy(entry));
   LONGS_EQUAL (size - 1, entries.size());
}

TEST(GroupTableEntries, Clear)
{
   setup_entries(10, 20);

   LONGS_EQUAL (10 * 20, entries.size());

   entries.clear();

   LONGS_EQUAL (0, entries.size());
}

TEST(GroupTableEntries, ForEach)
{
   // Create a multiple entries for multiple groups.
   for(uint8_t groups = 1; groups <= 10; ++groups)
   {
      for(int units = 0; units < 20; ++units)
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
