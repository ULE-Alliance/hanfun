// =============================================================================
/*!
 * @file       tests/core/test_weekly_scheduling.cpp
 *
 * This is file contains the unit tests for the Weekly Scheduling service
 * implementation.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/weekly_scheduling.h"
#include "hanfun/core/batch_program_management.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Common;

using namespace HF::Testing;

using namespace HF::Core::Scheduling::Weekly;

// =============================================================================
// Weekly Scheduling
// =============================================================================

//! Test Group for Scheduling::Weekly service parent class.
TEST_GROUP(Scheduling_Weekly)
{
   struct WeeklySchedulingBase: public InterfaceHelper<Scheduling::Weekly::IServerBase>
   {
      WeeklySchedulingBase(HF::Core::Unit0 &unit):
         InterfaceHelper<Scheduling::Weekly::IServerBase>(unit)
      {}

      uint8_t number_of_entries() const override
      {
         return mock("WeeklySchedulingBase").actualCall("number_of_entries")
                   .returnIntValueOrDefault(0);
      }
   };

   Testing::Device *device;
   WeeklySchedulingBase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new WeeklySchedulingBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;

      mock().clear();
   }
};

//! @test Scheduling::Weekly::uid should return @c HF::Interface::EVENT_SCHEDULING.
TEST(Scheduling_Weekly, UID)
{
   LONGS_EQUAL(HF::Interface::WEEKLY_SCHEDULING, service->uid());
}

//! @test Maximum Number Of Entries support.
TEST(Scheduling_Weekly, MaximumNumberOfEntries)
{
   HF::Attributes::IAttribute *attr = Scheduling::Weekly::create_attribute(
      Scheduling::MAXIMUM_NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(MaximumNumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(MaximumNumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::WEEKLY_SCHEDULING, attr->interface());

   delete attr;
}

//! @test Number Of Entries support.
TEST(Scheduling_Weekly, NumberOfEntries)
{
   HF::Attributes::IAttribute *attr = Scheduling::Weekly::create_attribute(
      Scheduling::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::WEEKLY_SCHEDULING, attr->interface());

   delete attr;
}

//! @test Status support.
TEST(Scheduling_Weekly, Status)
{
   HF::Attributes::IAttribute *attr = Scheduling::Weekly::create_attribute(
      Scheduling::STATUS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Status::ID, attr->uid());
   CHECK_EQUAL(Status::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::WEEKLY_SCHEDULING, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(Scheduling_Weekly, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr = Scheduling::Weekly::create_attribute(
      Scheduling::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// Weekly Scheduling - Entries
// =============================================================================

//! Test Group for GroupManagement service parent class.
TEST_GROUP(WeeklySchedulingEntries)
{
   using Entries = Scheduling::Entries<Day>;

   struct TestEntries: public Entries
   {
      Entries::Container &data()
      {
         return db;
      }
   };

   TestEntries entries;
   Entries::Container &data = entries.data();

   TEST_SETUP()
   {
      entries = TestEntries();
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }

   void IssueEvents(uint8_t number)
   {
      Entry event;

      for (uint8_t id = Entry::START_ID; id < number - Entry::START_ID;
           ++id)
      {
         event.id     = id;
         event.status = 0x01;
         event.pid    = id;
         data.emplace(id, event);
      }
   }
};

//! @test Entries next address
TEST(WeeklySchedulingEntries, Next_id)
{
   LONGS_EQUAL(Entry::START_ID, entries.next_id());   // 1st address

   IssueEvents(Entry::MAX_ID + 1);

   LONGS_EQUAL(Entry::AVAILABLE_ID, entries.next_id());

   data.erase(2);                            // erase group 2
   LONGS_EQUAL(2, entries.next_id());        // check if the next available address is 2

   Entry event;
   event.id = 2;                             // restore it
   data.emplace(2, event);

   LONGS_EQUAL(Entry::AVAILABLE_ID, entries.next_id());
}

//! @test Entries find by address
TEST(WeeklySchedulingEntries, Find_by_id)
{
   // Create 10 Events
   IssueEvents(10);

   Entry event;
   event.id  = 2;
   event.pid = 0x22;

   data.erase(2);                                         // erase group 2
   POINTERS_EQUAL(nullptr, entries.find(2).operator->()); // Try to find group 2 (should fail)
   data[2] = event;                                       // restore it

   CHECK(nullptr != entries.find(2).operator->());             // Try to find group 2 (OK)
   UNSIGNED_LONGS_EQUAL(0x22, entries.find(2)->pid);
}

//! @test Entries size
TEST(WeeklySchedulingEntries, Size)
{
   LONGS_EQUAL(0, entries.size());

   // Create 10 events
   IssueEvents(10);

   LONGS_EQUAL(10, entries.size());
}

//! @test Entries destroy by id
TEST(WeeklySchedulingEntries, Destroy_by_address)
{
   // Create 10 Events
   IssueEvents(10);

   LONGS_EQUAL(10, entries.size());

   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(10)); // Try to destroy Event 10 (NOK)
   CHECK_EQUAL(Common::Result::OK, entries.destroy(9));        // Try to destroy Event 9 (OK)
   CHECK_EQUAL(Common::Result::FAIL_ARG, entries.destroy(9));  // Try to destroy Event 9 again (NOK)

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries destroy by @c Event
TEST(WeeklySchedulingEntries, Destroy_by_group)
{
   // Create 10 Events
   IssueEvents(10);

   Entry event;

   LONGS_EQUAL(10, entries.size());

   // Try to destroy event 10 (NOK)
   event.id = 10;
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(event));

   // Try to destroy event 9 (OK)
   event.id = 9;
   UNSIGNED_LONGS_EQUAL(Common::Result::OK, entries.destroy(event));

   // Try to destroy event 9  again (NOK)
   event.id = 9;
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, entries.destroy(event));

   LONGS_EQUAL(9, entries.size());
}

//! @test Entries save new
TEST(WeeklySchedulingEntries, Save)
{
   LONGS_EQUAL(0, entries.size());

   Entry event;
   event.id  = 1;
   event.pid = 11;
   entries.save(event);

   UNSIGNED_LONGS_EQUAL(1, entries.size());
   UNSIGNED_LONGS_EQUAL(11, data.begin()->second.pid);
}

// =============================================================================
// Weekly Scheduling messages
// =============================================================================

TEST_GROUP(WeeklyScheduling_Messages)
{

   Common::ByteArray payload, expected, Test_Entry_payload;
   uint16_t size;

   Entry Test_entry;

   void Do_Test_Entry_Payload(Entry &message)
   {
      UNSIGNED_LONGS_EQUAL(0x11, message.id);
      UNSIGNED_LONGS_EQUAL(0x01, message.status);
      UNSIGNED_LONGS_EQUAL(SATURDAY, message.time.day);
      UNSIGNED_LONGS_EQUAL(14, message.time.hour);
      UNSIGNED_LONGS_EQUAL(50, message.time.minute);
      UNSIGNED_LONGS_EQUAL(0x12, message.pid);
   }

   TEST_SETUP()
   {
      size               = 0;
      Test_entry         = Entry(0x11, 0x01, Day(SATURDAY, 14, 50), 0x12);
      Test_Entry_payload = Common::ByteArray {0x11,                           // Event ID
                                              0x80,                           // Status 0x01 << 7
                                              static_cast<uint8_t>(SATURDAY), // Day of week
                                              14,                             // Hour
                                              50,                             // Minute
                                              0x12                    // Program ID.
      };
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

// ========== Entry ==========

TEST(WeeklyScheduling_Messages, Entry_size)
{
   Entry message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(6, message.size());
}

TEST(WeeklyScheduling_Messages, Entry_pack)
{
   Entry message = Test_entry;

   payload = Common::ByteArray(message.size());

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(6, size);
   CHECK_EQUAL(Test_Entry_payload, payload);
}

TEST(WeeklyScheduling_Messages, Entry_unpack)
{
   Entry message;

   payload = Test_Entry_payload;

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(6, size);

   Do_Test_Entry_Payload(message);
}

TEST(WeeklyScheduling_Messages, Entry_pack_fail_no_size)
{
   Entry message = Test_entry;

   payload = Common::ByteArray(message.size() - 1);

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

TEST(WeeklyScheduling_Messages, Entry_unpack_fail)
{
   Entry message;

   payload = Common::ByteArray {0x11,                   // Event ID
                                0x80,                   // Status 0x01 << 7
                                static_cast<uint8_t>(SATURDAY), // Day of week
                                14,                             // Hour
                                50,                             // Minute
                                                        // Program ID.   (missing)
   };

   size = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

TEST(WeeklyScheduling_Messages, Convert)
{
   // ========== 2000-JAN-01 00:00:00 - Sa ==========
   Core::Time::Date test_date((uint32_t)0);
   Day result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(SATURDAY, result.day);
   UNSIGNED_LONGS_EQUAL(0, result.hour);
   UNSIGNED_LONGS_EQUAL(0, result.minute);

   // ========== 2000-JAN-02 14:50:30 - Su ==========
   test_date = Core::Time::Date(0, 1, 2, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(SUNDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2001-MAY-28 14:50:30 - Mo ==========
   test_date = Core::Time::Date(1, 5, 28, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(MONDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2004-FEB-29 14:50:30 - Su ==========    (Leap year)
   test_date = Core::Time::Date(4, 2, 29, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(SUNDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2005-MAY-03 14:50:30 - Tu ==========
   test_date = Core::Time::Date(5, 5, 3, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(TUESDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2017-JUL-5 14:50:30 - We ==========
   test_date = Core::Time::Date(17, 7, 5, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(WEDNESDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2020-OCT-01 14:50:30 - Th ==========
   test_date = Core::Time::Date(20, 10, 1, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(THURSDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2023-SEP-01 14:50:30 - Fr ==========
   test_date = Core::Time::Date(23, 9, 1, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(FRIDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

   // ========== 2023-APR-01 14:50:30 - Sa ==========
   test_date = Core::Time::Date(23, 4, 1, 14, 50, 30);
   result = Day::convert(Core::Time::convert(test_date));

   UNSIGNED_LONGS_EQUAL(SATURDAY, result.day);
   UNSIGNED_LONGS_EQUAL(14, result.hour);
   UNSIGNED_LONGS_EQUAL(50, result.minute);

}

TEST(WeeklyScheduling_Messages, first)
{
   // ========== 2000-JAN-01 00:00:00 - Sa ==========
   Core::Time::Date test_date((uint32_t)0);
   Day result(SATURDAY,00,01);

   UNSIGNED_LONGS_EQUAL(Day::MINUTE, result.first(Core::Time::convert(test_date))); // 1 min after


   // ========== 2000-JAN-02 14:50:30 - Su ==========
   test_date = Core::Time::Date(0, 1, 2, 14, 50, 30);
   result = Day(SATURDAY,14,50);

   UNSIGNED_LONGS_EQUAL(6*Day::DAY,  result.first(Core::Time::convert(test_date))   // 6 days after
                                     - Core::Time::convert(test_date));

   // ========== 2001-MAY-28 14:50:30 - Mo ==========
   test_date = Core::Time::Date(1, 5, 28, 14, 50, 30);
   result = Day(TUESDAY,14,50);

   UNSIGNED_LONGS_EQUAL(1 * Day::DAY, result.first(Core::Time::convert(test_date)) // 1 day after
                                          - Core::Time::convert(test_date));

   // ========== 2004-FEB-29 14:50:30 - Su ==========    (Leap year)
   test_date = Core::Time::Date(4, 2, 29, 14, 50, 30);
   result = Day(SUNDAY,14,45);

   UNSIGNED_LONGS_EQUAL(Day::WEEK - 5 * Day::MINUTE, result.first(Core::Time::convert(test_date))
                                            - Core::Time::convert(test_date));      //next week

}

TEST(WeeklyScheduling_Messages, first_sequential_test)
{
   // ========== 2017-JUL-5 14:50:30 - We ==========
   Core::Time::Date test_date = Core::Time::Date(17, 7, 5, 00, 00, 00);
   Day result;
   uint8_t dow;

   for (int d = 0; d < 7; d++)
   {
      dow = (WEDNESDAY + d) % 7;
      for (int h = 0; h < 24; h++)
      {
         for (int m = 0; m < 60; m++)
         {
            result = Day(dow, h, m);
            UNSIGNED_LONGS_EQUAL(d * Day::DAY + h * Day::HOUR + m * Day::MINUTE ,
                                 result.first(Core::Time::convert(test_date))
                                               - Core::Time::convert(test_date));
         }
      }
   }
}

// ========== Get Entry Response ==========

TEST(WeeklyScheduling_Messages, GetEntryResponse_size)
{
   GetEntryResponse message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(1, message.size());

   // =============================================================================

   message = GetEntryResponse(Common::Result::OK, Test_entry);
   UNSIGNED_LONGS_EQUAL(7, message.size());
}

TEST(WeeklyScheduling_Messages, GetEntryResponse_pack)
{
   GetEntryResponse message(Common::Result::OK, Test_entry);

   payload  = Common::ByteArray(message.size());

   expected = Test_Entry_payload;
   expected.insert(expected.begin(), Common::Result::OK);

   size = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(7, size);
   CHECK_EQUAL(expected, payload);

   // =============================================================================
   message  = GetEntryResponse(Common::Result::FAIL_ARG);

   payload  = Common::ByteArray(message.size());

   expected = Common::ByteArray {Common::Result::FAIL_ARG};

   size     = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(message.size(), size);
   CHECK_EQUAL(expected, payload);
}

TEST(WeeklyScheduling_Messages, GetEntryResponse_pack_fail)
{
   GetEntryResponse message(Common::Result::OK, Test_entry);

   payload = Common::ByteArray(message.size() - 1);

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

TEST(WeeklyScheduling_Messages, GetEntryResponse_unpack)
{
   GetEntryResponse message;

   payload = Test_Entry_payload;
   payload.insert(payload.begin(), Common::Result::OK);

   size = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(7, size);
   UNSIGNED_LONGS_EQUAL(Common::Result::OK, message.code);
   Do_Test_Entry_Payload(message.entry);
}

TEST(WeeklyScheduling_Messages, GetEntryResponse_unpack_fail)
{
   GetEntryResponse message;

   payload = Test_Entry_payload;
   payload.insert(payload.begin(), Common::Result::OK);
   payload.erase(--payload.end());                          // Remove the last byte

   size = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

// =============================================================================
// Weekly Scheduling Client
// =============================================================================

//! Test Group for Weekly Scheduling Client interface class.
TEST_GROUP(WeeklySchedulingClient)
{
   struct WeeklySchedulingClient: public InterfaceHelper<Scheduling::Weekly::Client>
   {
      WeeklySchedulingClient():
         InterfaceHelper<Scheduling::Weekly::Client>()
      {}
   };

   WeeklySchedulingClient *client;

   Protocol::Address addr;

   TEST_SETUP()
   {
      client = new WeeklySchedulingClient();

      addr   = Protocol::Address(42, 0);

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete client;

      mock().clear();
   }
};

//! @test Activate Scheduler support.
TEST(WeeklySchedulingClient, ActivateScheduler)
{
   mock("Interface").expectOneCall("send");

   client->activate_scheduler(addr, 0x01);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(Scheduling::ACTIVATE_SCHEDULER_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   Scheduling::ActivateScheduler message;

   message.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x01, message.status);
}

//! @test Define Event support.
TEST(WeeklySchedulingClient, DefineEvent)
{
   mock("Interface").expectOneCall("send");
   Day it(SATURDAY, 14, 50);

   client->define_event(addr,
                        0x12,
                        0x01,
                        it,
                        0x21);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(Scheduling::DEFINE_EVENT_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   Scheduling::DefineEvent<Day> message;

   message.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x12, message.id);
   UNSIGNED_LONGS_EQUAL(0x01, message.status);
   UNSIGNED_LONGS_EQUAL(SATURDAY, message.time.day);
   UNSIGNED_LONGS_EQUAL(14, message.time.hour);
   UNSIGNED_LONGS_EQUAL(50, message.time.minute);
   UNSIGNED_LONGS_EQUAL(0x21, message.pid);
}

//! @test Update Event Status support.
TEST(WeeklySchedulingClient, UpdateEventStatus)
{
   mock("Interface").expectOneCall("send");

   client->update_event_status(addr, 0x12, 0x00);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(Scheduling::UPDATE_STATUS_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   Scheduling::UpdateStatus message;
   message.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x12, message.event_id);
   UNSIGNED_LONGS_EQUAL(0x00, message.status);
}

//! @test Get Event Entry support.
TEST(WeeklySchedulingClient, GetEventEntry)
{
   mock("Interface").expectOneCall("send");

   client->get_event_entry(addr, 0x12);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(Scheduling::GET_ENTRY_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   Scheduling::GetEntry message;
   message.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x12, message.event_id);
}

//! @test Delete Event support.
TEST(WeeklySchedulingClient, DeleteEvent)
{
   mock("Interface").expectOneCall("send");

   client->delete_event(addr, 0x12);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(Scheduling::DELETE_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);

   Scheduling::DeleteEvent message;
   message.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x12, message.event_id);
}

//! @test Delete All Events support.
TEST(WeeklySchedulingClient, DeleteAllEvents)
{
   mock("Interface").expectOneCall("send");

   client->delete_all_events(addr);

   mock("Interface").checkExpectations();

   LONGS_EQUAL(HF::Interface::SERVER_ROLE, client->sendMsg.itf.role);
   LONGS_EQUAL(client->uid(), client->sendMsg.itf.id);
   LONGS_EQUAL(Scheduling::DELETE_ALL_CMD, client->sendMsg.itf.member);
   LONGS_EQUAL(Protocol::Message::COMMAND_REQ, client->sendMsg.type);
}

// =============================================================================
// Weekly Scheduling Server
// =============================================================================

//! Test Group for Weekly Scheduling Server interface class.
TEST_GROUP(WeeklySchedulingServer)
{
   struct WeeklySchedulingServer: public Scheduling::Weekly::DefaultServer
   {
      using Parent = Scheduling::Weekly::DefaultServer;

      WeeklySchedulingServer(HF::Core::Unit0 &unit):
         Scheduling::Weekly::DefaultServer(unit)
      {}

      Common::Result activate_scheduler(const Protocol::Packet &packet,
                                        const Scheduling::ActivateScheduler &msg) override
      {
         mock("Scheduling::Weekly::Server").actualCall("activate_scheduler");
         return Parent::activate_scheduler(packet, msg);
      }

      Common::Result define_event(const Protocol::Packet &packet,
                                  Scheduling::DefineEvent<Day> &msg) override
      {
         mock("Scheduling::Weekly::Server").actualCall("define_event");
         return Parent::define_event(packet, msg);
      }

      Common::Result update_event_status(const Protocol::Packet &packet,
                                         const Scheduling::UpdateStatus &msg) override
      {
         mock("Scheduling::Weekly::Server").actualCall("update_event_status");
         return Parent::update_event_status(packet, msg);
      }

      Common::Result get_event_entry(const Protocol::Packet &packet,
                                     const Scheduling::GetEntry &msg) override
      {
         mock("Scheduling::Weekly::Server").actualCall("get_event_entry");
         return Parent::get_event_entry(packet, msg);
      }

      Common::Result delete_event(const Protocol::Packet &packet,
                                  const Scheduling::DeleteEvent &msg) override
      {
         mock("Scheduling::Weekly::Server").actualCall("delete_event");
         return Parent::delete_event(packet, msg);
      }

      Common::Result delete_all_events(const Protocol::Packet &packet) override
      {
         mock("Scheduling::Weekly::Server").actualCall("delete_all_events");
         return Parent::delete_all_events(packet);
      }

      void periodic(uint32_t time) override
      {
         mock("Scheduling::Weekly::Server").actualCall("periodic");
         Parent::periodic(time);
      }
      void notify(const HF::Attributes::IAttribute &old_value,
                  const HF::Attributes::IAttribute &new_value) const override
      {
         mock("Interface").actualCall("notify")
            .withParameterOfType("IAttribute", "old", &old_value)
            .withParameterOfType("IAttribute", "new", &new_value);
      }
   };

   struct BatchProgramServer: public BatchProgramManagement::DefaultServer
   {
      using Parent = BatchProgramManagement::DefaultServer;

      BatchProgramServer(HF::Core::Unit0 &unit):
         Parent(unit)
      {}

      Common::Result invoke_program(const Protocol::Packet &packet,
                                    BatchProgramManagement::InvokeProgram &msg) override
      {
         mock("BatchProgram::Server").actualCall("invoke_program")
            .withParameter("pid", msg.pid)
            .withParameterOfType("Address", "source", &packet.source)
            .withParameterOfType("Address", "dest", &packet.destination);

         return Common::Result::OK;
      }

   };

   struct TimeServer: public Core::Time::Server
   {
      using Parent = Core::Time::Server;

      TimeServer(HF::Core::Unit0 &unit):
         Parent(unit)
      {}

      uint32_t time() const
      {
         mock("Time::Server").actualCall("time");
         return Parent::time();
      }

      void time(uint32_t __value)
      {
         Parent::time(__value);
      }

   };

   Testing::Device *device;
   WeeklySchedulingServer *server;

   BatchProgramServer *batch_server;
   TimeServer *time_server;

   Protocol::Packet packet;
   Common::ByteArray payload;

   Protocol::Address addr;
   Testing::Link link;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      server                    = new WeeklySchedulingServer(*(device->unit0()));

      batch_server = new BatchProgramServer(*(device->unit0()));
      time_server = new TimeServer(*(device->unit0()));

      device->unit0()->batch_program(batch_server);
      device->unit0()->time(time_server);

      time_server->time(0);       // set the system timer to 0.   (Saturday)

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
      delete device;

      //NOTE: The batch_server & time_server are deleted by the "delete device"

      mock().clear();
   }

   Entry GenerateEntry(uint8_t _eid = 0x12,
                       uint8_t _st = 0x01,
                       Day _it = Day(SATURDAY,14,50),
                       uint8_t _pid = 0x21)
   {

      return Entry(_eid, _st, _it, _pid);
   }

   void SeedEntries(uint8_t count = 10)
   {
      for (uint8_t i = 0; i < count; ++i)
      {
         server->entries().save(GenerateEntry(i));
      }
   }
};

//! @test Maximum Number Of Entries support.
TEST(WeeklySchedulingServer, MaximumNumberOfEntries)
{
   LONGS_EQUAL(std::numeric_limits<uint8_t>::max(), server->maximum_number_of_entries());
   CHECK_ATTRIBUTE(WeeklySchedulingServer, MaximumNumberOfEntries, false,
                   maximum_number_of_entries, 42, 142);
}

//! @test Number Of Entries support.
TEST(WeeklySchedulingServer, NumberOfEntries)
{
   auto attr = server->attribute(Scheduling::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::WEEKLY_SCHEDULING, attr->interface());

   POINTERS_EQUAL(server, attr->owner());

   delete attr;

   attr = Core::create_attribute(server, Scheduling::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::WEEKLY_SCHEDULING, attr->interface());

   POINTERS_EQUAL(server, attr->owner());

   delete attr;

   SeedEntries(5);

   LONGS_EQUAL(5, server->number_of_entries());

   server->entries().clear();

   SeedEntries(15);

   LONGS_EQUAL(15, server->number_of_entries());
}

//! @test Status support.
TEST(WeeklySchedulingServer, Status)
{
   UNSIGNED_LONGS_EQUAL(0, server->status());
   CHECK_ATTRIBUTE(WeeklySchedulingServer, Status, false, status, 42, 142);
}

//! @test Activate Scheduler support.
TEST(WeeklySchedulingServer, ActivateScheduler)
{
   server->status(0x00);      // set the initial status of the scheduler.

   Scheduling::ActivateScheduler received(0x01);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it

   packet.message.itf.member = Scheduling::ACTIVATE_SCHEDULER_CMD;
   packet.message.length     = payload.size();

   Status old_value(0, server);
   Status new_value(1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   mock("Scheduling::Weekly::Server").expectOneCall("activate_scheduler");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = Scheduling::ACTIVATE_SCHEDULER_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::ActivateSchedulerResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
}

//! @test Define Event support.
TEST(WeeklySchedulingServer, DefineEvent)
{
   Scheduling::DefineEvent<Day> received = GenerateEntry();
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(0, server);
   NumberOfEntries new_value(1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server->entries().size());       // Check if the new scheduling is on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DefineEventResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   UNSIGNED_LONGS_EQUAL(0x12, resp.event_id);
}

//! @test Define Event support.
TEST(WeeklySchedulingServer, DefineEvent_next_available)
{
   SeedEntries(5);

   uint8_t expected_id = 2;

   server->entries().destroy(expected_id);

   Scheduling::DefineEvent<Day> received = GenerateEntry(Entry::AVAILABLE_ID);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(server->entries().size() + 1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(5, server->entries().size());       // Check if the new scheduling is on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DefineEventResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   UNSIGNED_LONGS_EQUAL(expected_id, resp.event_id);
}

//! @test Define Event support.
TEST(WeeklySchedulingServer, DefineEvent_fail_event_id_in_use)
{
   server->entries().save(GenerateEntry(0x12));

   Scheduling::DefineEvent<Day> received = GenerateEntry(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(server->entries().size() + 1, server);

   mock("Interface").expectNoCall("notify");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(1, server->entries().size());       // Check if we still have 1 scheduling on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DefineEventResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Define Event support.
TEST(WeeklySchedulingServer, DefineEvent_fail_limited_by_atr)
{
   server->maximum_number_of_entries(10);
   SeedEntries(10);

   Scheduling::DefineEvent<Day> received = GenerateEntry(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   mock("Interface").expectNoCall("notify");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_RESOURCES, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   LONGS_EQUAL(10, server->entries().size());       // Check if we still have 10 scheduling on the DB.

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DefineEventResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_RESOURCES, resp.code);
}

//! @test Update Event Status support.
TEST(WeeklySchedulingServer, UpdateEventStatus)
{
   SeedEntries();

   UNSIGNED_LONGS_EQUAL(0x01, server->entries().find(0x03)->status);

   Scheduling::UpdateStatus received(0x03, 0x00);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::UPDATE_STATUS_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("update_event_status");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   UNSIGNED_LONGS_EQUAL(0x00, server->entries().find(0x03)->status);

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::UpdateStatusResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   UNSIGNED_LONGS_EQUAL(0x03, resp.event_id);
}

//! @test Update Event Status support.
TEST(WeeklySchedulingServer, UpdateEventStatus_fail_no_event_id)
{
   SeedEntries();

   server->entries().destroy(0x03);

   Scheduling::UpdateStatus received(0x03, 0x00);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::UPDATE_STATUS_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("update_event_status");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::UpdateStatusResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Get Event Entry support.
TEST(WeeklySchedulingServer, GetEventEntry)
{
   SeedEntries(5);
   Entry TestEntry = GenerateEntry(0x12, 0x00, Day(SATURDAY, 14,50), 0x55);
   server->entries().save(TestEntry);

   Scheduling::GetEntry received(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::GET_ENTRY_CMD;
   packet.message.length     = payload.size();


   mock("Scheduling::Weekly::Server").expectOneCall("get_event_entry");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::GetEntryResponse<Day> resp;
   resp.unpack(response->message.payload);

   /* *INDENT-OFF* */
   LONGS_EQUAL(Common::Result::OK,              resp.code);
   UNSIGNED_LONGS_EQUAL(TestEntry.id,           resp.entry.id);
   UNSIGNED_LONGS_EQUAL(TestEntry.status,       resp.entry.status);
   UNSIGNED_LONGS_EQUAL(TestEntry.time.day,     resp.entry.time.day);
   UNSIGNED_LONGS_EQUAL(TestEntry.time.hour,    resp.entry.time.hour);
   UNSIGNED_LONGS_EQUAL(TestEntry.time.minute,  resp.entry.time.minute);
   UNSIGNED_LONGS_EQUAL(TestEntry.pid,          resp.entry.pid);
   /* *INDENT-ON* */
}

//! @test Get Event Entry support.
TEST(WeeklySchedulingServer, GetEventEntry_fail_no_event_id)
{
   SeedEntries(5);
   Scheduling::GetEntry received(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::GET_ENTRY_CMD;
   packet.message.length     = payload.size();


   mock("Scheduling::Weekly::Server").expectOneCall("get_event_entry");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::GetEntryResponse<Day> resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Delete Event support.
TEST(WeeklySchedulingServer, DeleteEvent)
{
   SeedEntries();

   Scheduling::DeleteEvent received(0x5);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DELETE_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("delete_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(server->entries().size() - 1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DeleteEventResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
   UNSIGNED_LONGS_EQUAL(0x5, resp.event_id);
}

//! @test Delete Event support.
TEST(WeeklySchedulingServer, DeleteEvent_fail_no_event_id)
{
   SeedEntries();

   Scheduling::DeleteEvent received(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DELETE_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Weekly::Server").expectOneCall("delete_event");
   mock("AbstractDevice").expectOneCall("send");
   mock("Interface").expectNoCall("notify");

   CHECK_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DeleteEventResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Delete All Events support.
TEST(WeeklySchedulingServer, DeleteAllEvents)
{
   SeedEntries();

   mock("Scheduling::Weekly::Server").expectOneCall("delete_all_events");

   packet.message.itf.member = Scheduling::DELETE_ALL_CMD;

   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(0, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();
   mock("Interface").checkExpectations();

   UNSIGNED_LONGS_EQUAL(0, server->entries().size());

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::DeleteAllResponse resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::OK, resp.code);
}

////! @test Periodic single entry.
TEST(WeeklySchedulingServer, Periodic_Single_Entry)
{
   server->entries().save(Entry(0x1, 0x01, Day(SATURDAY, 14,50), 0x01));

   mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
   mock("Scheduling::Weekly::Server").expectOneCall("periodic");
   mock("Time::Server").expectOneCall("time");

   mock("BatchProgram::Server").expectNoCall("invoke_program");

   server->periodic(0x11);

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();

   time_server->tick(15*(60*60));   // + 15H

   for (uint8_t i = 1; i < 0x0F; ++i)
   {
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Weekly::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
         .withParameterOfType("Address", "source", &addr)
         .withParameterOfType("Address", "dest", &addr);

      time_server->tick(Day::WEEK);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}

////! @test Periodic two entries overlap.
TEST(WeeklySchedulingServer, Periodic_two_Entries_overlap)
{
   server->entries().save(Entry(0x1, 0x01, Day(SATURDAY, 14,50), 0x01));
   server->entries().save(Entry(0x2, 0x01, Day(SATURDAY, 14,50), 0x02));

   mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
   mock("Scheduling::Weekly::Server").expectOneCall("periodic");
   mock("Time::Server").expectOneCall("time");

   mock("BatchProgram::Server").expectNoCall("invoke_program");

   server->periodic(0x11);

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();

   time_server->tick(15*(60*60));   // + 15H

   for (uint8_t i = 1; i < 0x0F; ++i)
   {
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Weekly::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
         .withParameterOfType("Address", "source", &addr)
         .withParameterOfType("Address", "dest", &addr);

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
               .withParameterOfType("Address", "source", &addr)
               .withParameterOfType("Address", "dest", &addr);

      time_server->tick(Day::WEEK);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}

////! @test Periodic tow entries - disable one during the test.
TEST(WeeklySchedulingServer, Periodic_two_Entries_disable_one)
{
   server->entries().save(Entry(0x1, 0x01, Day(SATURDAY, 14,50), 0x01));
   server->entries().save(Entry(0x2, 0x01, Day(SATURDAY, 14,50), 0x02));

   mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
   mock("Scheduling::Weekly::Server").expectOneCall("periodic");
   mock("Time::Server").expectOneCall("time");

   mock("BatchProgram::Server").expectNoCall("invoke_program");

   server->periodic(0x11);

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();

   time_server->tick(15*(60*60));   // + 15H

   for (uint8_t i = 1; i < 0x0F; ++i)
   {
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Weekly::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
         .withParameterOfType("Address", "source", &addr)
         .withParameterOfType("Address", "dest", &addr);

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
               .withParameterOfType("Address", "source", &addr)
               .withParameterOfType("Address", "dest", &addr);

      time_server->tick(Day::WEEK);
      server->periodic(0x11);
   }

   server->entries().find(0x01)->status=0x00; //Disable 1st entry

   for (uint8_t i = 1; i < 0x0F; ++i)
   {
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Weekly::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);

      time_server->tick(Day::WEEK);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}

////! @test Periodic two entries no overlap.
TEST(WeeklySchedulingServer, Periodic_two_Entries_no_overlap)
{
   server->entries().save(Entry(0x1, 0x01, Day(SATURDAY, 14,50), 0x01));
   server->entries().save(Entry(0x2, 0x01, Day(SUNDAY, 14,50), 0x02));

   mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
   mock("Scheduling::Weekly::Server").expectOneCall("periodic");
   mock("Time::Server").expectOneCall("time");

   mock("BatchProgram::Server").expectNoCall("invoke_program");

   server->periodic(0x11);

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Weekly::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();

   time_server->tick(15*(60*60));   // + 15H

   for (uint8_t i = 1; i < 0x0F; ++i)
   {
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Weekly::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);

      server->periodic(0x11);

      mock("AbstractDevice").checkExpectations();
      mock("Scheduling::Weekly::Server").checkExpectations();
      mock("BatchProgram::Server").checkExpectations();
      mock("Time::Server").checkExpectations();

      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Weekly::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);

      time_server->tick(Day::DAY);           // + 1 DAY
      server->periodic(0x11);

      mock("AbstractDevice").checkExpectations();
      mock("Scheduling::Weekly::Server").checkExpectations();
      mock("BatchProgram::Server").checkExpectations();
      mock("Time::Server").checkExpectations();

      time_server->tick(6 * Day::DAY);     // go to the next Saturday
   }

}
