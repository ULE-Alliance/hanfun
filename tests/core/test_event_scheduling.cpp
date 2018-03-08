// =============================================================================
/*!
 * @file       tests/core/test_event_scheduling.cpp
 *
 * This is file contains the unit tests for the Event Scheduling service
 * implementation.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================
#include "hanfun/core/event_scheduling.h"
#include "hanfun/core/batch_program_management.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Common;

using namespace HF::Testing;

using namespace HF::Core::Scheduling::Event;

// =============================================================================
// Event Scheduling
// =============================================================================

//! Test Group for Scheduling::Event service parent class.
TEST_GROUP(Scheduling_Event)
{
   struct EventSchedulingBase: public InterfaceHelper<Scheduling::Event::IServerBase>
   {
      EventSchedulingBase(HF::Core::Unit0 &unit):
         InterfaceHelper<Scheduling::Event::IServerBase>(unit)
      {}

      uint8_t number_of_entries() const override
      {
         return mock("EventSchedulingBase").actualCall("number_of_entries")
                   .returnIntValueOrDefault(0);
      }
   };

   Testing::Device *device;
   EventSchedulingBase *service;

   TEST_SETUP()
   {
      device  = new Testing::Device();
      service = new EventSchedulingBase(*(device->unit0()));

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete service;
      delete device;

      mock().clear();
   }
};

//! @test Scheduling::Event::uid should return @c HF::Interface::EVENT_SCHEDULING.
TEST(Scheduling_Event, UID)
{
   LONGS_EQUAL(HF::Interface::EVENT_SCHEDULING, service->uid());
}

//! @test Maximum Number Of Entries support.
TEST(Scheduling_Event, MaximumNumberOfEntries)
{
   HF::Attributes::IAttribute *attr = Scheduling::Event::create_attribute(
      Scheduling::MAXIMUM_NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(MaximumNumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(MaximumNumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::EVENT_SCHEDULING, attr->interface());

   delete attr;
}

//! @test Number Of Entries support.
TEST(Scheduling_Event, NumberOfEntries)
{
   HF::Attributes::IAttribute *attr = Scheduling::Event::create_attribute(
      Scheduling::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::EVENT_SCHEDULING, attr->interface());

   delete attr;
}

//! @test Status support.
TEST(Scheduling_Event, Status)
{
   HF::Attributes::IAttribute *attr = Scheduling::Event::create_attribute(
      Scheduling::STATUS_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(Status::ID, attr->uid());
   CHECK_EQUAL(Status::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::EVENT_SCHEDULING, attr->interface());

   delete attr;
}

//! @test Check nullptr return for invalid attribute
TEST(Scheduling_Event, InvalidAttribute)
{
   HF::Attributes::IAttribute *attr = Scheduling::Event::create_attribute(
      Scheduling::__LAST_ATTR__ + 1);

   CHECK_TRUE(attr == nullptr);
}

// =============================================================================
// Event Scheduling - Entries
// =============================================================================

//! Test Group for GroupManagement service parent class.
TEST_GROUP(EventSchedulingEntries)
{
   using Entries = Scheduling::Entries<Interval>;

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

      for (uint8_t id = Entry::START_ID; id < number - Entry::START_ID; ++id)
      {
         event.id     = id;
         event.status = 0x01;
         event.pid    = id;
         data.push_front(event);
      }
   }
};

//! @test Entries next address
TEST(EventSchedulingEntries, Next_id)
{
   LONGS_EQUAL(Entry::START_ID, entries.next_id());   // 1st address

   IssueEvents(Entry::MAX_ID + 1);

   LONGS_EQUAL(Entry::AVAILABLE_ID, entries.next_id());

   // erase entry 2
   UNSIGNED_LONGS_EQUAL(Result::OK, entries.destroy(2));

   // check if the next available address is 2
   LONGS_EQUAL(2, entries.next_id());

   // restore it
   Entry event;
   event.id = 2;
   data.push_front(event);

   LONGS_EQUAL(Entry::AVAILABLE_ID, entries.next_id());
}

//! @test Entries find by address
TEST(EventSchedulingEntries, Find_by_id)
{
   // Create 10 Events
   IssueEvents(10);

   Entry event;
   event.id  = 2;
   event.pid = 0x22;

   // erase entry 2
   UNSIGNED_LONGS_EQUAL(Result::OK, entries.destroy(2));

   // Try to find group 2 (should fail)
   POINTERS_EQUAL(nullptr, entries.find(2).operator->());

   // restore it
   data.push_front(event);

   // Try to find group 2 (OK)
   CHECK_TRUE(nullptr != entries.find(2).operator->());
   UNSIGNED_LONGS_EQUAL(0x22, entries.find(2)->pid);
}

//! @test Entries size
TEST(EventSchedulingEntries, Size)
{
   LONGS_EQUAL(0, entries.size());

   // Create 10 events
   IssueEvents(10);

   LONGS_EQUAL(10, entries.size());
}

//! @test Entries destroy by id
TEST(EventSchedulingEntries, Destroy_by_address)
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
TEST(EventSchedulingEntries, Destroy_by_group)
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
TEST(EventSchedulingEntries, Save_OK)
{
   LONGS_EQUAL(0, entries.size());

   Entry event;
   event.id  = 1;
   event.pid = 11;
   entries.save(event);

   UNSIGNED_LONGS_EQUAL(1, entries.size());
   UNSIGNED_LONGS_EQUAL(11, data.begin()->pid);
}

TEST(EventSchedulingEntries, Save_Duplicate)
{
   LONGS_EQUAL(0, entries.size());

   Entry event;
   event.id  = 1;
   event.pid = 11;
   UNSIGNED_LONGS_EQUAL(Result::OK, entries.save(event));

   UNSIGNED_LONGS_EQUAL(Result::FAIL_ARG, entries.save(event));
}

// =============================================================================
// Event Scheduling messages
// =============================================================================

TEST_GROUP(EventScheduling_Messages)
{
   Common::ByteArray payload, expected, Test_Entry_payload;
   uint16_t size;

   Entry Test_entry;

   void Do_Test_Entry_Payload(Entry &message)
   {
      UNSIGNED_LONGS_EQUAL(0x11, message.id);
      UNSIGNED_LONGS_EQUAL(0x01, message.status);
      UNSIGNED_LONGS_EQUAL(0x11111111, message.time.start);
      UNSIGNED_LONGS_EQUAL(0x22222222, message.time.end);
      UNSIGNED_LONGS_EQUAL(0x33333333, message.time.repeat);
      UNSIGNED_LONGS_EQUAL(0x12, message.pid);
   }

   TEST_SETUP()
   {
      size               = 0;
      Test_entry         = Entry(0x11, 0x01, Interval(0x11111111, 0x22222222, 0x33333333), 0x12);
      Test_Entry_payload = Common::ByteArray {0x11,                   // Event ID
                                              0x80,                   // Status 0x01 << 7
                                              0x11, 0x11, 0x11, 0x11, // Start Date
                                              0x22, 0x22, 0x22, 0x22, // End Date
                                              0x33, 0x33, 0x33, 0x33, // Interval
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

TEST(EventScheduling_Messages, Entry_size)
{
   Entry message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(15, message.size());
}

TEST(EventScheduling_Messages, Entry_pack)
{
   Entry message = Test_entry;

   payload = Common::ByteArray(message.size());

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(15, size);
   CHECK_EQUAL(Test_Entry_payload, payload);
}

TEST(EventScheduling_Messages, Entry_unpack)
{
   Entry message;

   payload = Test_Entry_payload;

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(15, size);

   Do_Test_Entry_Payload(message);
}

TEST(EventScheduling_Messages, Entry_pack_fail_no_size)
{
   Entry message = Test_entry;

   payload = Common::ByteArray(message.size() - 1);

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

TEST(EventScheduling_Messages, Entry_unpack_fail)
{
   Entry message;

   payload = Common::ByteArray {0x11,                   // Event ID
                                0x80,                   // Status 0x01 << 7
                                0x11, 0x11, 0x11, 0x11, // Start Date
                                0x22, 0x22, 0x22, 0x22, // End Date
                                0x33, 0x33, 0x33, 0x33, // Interval
                                                        // Program ID.   (missing)
   };

   size = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

// ========== Get Entry Response ==========

TEST(EventScheduling_Messages, GetEntryResponse_size)
{
   GetEntryResponse message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(1, message.size());

   // =============================================================================

   message = GetEntryResponse(Common::Result::OK, Test_entry);
   UNSIGNED_LONGS_EQUAL(16, message.size());
}

TEST(EventScheduling_Messages, GetEntryResponse_pack)
{
   GetEntryResponse message(Common::Result::OK, Test_entry);

   payload  = Common::ByteArray(message.size());

   expected = Test_Entry_payload;
   expected.insert(expected.begin(), Common::Result::OK);

   size = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(16, size);
   CHECK_EQUAL(expected, payload);

   // =============================================================================
   message  = GetEntryResponse(Common::Result::FAIL_ARG);

   payload  = Common::ByteArray(message.size());

   expected = Common::ByteArray {Common::Result::FAIL_ARG};

   size     = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(message.size(), size);
   CHECK_EQUAL(expected, payload);
}

TEST(EventScheduling_Messages, GetEntryResponse_pack_fail)
{
   GetEntryResponse message(Common::Result::OK, Test_entry);

   payload = Common::ByteArray(message.size() - 1);

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

TEST(EventScheduling_Messages, GetEntryResponse_unpack)
{
   GetEntryResponse message;

   payload = Test_Entry_payload;
   payload.insert(payload.begin(), Common::Result::OK);

   size = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(16, size);
   UNSIGNED_LONGS_EQUAL(Common::Result::OK, message.code);
   Do_Test_Entry_Payload(message.entry);
}

TEST(EventScheduling_Messages, GetEntryResponse_unpack_fail)
{
   GetEntryResponse message;

   payload = Test_Entry_payload;
   payload.insert(payload.begin(), Common::Result::OK);
   payload.erase(--payload.end());                          // Remove the last byte

   size = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

// =============================================================================
// Event Scheduling Client
// =============================================================================

//! Test Group for Event Scheduling Client interface class.
TEST_GROUP(EventSchedulingClient)
{
   struct EventSchedulingClient: public InterfaceHelper<Scheduling::Event::Client>
   {
      EventSchedulingClient():
         InterfaceHelper<Scheduling::Event::Client>()
      {}
   };

   EventSchedulingClient *client;

   Protocol::Address addr;

   TEST_SETUP()
   {
      client = new EventSchedulingClient();

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
TEST(EventSchedulingClient, ActivateScheduler)
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
TEST(EventSchedulingClient, DefineEvent)
{
   mock("Interface").expectOneCall("send");
   Interval it(0x1, 0xFFFFFFFF, 0x1111);

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

   Scheduling::Entry<Interval> message;

   message.unpack(client->sendMsg.payload);

   UNSIGNED_LONGS_EQUAL(0x12, message.id);
   UNSIGNED_LONGS_EQUAL(0x01, message.status);
   UNSIGNED_LONGS_EQUAL(0x01, message.time.start);
   UNSIGNED_LONGS_EQUAL(0xFFFFFFFF, message.time.end);
   UNSIGNED_LONGS_EQUAL(0x1111, message.time.repeat);
   UNSIGNED_LONGS_EQUAL(0x21, message.pid);
}

//! @test Update Event Status support.
TEST(EventSchedulingClient, UpdateEventStatus)
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
TEST(EventSchedulingClient, GetEventEntry)
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
TEST(EventSchedulingClient, DeleteEvent)
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
TEST(EventSchedulingClient, DeleteAllEvents)
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
// Event Scheduling Server
// =============================================================================

//! Test Group for Event Scheduling Server interface class.
TEST_GROUP(EventSchedulingServer)
{
   struct EventSchedulingServer: public Scheduling::Event::DefaultServer
   {
      using Parent = Scheduling::Event::DefaultServer;

      EventSchedulingServer(HF::Core::Unit0 &unit):
         Scheduling::Event::DefaultServer(unit)
      {}

      Common::Result activate_scheduler(const Protocol::Packet &packet,
                                        const Scheduling::ActivateScheduler &msg) override
      {
         mock("Scheduling::Event::Server").actualCall("activate_scheduler");
         return Parent::activate_scheduler(packet, msg);
      }

      Common::Result define_event(const Protocol::Packet &packet,
                                  Scheduling::Entry<Interval> &msg) override
      {
         mock("Scheduling::Event::Server").actualCall("define_event");
         return Parent::define_event(packet, msg);
      }

      Common::Result update_event_status(const Protocol::Packet &packet,
                                         const Scheduling::UpdateStatus &msg) override
      {
         mock("Scheduling::Event::Server").actualCall("update_event_status");
         return Parent::update_event_status(packet, msg);
      }

      Common::Result get_event_entry(const Protocol::Packet &packet,
                                     const Scheduling::GetEntry &msg) override
      {
         mock("Scheduling::Event::Server").actualCall("get_event_entry");
         return Parent::get_event_entry(packet, msg);
      }

      Common::Result delete_event(const Protocol::Packet &packet,
                                  const Scheduling::DeleteEvent &msg) override
      {
         mock("Scheduling::Event::Server").actualCall("delete_event");
         return Parent::delete_event(packet, msg);
      }

      Common::Result delete_all_events(const Protocol::Packet &packet) override
      {
         mock("Scheduling::Event::Server").actualCall("delete_all_events");
         return Parent::delete_all_events(packet);
      }

      void periodic(uint32_t time) override
      {
         mock("Scheduling::Event::Server").actualCall("periodic");
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
   EventSchedulingServer *server;

   Protocol::Packet packet;
   Common::ByteArray payload;

   Protocol::Address addr;
   Testing::Link link;

   TEST_SETUP()
   {
      device                    = new Testing::Device();
      server                    = new EventSchedulingServer(*(device->unit0()));

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

      mock().clear();
   }

   Entry GenerateEntry(uint8_t _eid = 0x12,
                       uint8_t _st = 0x01,
                       Interval _it = Interval(0x1, 0xFFFFFFFF, 0x1111),
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
TEST(EventSchedulingServer, MaximumNumberOfEntries)
{
   LONGS_EQUAL(std::numeric_limits<uint8_t>::max(), server->maximum_number_of_entries());
   CHECK_ATTRIBUTE(EventSchedulingServer, MaximumNumberOfEntries, false,
                   maximum_number_of_entries, 42, 142);
}

//! @test Number Of Entries support.
TEST(EventSchedulingServer, NumberOfEntries)
{
   auto attr = server->attribute(Scheduling::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::EVENT_SCHEDULING, attr->interface());

   POINTERS_EQUAL(server, attr->owner());

   delete attr;

   attr = Core::create_attribute(server, Scheduling::NUMBER_OF_ENTRIES_ATTR);

   CHECK_TRUE(attr != nullptr);

   LONGS_EQUAL(NumberOfEntries::ID, attr->uid());
   CHECK_EQUAL(NumberOfEntries::WRITABLE, attr->isWritable());
   LONGS_EQUAL(HF::Interface::EVENT_SCHEDULING, attr->interface());

   POINTERS_EQUAL(server, attr->owner());

   delete attr;

   SeedEntries(5);

   LONGS_EQUAL(5, server->number_of_entries());

   server->entries().clear();

   SeedEntries(15);

   LONGS_EQUAL(15, server->number_of_entries());
}

//! @test Status support.
TEST(EventSchedulingServer, Status)
{
   CHECK_TRUE(server->status());
   CHECK_ATTRIBUTE(EventSchedulingServer, Status, false, status, false, true);
}

//! @test Activate Scheduler support.
TEST(EventSchedulingServer, ActivateScheduler)
{
   server->status(false);      // set the initial status of the scheduler.

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

   mock("Scheduling::Event::Server").expectOneCall("activate_scheduler");
   mock("AbstractDevice").expectOneCall("send");

   packet.message.itf.member = Scheduling::ACTIVATE_SCHEDULER_CMD;

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, DefineEvent)
{
   Scheduling::Entry<Interval> received = GenerateEntry();
   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(0, server);
   NumberOfEntries new_value(1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, DefineEvent_next_available)
{
   SeedEntries(5);

   uint8_t expected_id = 2;

   server->entries().destroy(expected_id);

   Scheduling::Entry<Interval> received = GenerateEntry(Entry::AVAILABLE_ID);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(server->entries().size() + 1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, DefineEvent_fail_event_id_in_use)
{
   server->entries().save(GenerateEntry(0x12));

   Scheduling::Entry<Interval> received = GenerateEntry(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(server->entries().size() + 1, server);

   mock("Interface").expectNoCall("notify");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, DefineEvent_fail_limited_by_atr)
{
   server->maximum_number_of_entries(10);
   SeedEntries(10);

   Scheduling::Entry<Interval> received = GenerateEntry(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DEFINE_EVENT_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("define_event");
   mock("AbstractDevice").expectOneCall("send");

   mock("Interface").expectNoCall("notify");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_RESOURCES, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, UpdateEventStatus)
{
   SeedEntries();

   UNSIGNED_LONGS_EQUAL(0x01, server->entries().find(0x03)->status);

   Scheduling::UpdateStatus received(0x03, 0x00);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::UPDATE_STATUS_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("update_event_status");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, UpdateEventStatus_fail_no_event_id)
{
   SeedEntries();

   server->entries().destroy(0x03);

   Scheduling::UpdateStatus received(0x03, 0x00);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::UPDATE_STATUS_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("update_event_status");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, GetEventEntry)
{
   SeedEntries(5);
   Entry TestEntry = GenerateEntry(0x12, 0x00, Interval(0x1, 0xFF, 0x01), 0x55);
   server->entries().save(TestEntry);

   Scheduling::GetEntry received(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::GET_ENTRY_CMD;
   packet.message.length     = payload.size();


   mock("Scheduling::Event::Server").expectOneCall("get_event_entry");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::GetEntryResponse<Interval> resp;
   resp.unpack(response->message.payload);

   /* *INDENT-OFF* */
   LONGS_EQUAL(Common::Result::OK,              resp.code);
   UNSIGNED_LONGS_EQUAL(TestEntry.id,           resp.entry.id);
   UNSIGNED_LONGS_EQUAL(TestEntry.status,       resp.entry.status);
   UNSIGNED_LONGS_EQUAL(TestEntry.time.start,   resp.entry.time.start);
   UNSIGNED_LONGS_EQUAL(TestEntry.time.end,     resp.entry.time.end);
   UNSIGNED_LONGS_EQUAL(TestEntry.time.repeat,  resp.entry.time.repeat);
   UNSIGNED_LONGS_EQUAL(TestEntry.pid,          resp.entry.pid);
   /* *INDENT-ON* */
}

//! @test Get Event Entry support.
TEST(EventSchedulingServer, GetEventEntry_fail_no_event_id)
{
   SeedEntries(5);
   Scheduling::GetEntry received(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::GET_ENTRY_CMD;
   packet.message.length     = payload.size();


   mock("Scheduling::Event::Server").expectOneCall("get_event_entry");
   mock("AbstractDevice").expectOneCall("send");

   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
   mock("AbstractDevice").checkExpectations();

   // Check response packet destination address.
   LONGS_EQUAL(1, device->packets.size());

   Protocol::Packet *response = device->packets.back();

   CHECK_TRUE(response != nullptr);

   LONGS_EQUAL(42, response->destination.device);
   LONGS_EQUAL(0, response->destination.unit);
   LONGS_EQUAL(Protocol::Address::DEVICE, response->destination.mod);

   // ----- Check the response message -----

   Scheduling::GetEntryResponse<Interval> resp;
   resp.unpack(response->message.payload);
   LONGS_EQUAL(Common::Result::FAIL_ARG, resp.code);
}

//! @test Delete Event support.
TEST(EventSchedulingServer, DeleteEvent)
{
   SeedEntries();

   Scheduling::DeleteEvent received(0x5);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DELETE_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("delete_event");
   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(server->entries().size() - 1, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, DeleteEvent_fail_no_event_id)
{
   SeedEntries();

   Scheduling::DeleteEvent received(0x12);

   payload = ByteArray(received.size());

   received.pack(payload);    // pack it
   packet.message.itf.member = Scheduling::DELETE_CMD;
   packet.message.length     = payload.size();

   mock("Scheduling::Event::Server").expectOneCall("delete_event");
   mock("AbstractDevice").expectOneCall("send");
   mock("Interface").expectNoCall("notify");

   CHECK_EQUAL(Common::Result::FAIL_ARG, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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
TEST(EventSchedulingServer, DeleteAllEvents)
{
   SeedEntries();

   mock("Scheduling::Event::Server").expectOneCall("delete_all_events");

   packet.message.itf.member = Scheduling::DELETE_ALL_CMD;

   mock("AbstractDevice").expectOneCall("send");

   NumberOfEntries old_value(server->entries().size(), server);
   NumberOfEntries new_value(0, server);

   mock("Interface").expectOneCall("notify")
      .withParameterOfType("IAttribute", "old", &old_value)
      .withParameterOfType("IAttribute", "new", &new_value);

   CHECK_EQUAL(Common::Result::OK, server->handle(packet, payload, 0));

   mock("Scheduling::Event::Server").checkExpectations();
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

//! @test Periodic single entry.
TEST(EventSchedulingServer, Periodic_Single_Entry)
{
   BatchProgramServer *batch_server;
   TimeServer *time_server;

   batch_server = new BatchProgramServer(*(device->unit0()));
   time_server  = new TimeServer(*(device->unit0()));

   device->unit0()->batch_program(batch_server);
   device->unit0()->time(time_server);

   time_server->time(0);       // set the system timer to 0.

   server->entries().save(Entry(0x1, 0x01, Interval(0x11, 0xFF, 0x11), 0x01));

   for (uint8_t i = 1; i < 0x0F; ++i)
   {
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Scheduling::Event::Server").expectOneCall("periodic");
      mock("Time::Server").expectOneCall("time");

      mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
         .withParameterOfType("Address", "source", &addr)
         .withParameterOfType("Address", "dest", &addr);

      time_server->tick(0x11);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Event::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}

//! @test Periodic two entries - no overlap.
TEST(EventSchedulingServer, Periodic_two_Entries_no_overlap)
{
   BatchProgramServer *batch_server;
   TimeServer *time_server;

   batch_server = new BatchProgramServer(*(device->unit0()));
   time_server  = new TimeServer(*(device->unit0()));

   device->unit0()->batch_program(batch_server);
   device->unit0()->time(time_server);

   time_server->time(0);       // set the system timer to 0.

   server->entries().save(Entry(0x1, 0x01, Interval(0x11, 0xFF, 0x11), 0x01));
   server->entries().save(Entry(0x2, 0x01, Interval(0x110, 0x01FF, 0x11), 0x02));

   for (uint16_t i = 0x11; i <= 0x1FF; i += 0x11)
   {
      if (i <= 0xFF)                      // 1st entry
      {
         mock("Scheduling::Event::Server").expectOneCall("periodic");
         mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
         mock("Time::Server").expectOneCall("time");

         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }
      else if (0xFF < i && i < 0x110)     // GAP between entries
      {
         mock("Scheduling::Event::Server").expectOneCall("periodic");
         mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
         mock("Time::Server").expectOneCall("time");

         mock("BatchProgram::Server").expectNoCall("invoke_program");
      }
      else                                // 2nd entry
      {
         mock("Scheduling::Event::Server").expectOneCall("periodic");
         mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
         mock("Time::Server").expectOneCall("time");

         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }


      time_server->tick(0x11);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Event::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}

//! @test Periodic two entries - single entry overlap.
TEST(EventSchedulingServer, Periodic_two_Entries_single_overlap)
{
   BatchProgramServer *batch_server;
   TimeServer *time_server;

   batch_server = new BatchProgramServer(*(device->unit0()));
   time_server  = new TimeServer(*(device->unit0()));

   device->unit0()->batch_program(batch_server);
   device->unit0()->time(time_server);

   time_server->time(0);       // set the system timer to 0.

   server->entries().save(Entry(0x1, 0x01, Interval(0x11, 0xFF, 0x11), 0x01));
   server->entries().save(Entry(0x2, 0x01, Interval(0xFF, 0x01FF, 0x11), 0x02));

   for (uint16_t i = 0x11; i <= 0x01FF; i += 0x11)
   {
      mock("Scheduling::Event::Server").expectOneCall("periodic");
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Time::Server").expectOneCall("time");

      if (i < 0xFF)                       // 1st entry
      {
         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }

      if (i == 0xFF)                      // overlap entries
      {
         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);

         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }

      if (i > 0xFF)                       // 2nd entry
      {
         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }


      time_server->tick(0x11);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Event::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}

//! @test Periodic two entries - multiple entries overlap.
TEST(EventSchedulingServer, Periodic_two_Entries_multiple_overlap)
{
   BatchProgramServer *batch_server;
   TimeServer *time_server;

   batch_server = new BatchProgramServer(*(device->unit0()));
   time_server  = new TimeServer(*(device->unit0()));

   device->unit0()->batch_program(batch_server);
   device->unit0()->time(time_server);

   time_server->time(0);       // set the system timer to 0.

   server->entries().save(Entry(0x1, 0x01, Interval(0x11, 0xFF, 0x11), 0x01));
   server->entries().save(Entry(0x2, 0x01, Interval(0xAA, 0x01FF, 0x11), 0x02));

   for (uint16_t i = 0x11; i <= 0x01FF; i += 0x11)
   {
      mock("Scheduling::Event::Server").expectOneCall("periodic");
      mock("AbstractDevice").expectOneCall("address").andReturnValue(addr.device);
      mock("Time::Server").expectOneCall("time");

      if (i <= 0xFF)                       // 1st entry
      {
         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x01)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }

      if (0xAA <= i && i <= 0x01FF)       // 2nd entry
      {
         mock("BatchProgram::Server").expectOneCall("invoke_program").withParameter("pid", 0x02)
            .withParameterOfType("Address", "source", &addr)
            .withParameterOfType("Address", "dest", &addr);
      }


      time_server->tick(0x11);
      server->periodic(0x11);
   }

   mock("AbstractDevice").checkExpectations();
   mock("Scheduling::Event::Server").checkExpectations();
   mock("BatchProgram::Server").checkExpectations();
   mock("Time::Server").checkExpectations();
}
