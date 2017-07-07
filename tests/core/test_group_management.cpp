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

using namespace HF::Common;

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

/* =====================================
 *    Client to Servers Commands
 * =====================================*/

 /* ----- Create Group ------------- */

//! @test Check the Create Group message size
TEST(GroupManagement, CreateGroup_Size)
{
   CreateMessage *message;

   message = new CreateMessage("");

   UNSIGNED_LONGS_EQUAL(1, message->size());
   UNSIGNED_LONGS_EQUAL(message->min_size, message->size());

   delete message;

   std::string group_name("TestGroup");
   message = new CreateMessage(group_name);

   UNSIGNED_LONGS_EQUAL(1+group_name.length(), message->size());


   delete message;
}

//! @test Check the Create Group message pack
TEST(GroupManagement, CreateGroup_Pack)
{
   CreateMessage *message;

   std::string group_name("TestGroup");
   message = new CreateMessage(group_name);

   ByteArray expected = ByteArray { (uint8_t) group_name.length(), 'T', 'e', 's', 't', 'G', 'r', 'o', 'u', 'p' };
   ByteArray got = ByteArray(group_name.length() + 1);

   message->pack(got);

   CHECK_EQUAL(expected, got);

   delete message;
}

//! @test Check the Create Group message unpack
TEST(GroupManagement, CreateGroup_Unpack)
{
   CreateMessage *message;
   std::string group_name("TestGroup");

   message = new CreateMessage();
   ByteArray input = ByteArray { (uint8_t) group_name.length(), 'T', 'e', 's', 't', 'G', 'r', 'o', 'u', 'p' };

   message->unpack(input);

   UNSIGNED_LONGS_EQUAL(1+group_name.length(), message->size());
   CHECK_EQUAL(group_name, message->name);

   delete message;
}

/* ----- Create Group Response ------------- */

//! @test Check the CreateGroup Response message size
TEST(GroupManagement, CreateGroupResponse_size)
{
   CreateResponse *message;

   message = new CreateResponse();

   CHECK_EQUAL(Common::Result::OK, message->code);
   UNSIGNED_LONGS_EQUAL(message->min_size
                        +sizeof(uint16_t)    //Group Addr
                        , message->size());

   delete message;

   // ------ Check for response failure -----

   message = new CreateResponse();
   message->code= Common::Result::FAIL_AUTH; //create a FAIL  result code

   CHECK_EQUAL(Common::Result::FAIL_AUTH, message->code);
   UNSIGNED_LONGS_EQUAL(message->min_size, message->size());
   UNSIGNED_LONGS_EQUAL(1, message->size());

   delete message;

}

//! @test Check the CreateGroup Response message pack
TEST(GroupManagement, CreateGroupResponse_Pack)
{
   CreateResponse *message;

   // ----- Normal Case -------

   uint16_t addr= 0x5A55;
   message = new CreateResponse(addr);

   ByteArray expected = ByteArray{ 0x00,    // Response code : OK
                                 0x5A,    // addr MSB
                                 0x55 };   // addr LSB
   ByteArray got = ByteArray(1 + 1 + 1);

   message->pack(got);

   CHECK_EQUAL(expected, got);

   delete message;

   // ---- Wrong address -----

   addr = 0x8FFF;             //Addr outside range
   message = new CreateResponse(addr);

   CHECK_EQUAL(0, message->pack(got));

   delete message;

   // ----- FAIL response code ----

   addr = 0x5A55;
   message = new CreateResponse(addr);
   message->code = Common::Result::FAIL_AUTH;

   expected = ByteArray{ 0x01};
   got      = ByteArray(message->size());

   message->pack(got);

   CHECK_EQUAL(message->min_size, message->pack(got));
   CHECK_EQUAL(expected,got);


   delete message;
}

//! @test Check the CreateGroup Response message unpack
TEST(GroupManagement, CreateGroupResponse_UnPack)
{
   CreateResponse *message;

   // ----- Normal Case -------

   uint16_t addr= 0x5A55;
   message = new CreateResponse();

   ByteArray input = ByteArray{ 0x00,    // Response code : OK
                                 0x5A,    // addr MSB
                                 0x55 };   // addr LSB

   message->unpack(input);

   CHECK_EQUAL(0x00, (uint8_t)message->code);
   CHECK_EQUAL(addr, message->address);

   delete message;

   // ----- FAIL Case -------

   message = new CreateResponse();

   input = ByteArray { 0x01,    // Response code : FAIL: Not Auth
         0x5A,    // addr MSB
         0x55 };   // addr LSB

   message->unpack(input);

   CHECK_EQUAL(0x01, (uint8_t )message->code);
   CHECK_TEXT(addr != message->address,"On response code fail the addr is being unpacked\n");

   delete message;
}

//! @test Check the DeleteGroup message size
TEST(GroupManagement, DeleteGroupMessage_size)
{
   DeleteMessage *message;
   uint16_t addr;

   addr= 0x5A55;
   message = new DeleteMessage(addr);

   UNSIGNED_LONGS_EQUAL(2, message->size());
   UNSIGNED_LONGS_EQUAL(message->min_size, message->size());

   delete message;

}

//! @test Check the DeleteGroup message pack
TEST(GroupManagement, DeleteGroupMessage_Pack)
{
   DeleteMessage *message;
   uint16_t addr;
   ByteArray expected;
   ByteArray got;

   uint16_t size;

   addr= 0x5A55;
   expected= ByteArray{0x5A, 0x55};
   got=ByteArray(2);
   message = new DeleteMessage(addr);

   size = message->pack(got);

   CHECK_EQUAL(expected, got);
   UNSIGNED_LONGS_EQUAL(2, size);

   delete message;
}

//! @test Check the DeleteGroup message unpack
TEST(GroupManagement, DeleteGroupMessage_UnPack)
{
   DeleteMessage *message;
   uint16_t addr;
   ByteArray input;

   // -------- Normal Case ---------

   addr= 0x5A55;
   input= ByteArray{0x5A, 0x55};

   message = new DeleteMessage();
   message->unpack(input);

   CHECK_EQUAL(addr, message->address);

   delete message;
}

//! @test Check the Add to group message size
TEST(GroupManagement, AddMessage_size)
{
   AddMessage *message;
   uint16_t group_addr, dev_addr, unit_id;

   group_addr = 0x5A55;
   dev_addr = 0x0001;
   unit_id = 0xAA;

   message = new AddMessage(group_addr, dev_addr, unit_id);

   UNSIGNED_LONGS_EQUAL(5, message->size());
   UNSIGNED_LONGS_EQUAL(message->min_size, message->size());

   delete message;
}

//! @test Check the Add to group message pack
TEST(GroupManagement, AddMessage_Pack)
{
   AddMessage *message;
   uint16_t group_addr, dev_addr, unit_id;
   ByteArray expected, got;

   uint16_t size;

   group_addr  = 0x5A55;
   dev_addr    = 0x0102;
   unit_id     = 0xAB;
   expected    = ByteArray{0x5A,    //Group Addr (MSB)
                           0x55,    //Group Addr (LSB)
                           0x01,    //Dev Addr  (MSB)
                           0x02,    //Dev Addr  (LSB)
                           0xAB};   //UnitID
   got = ByteArray(5);

   message = new AddMessage(group_addr, dev_addr, unit_id);

   size = message->pack(got);

   CHECK_EQUAL(expected, got);
   UNSIGNED_LONGS_EQUAL(5,size);

   delete message;
}

//! @test Check the Add to group message unpack
TEST(GroupManagement, AddMessage_UnPack)
{
   AddMessage *message;
   uint16_t group_addr, dev_addr, unit_id;
   ByteArray input;

   uint16_t size;

   group_addr  = 0x5A55;
   dev_addr    = 0x0102;
   unit_id     = 0xAB;
   input       = ByteArray{0x5A,    //Group Addr (MSB)
                           0x55,    //Group Addr (LSB)
                           0x01,    //Dev Addr  (MSB)
                           0x02,    //Dev Addr  (LSB)
                           0xAB};   //UnitID


   message = new AddMessage();

   size = message->unpack(input);

   UNSIGNED_LONGS_EQUAL(5,size);
   UNSIGNED_LONGS_EQUAL(group_addr  , message->address);
   UNSIGNED_LONGS_EQUAL(dev_addr    , message->device );
   UNSIGNED_LONGS_EQUAL(unit_id     , message->unit);

   delete message;

}

//! @test Check the Get group Info Response size
TEST(GroupManagement, InfoResponse_size)
{
    InfoResponse *message;
    std::string name("Group");
    std::vector<Member> members;

    message = new InfoResponse(name,members);

    CHECK_EQUAL(1                      //Response code
                +1+name.length()       //Group Name
                +sizeof(uint8_t)      //N Members
                , message->size());

    message->code = Common::Result::FAIL_AUTH;     //Return code FAIL

    CHECK_EQUAL(message->min_size, message->size());
    CHECK_EQUAL(1, message->min_size);

    delete message;
}

//! @test Check the Remove from group message pack
TEST(GroupManagement, InfoResponse_Pack)
{
   InfoResponse *message;
   std::string name("Group");
   std::vector<Member> members;
   ByteArray expected, got;

   uint16_t size;

   message = new InfoResponse(name,members);

   expected    = ByteArray {  (uint8_t)Common::Result::OK,
                              (uint8_t)name.length(), 'G', 'r', 'o', 'u', 'p',
                              0};

   LONGS_EQUAL(expected.size(), message->size());

   got         = ByteArray(message->size());

   CHECK_EQUAL(0, message->members.size());

   size = message->pack(got);

   CHECK_EQUAL(expected, got);
   CHECK_EQUAL(message->size(), size);

   delete message;

   // --- Return error code --------

   message = new InfoResponse(name, members);
   message->code = Common::Result::FAIL_AUTH;

   expected = ByteArray { (uint8_t) Common::Result::FAIL_AUTH};

   LONGS_EQUAL(expected.size(), message->size());

   got = ByteArray(message->size());

   CHECK_EQUAL(0, message->members.size());

   size = message->pack(got);

   CHECK_EQUAL(expected, got);
   CHECK_EQUAL(message->size(), size);

   delete message;

}

//! @test Check the Remove from group message unpack
TEST(GroupManagement, InfoResponse_UnPack)
{
   InfoResponse *message;
   ByteArray input;
   std::string name("Group");

   uint16_t size;

   message = new InfoResponse();

   input    = ByteArray {  (uint8_t)Common::Result::OK,
                           (uint8_t)name.length(), 'G', 'r', 'o', 'u', 'p',
                           0};

   size = message->unpack(input);

   CHECK_EQUAL(input.size(),size);
   CHECK_EQUAL(input.size(),message->size());
   CHECK_EQUAL((uint8_t)Common::Result::OK, message->code);
   CHECK_EQUAL(name,message->name);
   CHECK_EQUAL(0, message->members.size());

   delete message;

   message = new InfoResponse();

   input = ByteArray { (uint8_t) Common::Result::OK,
                       (uint8_t) name.length(),
                       'G', 'r', 'o', 'u', 'p',
                       1,
                       0x00, 0x01,
                       0x12 };

   size = message->unpack(input);


   CHECK_EQUAL(input.size(), size);
   CHECK_EQUAL(input.size(), message->size());
   CHECK_EQUAL((uint8_t )Common::Result::OK, message->code);
   CHECK_EQUAL(name, message->name);
   CHECK_EQUAL(1, message->members.size());
   UNSIGNED_LONGS_EQUAL(0x0001, message->members.at(0).device);
   UNSIGNED_LONGS_EQUAL(0x12, message->members.at(0).unit);

   delete message;

}

#if 0
//! @teste Check the CreateGroup Response message size
TEST(GroupManagement, CreateGroupResponse_size)
{

}

//! @teste Check the CreateGroup Response message pack
TEST(GroupManagement, CreateGroupResponse_Pack)
{

}

//! @teste Check the CreateGroup Response message unpack
TEST(GroupManagement, CreateGroupResponse_UnPack)
{

}
#endif



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
