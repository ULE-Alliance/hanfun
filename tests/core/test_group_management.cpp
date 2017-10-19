// =============================================================================
/*!
 * @file       tests/core/test_group_management.cpp
 *
 * This is file contains the unit tests for the Group Management service
 * implementation.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/group_management.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Common;
using namespace HF::Testing;
using namespace HF::Core::GroupManagement;

// =============================================================================
// Group Management - Entries
// =============================================================================

//! Test Group for GroupManagement service parent class.
TEST_GROUP(GroupManagementEntries)
{
   struct TestEntries: public Entries
   {
      Entries::Container &data()
      {
         return db;
      }
   };

   TestEntries entries;

   TEST_SETUP()
   {
      entries = TestEntries();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Entries next address
TEST(GroupManagementEntries, Next_address)
{
   LONGS_EQUAL(GroupManagement::GroupAddress::START_ADDR, entries.next_address());   // 1st address

   auto &data = entries.data();

   Group group;

   for (uint16_t address = GroupAddress::START_ADDR; address <= GroupAddress::END_ADDR;
        ++address)
   {
      group.name    = "G";
      group.address = address;
      data.emplace(address, group);
   }

   LONGS_EQUAL(GroupManagement::GroupAddress::NO_ADDR, entries.next_address());

   data.erase(2);                          // erase group 2
   LONGS_EQUAL(2, entries.next_address()); // check if the next available address is 2
   data[2] = Group(2, "G");                // restore it

   LONGS_EQUAL(GroupManagement::GroupAddress::NO_ADDR, entries.next_address());
}

//! @test Entries find by address
TEST(GroupManagementEntries, Find_by_address)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR; i <= 10; ++i)
   {
      entries.data()[i] = Group(i, std::string("G") + std::to_string(i));
   }

   entries.data().erase(2);                                    // erase group 2
   POINTERS_EQUAL(nullptr, entries.find("G2").operator->());   // Try to find group 2 (should fail)
   entries.data()[2] = Group(2, "G2");                         // restore it

   CHECK(nullptr != entries.find("G2").operator->());          // Try to find group 2 (OK)
   CHECK_EQUAL(std::string("G2"), entries.find("G2")->name);   // Confirm by name.
}

//! @test Entries find by @c Group name
TEST(GroupManagementEntries, Find_by_name)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR; i <= 10; ++i)
   {
      entries.data()[i] = Group(i, std::string("G") + std::to_string(i));
   }

   entries.data().erase(2);                                   // erase group 2
   POINTERS_EQUAL(nullptr, entries.find(2).operator->());     // Try to find group 2 (should fail)
   entries.data()[2] = Group(2, "G2");                        // restore it

   CHECK(nullptr != entries.find(2).operator->());            // Try to find group 2 (OK)
   CHECK_EQUAL(std::string("G2"), entries.find("G2")->name);  // Confirm by name.
}

//! @test Entries size
TEST(GroupManagementEntries, Size)
{
   LONGS_EQUAL(0, entries.size());

   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR; i <= 10; ++i)
   {
      entries.data()[i] = Group(i, std::string("G") + std::to_string(i));
   }

   LONGS_EQUAL(10, entries.size());
}

//! @test Entries destroy by address
TEST(GroupManagementEntries, Destroy_by_address)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR; i <= 10; ++i)
   {
      entries.data()[i] = Group(i, std::string("G") + std::to_string(i));
   }

   LONGS_EQUAL(10, entries.size());

   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(11));    // Try to destroy group 11 (NOK)
   CHECK_EQUAL(Common::Result::OK, entries.destroy(10));          // Try to destroy group 10 (OK)
   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(10));    // Try to destroy group 10 again (NOK)

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries destroy by @c Group
TEST(GroupManagementEntries, Destroy_by_group)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR; i <= 10; ++i)
   {
      entries.data()[i] = Group(i, std::string("G") + std::to_string(i));
   }

   LONGS_EQUAL(10, entries.size());

   // Try to destroy group 11 (NOK)
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(Group(11, "G11")));

   // Try to destroy group 10 (OK)
   UNSIGNED_LONGS_EQUAL(Common::Result::OK, entries.destroy(Group(10, "G10")));

   // Try to destroy group 10 again (NOK)
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(Group(10, "G10")));

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries save new
TEST(GroupManagementEntries, Save)
{
   LONGS_EQUAL(0, entries.size());

   entries.save(Group(1, "G"));

   UNSIGNED_LONGS_EQUAL(1, entries.size());
   CHECK_EQUAL(std::string("G"), entries.data()[1].name);
}

// =============================================================================
// Group Management
// =============================================================================

//! Test Group for GroupManagement service parent class.
TEST_GROUP(GroupManagement)
{
   struct GroupManagementBase: public InterfaceParentHelper<GroupManagement::Base>
   {
      GroupManagementBase(HF::Core::Unit0 &unit):
         InterfaceParentHelper<GroupManagement::Base>(unit) {}
   };

   Testing::Device *device;
   GroupManagementBase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new GroupManagementBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;

      mock().clear();
   }
};

//! @test GroupManagement::uid should return @c HF::Interface::GROUP_MANAGEMENT.
TEST(GroupManagement, UID)
{
   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, service->uid());
}

//! @test Number Of Groups support.
TEST(GroupManagement, NumberOfGroups)
{
   auto attr = GroupManagement::create_attribute(GroupManagement::NUMBER_OF_GROUPS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfGroups::ID, attr->uid());
   CHECK_EQUAL(NumberOfGroups::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, attr->interface());

   delete attr;

   attr = Core::create_attribute((GroupManagement::IServer *) nullptr, NUMBER_OF_GROUPS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfGroups::ID, attr->uid());
   CHECK_EQUAL(NumberOfGroups::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, attr->interface());

   POINTERS_EQUAL(nullptr, attr->owner());

   delete attr;

   CHECK_ATTRIBUTE_ALLOC(NumberOfGroups);
}

//! @test Check nullptr return for invalid attribute
TEST(GroupManagement, Invalid_Attribute)
{
   auto attr = GroupManagement::create_attribute(GroupManagement::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);

   attr = Core::create_attribute((GroupManagement::IServer *) nullptr,
                                 GroupManagement::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// Code coverage helpers.
TEST(GroupManagement, Allocation)
{
   CHECK_ALLOC(Entries);
}

// =============================================================================
// GroupManagement::Group
// =============================================================================

//! @test Group add_member
TEST(GroupManagement, Add_member)
{
   Group group(1, "G1");

   LONGS_EQUAL(0, group.members().size());

   group.add(Member(1, 2));

   LONGS_EQUAL(1, group.members().size());

   CHECK_EQUAL(1, group.members()[0].device);
   CHECK_EQUAL(2, group.members()[0].unit);
}

//! @test Group remove_member
TEST(GroupManagement, Remove_member)
{
   Group group(1, "G1");
   Member member(1, 2);

   group.add(member);

   LONGS_EQUAL(1, group.members().size());
   CHECK_TRUE(group.remove(member));         // Remove member once (OK)
   CHECK_FALSE(group.remove(member));        // Remove again (NOK)
   LONGS_EQUAL(0, group.members().size());   // Members is empty
}

//! @test Group find_member
TEST(GroupManagement, Find_member)
{
   Group group(1, "G1");
   Member member(1, 2);

   group.add(Member(3, 4));

   CHECK(group.members().end() == group.find_member(member));     // Try to find the member (NOK)

   group.add(member);                                             // Add the member

   LONGS_EQUAL(2, group.members().size());                        // Group size should now be 2

   CHECK(group.members().end() != group.find_member(member));     // Try to find the member (OK)
   LONGS_EQUAL(1, group.find_member(member)->device);             // Confirm the device address
   LONGS_EQUAL(2, group.find_member(member)->unit);               // Confirm the unit address
}

// =============================================================================
// Client to Servers Commands
// =============================================================================

// =============================================================================
// Create Group
// =============================================================================

//! @test Check the Create Group message size
TEST(GroupManagement, Create_Group_Size)
{
   CreateMessage message("");

   UNSIGNED_LONGS_EQUAL(1, message.size());
   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());

   std::string group_name("TestGroup");
   message = CreateMessage(group_name);

   UNSIGNED_LONGS_EQUAL(1 + group_name.length(), message.size());
}

//! @test Check the Create Group message pack
TEST(GroupManagement, CreateGroup_Pack)
{
   std::string group_name("TestGroup");

   CreateMessage message(group_name);
   ByteArray expected = ByteArray {(uint8_t) group_name.length(),
                                   'T', 'e', 's', 't', 'G', 'r', 'o', 'u', 'p'};
   ByteArray got      = ByteArray(message.size());

   message.pack(got);

   CHECK_EQUAL(expected, got);
}

//! @test Check the Create Group message unpack
TEST(GroupManagement, CreateGroup_Unpack)
{
   std::string group_name("TestGroup");

   CreateMessage message(group_name);

   ByteArray input = ByteArray {(uint8_t) group_name.length(),
                                'T', 'e', 's', 't', 'G', 'r', 'o', 'u', 'p'};

   message.unpack(input);

   UNSIGNED_LONGS_EQUAL(1 + group_name.length(), message.size());
   CHECK_EQUAL(group_name, message.name);
}

// =============================================================================
// Create Group Response
// =============================================================================

//! @test Check the CreateGroup Response message size
TEST(GroupManagement, CreateGroupResponse_size)
{
   CreateResponse message;

   CHECK_EQUAL(Common::Result::OK, message.code);
   UNSIGNED_LONGS_EQUAL(CreateResponse::min_size + sizeof(uint16_t),   // Group Address
                        message.size());

   // ----- Check for response failure -----

   // Create a FAIL result code
   message = CreateResponse(Common::Result::FAIL_AUTH);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH, message.code);
   UNSIGNED_LONGS_EQUAL(CreateResponse::min_size, message.size());
   UNSIGNED_LONGS_EQUAL(1, message.size());
}

//! @test Check the CreateGroup Response message pack
TEST(GroupManagement, CreateGroupResponse_Pack)
{
   // ----- Normal Case -----

   CreateResponse message(0x5A55);

   ByteArray expected = ByteArray{Common::Result::OK,   // Response code : OK
                                  0x5A, 0x55};          // Group Address
   ByteArray got      = ByteArray(1 + 1 + 1);

   UNSIGNED_LONGS_EQUAL(message.size(), message.pack(got));

   CHECK_EQUAL(expected, got);

   // ----- Wrong address -----

   message = CreateResponse(0x8FFF);

   UNSIGNED_LONGS_EQUAL(0, message.pack(got));

   // ----- FAIL response code -----

   message  = CreateResponse(Common::Result::FAIL_AUTH, 0x5A55);

   expected = ByteArray{Common::Result::FAIL_AUTH};
   got      = ByteArray(message.size());

   UNSIGNED_LONGS_EQUAL(CreateResponse::min_size, message.pack(got));

   CHECK_EQUAL(expected, got);
}

//! @test Check the CreateGroup Response message unpack
TEST(GroupManagement, CreateGroupResponse_UnPack)
{
   // ----- Normal Case -----

   CreateResponse message(0x5A55);

   ByteArray input = ByteArray{Common::Result::OK,    // Response code
                               0x5A, 0x55};           // Group address

   message.unpack(input);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, message.code);
   UNSIGNED_LONGS_EQUAL(0x5A55, message.address);

   // ----- FAIL Case -----

   message = CreateResponse(0x5A55);

   input   = ByteArray {Common::Result::FAIL_AUTH,    // Response code
                        0x5A, 0x55};                  // Group address

   UNSIGNED_LONGS_EQUAL(CreateResponse::min_size, message.unpack(input));

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH, message.code);
   UNSIGNED_LONGS_EQUAL(GroupAddress::NO_ADDR, message.address);

   CHECK_TEXT(0x5A55 != message.address, "On response code fail the addr is being unpacked\n");
}

// =============================================================================
// Delete Group
// =============================================================================

//! @test Check the DeleteGroup message size
TEST(GroupManagement, DeleteGroupMessage_size)
{
   DeleteMessage message(0x5A55);

   UNSIGNED_LONGS_EQUAL(2, message.size());
   UNSIGNED_LONGS_EQUAL(DeleteMessage::min_size, message.size());
}

//! @test Check the DeleteGroup message pack
TEST(GroupManagement, DeleteGroupMessage_Pack)
{
   DeleteMessage message(0x5A55);

   ByteArray expected {0x5A, 0x55};
   ByteArray got(2);

   UNSIGNED_LONGS_EQUAL(DeleteMessage::min_size, message.pack(got));

   CHECK_EQUAL(expected, got);
}

//! @test Check the DeleteGroup message unpack
TEST(GroupManagement, DeleteGroupMessage_UnPack)
{
   // ----- Normal Case -----
   DeleteMessage message(0x5A55);

   ByteArray input{0x5A, 0x55};

   message.unpack(input);

   UNSIGNED_LONGS_EQUAL(0x5A55, message.address);
}

// =============================================================================
// Add Member
// =============================================================================

//! @test Check the Add to group message size
TEST(GroupManagement, AddMessage_size)
{
   AddMessage message(0x5A55, 0x0001, 0xAA);

   UNSIGNED_LONGS_EQUAL(AddMessage::min_size, message.size());
}

//! @test Check the Add to group message pack
TEST(GroupManagement, AddMessage_Pack)
{
   ByteArray expected {0x5A, 0x55,  // Group Address
                       0x01, 0x02,  // Dev Address
                       0xAB};       // Unit ID

   ByteArray got(5);

   AddMessage message(0x5A55, 0x0102, 0xAB);

   UNSIGNED_LONGS_EQUAL(AddMessage::min_size, message.pack(got));

   CHECK_EQUAL(expected, got);
}

//! @test Check the Add to group message unpack
TEST(GroupManagement, AddMessage_UnPack)
{
   ByteArray input {0x5A, 0x55,  // Group Address
                    0x01, 0x02,  // Device Address
                    0xAB};       // Unit ID

   AddMessage message;

   UNSIGNED_LONGS_EQUAL(AddMessage::min_size, message.unpack(input));

   UNSIGNED_LONGS_EQUAL(0x5A55, message.address);
   UNSIGNED_LONGS_EQUAL(0x0102, message.device);
   UNSIGNED_LONGS_EQUAL(0xAB, message.unit);
}

// =============================================================================
// Get Info Response
// =============================================================================

//! @test Check the Get group Info Response size
TEST(GroupManagement, InfoResponse_size)
{
   std::string name("Group");
   std::vector<Member> members;

   InfoResponse message(name, members);

   UNSIGNED_LONGS_EQUAL(1                       // Response code
                        + 1 + name.length()     // Group Name
                        + sizeof(uint16_t),     // N Members
                        message.size());

   message.code = Common::Result::FAIL_AUTH;

   UNSIGNED_LONGS_EQUAL(1, InfoResponse::min_size);
   UNSIGNED_LONGS_EQUAL(InfoResponse::min_size, message.size());
}

//! @test Check the Remove from group message pack
TEST(GroupManagement, InfoResponse_Pack)
{
   std::string name("Group");
   std::vector<Member> members;

   InfoResponse message(name, members);

   ByteArray expected {Common::Result::OK,
                       (uint8_t) name.length(), 'G', 'r', 'o', 'u', 'p',
                       0x00, 0x00};

   ByteArray got(message.size());

   UNSIGNED_LONGS_EQUAL(expected.size(), message.size());

   UNSIGNED_LONGS_EQUAL(0, message.members.size());

   UNSIGNED_LONGS_EQUAL(message.size(), message.pack(got));

   CHECK_EQUAL(expected, got);

   // ----- Return error code -----

   message  = InfoResponse(Common::Result::FAIL_AUTH, name, members);

   expected = ByteArray {(uint8_t) Common::Result::FAIL_AUTH};

   LONGS_EQUAL(expected.size(), message.size());

   got = ByteArray(message.size());

   UNSIGNED_LONGS_EQUAL(0, message.members.size());

   UNSIGNED_LONGS_EQUAL(message.size(), message.pack(got));

   CHECK_EQUAL(expected, got);
}

//! @test Check the Remove from group message unpack
TEST(GroupManagement, InfoResponse_UnPack)
{
   std::string name("Group");

   InfoResponse message;

   ByteArray input {(uint8_t) Common::Result::OK,
                    (uint8_t) name.length(), 'G', 'r', 'o', 'u', 'p',
                    0x00, 0x00};

   UNSIGNED_LONGS_EQUAL(message.size(), message.unpack(input));

   UNSIGNED_LONGS_EQUAL(input.size(), message.size());

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, message.code);

   CHECK_EQUAL(name, message.name);

   UNSIGNED_LONGS_EQUAL(0, message.members.size());

   message = InfoResponse();

   input   = ByteArray {(uint8_t) Common::Result::OK,
                        (uint8_t) name.length(),
                        'G', 'r', 'o', 'u', 'p',
                        0x00, 0x01,                // # of groups
                        0x00, 0x01,                // dev addr
                        0x12};                     // Unit addr

   UNSIGNED_LONGS_EQUAL(message.size(), message.unpack(input));

   UNSIGNED_LONGS_EQUAL(input.size(), message.size());

   UNSIGNED_LONGS_EQUAL((uint8_t) Common::Result::OK, message.code);

   CHECK_EQUAL(name, message.name);

   UNSIGNED_LONGS_EQUAL(1, message.members.size());
   UNSIGNED_LONGS_EQUAL(0x0001, message.members.at(0).device);
   UNSIGNED_LONGS_EQUAL(0x12, message.members.at(0).unit);
}

// =============================================================================
// Group Management Client
// =============================================================================

//! Test Group for Group Management Client interface class.
TEST_GROUP(GroupManagementClient)
{
   struct GroupManagementClient: public InterfaceHelper<GroupManagement::Client>
   {
      GroupManagementClient(HF::Core::Unit0 &unit):
         InterfaceHelper<GroupManagement::Client>(unit) {}

      void created(CreateResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("created");
      }

      void deleted(DeleteResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("deleted");
      }

      void added(AddResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("added");
      }

      void removed(RemoveResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("removed");
      }

      void got_info(InfoResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("got_info");
      }

   };

   Testing::Device *device;
   GroupManagementClient *client;

   Protocol::Address addr;
   Protocol::Packet packet;
   Testing::Link link;

   TEST_SETUP()
   {
      device                  = new Testing::Device();
      client                  = new GroupManagementClient(*(device->unit0()));

      addr                    = Protocol::Address(42, 0);

      link                    = Testing::Link();
      packet                  = Protocol::Packet();
      packet.message.type     = Protocol::Message::COMMAND_RES;
      packet.message.itf.role = HF::Interface::SERVER_ROLE;
      packet.message.itf.id   = HF::Interface::GROUP_MANAGEMENT;
      packet.link             = &link;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;
      delete device;

      mock().clear();
   }
};

//! @test Create support.
TEST(GroupManagementClient, Create)
{
   std::string name = "GroupName";
   mock("Interface").expectOneCall("send");

   client->create(name);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::CREATE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   CreateMessage message;

   message.unpack(client->sendMsg.payload);
   CHECK_EQUAL(std::string("GroupName"), message.name);
}

//! @test Delete support.
TEST(GroupManagementClient, Delete)
{
   mock("Interface").expectOneCall("send");

   client->remove(0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   DeleteMessage message;

   message.unpack(client->sendMsg.payload);
   LONGS_EQUAL(0x00, message.address);
}

//! @test Add support.
TEST(GroupManagementClient, Add)
{
   uint16_t group  = 0x0001;
   uint16_t device = 0x1234;
   uint8_t unit    = 0x56;

   mock("Interface").expectOneCall("send");

   client->add(group, device, unit);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::ADD_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   AddMessage message;

   message.unpack(client->sendMsg.payload);
   LONGS_EQUAL(0x001, message.address);
   LONGS_EQUAL(0x1234, message.device);
   LONGS_EQUAL(0x56, message.unit);
}

//! @test Remove support.
TEST(GroupManagementClient, Remove)
{
   mock("Interface").expectOneCall("send");

   client->remove(0x00, 0x00, 0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   RemoveMessage message;

   message.unpack(client->sendMsg.payload);

   LONGS_EQUAL(0x00, message.address);
   LONGS_EQUAL(0x0000, message.device);
   LONGS_EQUAL(0x00, message.unit);
}

//! @test Get Info support.
TEST(GroupManagementClient, GetInfo)
{
   uint16_t group = 0x0001;

   mock("Interface").expectOneCall("send");

   client->get_info(group);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::GET_INFO_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   InfoMessage message;

   message.unpack(client->sendMsg.payload);
   LONGS_EQUAL(0x0001, message.address);
}

//! @test Created support.
TEST(GroupManagementClient, Created_OK)
{
   Common::ByteArray payload =
   {
      0x00,               0x00, 0x00,
      Common::Result::OK,
      0x12,               0x34,
      0x00,               0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::CREATE_CMD;

   mock("GroupManagement::Client").expectOneCall("created");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test Created support - Creation failed.
TEST(GroupManagementClient, Created_FAIL)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::FAIL_AUTH,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::CREATE_CMD;

   mock("GroupManagement::Client").expectOneCall("created");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test deleted support.
TEST(GroupManagementClient, Deleted_OK)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::OK,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::DELETE_CMD;

   mock("GroupManagement::Client").expectOneCall("deleted");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test deleted support - deleted failed.
TEST(GroupManagementClient, Deleted_FAIL)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::FAIL_AUTH,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::DELETE_CMD;

   mock("GroupManagement::Client").expectOneCall("deleted");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test added support.
TEST(GroupManagementClient, Added_OK)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::OK,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::ADD_CMD;

   mock("GroupManagement::Client").expectOneCall("added");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test added support - added failed.
TEST(GroupManagementClient, Added_FAIL)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::FAIL_AUTH,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::ADD_CMD;

   mock("GroupManagement::Client").expectOneCall("added");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test removed support.
TEST(GroupManagementClient, Removed_OK)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::OK,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::REMOVE_CMD;

   mock("GroupManagement::Client").expectOneCall("removed");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test removed support - removed failed.
TEST(GroupManagementClient, Removed_FAIL)
{
   Common::ByteArray payload =
   {
      0x00,                      0x00, 0x00,
      Common::Result::FAIL_AUTH,
      0x00,                      0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::REMOVE_CMD;

   mock("GroupManagement::Client").expectOneCall("removed");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test got_info support.
TEST(GroupManagementClient, GOT_INFO_OK)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::OK,
      0x04, 'N', 'A', 'M', 'E',
      0x01,
      0x12, 0x34,
      0x56,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::GET_INFO_CMD;

   mock("GroupManagement::Client").expectOneCall("got_info");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test got_info support - got_info failed.
TEST(GroupManagementClient, GOT_INFO_FAIL)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      Common::Result::FAIL_AUTH,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::GET_INFO_CMD;

   mock("GroupManagement::Client").expectOneCall("got_info");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 3));

   mock("GroupManagement::Client").checkExpectations();
}

//! @test Handle - Invalid CMD.
TEST(GroupManagementClient, Handle_Invalid)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::GET_INFO_CMD + 1;

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_UNKNOWN,
                        client->handle(packet, payload, 3));
}

// =============================================================================
// Group Management Server
// =============================================================================

//! Test Group for Group Management Server interface class.
TEST_GROUP(GroupManagementServer)
{
   struct TestEntries: public Entries
   {
      Common::Result save(uint16_t address, const std::string &name) override
      {
         auto &call = mock("GroupManagement::Entries").actualCall("save");

         if (call.hasReturnValue())
         {
            return static_cast<Common::Result>(call.returnIntValue());
         }
         else
         {
            return Entries::save(address, name);
         }
      }

      using Entries::save;

      Common::Result destroy(const Group &entry) override
      {
         auto &call = mock("GroupManagement::Entries").actualCall("destroy");

         if (call.hasReturnValue())
         {
            return static_cast<Common::Result>(call.returnIntValue());
         }
         else
         {
            return Entries::destroy(entry);
         }
      }

      uint16_t next_address() const
      {
         auto &call = mock("GroupManagement::Entries").actualCall("next_address");

         if (call.hasReturnValue())
         {
            return call.returnIntValue();
         }
         else
         {
            return Entries::next_address();
         }
      }
   };

   struct GroupManagementServer: public GroupManagement::Server<TestEntries>
   {
      GroupManagementServer(HF::Core::Unit0 &unit):
         GroupManagement::Server<TestEntries>(unit) {}

      Common::Result create(Protocol::Packet &packet, CreateMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("create");
         return GroupManagement::Server<TestEntries>::create(packet, msg);
      }

      void created(const GroupPtr &group) override
      {
         mock("GroupManagement::Server").actualCall("created");
         GroupManagement::IServer::created(group);
      }

      Common::Result remove(Protocol::Packet &packet, DeleteMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("delete");
         return GroupManagement::Server<TestEntries>::remove(packet, msg);
      }

      void deleted(const Group &group) override
      {
         mock("GroupManagement::Server").actualCall("deleted");
         GroupManagement::Server<TestEntries>::deleted(group);
      }

      Common::Result add(Protocol::Packet &packet, const AddMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("add");
         return GroupManagement::Server<TestEntries>::add(packet, msg);
      }

      void added(const GroupPtr &group, const Member &member) override
      {
         mock("GroupManagement::Server")
            .actualCall("added")
            .withParameter("group", group->address)
            .withParameter("member.device", member.device)
            .withParameter("member.unit", member.unit);
         GroupManagement::IServer::added(group, member);
      }

      Common::Result remove(Protocol::Packet &packet, const RemoveMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("remove");
         return GroupManagement::Server<TestEntries>::remove(packet, msg);
      }

      void removed(const GroupPtr &group, const Member &member) override
      {
         mock("GroupManagement::Server").actualCall("removed");
         GroupManagement::IServer::removed(group, member);
      }

      Common::Result get_info(Protocol::Packet &packet, const InfoMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("get_info");
         return GroupManagement::Server<TestEntries>::get_info(packet, msg);
      }

      void notify(const HF::Attributes::IAttribute &old_value,
                  const HF::Attributes::IAttribute &new_value) const override
      {
         mock("Interface").actualCall("notify")
            .withParameterOfType("IAttribute", "old", &old_value)
            .withParameterOfType("IAttribute", "new", &new_value);
      }

      bool authorized(CMD member, const Protocol::Address &source,
                      const Protocol::Address &destination) override
      {
         auto res = GroupManagement::Server<TestEntries>::authorized(member, source, destination);

         return mock("GroupManagement::Server").actualCall("authorized")
                   .withParameter("member", member)
                   .withParameter("source", source.device)
                   .withParameter("destination", destination.device)
                   .returnBoolValueOrDefault(res);
      }
   };

   Testing::Concentrator *base;
   GroupManagementServer *server;

   Protocol::Packet packet;
   Common::ByteArray payload;

   Protocol::Address addr;
   Testing::Link link;

   uint16_t group_addr = 0x0002;       // Group Address to add
   uint16_t dev_addr   = 0x1234;
   uint8_t dev_unit    = 0x56;

   TEST_SETUP()
   {
      base                      = new Testing::Concentrator();
      server                    = new GroupManagementServer(*(base->unit0()));

      addr                      = Protocol::Address(42, 0);
      link                      = Testing::Link();

      packet                    = Protocol::Packet();
      packet.source             = addr;
      packet.destination        = Protocol::Address(0, 0);
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server->uid();
      packet.message.itf.member = 0xFF;

      packet.message.type       = Protocol::Message::COMMAND_REQ;
      packet.link               = &link;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete base;

      mock().clear();
   }

   GroupPtr create_group(const std::string &group_name)
   {
      Group group;

      group.address = server->next_address();
      group.name    = group_name;

      server->entries().save(group);

      return server->entries().find(group_name);
   }

   Member add_member(Group &group, const uint16_t dev, const uint8_t unit)
   {
      Member member(dev, unit);

      group.add(member);

      return member;
   }

   Member add_member(std::string name, const uint16_t dev, const uint8_t unit)
   {
      auto it = server->entries().find(name);

      return add_member(*(const_cast<Group *>(it.operator->())), dev, unit);
   }

   Member add_member(uint16_t addr, const uint16_t dev, const uint8_t unit)
   {
      auto it = server->entries().find(addr);

      return add_member(*(const_cast<Group *>(it.operator->())), dev, unit);
   }

   void fill_groups(uint8_t count = 10, const std::string prefix = std::string("MyGroup"))
   {
      for (uint8_t i = 0; i < count; ++i)
      {
         std::string name = prefix + std::to_string(i);
         create_group(name);
      }
   }

   void fill_group(Group &group, uint16_t &device, uint8_t &unit, uint8_t count = 10)
   {
      uint16_t _device;
      uint8_t _unit;

      uint8_t selected = generate_random(0, count - 1);

      for (uint8_t i = 0; i < count; ++i)
      {
         _device = generate_random(1, Protocol::BROADCAST_ADDR - 1);
         _unit   = generate_random(1, Protocol::BROADCAST_UNIT - 1);
         group.add(_device, _unit);

         if (i == selected)
         {
            device = _device;
            unit   = _unit;
         }
      }
   }

   void setup_device(Testing::Concentrator *_base, uint16_t address)
   {
      assert(_base != nullptr);

      auto device = DevMgt::create_device(*_base, address);
      auto unit   = DevMgt::add_unit0(device);

      DevMgt::fill_unit(unit, HF::Interface::GROUP_TABLE, HF::Interface::SERVER_ROLE);
   }
};

//! @test Attribute pack support.
TEST(GroupManagementServer, AttributePack)
{
   CHECK_ATTRIBUTE_PACK(GroupManagementServer, NumberOfGroups);
}

//! @test Number Of Groups support.
TEST(GroupManagementServer, NumberOfGroups)
{
   auto attr = server->attribute(NUMBER_OF_GROUPS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfGroups::ID, attr->uid());
   CHECK_EQUAL(NumberOfGroups::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, attr->interface());

   POINTERS_EQUAL(server, attr->owner());

   delete attr;

   attr = Core::create_attribute(server, NUMBER_OF_GROUPS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfGroups::ID, attr->uid());
   CHECK_EQUAL(NumberOfGroups::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, attr->interface());

   POINTERS_EQUAL(server, attr->owner());

   delete attr;
}

//! @test Invalid argument.
TEST(GroupManagementServer, Invalid_Argument)
{
   auto attr = server->attribute(NUMBER_OF_GROUPS_ATTR + 1);

   CHECK_TRUE(attr == nullptr);

   attr = Core::create_attribute(server, NUMBER_OF_GROUPS_ATTR + 1);

   CHECK_TRUE(attr == nullptr);
}

//! @test Handle - Invalid CMD.
TEST(GroupManagementServer, Handle_Invalid)
{
   Common::ByteArray payload {
      0x00, 0x00, 0x00,
      0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::GET_INFO_CMD + 1;

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_UNKNOWN,
                        server->handle(packet, payload, 3));
}

//! @test Create support.
TEST(GroupManagementServer, Create)
{
   std::string group_name("MyGroup");

   // Create a new Group request with the name "MyGroup"
   CreateMessage received(group_name);
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   NumberOfGroups old_value(0, server);
   NumberOfGroups new_value(1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", CREATE_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device);
   mock("GroupManagement::Server").expectOneCall("created");
   mock("HF::Transport::Group").expectOneCall("create")
      .ignoreOtherParameters()
      .andReturnValue(Common::Result::OK);
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        server->handle(packet, payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server->entries().size());    // Check if the new group is on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   CHECK_EQUAL(group_name, server->entries().begin()->second.name);

   // ----- Check the response message -----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   LONGS_EQUAL(1, resp.address);
}

//! @test Create support - Fail.
TEST(GroupManagementServer, Create_Fail_Group_Exists)
{
   std::string group_name("MyGroup");

   Group group(0x4242, group_name);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->entries().save(group));

   uint16_t size = server->entries().size();

   // Create a new Group request with the name "MyGroup"

   CreateMessage received(group_name);
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("Interface").expectNoCall("notify");

   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectNoCall("created");
   mock("HF::Transport::Group").expectNoCall("create");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG,
                        server->handle(packet, payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
   LONGS_EQUAL(GroupAddress::NO_ADDR, resp.address);
}

//! @test Create support - Fail Address.
TEST(GroupManagementServer, Create_Fail_Address)
{
   std::string group_name("MyGroup");

   uint16_t size = server->entries().size();

   // Create a new Group request with the name "MyGroup"
   CreateMessage received(group_name);
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("Interface").expectNoCall("notify");

   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectNoCall("created");
   mock("HF::Transport::Group").expectNoCall("create");
   mock("AbstractDevice").expectOneCall("send");

   mock("GroupManagement::Entries").expectOneCall("next_address")
      .andReturnValue(GroupAddress::NO_ADDR);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_RESOURCES,
                        server->handle(packet, payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("GroupManagement::Entries").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, resp.code);
   LONGS_EQUAL(GroupAddress::NO_ADDR, resp.address);
}

//! @test Create support - Fail Transport.
TEST(GroupManagementServer, Create_Fail_Transport)
{
   std::string group_name("MyGroup");

   uint16_t size = server->entries().size();

   // Create a new Group request with the name "MyGroup"
   CreateMessage received(group_name);
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   NumberOfGroups old_value(0, server);
   NumberOfGroups new_value(1, server);

   mock("Interface").expectNoCall("notify");

   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectNoCall("created");
   mock("HF::Transport::Group").expectOneCall("create")
      .ignoreOtherParameters()
      .andReturnValue(Common::Result::FAIL_RESOURCES);
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_RESOURCES,
                        server->handle(packet, payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, resp.code);
   LONGS_EQUAL(GroupAddress::NO_ADDR, resp.address);
}

//! @test Create support - Fail Entries Save.
TEST(GroupManagementServer, Create_Fail_Save)
{
   std::string group_name("MyGroup");

   uint16_t size = server->entries().size();

   // Create a new Group request with the name "MyGroup"
   CreateMessage received(group_name);
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("Interface").expectNoCall("notify");

   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectNoCall("created");
   mock("HF::Transport::Group").expectOneCall("create")
      .ignoreOtherParameters()
      .andReturnValue(Common::Result::OK);
   mock("HF::Transport::Group").expectOneCall("remove")
      .ignoreOtherParameters();
   mock("GroupManagement::Entries").expectOneCall("save")
      .andReturnValue(Common::Result::FAIL_UNKNOWN);

   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_UNKNOWN,
                        server->handle(packet, payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("GroupManagement::Entries").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_UNKNOWN, resp.code);
   LONGS_EQUAL(GroupAddress::NO_ADDR, resp.address);
}

//! @test Create support - Fail Auth.
TEST(GroupManagementServer, Create_Fail_Auth)
{
   std::string group_name("MyGroup");

   uint16_t size = server->entries().size();

   // Create a new Group request with the name "MyGroup"
   CreateMessage received(group_name);
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("Interface").expectNoCall("notify");

   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", CREATE_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device)
      .andReturnValue(false);
   mock("GroupManagement::Server").expectNoCall("created");
   mock("HF::Transport::Group").expectNoCall("create");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH,
                        server->handle(packet, payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("GroupManagement::Entries").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_AUTH, resp.code);
   LONGS_EQUAL(GroupAddress::NO_ADDR, resp.address);
}

//! @test Delete support.
TEST(GroupManagementServer, Delete)
{
   fill_groups();

   auto group = server->entry(group_addr);

   CHECK_TRUE(group != nullptr);

   fill_group(*group, dev_addr, dev_unit, 10);

   auto dev_entries = group->members();

   uint8_t size     = server->entries().size();

   DeleteMessage received(group_addr);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);

   packet.message.itf.member = GroupManagement::DELETE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   NumberOfGroups old_value(10, server);
   NumberOfGroups new_value(9, server);

   mock("AbstractDevice").expectNCalls(dev_entries.size() + 1, "send");

   mock("GroupManagement::Server").expectOneCall("delete");
   mock("GroupManagement::Server").expectOneCall("deleted");

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   mock("HF::Transport::Group").expectOneCall("remove")
      .withParameter("group", group_addr)
      .ignoreOtherParameters();

   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", DELETE_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   LONGS_EQUAL(size - 1, server->entries().size());

   // Should send messages to devices to remove them from the group.

   for (uint8_t i = 0; i < dev_entries.size(); ++i)
   {
      auto packet  = base->packets[i];
      auto &member = dev_entries[i];

      CHECK_INDEX("Dst Address : ", i, member.device, packet->destination.device);
      CHECK_INDEX("Dst Unit : ", i, (uint8_t) 0, packet->destination.unit);

      CHECK_INDEX("Dst Mod : ", i,
                  (uint8_t) Protocol::Address::DEVICE,
                  (uint8_t) packet->destination.mod);

      CHECK_INDEX("Msg Type : ", i,
                  HF::Protocol::Message::COMMAND_REQ, packet->message.type);

      CHECK_INDEX("Msg Itf UID : ", i,
                  (uint16_t) HF::Interface::GROUP_TABLE, (uint16_t) packet->message.itf.id);
      CHECK_INDEX("Msg Itf Role : ", i,
                  (uint8_t) HF::Interface::Role::SERVER_ROLE, (uint8_t) packet->message.itf.role);
      CHECK_INDEX("Msg Itf Member : ", i,
                  (uint8_t) GroupTable::REMOVE_CMD, packet->message.itf.member);

      GroupTable::Entry entry;

      CHECK_INDEX("GroupTable::Entry Unpack : ", i,
                  entry.size(), entry.unpack(packet->message.payload));

      CHECK_INDEX("GroupTable::Entry::group : ", i, group_addr, entry.group);
      CHECK_INDEX("GroupTable::Entry::unit : ", i, member.unit, entry.unit);
   }

   // Should respond to message.

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(42, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, _packet->message.itf.member);

   DeleteResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::OK, response.code);
}

//! @test Delete support - Fail Group.
TEST(GroupManagementServer, Delete_Fail_Group)
{
   fill_groups();

   group_addr = server->entries().size() + 1;

   uint8_t size = server->entries().size();

   DeleteMessage received(group_addr);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);

   packet.message.itf.member = GroupManagement::DELETE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("AbstractDevice").expectOneCall("send");

   mock("GroupManagement::Server").expectOneCall("delete");
   mock("GroupManagement::Server").expectNoCall("deleted");

   mock("Interface").expectNoCall("notify");

   mock("HF::Transport::Group").expectNoCall("remove");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG,
                        server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   // Should respond to message.

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(42, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, _packet->message.itf.member);

   DeleteResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Delete support - Fail Destroy.
TEST(GroupManagementServer, Delete_Fail_Destroy)
{
   fill_groups();

   uint8_t size = server->entries().size();

   DeleteMessage received(group_addr);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);

   packet.message.itf.member = GroupManagement::DELETE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("AbstractDevice").expectOneCall("send");

   mock("GroupManagement::Server").expectOneCall("delete");
   mock("GroupManagement::Server").expectNoCall("deleted");

   mock("Interface").expectNoCall("notify");

   mock("HF::Transport::Group").expectNoCall("remove");

   mock("GroupManagement::Entries").expectOneCall("destroy")
      .andReturnValue(Common::Result::FAIL_UNKNOWN);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_UNKNOWN,
                        server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("GroupManagement::Entries").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   // Should respond to message.

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(42, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, _packet->message.itf.member);

   DeleteResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_UNKNOWN, response.code);
}

//! @test Delete support - Fail Destroy.
TEST(GroupManagementServer, Delete_Fail_Auth)
{
   fill_groups();

   uint8_t size = server->entries().size();

   DeleteMessage received(group_addr);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);

   packet.message.itf.member = GroupManagement::DELETE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("AbstractDevice").expectOneCall("send");

   mock("GroupManagement::Server").expectOneCall("delete");
   mock("GroupManagement::Server").expectNoCall("deleted");

   mock("Interface").expectNoCall("notify");

   mock("HF::Transport::Group").expectNoCall("remove");

   mock("GroupManagement::Entries").expectNoCall("destroy");

   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", DELETE_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device)
      .andReturnValue(false);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH,
                        server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
   mock("GroupManagement::Entries").checkExpectations();

   LONGS_EQUAL(size, server->entries().size());

   // Should respond to message.

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(42, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, _packet->message.itf.member);

   DeleteResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
}

//! @test Add support - Step 1.
TEST(GroupManagementServer, Add_Step1)
{
   fill_groups();

   setup_device(base, dev_addr);

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");

   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", ADD_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", dev_addr);

   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(dev_addr, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_REQ, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_TABLE, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupTable::ADD_CMD, _packet->message.itf.member);

   GroupTable::Entry entry;

   LONGS_EQUAL(entry.size(), entry.unpack(_packet->message.payload));

   LONGS_EQUAL(group_addr, entry.group);
   LONGS_EQUAL(dev_unit, entry.unit);
}

//! @test Add support - Step 1 (Fail No Device).
TEST(GroupManagementServer, Add_Step1_Fail_No_Device)
{
   fill_groups();

   // Setup Add request.

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Add support - Step 1 (Fail No Unit 0).
TEST(GroupManagementServer, Add_Step1_Fail_No_Unit0)
{
   fill_groups();

   // Setup Add request.

   auto device = DevMgt::create_device(*base, dev_addr);

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_SUPPORT, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, response.code);
}

//! @test Add support - Step 1 (Fail No Interface).
TEST(GroupManagementServer, Add_Step1_Fail_No_Interface)
{
   // Setup Add request.

   fill_groups();

   auto device = DevMgt::create_device(*base, dev_addr);
   auto unit   = DevMgt::add_unit0(device);

   DevMgt::fill_unit(unit, HF::Interface::IDENTIFY, HF::Interface::Role::SERVER_ROLE);

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_SUPPORT, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_SUPPORT, response.code);
}

//! @test Add support - Step 1 - Fail Group.
TEST(GroupManagementServer, Add_Step1_Fail_Group)
{
   fill_groups();

   setup_device(base, dev_addr);

   group_addr = server->entries().size() + 1;

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Add support - Step 1 - Fail Member.
TEST(GroupManagementServer, Add_Step1_Fail_Member)
{
   fill_groups();

   setup_device(base, dev_addr);

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   CHECK_TRUE(group->add(dev_addr, dev_unit));

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Add support - Step 1 - Fail Size.
TEST(GroupManagementServer, Add_Step1_Fail_Size)
{
   fill_groups();

   setup_device(base, dev_addr);

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   // HACK: Fill all the available members.
   const_cast<Group::Container &>(group->members()) = std::vector<Member>(Group::MAX_MEMBERS);

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_RESOURCES, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, response.code);
}

//! @test Add support - Step 1 - Fail Size.
TEST(GroupManagementServer, Add_Step1_Fail_Size_Pending)
{
   fill_groups();

   setup_device(base, dev_addr);

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   // HACK: Fill all the available members minus 1.
   const_cast<Group::Container &>(group->members()) = std::vector<Member>(Group::MAX_MEMBERS - 1);

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_RESOURCES, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, response.code);
}

//! @test Add support - Step 1 - Fail device address.
TEST(GroupManagementServer, Add_Step1_Fail_Device_Broadcast_Address)
{
   fill_groups();

   setup_device(base, dev_addr);

   AddMessage received(group_addr, Protocol::BROADCAST_ADDR, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Add support - Step 1 - Fail device address.
TEST(GroupManagementServer, Add_Step1_Fail_Unit_Broadcast_Address)
{
   fill_groups();

   setup_device(base, dev_addr);

   AddMessage received(group_addr, dev_addr, Protocol::BROADCAST_UNIT);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Add support - Step 1 - Fail Auth.
TEST(GroupManagementServer, Add_Step1_Fail_Auth)
{
   fill_groups();

   AddMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("add");

   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", ADD_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", dev_addr)
      .andReturnValue(false);

   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::ADD_CMD, _packet->message.itf.member);

   AddResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
}

//! @test Add support - Step 2.
TEST(GroupManagementServer, Add_Step2)
{
   fill_groups();

   setup_device(base, dev_addr);

   // Create request for the response below.
   packet.message.reference = 0xAA;
   AddMessage request(group_addr, dev_addr, dev_unit);

   server->add(packet, request);

   // Handle response from GroupTable::Server

   GroupTable::Response response_gt(Common::Result::OK, group_addr, dev_unit);

   payload                   = ByteArray(response_gt.size() + 6);

   packet.source             = Protocol::Address(dev_addr, 0);

   packet.message.itf.role   = HF::Interface::SERVER_ROLE;
   packet.message.itf.id     = HF::Interface::GROUP_TABLE;
   packet.message.itf.member = GroupTable::ADD_CMD;

   packet.message.type       = Protocol::Message::COMMAND_RES;

   packet.message.length     = payload.size();

   response_gt.pack(payload, 3);

   mock("GroupManagement::Server").expectOneCall("added")
      .withParameter("group", group_addr)
      .withParameter("member.device", dev_addr)
      .withParameter("member.unit", dev_unit);
   mock("HF::Transport::Group").expectOneCall("add")
      .withParameter("group", group_addr)
      .withParameter("device", dev_addr)
      .ignoreOtherParameters()
      .andReturnValue(Common::Result::OK);
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   auto group = server->entry(group_addr);

   CHECK_TRUE(group != nullptr);
   CHECK_TRUE(group->exists(dev_addr, dev_unit));

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(addr.device, _packet->destination.device);
   LONGS_EQUAL(addr.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(0xAA, _packet->message.reference);

   // ----- Check the response message -----

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(_packet->message.payload));
   LONGS_EQUAL(Common::Result::OK, resp.code);
}

//! @test Add support - Step 2 (No Request).
TEST(GroupManagementServer, Add_Step2_NoRequest)
{
   fill_groups();

   setup_device(base, dev_addr);

   // Handle response from GroupTable::Server

   GroupTable::Response response_gt(Common::Result::OK, group_addr, dev_unit);

   payload                   = ByteArray(response_gt.size() + 6);

   packet.source             = Protocol::Address(dev_addr, 0);

   packet.message.itf.role   = HF::Interface::SERVER_ROLE;
   packet.message.itf.id     = HF::Interface::GROUP_TABLE;
   packet.message.itf.member = GroupTable::ADD_CMD;

   packet.message.type       = Protocol::Message::COMMAND_RES;

   packet.message.length     = payload.size();

   response_gt.pack(payload, 3);

   mock("GroupManagement::Server").expectNoCall("added");
   mock("HF::Transport::Group").expectNoCall("add");
   mock("AbstractDevice").expectNoCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();
}

//! @test Add support - Step 2.
TEST(GroupManagementServer, Add_Step2_No_Group)
{
   fill_groups();

   setup_device(base, dev_addr);

   // Create request for the response below.

   packet.message.reference = 0xAA;
   AddMessage request(group_addr, dev_addr, dev_unit);

   server->add(packet, request);

   // Remove group before response is received.

   server->entries().destroy(group_addr);

   // Handle response from GroupTable::Server

   GroupTable::Response response_gt(Common::Result::OK, group_addr, dev_unit);

   payload                   = ByteArray(response_gt.size() + 6);

   packet.source             = Protocol::Address(dev_addr, 0);

   packet.message.itf.role   = HF::Interface::SERVER_ROLE;
   packet.message.itf.id     = HF::Interface::GROUP_TABLE;
   packet.message.itf.member = GroupTable::ADD_CMD;

   packet.message.type       = Protocol::Message::COMMAND_RES;

   packet.message.length     = payload.size();

   response_gt.pack(payload, 3);

   mock("GroupManagement::Server").expectNoCall("added");
   mock("HF::Transport::Group").expectNoCall("add");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(addr.device, _packet->destination.device);
   LONGS_EQUAL(addr.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(0xAA, _packet->message.reference);

   // ----- Check the response message -----

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(_packet->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Add support - Step 2 (Transport Layer Failure).
TEST(GroupManagementServer, Add_Step2_Transport_Fail)
{
   fill_groups();

   setup_device(base, dev_addr);

   // Create request for the response below.

   packet.message.reference = 0xAA;
   AddMessage request(group_addr, dev_addr, dev_unit);

   server->add(packet, request);

   // Handle response from GroupTable::Server

   GroupTable::Response response_gt(Common::Result::OK, group_addr, dev_unit);

   payload                   = ByteArray(response_gt.size() + 6);

   packet.source             = Protocol::Address(dev_addr, 0);

   packet.message.itf.role   = HF::Interface::SERVER_ROLE;
   packet.message.itf.id     = HF::Interface::GROUP_TABLE;
   packet.message.itf.member = GroupTable::ADD_CMD;

   packet.message.type       = Protocol::Message::COMMAND_RES;

   packet.message.length     = payload.size();

   response_gt.pack(payload, 3);

   mock("GroupManagement::Server").expectNoCall("added");
   mock("HF::Transport::Group").expectOneCall("add")
      .withParameter("group", group_addr)
      .withParameter("device", dev_addr)
      .ignoreOtherParameters()
      .andReturnValue(Common::Result::FAIL_RESOURCES);
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(addr.device, _packet->destination.device);
   LONGS_EQUAL(addr.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(0xAA, _packet->message.reference);

   // ----- Check the response message -----

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(_packet->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, resp.code);
}

//! @test Add support - Step 2 - Fail entry update.
TEST(GroupManagementServer, Add_Step2_Fail_Update)
{
   fill_groups();

   setup_device(base, dev_addr);

   // Create request for the response below.
   packet.message.reference = 0xAA;
   AddMessage request(group_addr, dev_addr, dev_unit);

   server->add(packet, request);

   auto group = server->entry(group_addr);

   CHECK_TRUE(group != nullptr);

   group->remove(Protocol::BROADCAST_ADDR, Protocol::BROADCAST_UNIT);

   // Handle response from GroupTable::Server

   GroupTable::Response response_gt(Common::Result::OK, group_addr, dev_unit);

   payload                   = ByteArray(response_gt.size() + 6);

   packet.source             = Protocol::Address(dev_addr, 0);

   packet.message.itf.role   = HF::Interface::SERVER_ROLE;
   packet.message.itf.id     = HF::Interface::GROUP_TABLE;
   packet.message.itf.member = GroupTable::ADD_CMD;

   packet.message.type       = Protocol::Message::COMMAND_RES;

   packet.message.length     = payload.size();

   response_gt.pack(payload, 3);

   mock("GroupManagement::Server").expectNoCall("added");
   mock("HF::Transport::Group").expectOneCall("add")
      .withParameter("group", group_addr)
      .withParameter("device", dev_addr)
      .ignoreOtherParameters()
      .andReturnValue(Common::Result::OK);
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(addr.device, _packet->destination.device);
   LONGS_EQUAL(addr.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(0xAA, _packet->message.reference);

   // ----- Check the response message -----

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(_packet->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_MODIFIED, resp.code);
}

//! @test Remove support.
TEST(GroupManagementServer, Remove)
{
   fill_groups();

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   fill_group(*group, dev_addr, dev_unit);

   uint16_t size = group->members().size();

   RemoveMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("remove");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", REMOVE_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", dev_addr);

   mock("AbstractDevice").expectNCalls(2, "send");
   mock("HF::Transport::Group").expectOneCall("remove")
      .withParameter("ep", base)
      .withParameter("group", group_addr)
      .withParameter("device", dev_addr);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.front();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(dev_addr, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_REQ, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_TABLE, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupTable::REMOVE_CMD, _packet->message.itf.member);

   GroupTable::Entry entry;

   LONGS_EQUAL(entry.size(), entry.unpack(_packet->message.payload));

   LONGS_EQUAL(group_addr, entry.group);
   LONGS_EQUAL(dev_unit, entry.unit);

   // Check for response.
   _packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(addr.device, _packet->destination.device);
   LONGS_EQUAL(addr.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, _packet->message.itf.member);

   RemoveResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::OK, resp.code);

   LONGS_EQUAL(size - 1, group->members().size());

   CHECK_FALSE(group->exists(dev_addr, dev_unit));
}

//! @test Remove support - Fail Group.
TEST(GroupManagementServer, Remove_Fail_Group)
{
   fill_groups();

   group_addr = server->entries().size() + 1;

   RemoveMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");
   mock("HF::Transport::Group").expectNoCall("remove");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, _packet->message.itf.member);

   RemoveResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Remove support - Fail Member.
TEST(GroupManagementServer, Remove_Fail_Member)
{
   fill_groups();

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   fill_group(*group, dev_addr, dev_unit);

   do {
      dev_addr = generate_random(1, Protocol::BROADCAST_ADDR - 1);
      dev_unit = generate_random(1, Protocol::BROADCAST_UNIT - 1);
   } while (group->exists(dev_addr, dev_unit));

   RemoveMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");
   mock("HF::Transport::Group").expectNoCall("remove");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, _packet->message.itf.member);

   RemoveResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Remove support - Fail Device Address.
TEST(GroupManagementServer, Remove_Fail_Device_Broadcast_Address)
{
   fill_groups();

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   fill_group(*group, dev_addr, dev_unit);

   RemoveMessage received(group_addr, Protocol::BROADCAST_ADDR, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");
   mock("HF::Transport::Group").expectNoCall("remove");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, _packet->message.itf.member);

   RemoveResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Remove support - Fail Unit Address.
TEST(GroupManagementServer, Remove_Fail_Unit_Broadcast_Address)
{
   fill_groups();

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   fill_group(*group, dev_addr, dev_unit);

   RemoveMessage received(group_addr, dev_addr, Protocol::BROADCAST_UNIT);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");
   mock("HF::Transport::Group").expectNoCall("remove");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, _packet->message.itf.member);

   RemoveResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
}

//! @test Remove support - Fail Auth.
TEST(GroupManagementServer, Remove_Fail_Auth)
{
   fill_groups();

   auto group = server->entries().find(group_addr);

   CHECK_TRUE(group != nullptr);

   fill_group(*group, dev_addr, dev_unit);

   RemoveMessage received(group_addr, dev_addr, dev_unit);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);   // pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received.size();

   mock("GroupManagement::Server").expectOneCall("remove");
   mock("AbstractDevice").expectOneCall("send");
   mock("HF::Transport::Group").expectNoCall("remove");

   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", REMOVE_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", dev_addr)
      .andReturnValue(false);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("HF::Transport::Group").checkExpectations();

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(packet.source.device, _packet->destination.device);
   LONGS_EQUAL(packet.source.unit, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   LONGS_EQUAL(HF::Protocol::Message::COMMAND_RES, _packet->message.type);

   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, _packet->message.itf.id);
   LONGS_EQUAL(HF::Interface::Role::SERVER_ROLE, _packet->message.itf.role);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, _packet->message.itf.member);

   RemoveResponse response;

   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));

   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
}

//! @test Get Info support.
TEST(GroupManagementServer, GetInfo_no_group)
{
   LONGS_EQUAL(0, server->entries().size());    // check if there are no groups

   uint16_t group_addr = 0x0002;                // Group Address to remove

   InfoMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);             // pack it

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received_payload.size();
   packet.message.payload    = received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectNoCall("got_info");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG,
                        server->handle(packet, received_payload, 0));

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   InfoResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Get Info support.
TEST(GroupManagementServer, GetInfo_No_Members)
{
   LONGS_EQUAL(0, server->entries().size());    // Check if there are no groups

   uint16_t group_addr = 0x0001;                // Group Address to remove

   std::string group_name("MyGroup");
   InfoMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);             // Pack it

   // ----- Create Group -----
   GroupPtr group_ptr = create_group(group_name);

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received_payload.size();
   packet.message.payload    = received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", GET_INFO_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        server->handle(packet, received_payload, 0));

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   InfoResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::OK, resp.code);
   CHECK_EQUAL(group_name, resp.name);
   LONGS_EQUAL(0, resp.members.size());
}

//! @test Get Info support.
TEST(GroupManagementServer, GetInfo_10_Members)
{
   LONGS_EQUAL(0, server->entries().size());    // check if there are no groups

   uint16_t group_addr = 0x0001;                // Group Address to remove

   std::string group_name("MyGroup");
   InfoMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);             // pack it

   // ----- Create Group -----
   GroupPtr group_ptr = create_group(group_name);

   for (uint16_t i = 0; i < 5; ++i)
   {
      for (uint16_t j = 0; j < 2; ++j)
      {
         add_member(group_name, 0x0001 + i, 0x12 + j);
      }
   }

   LONGS_EQUAL(10, group_ptr->members().size());  // check if there are 10 members

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received_payload.size();
   packet.message.payload    = received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", GET_INFO_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        server->handle(packet, received_payload, 0));

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   InfoResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::OK, resp.code);
   CHECK_EQUAL(group_name, resp.name);
   LONGS_EQUAL(10, resp.members.size());
}

//! @test Get Info support - Fail Group.
TEST(GroupManagementServer, GetInfo_Fail_No_Group)
{
   InfoMessage received(group_addr);

   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", GET_INFO_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG,
                        server->handle(packet, payload, 3));

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(42, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   InfoResponse response;
   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));
   LONGS_EQUAL(InfoResponse::min_size, response.size());
   LONGS_EQUAL(Common::Result::FAIL_ARG, response.code);
   CHECK_TRUE(response.name.empty());
   LONGS_EQUAL(0, response.members.size());
}

//! @test Get Info support - Fail Auth.
TEST(GroupManagementServer, GetInfo_Fail_Auth)
{
   fill_groups();

   InfoMessage received(group_addr);
   payload = ByteArray(received.size() + 6);

   received.pack(payload, 3);

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type       = Protocol::Message::COMMAND_REQ;
   packet.message.length     = payload.size();

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectOneCall("authorized")
      .withParameter("member", GET_INFO_CMD)
      .withParameter("source", packet.source.device)
      .withParameter("destination", packet.destination.device)
      .andReturnValue(false);

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH,
                        server->handle(packet, payload, 3));

   Protocol::Packet *_packet = base->packets.back();

   CHECK_TRUE(_packet != nullptr);

   LONGS_EQUAL(42, _packet->destination.device);
   LONGS_EQUAL(0, _packet->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, _packet->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   InfoResponse response;
   LONGS_EQUAL(response.size(), response.unpack(_packet->message.payload));
   LONGS_EQUAL(InfoResponse::min_size, response.size());
   LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);
   CHECK_TRUE(response.name.empty());
   LONGS_EQUAL(0, response.members.size());
}

TEST(GroupManagementServer, Allocation)
{
   CHECK_ALLOC(IGroupTable, *server);
   CHECK_ALLOC(GroupTableClient, *server);
}
