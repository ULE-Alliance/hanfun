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
   LONGS_EQUAL(GroupManagement::GroupAddress::START_ADDR,entries.next_address());   // 1st address

   // Fill the container to the max
   for (uint16_t i = GroupAddress::START_ADDR;
                 i <= GroupAddress::END_ADDR+10;
                 ++i)
   {
      entries.data()[i] = Group(i, "G");
   }

   entries.data().erase(2);                     // erase group 2
   LONGS_EQUAL(2,entries.next_address());       // check if the next available address is 2
   entries.data()[2] = Group(2, "G");           // restore it

   LONGS_EQUAL(GroupManagement::GroupAddress::END_ADDR,entries.next_address());
}

//! @test Entries find by address
TEST(GroupManagementEntries, Find_by_address)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR;
         i <=  10;
         ++i)
   {
      entries.data()[i] = Group(i, std::string("G")+std::to_string(i));
   }

   entries.data().erase(2);                                       // erase group 2
   POINTERS_EQUAL( nullptr,  entries.find("G2").operator ->());   //Try to find group 2 (should fail)
   entries.data()[2] = Group(2, "G2");                            // restore it

   CHECK( nullptr !=  entries.find("G2").operator ->());          //Try to find group 2 (OK)
   CHECK_EQUAL(std::string("G2"), entries.find("G2")->name);      //Confirm by name.
}

//! @test Entries find by @c Group name
TEST(GroupManagementEntries, Find_by_name)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR;
         i <=  10;
         ++i)
   {
      entries.data()[i] = Group(i, std::string("G")+std::to_string(i));
   }

   entries.data().erase(2);                                       // erase group 2
   POINTERS_EQUAL( nullptr,  entries.find(2).operator ->());      //Try to find group 2 (should fail)
   entries.data()[2] = Group(2, "G2");                            // restore it

   CHECK( nullptr !=  entries.find(2).operator ->());             //Try to find group 2 (OK)
   CHECK_EQUAL(std::string("G2"), entries.find("G2")->name);      //Confirm by name.
}

//! @test Entries size
TEST(GroupManagementEntries, Size)
{
   LONGS_EQUAL(0, entries.size());

   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR;
         i <=  10;
         ++i)
   {
      entries.data()[i] = Group(i, std::string("G")+std::to_string(i));
   }

   LONGS_EQUAL(10, entries.size());
}

//! @test Entries destroy by address
TEST(GroupManagementEntries, Destroy_by_address)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR;
         i <=  10;
         ++i)
   {
      entries.data()[i] = Group(i, std::string("G")+std::to_string(i));
   }

   LONGS_EQUAL(10, entries.size());

   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(11));       //Try to destroy group 11 (NOK)
   CHECK_EQUAL(Common::Result::OK, entries.destroy(10));             //Try to destroy group 10 (OK)
   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(10));       //Try to destroy group 10 again (NOK)

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries destroy by @c Group
TEST(GroupManagementEntries, Destroy_by_group)
{
   // Create 10 Groups
   for (uint16_t i = GroupAddress::START_ADDR;
         i <=  10;
         ++i)
   {
      entries.data()[i] = Group(i, std::string("G")+std::to_string(i));
   }

   LONGS_EQUAL(10, entries.size());

   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy( Group(11,"G11") ) );       //Try to destroy group 11 (NOK)
   CHECK_EQUAL(Common::Result::OK, entries.destroy( Group(10,"G10") ) );             //Try to destroy group 10 (OK)
   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy( Group(10,"G10") ) );       //Try to destroy group 10 again (NOK)

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries save new
TEST(GroupManagementEntries, Save)
{
   LONGS_EQUAL(0, entries.size());

   entries.save( Group(1,"G"));

   LONGS_EQUAL(1, entries.size());
   CHECK_EQUAL(std::string("G"),entries.data()[1].name);
}

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
 *    Helper functions
 * =====================================*/

//! @test Group add_member
TEST(GroupManagement, Add_member)
{
   Group group(1,"G1");

   LONGS_EQUAL(0, group.members.size());

   group.add_member(Member(1,2));

   LONGS_EQUAL(1, group.members.size());

   CHECK_EQUAL(1, group.members[0].device);
   CHECK_EQUAL(2, group.members[0].unit);
}

//! @test Group remove_member
TEST(GroupManagement, Remove_member)
{
   Group group(1,"G1");
   Member member(1,2);
   group.add_member(member);

   LONGS_EQUAL(1, group.members.size());
   CHECK_TRUE( group.remove_member(member) );   // Remove member once (OK)
   CHECK_FALSE( group.remove_member(member) );  // Remove again (NOK)
   LONGS_EQUAL(0, group.members.size());        // Members is empty
}

//! @test Group find_member
TEST(GroupManagement, Find_member)
{
   Group group(1,"G1");
   Member member(1,2);

   group.add_member(Member(3,4));

   CHECK(group.members.end() == group.find_member(member));    // Try to find the member (NOK)

   group.add_member(member);                                   // Add the member

   LONGS_EQUAL(2, group.members.size());                       // Group size should now be 2

   CHECK(group.members.end() != group.find_member(member));    // Try to find the member (OK)
   LONGS_EQUAL(1, group.find_member(member)->device);          // Confirm the device address
   LONGS_EQUAL(2, group.find_member(member)->unit);            // Confirm the unit address
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

/* ----- Delete Group ------------- */

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

/* ----- Add Member ------------- */

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

/* ----- Get Info Response ------------- */

//! @test Check the Get group Info Response size
TEST(GroupManagement, InfoResponse_size)
{
    InfoResponse *message;
    std::string name("Group");
    std::vector<Member> members;

    message = new InfoResponse(name,members);

    CHECK_EQUAL(1                      //Response code
                +1+name.length()       //Group Name
                +sizeof(uint16_t)      //N Members
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
                              0x00,0x00};

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
                           0x00,0x00};

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
                       0x00,0x01,                  //# of groups
                       0x00, 0x01,                 //dev addr
                       0x12 };                     //Unit addr

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

      void created (CreateResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("created");
      }

      void deleted (DeleteResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("deleted");
      }

      void added (AddResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("added");
      }

      void removed (RemoveResponse &response)
      {
         UNUSED(response);
         mock("GroupManagement::Client").actualCall("removed");
      }

      void got_info (InfoResponse &response)
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
      device = new Testing::Device();
      client = new GroupManagementClient(*(device->unit0()));

      addr   = Protocol::Address(42, 0);

      link = Testing::Link();
      packet                  = Protocol::Packet();
      packet.message.type     = Protocol::Message::COMMAND_RES;
      packet.message.itf.role = HF::Interface::SERVER_ROLE;
      packet.message.itf.id = HF::Interface::GROUP_MANAGEMENT;
      packet.link = &link;

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
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->create(name);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::CREATE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Delete support.
TEST(GroupManagementClient, Delete)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->remove(0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::DELETE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Add support.
TEST(GroupManagementClient, Add)
{
   uint16_t group    = 0x0001;
   uint16_t device   = 0x1234;
   uint8_t unit      = 0x56;

   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->add(group, device, unit);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::ADD_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Remove support.
TEST(GroupManagementClient, Remove)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->remove(0x00, 0x00, 0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::REMOVE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Get Info support.
TEST(GroupManagementClient, GetInfo)
{
   uint16_t group    = 0x0001;

   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client->get_info(group);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(GroupManagement::GET_INFO_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Created support.
TEST(GroupManagementClient, Created_OK)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::OK,
       0x12, 0x34,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::CREATE_CMD;



   mock("GroupManagement::Client").expectOneCall("created");


   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test Created support - Creation failed.
TEST(GroupManagementClient, Created_FAIL)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::CREATE_CMD;

   mock("GroupManagement::Client").expectOneCall("created");

   // This needs to use a temporary variable
   // otherwise the handle method will be called twice.
   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}


//! @test deleted support.
TEST(GroupManagementClient, Deleted_OK)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::OK,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::DELETE_CMD;



   mock("GroupManagement::Client").expectOneCall("deleted");


   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test deleted support - deleted failed.
TEST(GroupManagementClient, Deleted_FAIL)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::DELETE_CMD;

   mock("GroupManagement::Client").expectOneCall("deleted");

   // This needs to use a temporary variable
   // otherwise the handle method will be called twice.
   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test added support.
TEST(GroupManagementClient, Added_OK)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::OK,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::ADD_CMD;



   mock("GroupManagement::Client").expectOneCall("added");


   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test added support - added failed.
TEST(GroupManagementClient, Added_FAIL)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::ADD_CMD;

   mock("GroupManagement::Client").expectOneCall("added");

   // This needs to use a temporary variable
   // otherwise the handle method will be called twice.
   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}


//! @test removed support.
TEST(GroupManagementClient, Removed_OK)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::OK,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::REMOVE_CMD;



   mock("GroupManagement::Client").expectOneCall("removed");


   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test removed support - removed failed.
TEST(GroupManagementClient, Removed_FAIL)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::REMOVE_CMD;

   mock("GroupManagement::Client").expectOneCall("removed");

   // This needs to use a temporary variable
   // otherwise the handle method will be called twice.
   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test got_info support.
TEST(GroupManagementClient, GOT_INFO_OK)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::OK,
       0x04 ,'N', 'A', 'M', 'E',
       0x01,
       0x12,0x34,
       0x56,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::GET_INFO_CMD;



   mock("GroupManagement::Client").expectOneCall("got_info");


   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

//! @test got_info support - got_info failed.
TEST(GroupManagementClient, GOT_INFO_FAIL)
{
   Common::ByteArray payload =
   {
       0x00, 0x00, 0x00,
       Common::Result::FAIL_AUTH,
       0x00, 0x00, 0x00,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = GroupManagement::GET_INFO_CMD;

   mock("GroupManagement::Client").expectOneCall("got_info");

   // This needs to use a temporary variable
   // otherwise the handle method will be called twice.
   Result result = client->handle(packet, payload, 3);
   CHECK_EQUAL(Common::Result::OK, result);

   mock("GroupManagement::Client").checkExpectations();
}

// =============================================================================
// Group Management Server
// =============================================================================

//! Test Group for Group Management Server interface class.
TEST_GROUP(GroupManagementServer)
{
   // TODO Add required unit tests.
   struct GroupManagementServer: public GroupManagement::DefaultServer
   {
      GroupManagementServer(HF::Core::Unit0 &unit): GroupManagement::DefaultServer(unit) {}

      Common::Result create(Protocol::Packet &packet, CreateMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("create");
         return GroupManagement::DefaultServer::create(packet, msg);
      }

      void created (const GroupPtr &group) override
      {
         mock("GroupManagement::Server").actualCall("created");
         GroupManagement::IServer::created(group);
      }

      Common::Result remove(Protocol::Packet &packet, DeleteMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("delete");
         return GroupManagement::DefaultServer::remove(packet, msg);
      }

      void deleted (const Group group) override
      {
         mock("GroupManagement::Server").actualCall("deleted");
         GroupManagement::IServer::deleted(group);
      }

      Common::Result add(Protocol::Packet &packet, const AddMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("add");
         return GroupManagement::DefaultServer::add(packet, msg);
      }

      void added (const GroupPtr &group) override
      {
         mock("GroupManagement::Server").actualCall("added");
         GroupManagement::IServer::added(group);
      }

      virtual Common::Result remove(Protocol::Packet &packet, const RemoveMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("remove");
         return GroupManagement::DefaultServer::remove(packet, msg);
      }

      void removed (const GroupPtr &group) override
      {
         mock("GroupManagement::Server").actualCall("removed");
         GroupManagement::IServer::removed(group);
      }

      Common::Result get_info(Protocol::Packet &packet, const InfoMessage &msg) override
      {
         mock("GroupManagement::Server").actualCall("get_info");
         return GroupManagement::DefaultServer::get_info(packet, msg);
      }

      void got_info (const GroupPtr &group) override
      {
         mock("GroupManagement::Server").actualCall("got_info");
         GroupManagement::IServer::got_info(group);
      }

      void notify(const HF::Attributes::IAttribute &old_value,
                  const HF::Attributes::IAttribute &new_value) const
      {
         mock("Interface").actualCall("notify")
               .withParameterOfType("IAttribute", "old", &old_value)
               .withParameterOfType("IAttribute", "new", &new_value);
      }
   };

   Testing::Device *device;
   GroupManagementServer *server;

   Protocol::Packet packet;
   Common::ByteArray payload;

   Protocol::Address addr;
   Testing::Link link;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      server                    = new GroupManagementServer(*(device->unit0()));

      addr = Protocol::Address(42, 0);
      link = Testing::Link();

      packet                    = Protocol::Packet();
      packet.source             = addr;
      packet.message.itf.role   = HF::Interface::SERVER_ROLE;
      packet.message.itf.id     = server->uid();
      packet.message.itf.member = 0xFF;

      packet.message.type = Protocol::Message::COMMAND_RES;
      //packet.message.itf.id = HF::Interface::GROUP_MANAGEMENT;
      packet.link = &link;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete device;

      mock().clear();
   }

   GroupPtr create_group(const std::string &group_name)
   {
      Group group;

      group.address = server->next_address();
      group.name = group_name;

      server->entries().save(group);

      return server->entries().find(group_name);
   }

   Member add_member (Group &group, const uint16_t dev, const uint8_t unit)
   {
      Member member(dev, unit);
      group.add_member(member);

      return member;
   }

   Member add_member(std::string name, const uint16_t dev, const uint8_t unit)
   {
      auto it = server->entries().find(name);

      return add_member(*(const_cast<Group *>(it.operator ->())),dev,unit);
   }

   Member add_member (uint16_t addr, const uint16_t dev, const uint8_t unit)
   {
      auto it = server->entries().find(addr);

      return add_member(*(const_cast<Group *>(it.operator ->())),dev,unit);
   }



};

//! @test Number Of Groups support.
TEST(GroupManagementServer, NumberOfGroups)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE_PACK(GroupManagementServer, NumberOfGroups);
}

//! @test Create support.
TEST(GroupManagementServer, Create)
{
   std::string group_name("MyGroup");

   CreateMessage received(group_name);              //Create a new Group request with the name "MyGroup"
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::CREATE_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length     = received_payload.size();

   NumberOfGroups old_value(0, server);
   NumberOfGroups new_value(1, server);

   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &old_value)
         .withParameterOfType("IAttribute", "new", &new_value);

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("create");
   mock("GroupManagement::Server").expectOneCall("created");

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, received_payload, 0));

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1,server->entries().size());        //Check if the new group is on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   CHECK_EQUAL(group_name, server->entries().begin()->second.name);

   // ------ Check the response message ----

   CreateResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   LONGS_EQUAL(1, resp.address);
}

//! @test Delete support.
TEST(GroupManagementServer, Delete_no_group_before)
{
   uint16_t group_addr = 0x0001;
   DeleteMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::DELETE_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("delete");
   mock("GroupManagement::Server").expectNCalls(0, "deleted");
   mock("Interface").expectNoCall("notify");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::FAIL_ARG, result);

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   // ------ Check the response message ----

   DeleteResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Delete support.
TEST(GroupManagementServer, Delete_existing_group)
{
   // ----- Create 10 EMPTY groups -------
   for (uint16_t i= 0; i<10; ++i)
   {
      std::string name = std::string("MyGroup")+ std::to_string(i);
      create_group(name);
   }

   LONGS_EQUAL(10, server->entries().size());


   uint16_t group_addr = 0x0002;             //Group Address to remove
   DeleteMessage received(group_addr);
   ByteArray received_payload(received.size());

   auto _entry = server->entries().find(group_addr);     //try to find the group with "group_addr"
   CHECK_TRUE(_entry != nullptr);                        //should be != that nullptr

   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::DELETE_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();

   NumberOfGroups old_value(10, server);
   NumberOfGroups new_value(9, server);


   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("delete");
   mock("GroupManagement::Server").expectOneCall("deleted");
   mock("Interface").expectOneCall("notify")
         .withParameterOfType("IAttribute", "old", &old_value)
         .withParameterOfType("IAttribute", "new", &new_value);

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::OK, result);

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   // ------ Check if the group was deleted ----

   LONGS_EQUAL(9, server->entries().size());
   _entry = server->entries().find(group_addr);
   CHECK_TRUE(_entry == nullptr);

   // ------ Check the response message ----

   DeleteResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::OK, resp.code);
}

//! @test Add support.
TEST(GroupManagementServer, Add_1)
{
   // ----- Create 10 EMPTY groups -------
   for (uint16_t i = 0; i < 10; ++i)
   {
      std::string name = std::string("MyGroup") + std::to_string(i);
      create_group(name);
   }

   LONGS_EQUAL(10, server->entries().size());

   uint16_t group_addr  = 0x0002;             //Group Address to add
   uint16_t dev_addr    = 0x1234;
   uint16_t dev_unit    = 0x56;
   AddMessage received(group_addr, dev_addr, dev_unit);
   ByteArray received_payload(received.size());


   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload=received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("add");
   mock("GroupManagement::Server").expectOneCall("added");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::OK, result);

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // ------ Check the response message ----

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::OK, resp.code);
}

TEST(GroupManagementServer, Add_without_groups)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   uint16_t group_addr  = 0x0002;             //Group Address to add
   uint16_t dev_addr    = 0x1234;
   uint16_t dev_unit    = 0x56;
   AddMessage received(group_addr, dev_addr, dev_unit);
   ByteArray received_payload(received.size());


   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload=received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("add");
   mock("GroupManagement::Server").expectNoCall("added");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::FAIL_ARG, result);

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

TEST(GroupManagementServer, Add_2)
{
   // ----- Create 10 EMPTY groups -------
   for (uint16_t i = 0; i < 10; ++i)
   {
      std::string name = std::string("MyGroup") + std::to_string(i);
      create_group(name);
   }

   LONGS_EQUAL(10, server->entries().size());

   uint16_t group_addr;             //Group Address to add
   uint16_t dev_addr;
   uint16_t dev_unit;
   Common::Result result;
   AddResponse resp;

   for (uint16_t i = 0; i < 2; ++i)
   {
      group_addr  = 0x0002;             //Group Address to add
      dev_addr    = 0x1234;
      dev_unit    = 0x56+i;
      AddMessage received(group_addr, dev_addr, dev_unit);
      ByteArray received_payload(received.size());

      received.pack(received_payload);              //pack it

      packet.message.itf.member = GroupManagement::ADD_CMD;
      packet.message.type = Protocol::Message::COMMAND_REQ;
      packet.message.length = received_payload.size();
      packet.message.payload = received_payload;

      mock("AbstractDevice").expectOneCall("send");
      mock("GroupManagement::Server").expectOneCall("add");
      mock("GroupManagement::Server").expectOneCall("added");

      result = server->handle(packet, received_payload, 0);

      CHECK_EQUAL(Common::Result::OK, result);

      Protocol::Packet *response = device->packets.back();

      CHECK_TRUE(response != nullptr);

      LONGS_EQUAL(42, response->destination.device);
      LONGS_EQUAL(0, response->destination.unit);
      LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

      mock("GroupManagement::Server").checkExpectations();
      mock("AbstractDevice").checkExpectations();

      LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
      LONGS_EQUAL(Common::Result::OK, resp.code);
   }
}

//! @test Add support.
TEST(GroupManagementServer, Add_twice)
{
   std::string group_name("MyGroup");

   // --------- Create Group -----------
   GroupPtr group_ptr = create_group(group_name);

   LONGS_EQUAL(1, server->entries().size());

   uint16_t group_addr = 0x0001;             //Group Address to add
   uint16_t dev_addr = 0x1234;
   uint16_t dev_unit = 0x56;

   add_member(group_name,dev_addr,dev_unit);    //Manually add to the group

   LONGS_EQUAL(1, group_ptr->members.size());

   AddMessage received(group_addr, dev_addr, dev_unit);  //Add message to test...
   ByteArray received_payload(received.size());


   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::ADD_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload=received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("add");
   mock("GroupManagement::Server").expectNoCall("added");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::FAIL_ARG, result);
   LONGS_EQUAL(1, group_ptr->members.size());               //check if the members size is still 1


   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // ------ Check the response message ----

   AddResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Remove support.
TEST(GroupManagementServer, Remove_without_groups)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   uint16_t group_addr  = 0x0002;             //Group Address to remove
   uint16_t dev_addr    = 0x1234;
   uint16_t dev_unit    = 0x56;
   RemoveMessage received(group_addr, dev_addr, dev_unit);
   ByteArray received_payload(received.size());


   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload=received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("remove");
   mock("GroupManagement::Server").expectNoCall("added");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::FAIL_ARG, result);

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   RemoveResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

TEST(GroupManagementServer, Remove_non_existing_device)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   std::string group_name("MyGroup");

   uint16_t group_addr  = 0x0002;             //Group Address to remove
   uint16_t dev_addr    = 0x1234;
   uint16_t dev_unit    = 0x56;
   RemoveMessage received(group_addr, dev_addr, dev_unit);
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   // --------- Create Group -----------
   GroupPtr group_ptr = create_group(group_name);

   // -------- Add Member -------------
   add_member(group_name,0x0001,0x12);

   LONGS_EQUAL(1, group_ptr->members.size());

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload=received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("remove");
   mock("GroupManagement::Server").expectNoCall("added");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::FAIL_ARG, result);

   LONGS_EQUAL(1, group_ptr->members.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   RemoveResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

TEST(GroupManagementServer, Remove_existing_device_with_other_dev_in_group)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   std::string group_name("MyGroup");

   uint16_t group_addr  = 0x0001;             //Group Address to remove
   uint16_t dev_addr    = 0x1234;
   uint16_t dev_unit    = 0x56;
   RemoveMessage received(group_addr, dev_addr, dev_unit);
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   // --------- Create Group -----------
   GroupPtr group_ptr = create_group(group_name);

   // -------- Add Member -------------
   Member no_del = add_member(group_name,0x0001,0x12);
   add_member(group_name,dev_addr,dev_unit);

   LONGS_EQUAL(2, group_ptr->members.size());

   packet.message.itf.member = GroupManagement::REMOVE_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload=received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("remove");
   mock("GroupManagement::Server").expectOneCall("removed");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::OK, result);

   LONGS_EQUAL(1, group_ptr->members.size());
   CHECK_EQUAL(no_del.device, group_ptr->members[0].device);
   CHECK_EQUAL(no_del.unit, group_ptr->members[0].unit);

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   mock("GroupManagement::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   RemoveResponse resp;
   LONGS_EQUAL(resp.size(), resp.unpack(response->message.payload));
   LONGS_EQUAL(Common::Result::OK, resp.code);
}

//! @test Get Info support.
TEST(GroupManagementServer, GetInfo_no_group)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   uint16_t group_addr = 0x0002;             //Group Address to remove

   InfoMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload = received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectNoCall("got_info");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::FAIL_ARG, result);

   Protocol::Packet *response = device->packets.back();

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
TEST(GroupManagementServer, GetInfo_no_members)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   uint16_t group_addr = 0x0001;             //Group Address to remove

   std::string group_name("MyGroup");
   InfoMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   // --------- Create Group -----------
   GroupPtr group_ptr = create_group(group_name);

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload = received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectOneCall("got_info");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::OK, result);

   Protocol::Packet *response = device->packets.back();

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
TEST(GroupManagementServer, GetInfo_10_members)
{
   LONGS_EQUAL(0, server->entries().size());  //check if there are no groups

   uint16_t group_addr = 0x0001;             //Group Address to remove

   std::string group_name("MyGroup");
   InfoMessage received(group_addr);
   ByteArray received_payload(received.size());

   received.pack(received_payload);              //pack it

   // --------- Create Group -----------
   GroupPtr group_ptr = create_group(group_name);

   for(uint16_t i = 0; i<5; ++i)
   {
      for(uint16_t j = 0; j<2; ++j)
      {
         add_member(group_name,0x0001+i,0x12+j);
      }
   }

   LONGS_EQUAL(10, group_ptr->members.size());  //check if there are 10 members

   packet.message.itf.member = GroupManagement::GET_INFO_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = received_payload.size();
   packet.message.payload = received_payload;

   mock("AbstractDevice").expectOneCall("send");
   mock("GroupManagement::Server").expectOneCall("get_info");
   mock("GroupManagement::Server").expectOneCall("got_info");

   Common::Result result = server->handle(packet, received_payload, 0);

   CHECK_EQUAL(Common::Result::OK, result);

   Protocol::Packet *response = device->packets.back();

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
