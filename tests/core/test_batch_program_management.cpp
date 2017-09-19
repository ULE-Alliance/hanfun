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

#include <random>

using namespace HF;
using namespace HF::Core;
using namespace HF::Common;
using namespace HF::Testing;

using namespace HF::Core::BatchProgramManagement;


// =============================================================================
// Batch Program Management - Entries
// =============================================================================

//! Test Group for BatchProgram service parent class.
TEST_GROUP(BatchProgramEntries)
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

//! @test Entries next PID
TEST(BatchProgramEntries, Next_PID)
{
   LONGS_EQUAL(BatchProgramManagement::Entry::START_PID, entries.next_PID());   // 1st address

   auto &data = entries.data();

   Entry entry;

   for (uint16_t PID = BatchProgramManagement::Entry::START_PID;
         PID <= BatchProgramManagement::Entry::MAX_PID; ++PID)
   {
      entry.name    = "P";
      entry.ID = PID;
      data.emplace(PID, entry);
   }

   LONGS_EQUAL(BatchProgramManagement::Entry::AVAILABLE_PID, entries.next_PID());

   data.erase(2);                          // erase program 2
   LONGS_EQUAL(2, entries.next_PID()); // check if the next available PID is 2
   data.emplace(2, entry);                // restore it

   LONGS_EQUAL(BatchProgramManagement::Entry::AVAILABLE_PID, entries.next_PID());
}
//! @test Entries find by PID
TEST(BatchProgramEntries, Find_by_PID)
{
   auto &data = entries.data();
   Entry entry;

   // Create 10 Programs
   for (uint16_t i = BatchProgramManagement::Entry::START_PID; i < 10; ++i)
   {
      entry.name=std::string("P") + std::to_string(i);
      entry.ID = i;
      data.emplace(i, entry);
   }

   data.erase(2);                                    // erase program 2
   POINTERS_EQUAL(nullptr, entries.find("P2").operator->());   // Try to find prog 2 (should fail)

   entry.name=std::string("P2");
   entry.ID = 2;
   data.emplace(2, entry);                                     // restore it

   CHECK(nullptr != entries.find("P2").operator->());          // Try to find prog 2 (OK)
   CHECK_EQUAL(std::string("P2"), entries.find("P2")->name);   // Confirm by name.
   UNSIGNED_LONGS_EQUAL(2, entries.find("P2")->ID);            // Confirm by PID.
}


//! @test Entries find by Program name
TEST(BatchProgramEntries, Find_by_name)
{
   auto &data = entries.data();
   Entry entry;

   // Create 10 Programs
   for (uint16_t i = BatchProgramManagement::Entry::START_PID; i < 10; ++i)
   {
      entry.name = std::string("P") + std::to_string(i);
      entry.ID = i;
      data.emplace(i, entry);
   }

   data.erase(2);                                              // erase program 2
   POINTERS_EQUAL(nullptr, entries.find(2).operator->());      // Try to find prog 2 (should fail)

   entry.name = std::string("P2");
   entry.ID = 2;
   data.emplace(2, entry);                                     // restore it

   CHECK(nullptr != entries.find(2).operator->());             // Try to find prog 2 (OK)
   CHECK_EQUAL(std::string("P2"), entries.find(2)->name);      // Confirm by name.
   UNSIGNED_LONGS_EQUAL(2, entries.find(2)->ID);               // Confirm by PID.
}

//! @test Entries size
TEST(BatchProgramEntries, Size)
{
   LONGS_EQUAL(0, entries.size());

   Entry entry;

   // Create 10 Programs
   for (uint16_t i = BatchProgramManagement::Entry::START_PID; i < 10; ++i)
   {
      entry.name = std::string("P") + std::to_string(i);
      entry.ID = i;
      entries.data().emplace(i, entry);
   }

   LONGS_EQUAL(10, entries.size());
}

//! @test Entries destroy by PID
TEST(BatchProgramEntries, Destroy_by_PID)
{
   auto &data = entries.data();
   Entry entry;

   // Create 10 Programs
   for (uint16_t i = BatchProgramManagement::Entry::START_PID; i < 10; ++i)
   {
      entry.name = std::string("P") + std::to_string(i);
      entry.ID = i;
      data.emplace(i, entry);
   }

   LONGS_EQUAL(10, entries.size());

   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(10));    // Try to destroy prog 10 (NOK)
   CHECK_EQUAL(Common::Result::OK, entries.destroy(9));           // Try to destroy prog 9 (OK)
   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(9));     // Try to destroy prog 9 again (NOK)

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries destroy by Program
TEST(BatchProgramEntries, Destroy_by_program)
{
   auto &data = entries.data();
   Entry entry;

   // Create 10 Programs
   for (uint16_t i = BatchProgramManagement::Entry::START_PID; i < 10; ++i)
   {
      entry.name = std::string("P") + std::to_string(i);
      entry.ID = i;
      data.emplace(i, entry);
   }

   LONGS_EQUAL(10, entries.size());

   // Try to destroy prog 10 (NOK)
   entry.name = std::string("P10");
   entry.ID = 10;
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(entry));

   // Try to destroy prog 9 (OK)
   entry.name = std::string("P9");
   entry.ID = 9;
   UNSIGNED_LONGS_EQUAL(Common::Result::OK, entries.destroy(entry));

   // Try to destroy prog 9 again (NOK)
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(entry));

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries save new
TEST(BatchProgramEntries, Save)
{
   LONGS_EQUAL(0, entries.size());

   Entry entry;
   entry.name = std::string("P0");
   entry.ID = 0;
   entries.save(entry);

   UNSIGNED_LONGS_EQUAL(1, entries.size());
   CHECK_EQUAL(std::string("P0"), entries.data()[0].name);
}

// =============================================================================
// Batch Program Management
// =============================================================================

//! Test Group for BatchProgramManagement interface parent class.
TEST_GROUP(BatchProgramManagement)
{
   struct BatchProgramManagementBase: public InterfaceParentHelper<
         BatchProgramManagement::Base>
   {
      BatchProgramManagementBase(HF::Core::Unit0 &unit):
         InterfaceParentHelper<BatchProgramManagement::Base>(unit) {}
   };

   Testing::Device *device;
   BatchProgramManagementBase *service;

   TEST_SETUP()
   {
      device = new Testing::Device();
      service = new BatchProgramManagementBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;

      mock().clear();
   }
};

//! @test BatchProgramManagement::uid should return @c HF::Interface::BATCH_PROGRAM_MANAGEMENT.
TEST(BatchProgramManagement, UID)
{
   LONGS_EQUAL(HF::Interface::BATCH_PROGRAM_MANAGEMENT, service->uid());
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
      BatchProgramManagementClient(HF::Core::Unit0 &unit):
         InterfaceHelper<BatchProgramManagement::Client>(unit){}

      void defined (DefineProgramResponse &response)
      {
         UNUSED(response);
         mock("BatchProgramManagement::Client").actualCall("defined");
      }

      void deleted (DeleteProgramResponse &response)
      {
         UNUSED(response);
         mock("BatchProgramManagement::Client").actualCall("deleted");
      }

      void deleted (DeleteAllProgramsResponse &response)
      {
         UNUSED(response);
         mock("BatchProgramManagement::Client").actualCall("deleted");
      }

      void invoked (InvokeProgramResponse &response)
      {
         UNUSED(response);
         mock("BatchProgramManagement::Client").actualCall("invoked");
      }

      void got_actions (GetProgramActionsResponse &response)
      {
         UNUSED(response);
         mock("BatchProgramManagement::Client").actualCall("got_actions");
      }
   };

   Testing::Device *device;
   BatchProgramManagementClient *client;

   Protocol::Address addr;
   Protocol::Packet packet;
   Testing::Link link;

   std::vector<Action> actions;

   TEST_SETUP()
   {
      device = new Testing::Device();
      client = new BatchProgramManagementClient(*(device->unit0()));

      addr = Protocol::Address(42, 0);

      link = Testing::Link();
      packet = Protocol::Packet();
      packet.message.type = Protocol::Message::COMMAND_RES;
      packet.message.itf.role = HF::Interface::SERVER_ROLE;
      packet.message.itf.id = HF::Interface::BATCH_PROGRAM_MANAGEMENT;
      packet.link = &link;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;
      delete device;

      actions.clear();

      mock().clear();
   }

   ByteArray GenerateRandomPayload ( uint8_t size)
   {
      ByteArray _payload(size);
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0x00, 0xFF);

      for (int n = 0; n < size; ++n)
         _payload[n]=dis(gen);

      return _payload;
   }

   Action GenerateAction ( uint8_t _UID, HF::Protocol::Message::Type _msg_type,
                           uint8_t _itf_type, uint16_t _itf_UID, uint8_t _itf_member,
                           uint8_t payload_size)
   {
      ByteArray _payload = GenerateRandomPayload(payload_size);
      return Action(_UID, _msg_type, _itf_type, _itf_UID, _itf_member, _payload);
   }

   void DoActionTests(Entry expected, DefineProgram actual)
   {
      UNSIGNED_LONGS_EQUAL(expected.ID, actual.ID);
      CHECK_EQUAL(expected.name, actual.name);

      for(uint8_t i=0; i< expected.actions.size(); ++i)
      {
         UNSIGNED_LONGS_EQUAL(expected.actions[i].reference,         actual.actions[0].reference);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].type,              actual.actions[0].type);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].itf.role,          actual.actions[0].itf.role);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].itf.id,            actual.actions[0].itf.id);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].itf.member,        actual.actions[0].itf.member);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].payload.size(),    actual.actions[0].payload.size());
         CHECK_EQUAL(expected.actions[i].payload,                    actual.actions[0].payload);
      }
   }

};

//! @test Define Program support.
TEST(BatchProgramManagementClient, DefineProgram)
{
   mock("Interface").expectOneCall("send");

   auto msg_type = HF::Protocol::Message::Type::COMMAND_REQ;

   actions.push_back(GenerateAction(0x11,                                     // UID
                                    msg_type,                                 // Msg type
                                    0x00,                                     // Itf type
                                    0x2233,                                   // Itf UID
                                    0x44,                                     // Itf Member
                                    10));                                     // Payload size

   Entry test(0x12, std::string("TEST"), actions);

   client->define_program(addr,test);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::DEFINE_PROGRAM_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   DefineProgram message;

   message.unpack(client->sendMsg.payload);

   DoActionTests(test,message);
}

//! @test Defined support.
TEST(BatchProgramManagementClient, Defined_OK)
{
   Common::ByteArray payload =
   {
      Common::Result::OK,
      0x12,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;

   mock("BatchProgramManagement::Client").expectOneCall("defined");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 0));

   mock("BatchProgramManagement::Client").checkExpectations();
}

//! @test Invoke Program support.
TEST(BatchProgramManagementClient, InvokeProgram)
{
   mock("Interface").expectOneCall("send");

   client->invoke_program(addr, 0x01);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::INVOKE_PROGRAM_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   InvokeProgram actual;
   actual.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x01, actual.ID);
}

//! @test invoked support.
TEST(BatchProgramManagementClient, Invoked_OK)
{
   Common::ByteArray payload =
   {
      Common::Result::OK,
      0x12,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = BatchProgramManagement::INVOKE_PROGRAM_CMD;

   mock("BatchProgramManagement::Client").expectOneCall("invoked");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 0));

   mock("BatchProgramManagement::Client").checkExpectations();
}

//! @test Delete Program support.
TEST(BatchProgramManagementClient, DeleteProgram)
{
   mock("Interface").expectOneCall("send");

   client->delete_program(addr, 0x01);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::DELETE_PROGRAM_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   DeleteProgram actual;
   actual.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x01, actual.ID);
}

//! @test Deleted support.
TEST(BatchProgramManagementClient, Deleted_OK_one)
{
   Common::ByteArray payload =
   {
      Common::Result::OK,
      0x12,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = BatchProgramManagement::DELETE_PROGRAM_CMD;

   mock("BatchProgramManagement::Client").expectOneCall("deleted");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 0));

   mock("BatchProgramManagement::Client").checkExpectations();
}

//! @test Delete All Programs support.
TEST(BatchProgramManagementClient, DeleteAllPrograms)
{
   mock("Interface").expectOneCall("send");

   client->delete_all_programs(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

//! @test Deleted support.
TEST(BatchProgramManagementClient, Deleted_OK_all)
{
   Common::ByteArray payload =
   {
      Common::Result::OK,
   };

   packet.message.length     = payload.size();
   packet.message.itf.member = BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD;

   mock("BatchProgramManagement::Client").expectOneCall("deleted");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 0));

   mock("BatchProgramManagement::Client").checkExpectations();
}

//! @test Get Program Actions support.
TEST(BatchProgramManagementClient, GetProgramActions)
{
   mock("Interface").expectOneCall("send");

   client->get_program_actions(addr, 0x01);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   GetProgramActions actual;
   actual.unpack(client->sendMsg.payload);
   UNSIGNED_LONGS_EQUAL(0x01, actual.ID);
}

//! @test Got actions support.
TEST(BatchProgramManagementClient, Got_actions)
{
   actions.push_back(GenerateAction(0x11,                         // UID
               HF::Protocol::Message::Type::COMMAND_REQ,          // Msg type
               0x00,                                              // Itf type
               0x2233,                                            // Itf UID
               0x44,                                              // Itf Member
               10));                                              // Payload size

   Entry test(0x12, std::string("TEST"), actions);


   Common::ByteArray payload (test.size());
   test.pack(payload);

   packet.message.length     = payload.size();
   packet.message.itf.member = BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD;

   mock("BatchProgramManagement::Client").expectOneCall("got_actions");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK,
                        client->handle(packet, payload, 0));

   mock("BatchProgramManagement::Client").checkExpectations();
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
      BatchProgramManagementServer(HF::Core::Unit0 &unit):
         InterfaceHelper<BatchProgramManagement::Server>(unit){}

      Common::Result define_program(const Protocol::Packet &packet, DefineProgram &msg) override
      {
         mock("BatchProgramManagement::Server").actualCall("define_program");
         return InterfaceHelper<BatchProgramManagement::Server>::define_program(packet, msg);
      }

      Common::Result invoke_program(const Protocol::Packet &packet, InvokeProgram &msg) override
      {
         mock("BatchProgramManagement::Server").actualCall("invoke_program");
         return InterfaceHelper<BatchProgramManagement::Server>::invoke_program(packet, msg);
      }

      Common::Result delete_program(const Protocol::Packet &packet, InvokeProgram &msg) override
      {
         mock("BatchProgramManagement::Server").actualCall("delete_program");
         return InterfaceHelper<BatchProgramManagement::Server>::delete_program(packet, msg);
      }

      Common::Result delete_all_programs(const Protocol::Packet &packet) override
      {
         mock("BatchProgramManagement::Server").actualCall("delete_all_programs");
         return InterfaceHelper<BatchProgramManagement::Server>::delete_all_programs(packet);
      }

      Common::Result get_program_actions(const Protocol::Packet &packet,
                               GetProgramActions &msg) override
      {
         mock("BatchProgramManagement::Server").actualCall("get_program_actions");
         return InterfaceHelper<BatchProgramManagement::Server>::get_program_actions(packet, msg);
      }
   };


   class TestUnit: public Unit::AbstractUnit
   {

      public:
      TestUnit (IDevice &device) :
            Unit::AbstractUnit(device)
      {
      }

      Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload,
                             uint16_t offset)
      {
         UNUSED(packet);
         UNUSED(payload);
         UNUSED(offset);
         mock("TestUnit").actualCall("handle");
         return static_cast<Common::Result>(mock("TestUnit").returnIntValueOrDefault(
               static_cast<int>(Common::Result::OK)));
      }

      uint8_t id () const
      {
         return mock("TestUnit").actualCall("id").returnIntValueOrDefault(0x01);
      }

      uint16_t uid () const
      {
         return mock("TestUnit").actualCall("uid").returnIntValueOrDefault(0x01);
      }

      HF::Attributes::List attributes (Common::Interface itf, uint8_t pack_id,
                                       const HF::Attributes::UIDS &uids) const
                                       {
         UNUSED(itf);
         UNUSED(pack_id);
         UNUSED(uids);

         return HF::Attributes::List();
      }

   };


   Testing::Concentrator *base;
   BatchProgramManagementServer *server;
   TestUnit *unit;


   Protocol::Packet packet;
   Common::ByteArray payload;

   Protocol::Address addr;
   Testing::Link link;


   TEST_SETUP()
   {
      base = new Testing::Concentrator();
      unit = new TestUnit(*base);

      server = new BatchProgramManagementServer(*(base->unit0()));

      addr = Protocol::Address(42, 0);
      link = Testing::Link();

      packet = Protocol::Packet();
      packet.source = addr;
      packet.destination = Protocol::Address(0, 0);
      packet.message.itf.role = HF::Interface::SERVER_ROLE;
      packet.message.itf.id = server->uid();
      packet.message.itf.member = 0xFF;

      packet.message.type = Protocol::Message::COMMAND_REQ;
      packet.link = &link;

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete server;
      delete unit;
      delete base;

      mock().clear();
   }

   ByteArray GenerateRandomPayload (uint8_t size)
   {
      ByteArray _payload(size);
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0x00, 0xFF);

      for (int n = 0; n < size; ++n)
         _payload[n] = dis(gen);

      return _payload;
   }

   Action GenerateAction (uint8_t _UID, HF::Protocol::Message::Type _msg_type,
                          uint8_t _itf_type,
                          uint16_t _itf_UID, uint8_t _itf_member,
                          uint8_t payload_size)
   {
      ByteArray _payload = GenerateRandomPayload(payload_size);
      return Action(_UID, _msg_type, _itf_type, _itf_UID, _itf_member, _payload);
   }

   void DoActionTests (Entry expected, DefineProgram actual)
   {
      UNSIGNED_LONGS_EQUAL(expected.ID, actual.ID);
      CHECK_EQUAL(expected.name, actual.name);

      for (uint8_t i = 0; i < expected.actions.size(); ++i)
      {
         UNSIGNED_LONGS_EQUAL(expected.actions[i].reference, actual.actions[0].reference);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].type, actual.actions[0].type);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].itf.role, actual.actions[0].itf.role);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].itf.id, actual.actions[0].itf.id);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].itf.member, actual.actions[0].itf.member);
         UNSIGNED_LONGS_EQUAL(expected.actions[i].payload.size(), actual.actions[0].payload.size());
         CHECK_EQUAL(expected.actions[i].payload, actual.actions[0].payload);
      }
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
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
               HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
               0x00,                                     // Itf type
               0x2233,                                   // Itf UID
               0x44,                                     // Itf Member
               10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   DefineProgram received(_received);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();



   mock("BatchProgramManagement::Server").expectOneCall("define_program");

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   LONGS_EQUAL(1, server->entries().size());    // Check if the new program is on the DB.
   CHECK_TRUE( server->entry(0x12) != nullptr);

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   CHECK_EQUAL(std::string("TEST"), server->entries().begin()->second.name);

   // ----- Check the response message -----

   DefineProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   LONGS_EQUAL(0x12, resp.ID);
}

//! @test Define Program support. Search for next available PID
TEST(BatchProgramManagementServer, DefineProgram_next_available)
{
   Entry entry;

   entry.name = std::string("P18");
   entry.ID = 0x12;
   server->entries().save(entry);


   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
               HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
               0x00,                                     // Itf type
               0x2233,                                   // Itf UID
               0x44,                                     // Itf Member
               10));                                     // Payload size

   Entry _received(Entry::AVAILABLE_PID, std::string("TEST"), actions);

   DefineProgram received(_received);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();



   mock("BatchProgramManagement::Server").expectOneCall("define_program");

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   LONGS_EQUAL(2, server->entries().size());    // Check if the new program is on the DB.
   CHECK_TRUE( server->entry(Entry::START_PID) != nullptr);

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   CHECK_EQUAL(std::string("TEST"), server->entries().begin()++->second.name); // Check the 2nd entry

   // ----- Check the response message -----

   DefineProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   LONGS_EQUAL(Entry::START_PID, resp.ID);
}

//! @test Define Program support.
TEST(BatchProgramManagementServer, DefineProgram_fail_no_UID_in_device)
{
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x11,                // UID (Different from the UID in the dev)
               HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
               0x00,                                     // Itf type
               0x2233,                                   // Itf UID
               0x44,                                     // Itf Member
               10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   DefineProgram received(_received);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();



   mock("BatchProgramManagement::Server").expectOneCall("define_program");

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   LONGS_EQUAL(0, server->entries().size());    // Check if the new program is on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   DefineProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Define Program support. Fail because the PID is in use.
TEST(BatchProgramManagementServer, DefineProgram_fail_same_ID)
{
   Entry entry;

   entry.name=std::string("P18");
   entry.ID=0x12;
   server->entries().save(entry);

   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
               HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
               0x00,                                     // Itf type
               0x2233,                                   // Itf UID
               0x44,                                     // Itf Member
               10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   DefineProgram received(_received);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();



   mock("BatchProgramManagement::Server").expectOneCall("define_program");

   packet.message.itf.member = BatchProgramManagement::DEFINE_PROGRAM_CMD;

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   LONGS_EQUAL(1, server->entries().size());    // Check if we only have 1 program on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   CHECK_EQUAL(std::string("P18"), server->entries().begin()->second.name);

   // ----- Check the response message -----

   DefineProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Invoke Program support.
TEST(BatchProgramManagementServer, InvokeProgram)
{

   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   server->entries().save(_received);

   InvokeProgram received(0x12);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::INVOKE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("TestUnit").expectOneCall("id").andReturnValue(1);
   mock("TestUnit").expectOneCall("handle");
   mock("BatchProgramManagement::Server").expectOneCall("invoke_program");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();
   mock("TestUnit").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   InvokeProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   LONGS_EQUAL(0x12, resp.ID);
}

/** @test Invoke Program support.
 *
 * Should fail because the device doesn't have the specified UID.
 */
TEST(BatchProgramManagementServer, InvokeProgram_fail_no_UID)
{

   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x11,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   server->entries().save(_received);

   InvokeProgram received(0x12);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::INVOKE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("TestUnit").expectOneCall("id");
   mock("TestUnit").expectNoCall("handle");
   mock("BatchProgramManagement::Server").expectOneCall("invoke_program");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();
   mock("TestUnit").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   InvokeProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

/** @test Invoke Program support.
 *
 * Should fail because the unit handler sent an error.
 */
TEST(BatchProgramManagementServer, InvokeProgram_fail_program_error)
{

   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   server->entries().save(_received);

   InvokeProgram received(0x12);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::INVOKE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("TestUnit").expectOneCall("id");
   mock("TestUnit").expectOneCall("handle").andReturnValue((int)Common::Result::FAIL_ARG);
   mock("BatchProgramManagement::Server").expectOneCall("invoke_program");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();
   mock("TestUnit").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   InvokeProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Delete Program support.
TEST(BatchProgramManagementServer, DeleteProgram)
{
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   server->entries().save(_received);

   DeleteProgram received(0x12);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::DELETE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("BatchProgramManagement::Server").expectOneCall("delete_program");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   UNSIGNED_LONGS_EQUAL(0, server->entries().size());

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   DeleteProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   UNSIGNED_LONGS_EQUAL(0x12, resp.ID);
}

/**! @test Delete Program support.
 *
 *Fail because Program ID doesn't exist
 */
TEST(BatchProgramManagementServer, DeleteProgram_fail_no_program)
{
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   server->entries().save(_received);

   DeleteProgram received(0x10);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::DELETE_PROGRAM_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("BatchProgramManagement::Server").expectOneCall("delete_program");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   UNSIGNED_LONGS_EQUAL(1, server->entries().size());

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   DeleteProgramResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Delete All Programs support.
TEST(BatchProgramManagementServer, DeleteAllPrograms)
{
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x12, std::string("TEST"), actions);

   server->entries().save(_received);

   UNSIGNED_LONGS_EQUAL(1, server->entries().size());

   mock("BatchProgramManagement::Server").expectOneCall("delete_all_programs");

   packet.message.itf.member = BatchProgramManagement::DELETE_ALL_PROGRAMS_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   UNSIGNED_LONGS_EQUAL(0, server->entries().size());

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   DeleteAllProgramsResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);

}

//! @test Get Program Actions support.
TEST(BatchProgramManagementServer, GetProgramActions)
{
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x10, std::string("TEST"), actions);

   server->entries().save(_received);

   GetProgramActions received(0x10);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("BatchProgramManagement::Server").expectOneCall("get_program_actions");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   UNSIGNED_LONGS_EQUAL(1, server->entries().size());

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   GetProgramActionsResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   DoActionTests(_received, resp.program);
}

/**! @test Get Program Actions support.
 *
 * Fail because the device don't have the required program.
 */
TEST(BatchProgramManagementServer, GetProgramActions_fail_no_program)
{
   std::vector<Action> actions;
   actions.push_back(GenerateAction(0x01,                // UID
         HF::Protocol::Message::Type::COMMAND_REQ, // Msg type
         0x00,                                     // Itf type
         0x2233,                                   // Itf UID
         0x44,                                     // Itf Member
         10));                                     // Payload size

   Entry _received(0x10, std::string("TEST"), actions);

   server->entries().save(_received);

   GetProgramActions received(0x22);
   payload = ByteArray(received.size());

   received.pack(payload);

   packet.message.itf.member = BatchProgramManagement::GET_PROGRAM_ACTIONS_CMD;
   packet.message.type = Protocol::Message::COMMAND_REQ;
   packet.message.length = payload.size();

   mock("BatchProgramManagement::Server").expectOneCall("get_program_actions");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("BatchProgramManagement::Server").checkExpectations();

   UNSIGNED_LONGS_EQUAL(1, server->entries().size());

   // Check response packet destination address.
   LONGS_EQUAL(1, base->packets.size());

   Protocol::Packet *response = base->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   GetProgramActionsResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}
