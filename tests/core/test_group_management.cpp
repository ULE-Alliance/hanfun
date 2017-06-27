// =============================================================================
/*!
 * @file       tests/core/test_group_management.cpp
 *
 * This is file contains the unit tests for the Group Management service
 * implementation.
 *
 * @version    x.x.x
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

using namespace HF::Testing;

using namespace HF::Core::GroupManagement;

// =============================================================================
// Group Management
// =============================================================================

//! Test Group for GroupManagement service parent class.
TEST_GROUP(GroupManagement)
{
   struct GroupManagementBase: public InterfaceParentHelper<GroupManagement::Base>
   {
      GroupManagementBase(HF::Core::Unit0 &unit): InterfaceParentHelper<GroupManagement::Base>(unit) {}
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
   HF::Attributes::IAttribute *attr = GroupManagement::create_attribute(GroupManagement::NUMBER_OF_GROUPS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfGroups::ID, attr->uid());
   CHECK_EQUAL(NumberOfGroups::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::GROUP_MANAGEMENT, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(GroupManagement, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr = GroupManagement::create_attribute(GroupManagement::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// Group Management Client
// =============================================================================

//! Test Group for Group Management Client interface class.
TEST_GROUP(GroupManagementClient)
{
   // TODO Add required unit tests.
   struct GroupManagementClient: public InterfaceHelper<GroupManagement::Client>
   {
      GroupManagementClient(HF::Core::Unit0 &unit): InterfaceHelper<GroupManagement::Client>(unit) {}

   };

   Testing::Device *device;
   GroupManagementClient *client;

   Protocol::Address addr;


   TEST_SETUP()
   {
      device = new Testing::Device();
      client = new GroupManagementClient(*(device->unit0()));

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

//! @test Create support.
TEST(GroupManagementClient, Create)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->create(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::CREATE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Delete support.
TEST(GroupManagementClient, Delete)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->remove(addr, 0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Add support.
TEST(GroupManagementClient, Add)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->add(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::ADD_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Remove support.
TEST(GroupManagementClient, Remove)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->remove(addr, 0x00, 0x00, 0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Get Info support.
TEST(GroupManagementClient, GetInfo)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->get_info(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::GET_INFO_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

// =============================================================================
// Group Management Server
// =============================================================================

//! Test Group for Group Management Server interface class.
TEST_GROUP(GroupManagementServer)
{
   // TODO Add required unit tests.
   struct GroupManagementServer: public InterfaceHelper<GroupManagement::Server>
   {
      GroupManagementServer(HF::Core::Unit0 &unit): InterfaceHelper<GroupManagement::Server>(unit) {}

      void create(const Protocol::Address &addr) override
      {
         mock("GroupManagement::Server").actualCall("create");
         InterfaceHelper<GroupManagement::Server>::create(addr);
      }

      void remove(const Protocol::Address &addr, uint16_t group) override
      {
         mock("GroupManagement::Server").actualCall("delete");
         InterfaceHelper<GroupManagement::Server>::remove(addr, group);
      }

      void add(const Protocol::Address &addr) override
      {
         mock("GroupManagement::Server").actualCall("add");
         InterfaceHelper<GroupManagement::Server>::add(addr);
      }

      void remove(const Protocol::Address &addr, uint16_t group, uint16_t device, uint8_t unit) override
      {
         mock("GroupManagement::Server").actualCall("remove");
         InterfaceHelper<GroupManagement::Server>::remove(addr, group, device, unit);
      }

      void get_info(const Protocol::Address &addr) override
      {
         mock("GroupManagement::Server").actualCall("get_info");
         InterfaceHelper<GroupManagement::Server>::get_info(addr);
      }

   };

   Testing::Device *device;
   GroupManagementServer *server;


   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      server                    = new GroupManagementServer(*(device->unit0()));

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

//! @test Number Of Groups support.
TEST(GroupManagementServer, NumberOfGroups)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(GroupManagementServer, NumberOfGroups, false, number_of_groups, 42, 142);
}

//! @test Create support.
TEST(GroupManagementServer, Create)
{
   // FIXME Generated Stub.
   mock("GroupManagement::Server").expectOneCall("create");

   packet.message.itf.member = GroupManagement::CREATE_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
}

//! @test Delete support.
TEST(GroupManagementServer, Delete)
{
   // FIXME Generated Stub.
   mock("GroupManagement::Server").expectOneCall("delete");

   packet.message.itf.member = GroupManagement::DELETE_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
}

//! @test Add support.
TEST(GroupManagementServer, Add)
{
   // FIXME Generated Stub.
   mock("GroupManagement::Server").expectOneCall("add");

   packet.message.itf.member = GroupManagement::ADD_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
}

//! @test Remove support.
TEST(GroupManagementServer, Remove)
{
   // FIXME Generated Stub.
   mock("GroupManagement::Server").expectOneCall("remove");

   packet.message.itf.member = GroupManagement::REMOVE_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
}

//! @test Get Info support.
TEST(GroupManagementServer, GetInfo)
{
   // FIXME Generated Stub.
   mock("GroupManagement::Server").expectOneCall("get_info");

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 3));

   mock("GroupManagement::Server").checkExpectations();
}
