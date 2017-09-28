// =============================================================================
/*!
 * @file       src/core/scheduling_server.cpp
 *
 * This file contains the implementation of the Server functionality for the
 * Scheduling service.
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

#include "hanfun/core/scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;

// =============================================================================
// Event Scheduling Interface : Server Role
// =============================================================================

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::UIDS IServer::attributes(uint8_t pack_id) const
{
   HF::Attributes::UIDS result({MAXIMUM_NUMBER_OF_ENTRIES_ATTR, NUMBER_OF_ENTRIES_ATTR,
                                STATUS_ATTR});

   if (pack_id == HF::Attributes::ALL)
   {}

   return result;
}

// =============================================================================
// Server::attributes
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *IServer::attribute(uint8_t uid)
{
   using namespace HF::Core::Scheduling;

   Scheduling::Attributes attr = static_cast<Scheduling::Attributes>(uid);

   switch (attr)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::maximum_number_of_entries;
         auto setter = (void (IServer::*)(uint8_t)) & IServer::maximum_number_of_entries;

         return new Attribute(*this, attr, getter, setter, MaximumNumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::number_of_entries;
         auto setter = (void (IServer::*)(uint8_t)) & IServer::number_of_entries;

         return new Attribute(*this, attr, getter, setter, NumberOfEntries::WRITABLE);
      }

      case STATUS_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::status;
         auto setter = (void (IServer::*)(uint8_t)) & IServer::status;

         return new Attribute(*this, attr, getter, setter, Status::WRITABLE);
      }

      default:
         return nullptr;
   }
}


// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                       uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case ACTIVATE_SCHEDULER_CMD:
      {
         ActivateScheduler msg;
         msg.unpack(payload, offset);
         return activate_scheduler(packet, msg);
      }

      case DEFINE_CMD:
      {
         return Common::Result::FAIL_SUPPORT;
      }

      case UPDATE_STATUS_CMD:
      {
         return Common::Result::FAIL_SUPPORT;
      }

      case GET_ENTRY_CMD:
      {
         get_event_entry(packet.source);
         break;
      }

      case DELETE_CMD:
      {
         delete_event(packet.source);
         break;
      }

      case DELETE_ALL_CMD:
      {
         delete_all_events(packet.source);
         break;
      }

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::OK;
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::activate_scheduler
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::activate_scheduler(const Protocol::Packet &packet, ActivateScheduler &msg)
{
   Common::Result result = Common::Result::OK;

   HF_ASSERT(msg.status <= 0x01, {result = Common::Result::FAIL_ARG;goto _end;})
   this->status(msg.status);

   _end:
   ActivateSchedulerResponse response(result);

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
void IServer::get_event_entry(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::delete_event
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::delete_event(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::delete_all_events
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::delete_all_events(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::maximum_number_of_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::maximum_number_of_entries() const
{
   return _maximum_number_of_entries;
}

// =============================================================================
// Server::maximum_number_of_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::maximum_number_of_entries(uint8_t __value)
{
   HF_SCHEDULING_SETTER_HELPER(MaximumNumberOfEntries, _maximum_number_of_entries, __value);
}

// =============================================================================
// Server::number_of_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::number_of_entries() const
{
   return _number_of_entries;
}

// =============================================================================
// Server::number_of_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::number_of_entries(uint8_t __value)
{
   HF_SCHEDULING_SETTER_HELPER(NumberOfEntries, _number_of_entries, __value);
}

// =============================================================================
// Server::status
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::status() const
{
   return _status;
}

// =============================================================================
// Server::status
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::status(uint8_t __value)
{
   HF_SCHEDULING_SETTER_HELPER(Status, _status, __value);
}
