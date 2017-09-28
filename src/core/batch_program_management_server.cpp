// =============================================================================
/*!
 * @file       src/core/batch_program_management_server.cpp
 *
 * This file contains the implementation of the Batch Program Management interface : Server role.
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

#include "hanfun/core/batch_program_management.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::BatchProgramManagement;

// =============================================================================
// Batch Program Management Interface : Server Role
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
   HF::Attributes::UIDS result({MAXIMUM_NUMBER_OF_ENTRIES_ATTR, NUMBER_OF_ENTRIES_ATTR});

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
   using namespace HF::Core::BatchProgramManagement;

   BatchProgramManagement::Attributes attr =
      static_cast<BatchProgramManagement::Attributes>(uid);

   switch (attr)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::maximum_number_of_entries;
         auto setter = (void (IServer::*) (uint8_t)) & IServer::maximum_number_of_entries;

         return new Attribute(*this, attr, getter, setter, MaximumNumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::number_of_entries;
         auto setter = (void (IServer::*) (uint8_t))nullptr;

         return new Attribute(*this, attr, getter, setter, NumberOfEntries::WRITABLE);
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
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case DEFINE_PROGRAM_CMD:
      {
         DefineProgram msg;
         msg.unpack(payload, offset);
         return define_program(packet, msg);
      }

      case INVOKE_PROGRAM_CMD:
      {
         InvokeProgram msg;
         msg.unpack(payload, offset);
         return invoke_program(packet, msg);
      }

      case DELETE_PROGRAM_CMD:
      {
         DeleteProgram msg;
         msg.unpack(payload, offset);
         return delete_program(packet, msg);
      }

      case DELETE_ALL_PROGRAMS_CMD:
      {
         return delete_all_programs(packet);
      }

      case GET_PROGRAM_ACTIONS_CMD:
      {
         GetProgramActions msg;
         msg.unpack(payload, offset);
         return get_program_actions(packet, msg);
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
// Server::define_program
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::define_program(const Protocol::Packet &packet, DefineProgram &msg)
{
   Common::Result result = Common::Result::OK;

   uint8_t pid           = Entry::AVAILABLE_PID;

   uint8_t size          = entries().size();

   if (size == maximum_number_of_entries())
   {
      result = Common::Result::FAIL_RESOURCES;
      goto _end;
   }

   if (msg.pid == Entry::AVAILABLE_PID)
   {
      pid = next_pid();

      if (pid == Entry::AVAILABLE_PID)
      {
         result = Common::Result::FAIL_RESOURCES;
         goto _end;
      }

      msg.pid = pid;
   }
   else
   {
      if (entry(msg.pid) != nullptr)
      {
         result = Common::Result::FAIL_ARG;
         goto _end;
      }
   }

   if (entry(msg.name) != nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   // Check if all the units exists in the device,
   // and if the message type is the right one.
   std::all_of(msg.actions.begin(), msg.actions.end(), [&result, this](Action &a)
   {
      if (unit().device().unit(a.reference) == nullptr || (
             a.type != Protocol::Message::Type::COMMAND_REQ &&
             a.type != Protocol::Message::Type::SET_ATTR_REQ))
      {
         result = Common::Result::FAIL_ARG; // goto not allowed where! A simple return will do in this case.
         return false;
      }

      return true;
   });

   if (result != Common::Result::OK)       // test for the lambda function return value
   {
      goto _end;
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

   DefineProgramResponse response(result, msg.pid);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::invoke_program
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::invoke_program(const Protocol::Packet &packet, InvokeProgram &msg)
{
   Common::Result result       = Common::Result::OK;
   Protocol::Address localhost = packet.destination;

   auto prog                   = entry(msg.pid);

   if (prog == nullptr)           // Check if the program ID exists
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }


   std::all_of(prog->actions.begin(), prog->actions.end(),
               [&localhost, &result, this](Action &a)
   {
      localhost.unit = a.reference;
      Protocol::Packet _packet(localhost, localhost, a);
      auto dest_unit = unit().device().unit(a.reference);

      if (dest_unit == nullptr)
      {
         result = Common::Result::FAIL_ARG;
         return false;     // goto not allowed where! A simple return will do in this case.
      }

      result = dest_unit->handle(_packet, a.payload, 0);

      if (result != Common::Result::OK)
      {
         return false;     // goto not allowed where! A simple return will do in this case.
      }

      return true;
   });

   _end:

   InvokeProgramResponse response(result, msg.pid);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::delete_program
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::delete_program(const Protocol::Packet &packet, DeleteProgram &msg)
{
   Common::Result result = Common::Result::OK;
   uint8_t size          = entries().size();

   if (entry(msg.pid) == nullptr)
   {
      result = Common::Result::FAIL_ARG;
      goto _end;
   }

   result = entries().destroy(msg.pid);

   _end:

   if (size != entries().size())
   {
      HF_NOTIFY_HELPER(NumberOfEntries, size, entries().size());
   }

   DeleteProgramResponse response(result, msg.pid);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
}

// =============================================================================
// Server::delete_all_programs
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::delete_all_programs(const Protocol::Packet &packet)
{
   uint8_t size = entries().size();

   entries().clear();

   if (size != entries().size())
   {
      HF_NOTIFY_HELPER(NumberOfEntries, size, entries().size());
   }

   DeleteAllProgramsResponse response(Common::Result::OK);

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return Common::Result::OK;
}

// =============================================================================
// Server::get_program_actions
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::get_program_actions(const Protocol::Packet &packet,
                                            GetProgramActions &msg)
{
   Common::Result result = Common::Result::OK;
   GetProgramActionsResponse response;

   auto prog = entry(msg.pid);

   if (prog == nullptr)
   {
      result   = Common::Result::FAIL_ARG;
      response = GetProgramActionsResponse(result);
      goto _end;
   }

   response = GetProgramActionsResponse(result, prog.operator*());

   _end:

   Protocol::Message message(packet.message, response.size());

   response.pack(message.payload);

   send(packet.source, message, packet.link);

   return result;
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
   HF_SETTER_HELPER(MaximumNumberOfEntries, _maximum_number_of_entries, __value);
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
   return entries().size();
}
