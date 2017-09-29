// =============================================================================
/*!
 * @file       src/core/event_scheduling_server.cpp
 *
 * This file contains the implementation of the Event Scheduling service : Server role.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/event_scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling::Event;

// =============================================================================
// IServer::define_event
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Scheduling::Event::IServer::define_event(const Protocol::Packet &packet,
                                                        Scheduling::DefineEvent<Interval> &msg)
{
   Common::Result result = Common::Result::OK;

   uint8_t id            = Entry::AVAILABLE_ID;

   uint8_t size          = entries().size();

   if (size == maximum_number_of_entries())
   {
      result = Common::Result::FAIL_RESOURCES;
      goto _end;
   }

   if (msg.id == Entry::AVAILABLE_ID)
   {
      id = next_id();

      if (id == Entry::AVAILABLE_ID)
      {
         result = Common::Result::FAIL_RESOURCES;
         goto _end;
      }

      msg.id = id;
   }
   else
   {
      if (entry(msg.id) != nullptr)
      {
         result = Common::Result::FAIL_ARG;
         goto _end;
      }
   }

   result = entries().save(static_cast<Entry>(msg));

   if (result != Common::Result::OK)
   {
      result = Common::Result::FAIL_UNKNOWN;
      goto _end;
   }

   _end:

   if (size != entries().size())
   {
      HF_NOTIFY_HELPER(NumberOfEntries, size, entries().size());
   }

   DefineEventResponse response(result, msg.id);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::update_event_status
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Scheduling::Event::IServer::update_event_status(const Protocol::Packet &packet,
                                                               UpdateStatus &msg)
{
   Common::Result result = Common::Result::OK;

   auto eventPtr         = entry(msg.event_id);

   if (eventPtr == nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   eventPtr->status = msg.status;

   _end:

   UpdateStatusResponse response(result, msg.event_id);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::get_event_entry
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Scheduling::Event::IServer::get_event_entry(const Protocol::Packet &packet,
                                                           GetEntry &msg)
{
   Common::Result result = Common::Result::OK;
   GetEntryResponse response;

   auto eventPtr = entry(msg.event_id);

   if (eventPtr == nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   response.entry = eventPtr.operator*();

   _end:

   response.code = result;
   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::delete_event
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Scheduling::Event::IServer::delete_event(const Protocol::Packet &packet,
                                                        DeleteEvent &msg)
{
   uint8_t size = entries().size();
   DeleteEventResponse response;

   auto eventPtr = entry(msg.event_id);

   if (eventPtr == nullptr)
   {
      response.code = Common::Result::FAIL_ARG;
      goto _end;
   }

   response.code = entries().destroy(*eventPtr);

   if (response.code == Common::Result::OK)
   {
      response.event_id = msg.event_id;
      HF_NOTIFY_HELPER(NumberOfEntries, size, entries().size());
   }

   _end:

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return response.code;
}

// =============================================================================
// Server::delete_all_events
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Scheduling::Event::IServer::delete_all_events(const Protocol::Packet &packet)
{
   const static Common::Result result = Common::Result::OK;
   uint8_t size                       = entries().size();

   entries().clear();

   HF_NOTIFY_HELPER(NumberOfEntries, size, entries().size());

   DeleteAllResponse response(result);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return response.code;
}
