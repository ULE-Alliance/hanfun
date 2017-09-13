// =============================================================================
/*!
 * @file       tests/core/test_batch_program_management.cpp
 *
 * This is file contains the unit tests for the Batch Program Management interface
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "../../inc/hanfun/core/batch_program_management.h"
#include "test_helper.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Common;
using namespace HF::Testing;

using namespace HF::Core::BatchProgramManagement;

// =============================================================================
// Batch Program Management
// =============================================================================

//! Test Group for BatchProgramManagement interface parent class.
TEST_GROUP(BatchProgramManagement)
{
   struct BatchProgramManagementBase: public InterfaceParentHelper<
         BatchProgramManagement::Base>
   {};

   BatchProgramManagementBase interface;

   TEST_SETUP()
   {
      interface = BatchProgramManagementBase();

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test BatchProgramManagement::uid should return @c HF::Interface::BATCH_PROGRAM_MANAGEMENT.
TEST(BatchProgramManagement, UID)
{
   LONGS_EQUAL(HF::Interface::BATCH_PROGRAM_MANAGEMENT, interface.uid());
}

//! @test Maximum Number Of Entries support.
TEST(BatchProgramManagement, MaximumNumberOfEntries)
{
   HF::Attributes::IAttribute *attr = BatchProgramManagement::create_attribute(
      BatchProgramManagement::MAXIMUM_NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(MaximumNumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(MaximumNumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::BATCH_PROGRAM_MANAGEMENT, attr->interface());

   delete attr;
}

//! @test Number Of Entries support.
TEST(BatchProgramManagement, NumberOfEntries)
{
   HF::Attributes::IAttribute *attr = BatchProgramManagement::create_attribute(
      BatchProgramManagement::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::BATCH_PROGRAM_MANAGEMENT, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(BatchProgramManagement, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr = BatchProgramManagement::create_attribute(
      BatchProgramManagement::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// BatchProgramManagement Messages
// =============================================================================
//! Test group for the messages tests.
TEST_GROUP(BatchProgramManagementMessages)
{
   Common::ByteArray payload, expected;

   TEST_SETUP()
   {
      mock("support").ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

//! @test Check the DefineProgram message size
TEST(BatchProgramManagementMessages, DefineProgram_size)
{
   DefineProgram message;

   UNSIGNED_LONGS_EQUAL(3, message.size());
   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());


   ByteArray program1 = ByteArray { 0x12,
                                   0x34,
                                   0x56 };

   std::vector<Action> actions;

   Action action1 = Action(0x98,
                           HF::Protocol::Message::Type::COMMAND_REQ,
                           0x00,
                           0x0134,
                           0x56,
                           program1);

   actions.push_back(action1);

   message = DefineProgram(0x12,
                           "TEST",
                           actions);

   UNSIGNED_LONGS_EQUAL(message.min_size+4+1*(7+3), message.size());
}

//! @test Check the DefineProgram message pack
TEST(BatchProgramManagementMessages, DefineProgram_pack)
{
   ByteArray program1 = ByteArray { 0x12,
                                   0x34,
                                   0x56 };

   std::vector<Action> actions;

   Action action1 = Action(0x98,
                           HF::Protocol::Message::Type::COMMAND_REQ,
                           0x00,
                           0x0134,
                           0x56,
                           program1);

   actions.push_back(action1);

   DefineProgram message(0x12,
                         "TEST",
                         actions);

   payload = ByteArray(message.size());
   uint16_t size = message.pack(payload,0);

   UNSIGNED_LONGS_EQUAL(payload.size(), message.size());
   UNSIGNED_LONGS_EQUAL(size, message.size());

   expected = ByteArray {0x12,
                         0x04,
                         'T','E','S','T',
                         0x01,
                         0x98,                     // UID
                         0x01,                     // Message type
                         (0x00 & 0x01)<<8|0x01,    // Interface Type + Itf ID (MSB)
                         0x34,
                         0x56,
                         0x00,0x03,
                         0x12,
                         0x34,
                         0x56};

   CHECK_EQUAL(expected, payload);
}

//! @test Check the DefineProgram message unpack
TEST(BatchProgramManagementMessages, DefineProgram_unpack)
{
   uint16_t size = 0;

   payload = ByteArray {0x12,                         // Program ID
                        0x04,                         // Program Name length
                        'T','E','S','T',              // Program Name
                        0x01,                         // n actions
                        0x98,                         // UID
                        0x01,                         // Message type
                        (0x00 & 0x01)<<8|0x01,        // Interface Type + Itf ID (MSB)
                        0x34,                         // Itf (LSB)
                        0x56,                         // Itf member
                        0x00,0x03,                    // Payload size
                        0x12,
                        0x34,
                        0x56};

   ByteArray program1 = ByteArray { 0x12,
                                      0x34,
                                      0x56 };

   DefineProgram Message;
   size = Message.unpack(payload, 0);
   UNSIGNED_LONGS_EQUAL(payload.size(),size);
   UNSIGNED_LONGS_EQUAL(0x12,       Message.ID);
   UNSIGNED_LONGS_EQUAL(0x4,        Message.name.size());
   CHECK_EQUAL(std::string("TEST"), Message.name);
   UNSIGNED_LONGS_EQUAL(0x1,        Message.actions.size());
   UNSIGNED_LONGS_EQUAL(0x98,       Message.actions[0].reference);
   UNSIGNED_LONGS_EQUAL(0x01,       Message.actions[0].type);
   UNSIGNED_LONGS_EQUAL(0x00,       Message.actions[0].itf.role);
   UNSIGNED_LONGS_EQUAL(0x0134,     Message.actions[0].itf.id);
   UNSIGNED_LONGS_EQUAL(0x56,       Message.actions[0].itf.member);
   UNSIGNED_LONGS_EQUAL(0x0003,     Message.actions[0].payload.size());
   CHECK_EQUAL( program1 ,          Message.actions[0].payload);
}

//! @test Check the DefineProgram message pack failure
TEST(BatchProgramManagementMessages, DefineProgram_pack_fail)
{
   ByteArray program1 = ByteArray { 0x12,
                                    0x34,
                                    0x56 };

   std::vector<Action> actions;

   Action action1 = Action(0x98,
                           HF::Protocol::Message::Type::COMMAND_REQ,
                           0x00,
                           0x0134,
                           0x56,
                           program1);

   actions.push_back(action1);

   DefineProgram message(0x12,
                         "TEST",
                         actions);

   payload = ByteArray(message.size() - 2);
   uint16_t size = message.pack(payload,0);

   UNSIGNED_LONGS_EQUAL(0, size);
}

//! @test Check the DefineProgram message unpack failure
TEST(BatchProgramManagementMessages, DefineProgram_unpack_fail)
{
   uint16_t size = 0;

   payload = ByteArray {0x12,
                            0x04,                     // Program ID
                            'T','E','S','T',          // Program Name
                            0x01,                     // n actions
                            0x98,                     // UID
                            0x01,                     // Message type
                            (0x00 & 0x01)<<8|0x01,    // Interface Type + Itf ID (MSB)
                            0x34,                     // Itf (LSB)
                            0x56,                     // Itf member
                            0x00,0x03,                // Payload size
                            0x12,
                                                      // Missing packet
                            0x56};

   DefineProgram Message;

   size = Message.unpack(payload, 0);
   UNSIGNED_LONGS_EQUAL(0,size);
}

// =============================================================================

//! @test Check the DefineProgramResponse message size
TEST(BatchProgramManagementMessages, DefineProgramResponse_size)
{
   DefineProgramResponse response(Common::Result::OK, 0x12);
   UNSIGNED_LONGS_EQUAL(2, response.size());
   CHECK_TRUE(response.min_size != response.size());

   response = DefineProgramResponse(Common::Result::FAIL_AUTH);
   UNSIGNED_LONGS_EQUAL(1, response.size());
   UNSIGNED_LONGS_EQUAL(response.min_size, response.size());
}

//! @test Check the DefineProgramResponse message pack
TEST(BatchProgramManagementMessages, DefineProgramResponse_pack)
{
   DefineProgramResponse response(Common::Result::OK, 0x12);
   payload= ByteArray(response.size());
   response.pack(payload,0);

   expected = ByteArray {  0x00,    // Response code
                           0x12};   // Program ID

   CHECK_EQUAL(expected, payload);

   response = DefineProgramResponse(Common::Result::FAIL_AUTH);
   payload = ByteArray(response.size());
   response.pack(payload, 0);

   expected = ByteArray { 0x01};    // Response code

   CHECK_EQUAL(expected, payload);
}

//! @test Check the DefineProgramResponse message unpack
TEST(BatchProgramManagementMessages, DefineProgramResponse_unpack)
{
   DefineProgramResponse response;

   payload = ByteArray  {  static_cast<uint8_t>(Common::Result::OK),    // Response code
                           0x12};                                       // Program ID

   response.unpack(payload,0);

   UNSIGNED_LONGS_EQUAL(static_cast<Common::Result>(Common::Result::OK) ,response.code);
   UNSIGNED_LONGS_EQUAL(0x12, response.ID);

   payload = ByteArray { static_cast<uint8_t>(Common::Result::FAIL_AUTH)};

   UNSIGNED_LONGS_EQUAL(1,response.unpack(payload,0));
   UNSIGNED_LONGS_EQUAL(static_cast<Common::Result>(Common::Result::FAIL_AUTH) ,response.code);
}

//! @test Check the DefineProgramResponse message pack failure
TEST(BatchProgramManagementMessages, DefineProgramResponse_pack_fail)
{
   DefineProgramResponse response(Common::Result::OK, 0x12);
   payload= ByteArray(response.size()-1);
   LONGS_EQUAL(0,response.pack(payload,0));
}

//! @test Check the DefineProgramResponse message unpack failure
TEST(BatchProgramManagementMessages, DefineProgramResponse_unpack_fail)
{
   DefineProgramResponse response;

   payload = ByteArray  {  static_cast<uint8_t>(Common::Result::OK)    // Response code
                           };                                          // Missing Program ID

   UNSIGNED_LONGS_EQUAL(0, response.unpack(payload,0));
}

// =============================================================================

//! @test Check the InvokeProgram message size
TEST(BatchProgramManagementMessages, InvokeProgram_size)
{
   InvokeProgram message;
   UNSIGNED_LONGS_EQUAL(1, message.size());
   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());

   message = InvokeProgram(0x01);
   UNSIGNED_LONGS_EQUAL(1, message.size());
   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
}

//! @test Check the InvokeProgram message pack
TEST(BatchProgramManagementMessages, InvokeProgram_pack)
{
   InvokeProgram message(0x01);
   payload = ByteArray(message.size());

   expected = ByteArray{0x01};

   message.pack(payload,0);
   CHECK_EQUAL(expected, payload);
}

//! @test Check the InvokeProgram message unpack
TEST(BatchProgramManagementMessages, InvokeProgram_unpack)
{
   InvokeProgram message;
   payload = ByteArray{0x01};

   message.unpack(payload,0);
   CHECK_EQUAL(0x01, message.ID);
}

//! @test Check the InvokeProgram message pack failure
TEST(BatchProgramManagementMessages, InvokeProgram_pack_fail)
{
   InvokeProgram message(0x01);
   payload = ByteArray(0);

   UNSIGNED_LONGS_EQUAL(0, message.pack(payload,0));
}

//! @test Check the InvokeProgram message unpack failure
TEST(BatchProgramManagementMessages, InvokeProgram_unpack_fail)
{
   InvokeProgram message;
   payload = ByteArray{};

   UNSIGNED_LONGS_EQUAL(0, message.unpack(payload,0));
}

// =============================================================================

//! @test Check the GetProgramActionsResponse message size
TEST(BatchProgramManagementMessages, GetProgramActionsResponse_size)
{
   GetProgramActionsResponse response(Common::Result::FAIL_AUTH);
   UNSIGNED_LONGS_EQUAL(1, response.size());
   UNSIGNED_LONGS_EQUAL(response.min_size, response.size());

   ByteArray program1 = ByteArray { 0x12,
                                    0x34,
                                    0x56 };

   std::vector<Action> actions;

   Action action1 = Action(0x98,
                           HF::Protocol::Message::Type::COMMAND_REQ,
                           0x00,
                           0x0134,
                           0x56,
                           program1);

   actions.push_back(action1);
   Entry entry(0x12,
               std::string("TEST"),
               actions);

   response = GetProgramActionsResponse(Common::Result::OK, entry);
   UNSIGNED_LONGS_EQUAL(1+          // Response Code
                        3+4+        // Prog ID + Name Length + n actions + 4 char
                        1*(7+3),    // 1 entry  with 3byte payload
                        response.size());

}

//! @test Check the GetProgramActionsResponse message pack
TEST(BatchProgramManagementMessages, GetProgramActionsResponse_pack)
{
   uint16_t size;

   GetProgramActionsResponse response(Common::Result::FAIL_AUTH);
   payload = ByteArray(response.size());
   size = response.pack(payload);
   UNSIGNED_LONGS_EQUAL(response.size(), size);
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH, payload[0]);

   std::vector<Action> actions;
   ByteArray program1 = ByteArray { 0x12,
                                    0x34,
                                    0x56 };

   Action action1 = Action(0x98,
                           HF::Protocol::Message::Type::COMMAND_REQ,
                           0x00,
                           0x0134,
                           0x56,
                           program1);

   actions.push_back(action1);

   Entry entry(0x12,
               std::string("TEST"),
               actions);

   response = GetProgramActionsResponse(Common::Result::OK, entry);
   payload = ByteArray(response.size());
   size= response.pack(payload,0);
   UNSIGNED_LONGS_EQUAL(response.size(), size);

   expected = ByteArray {  static_cast<uint8_t>(Common::Result::OK),
                           0x12,                         // Program ID
                           0x04,                         // Program Name length
                           'T','E','S','T',              // Program Name
                           0x01,                         // n actions
                           0x98,                         // UID
                           0x01,                         // Message type
                           (0x00 & 0x01)<<8|0x01,        // Interface Type + Itf ID (MSB)
                           0x34,                         // Itf (LSB)
                           0x56,                         // Itf member
                           0x00,0x03,                    // Payload size
                           0x12,
                           0x34,
                           0x56};

   CHECK_EQUAL(expected, payload);
}

//! @test Check the GetProgramActionsResponse message unpack
TEST(BatchProgramManagementMessages, GetProgramActionsResponse_unpack)
{
   uint16_t size;

   GetProgramActionsResponse response;
   payload = ByteArray{static_cast<uint8_t>(Common::Result::FAIL_AUTH)};
   size = response.unpack(payload);
   UNSIGNED_LONGS_EQUAL(response.size(), size);
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_AUTH, response.code);


   ByteArray program1 = ByteArray { 0x12,
                                    0x34,
                                    0x56 };


   payload = ByteArray {  static_cast<uint8_t>(Common::Result::OK),
                           0x12,                         // Program ID
                           0x04,                         // Program Name length
                           'T','E','S','T',              // Program Name
                           0x01,                         // n actions
                           0x98,                         // UID
                           0x01,                         // Message type
                           (0x00 & 0x01)<<8|0x01,        // Interface Type + Itf ID (MSB)
                           0x34,                         // Itf (LSB)
                           0x56,                         // Itf member
                           0x00,0x03,                    // Payload size
                           0x12,
                           0x34,
                           0x56};

   size= response.unpack(payload,0);
   UNSIGNED_LONGS_EQUAL(payload.size(), size);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,  response.code);
   UNSIGNED_LONGS_EQUAL(0x04,                response.program.name.size());
   CHECK_EQUAL(std::string("TEST"),          response.program.name);
   UNSIGNED_LONGS_EQUAL(0x01,                response.program.actions.size());
   UNSIGNED_LONGS_EQUAL(0x98,                response.program.actions[0].reference);
   UNSIGNED_LONGS_EQUAL(0x01,                response.program.actions[0].type);
   UNSIGNED_LONGS_EQUAL(0x00,                response.program.actions[0].itf.role);
   UNSIGNED_LONGS_EQUAL(0x0134,              response.program.actions[0].itf.id);
   UNSIGNED_LONGS_EQUAL(0x56,                response.program.actions[0].itf.member);
   UNSIGNED_LONGS_EQUAL(0x0003,              response.program.actions[0].payload.size());
   CHECK_EQUAL(program1,                     response.program.actions[0].payload);
}

//! @test Check the GetProgramActionsResponse message pack failure
TEST(BatchProgramManagementMessages, GetProgramActionsResponse_pack_fail)
{
   GetProgramActionsResponse response(Common::Result::FAIL_AUTH);

   payload = ByteArray(response.size() - 1);
   uint16_t size = response.pack(payload,0);

   UNSIGNED_LONGS_EQUAL(0, size);
}

//! @test Check the GetProgramActionsResponse message unpack failure
TEST(BatchProgramManagementMessages, GetProgramActionsResponse_unpack_fail)
{
   uint16_t size = 0;

   payload = ByteArray {   static_cast<uint8_t>(Common::Result::OK),
                           0x12,
                           0x04,                     // Program ID
                           'T','E','S','T',          // Program Name
                           0x01,                     // n actions
                           0x98,                     // UID
                           0x01,                     // Message type
                           (0x00 & 0x01)<<8|0x01,    // Interface Type + Itf ID (MSB)
                           0x34,                     // Itf (LSB)
                           0x56,                     // Itf member
                           0x00,0x03,                // Payload size
                           0x12,
                                                     // Missing packet
                           0x56};

   GetProgramActionsResponse response;

   size= response.unpack(payload,0);
   UNSIGNED_LONGS_EQUAL(0,size);
}

// =============================================================================
// Batch Program Management Client
// =============================================================================

//! Test Group for Batch Program Management Client interface class.
TEST_GROUP(BatchProgramManagementClient)
{
   // TODO Add required unit tests.
   struct BatchProgramManagementClient: public InterfaceHelper<
         BatchProgramManagement::Client>
   {
      void invoke_program(const Protocol::Address &addr) override
      {
         mock("BatchProgramManagement::Client").actualCall("invoke_program");
         InterfaceHelper<BatchProgramManagement::Client>::invoke_program(addr);
      }

   };

   BatchProgramManagementClient client;

   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      client                    = BatchProgramManagementClient();

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

//! @test Define Program support.
TEST(BatchProgramManagementClient, DefineProgram)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.define_program(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::DEFINE_PROGRAM_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Invoke Program support.
TEST(BatchProgramManagementClient, InvokeProgram)
{
   // FIXME Generated Stub.
   mock("BatchProgramManagement::Client").expectOneCall("invoke_program");

   packet.message.itf.member = BatchProgramManagement::INVOKE_PROGRAM_CMD;

   CHECK_EQUAL(Common::Result::OK, client.handle(packet, payload, 3));

   mock("BatchProgramManagement::Client").checkExpectations();
}

//! @test Delete Program support.
TEST(BatchProgramManagementClient, DeleteProgram)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.delete_program(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::DELETE_PROGRAM_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Delete All Programs support.
TEST(BatchProgramManagementClient, DeleteAllPrograms)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.delete_all_programs(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

//! @test Get Program Actions support.
TEST(BatchProgramManagementClient, GetProgramActions)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   client.get_program_actions(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client.sendMsg.itf.role);
   LONGS_EQUAL(client.uid(), client.sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD, client.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client.sendMsg.type);
}

// =============================================================================
// Batch Program Management Server
// =============================================================================

//! Test Group for Batch Program Management Server interface class.
TEST_GROUP(BatchProgramManagementServer)
{
   // TODO Add required unit tests.
   struct BatchProgramManagementServer: public InterfaceHelper<
         BatchProgramManagement::Server>
   {
      void define_program(const Protocol::Address &addr) override
      {
         mock("BatchProgramManagement::Server").actualCall("define_program");
         InterfaceHelper<BatchProgramManagement::Server>::define_program(addr);
      }

      void delete_program(const Protocol::Address &addr) override
      {
         mock("BatchProgramManagement::Server").actualCall("delete_program");
         InterfaceHelper<BatchProgramManagement::Server>::delete_program(addr);
      }

      void delete_all_programs(const Protocol::Address &addr) override
      {
         mock("BatchProgramManagement::Server").actualCall("delete_all_programs");
         InterfaceHelper<BatchProgramManagement::Server>::delete_all_programs(addr);
      }

      void get_program_actions(const Protocol::Address &addr) override
      {
         mock("BatchProgramManagement::Server").actualCall("get_program_actions");
         InterfaceHelper<BatchProgramManagement::Server>::get_program_actions(addr);
      }

   };

   BatchProgramManagementServer server;

   Protocol::Address addr;

   Protocol::Packet packet;
   Common::ByteArray payload;

   TEST_SETUP()
   {
      server                    = BatchProgramManagementServer();

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

//! @test Maximum Number Of Entries support.
TEST(BatchProgramManagementServer, MaximumNumberOfEntries)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(BatchProgramManagementServer, MaximumNumberOfEntries, false,
                   maximum_number_of_entries, 42, 142);
}

//! @test Number Of Entries support.
TEST(BatchProgramManagementServer, NumberOfEntries)
{
   // FIXME Generated Stub.
   CHECK_ATTRIBUTE(BatchProgramManagementServer, NumberOfEntries, false, number_of_entries,
                   42, 142);
}

//! @test Define Program support.
TEST(BatchProgramManagementServer, DefineProgram)
{
   // FIXME Generated Stub.
   mock("BatchProgramManagement::Server").expectOneCall("define_program");

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("BatchProgramManagement::Server").checkExpectations();
}

//! @test Invoke Program support.
TEST(BatchProgramManagementServer, InvokeProgram)
{
   // FIXME Generated Stub.
   mock("Interface").expectOneCall("send");

   server.invoke_program(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::CLIENT_ROLE, server.sendMsg.itf.role);
   LONGS_EQUAL(server.uid(), server.sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::INVOKE_PROGRAM_CMD, server.sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, server.sendMsg.type);
}

//! @test Delete Program support.
TEST(BatchProgramManagementServer, DeleteProgram)
{
   // FIXME Generated Stub.
   mock("BatchProgramManagement::Server").expectOneCall("delete_program");

   packet.message.itf.member = BatchProgramManagement::DELETE_PROGRAM_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("BatchProgramManagement::Server").checkExpectations();
}

//! @test Delete All Programs support.
TEST(BatchProgramManagementServer, DeleteAllPrograms)
{
   // FIXME Generated Stub.
   mock("BatchProgramManagement::Server").expectOneCall("delete_all_programs");

   packet.message.itf.member = BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("BatchProgramManagement::Server").checkExpectations();
}

//! @test Get Program Actions support.
TEST(BatchProgramManagementServer, GetProgramActions)
{
   // FIXME Generated Stub.
   mock("BatchProgramManagement::Server").expectOneCall("get_program_actions");

   packet.message.itf.member = BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD;

   CHECK_EQUAL(Common::Result::OK, server.handle(packet, payload, 3));

   mock("BatchProgramManagement::Server").checkExpectations();
}
