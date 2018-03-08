// =============================================================================
/*!
 * @file       src/core/batch_program_management_client.cpp
 *
 * This file contains the implementation of the Batch Program Management interface : Client role.
 *
 * @version    1.5.2
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
// Batch Program Management : Client Role
// =============================================================================


// =============================================================================
// Client::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Client::payload_size(Protocol::Message::Interface &itf) const
{
   switch (itf.member)
   {
      case DEFINE_PROGRAM_CMD:
         return payload_size_helper<DefineProgramResponse>();

      case INVOKE_PROGRAM_CMD:
         return payload_size_helper<InvokeProgramResponse>();

      case DELETE_PROGRAM_CMD:
         return payload_size_helper<DeleteProgramResponse>();

      case DELETE_ALL_PROGRAMS_CMD:
         return payload_size_helper<DeleteAllProgramsResponse>();

      case GET_PROGRAM_ACTIONS_CMD:
         return payload_size_helper<GetProgramActionsResponse>();

      default:
         return 0;
   }
}
// =============================================================================
// Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   assert(packet.link != nullptr);

   switch (packet.message.itf.member)
   {
      case DEFINE_PROGRAM_CMD:
      {
         DefineProgramResponse response;
         response.unpack(payload, offset);
         defined(response);
         break;
      }

      case INVOKE_PROGRAM_CMD:
      {
         InvokeProgramResponse response;
         response.unpack(payload, offset);
         invoked(response);
         break;
      }

      case DELETE_PROGRAM_CMD:
      {
         DeleteProgramResponse response;
         response.unpack(payload, offset);
         deleted(response);
         break;
      }

      case DELETE_ALL_PROGRAMS_CMD:
      {
         DeleteAllProgramsResponse response;
         response.unpack(payload, offset);
         deleted(response);
         break;
      }

      case GET_PROGRAM_ACTIONS_CMD:
      {
         GetProgramActionsResponse response;
         response.unpack(payload, offset);
         got_actions(response);
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
// Client::define_program
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::define_program(const Protocol::Address &addr, Entry &program)
{
   Protocol::Message message(program.size());
   HF_ASSERT(program.pack(message.payload) != 0, {return Common::Result::FAIL_ARG;});

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = DEFINE_PROGRAM_CMD;

   send(addr, message);

   return Common::Result::OK;
}

// =============================================================================
// Client::invoke_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::invoke_program(const Protocol::Address &addr, uint8_t pid)
{
   InvokeProgram msg(pid);

   Protocol::Message message(msg.size());
   msg.pack(message.payload);

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = INVOKE_PROGRAM_CMD;

   send(addr, message);
}

// =============================================================================
// Client::delete_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::delete_program(const Protocol::Address &addr, uint8_t pid)
{
   DeleteProgram msg(pid);

   Protocol::Message message(msg.size());
   msg.pack(message.payload);

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = DELETE_PROGRAM_CMD;

   send(addr, message);
}

// =============================================================================
// Client::delete_all_programs
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::delete_all_programs(const Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = DELETE_ALL_PROGRAMS_CMD;

   send(addr, message);
}

#ifdef HF_CORE_BATCH_PROGRAM_MANAGEMENT_GET_PROGRAM_ACTIONS_CMD
// =============================================================================
// Client::get_program_actions
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::get_program_actions(const Protocol::Address &addr, uint8_t pid)
{
   GetProgramActions msg(pid);

   Protocol::Message message(msg.size());
   msg.pack(message.payload);

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = GET_PROGRAM_ACTIONS_CMD;

   send(addr, message);
}
#endif
