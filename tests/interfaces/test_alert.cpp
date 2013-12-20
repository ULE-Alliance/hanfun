// =============================================================================
/*!
 * \file       tests/interface/test_alert.cpp
 *
 * This is file contains the unit tests for the Alert Interface implementation.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#include <string>
#include <sstream>

#include "hanfun/interfaces/alert.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// Alert
// =============================================================================

//! Test Group for Alert interface parent class.
TEST_GROUP (Alert)
{
   class TestAlert:public Testing::InterfaceParentHelper <Alert>
   {};

   TestAlert interface;
};

//! \test Alert::uid should return \c Interface::ALERT.
TEST (Alert, UID)
{
   CHECK_EQUAL (Interface::ALERT, interface.uid ());
}

// =============================================================================
// Alert / Messages
// =============================================================================

//! Alert::Message test group.
TEST_GROUP (AlertMessage)
{
   Alert::Message *message;

   ByteArray expected;

   TEST_SETUP ()
   {
      message = new Alert::Message ();

      uint8_t data[] = {0x00, 0x00, 0x00,
                        0x5A, 0xA5,            // Unit Type.
                        0xFF, 0xA5, 0x5A,0xBB, // State.
                        0x00, 0x00, 0x00};
      expected = ByteArray (data, sizeof(data));
   }

   TEST_TEARDOWN ()
   {
      delete message;
   }

};

//! \test Alert::Message::size should return the correct value.
TEST (AlertMessage, Size)
{
   LONGS_EQUAL (6, message->size ());

   LONGS_EQUAL (0, message->type);
   LONGS_EQUAL (0, message->state);
}

//! \test Alert::Message::pack should write the correct values to the ByteArray.
TEST (AlertMessage, Pack)
{
   message->type  = 0x5AA5;
   message->state = 0xFFA55ABB;

   ByteArray array (message->size () + 6);

   size_t    wsize = message->pack (array, 3);

   LONGS_EQUAL (message->size (), wsize);

   CHECK_EQUAL (expected, array);
}

//! \test Alert::Message::unpack should read the correct values from the ByteArray.
TEST (AlertMessage, Unpack)
{
   size_t rsize = message->unpack (expected, 3);

   LONGS_EQUAL (message->size (), rsize);

   LONGS_EQUAL (0x5AA5, message->type);
   LONGS_EQUAL (0xFFA55ABB, message->state);
}

// =============================================================================
// AlertServer
// =============================================================================

//! Test Group for AlertServer interface class.
TEST_GROUP (AlertServer)
{
   struct TestAlertServer:public Testing::InterfaceHelper <AlertServer>
   {
      using AlertServer::status;
   };

   TestAlertServer *server;

   TEST_SETUP ()
   {
      server = new TestAlertServer ();
      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      delete server;
      mock ().clear ();
   }

   void check_alarm (bool expected, bool state, uint32_t index, const char *fileName, int lineNumber)
   {
      if (state != expected)
      {
         std::ostringstream error;
         error << "Alarm at index " << index << std::endl
               << "\t\t expected : " << expected << std::endl
               << "\t\t but was  : " << state;
         FAIL_TEST_LOCATION (error.str ().c_str (), fileName, lineNumber);
      }
   }

};

#define CHECK_ALARM(_expected, _state, _index) \
   check_alarm (_expected, _state, _index, __FILE__, __LINE__)

//! \test Should disable all alarms.
TEST (AlertServer, DisableAll)
{
   CHECK_EQUAL (UINT32_MAX, server->enabled ());
   server->disableAll ();
   CHECK_EQUAL (0, server->enabled ());
}

//! \test Should enable all alarms.
TEST (AlertServer, EnableAll)
{
   server->disableAll ();
   server->enableAll ();
   CHECK_EQUAL (UINT32_MAX, server->enabled ());
}

//! \test Should enable only the selected alarm.
TEST (AlertServer, Enable)
{
   server->disableAll ();

   CHECK_FALSE (server->enabled (42));

   for (int i = 0; i < 32; i++)
   {
      server->enable (i);

      CHECK_ALARM (true, server->enabled (i), i);
      CHECK_ALARM (false, server->disabled (i), i);

      for (int j = 0; j < 32; j++)
      {
         if (j != i)
         {
            CHECK_ALARM (false, server->enabled (j), j);
            CHECK_ALARM (true, server->disabled (j), j);
         }
      }

      server->disable (i);
   }
}

//! \test Should disable only the selected alarm.
TEST (AlertServer, Disable)
{
   server->enableAll ();

   CHECK_TRUE (server->disabled (42));

   for (int i = 0; i < 32; i++)
   {
      server->disable (i);

      CHECK_ALARM (false, server->enabled (i), i);
      CHECK_ALARM (true, server->disabled (i), i);

      for (int j = 0; j < 32; j++)
      {
         if (j != i)
         {
            CHECK_ALARM (true, server->enabled (j), j);
            CHECK_ALARM (false, server->disabled (j), j);
         }
      }

      server->enable (i);
   }
}

//! \test Should change the alarm state correctly.
TEST (AlertServer, State)
{
   CHECK_FALSE (server->state (42, true));

   CHECK_FALSE (server->state (42));

   CHECK_EQUAL (0, server->state ());

   // Check individual assignment.
   for (int i = 0; i < 32; i++)
   {
      CHECK_ALARM (true, server->state (i, true), i);
      CHECK_ALARM (true, server->state (i), i);

      for (int j = 0; j < 32; j++)
      {
         if (i != j)
         {
            CHECK_ALARM (false, server->state (j), j);
         }
      }

      CHECK_TRUE (server->state (i, false));
      CHECK_FALSE (server->state (i));
   }

   // Check clear.
   for (int i = 0; i < 32; i++)
   {
      server->state (i, true);
   }

   CHECK_EQUAL (UINT32_MAX, server->state ());

   server->clear ();

   CHECK_EQUAL (0, server->state ());
}

//! \test Should create the correct Alert::Message.
TEST (AlertServer, Status)
{
   server->disableAll ();
   server->enable (3);
   server->clear ();

   server->state (3, true);

   Alert::Message *msg = server->status (5);

   CHECK_FALSE (msg == nullptr);

   CHECK_EQUAL (8, msg->state);
   CHECK_EQUAL (5, msg->type);

   delete msg;
}

//! \test Should send the a Alert::Message.
TEST (AlertServer, Status2)
{
   server->disableAll ();
   server->enable (3);
   server->clear ();
   server->state (3, true);

   Message::Address addr;

   mock ("Interface").expectOneCall ("sendMessage");

   server->status (addr, 5);

   mock ("Interface").checkExpectations ();

   CHECK_TRUE (server->sendMsg.payload != nullptr);

   LONGS_EQUAL (Interface::SERVER_ROLE, server->sendMsg.itf.role);
   LONGS_EQUAL (server->uid (), server->sendMsg.itf.uid);
   LONGS_EQUAL (Alert::STATUS_CMD, server->sendMsg.itf.member);
   LONGS_EQUAL (Protocol::Message::COMMAND_REQ, server->sendMsg.type);
}

// =============================================================================
// AlertClient
// =============================================================================

//! Test Group for AlertClient interface class.
TEST_GROUP (AlertClient)
{
   struct TestAlertClient:public Testing::InterfaceHelper <AlertClient>
   {
      uint16_t profile_uid;
      uint32_t state;

      TestAlertClient():profile_uid (0), state (0) {}

      void status (Message &message)
      {
         mock ("AlertClient").actualCall ("status");
         profile_uid = message.type;
         state       = message.state;
      }
   };

   TestAlertClient *client;

   Protocol::Message message;
   ByteArray expected;

   TEST_SETUP ()
   {
      mock ("AlertClient").ignoreOtherCalls ();

      client = new TestAlertClient ();

      uint8_t data[] = {0x00, 0x00, 0x00,
                        0x5A, 0xA5,            // Unit Type.
                        0xFF, 0xA5, 0x5A,0xBB, // State.
                        0x00, 0x00, 0x00};
      expected           = ByteArray (data, sizeof(data));

      message.itf.role   = Interface::SERVER_ROLE;
      message.itf.uid    = client->uid ();
      message.itf.member = Alert::STATUS_CMD;

      message.length     = expected.size ();
   }

   TEST_TEARDOWN ()
   {
      delete client;
      mock ().clear ();
   }
};

//! \test Should handle valid message.
TEST (AlertClient, Handle_Valid_Message)
{
   mock ("AlertClient").expectOneCall ("status");

   CHECK_TRUE (client->handle (message, expected, 3));

   LONGS_EQUAL (0x5AA5, client->profile_uid);
   LONGS_EQUAL (0xFFA55ABB, client->state);

   mock ("AlertClient").checkExpectations ();
}

//! \test Should not handle message from invalid role.
TEST (AlertClient, Handle_Invalid_Role)
{
   message.itf.role = Interface::CLIENT_ROLE;

   CHECK_FALSE (client->handle (message, expected, 3));
}

//! \test Should not handle message from invalid interface UID.
TEST (AlertClient, Handle_Invalid_UID)
{
   message.itf.uid = client->uid () + 1;

   CHECK_FALSE (client->handle (message, expected, 3));
}

//! \test Should not handle message with invalid payload size.
TEST (AlertClient, Handle_Invalid_Payload_Size)
{
   Alert::Message alert_msg;
   message.length = alert_msg.size () - 1;

   CHECK_FALSE (client->handle (message, expected, 3));
}

//! \test Should not handle message with not enough payload.
TEST (AlertClient, Handle_Invalid_Payload)
{
   CHECK_FALSE (client->handle (message, expected, 10));
}