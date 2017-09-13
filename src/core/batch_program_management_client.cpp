// =============================================================================
/*!
 * @file       src/core/batch_program_management_client.cpp
 *
 * This file contains the implementation of the Batch Program Management interface : Client role.
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

#include "../../inc/hanfun/core/batch_program_management.h"

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
// Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case INVOKE_PROGRAM_CMD:
      {
         invoke_program(packet.source);
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
void Client::define_program(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = DEFINE_PROGRAM_CMD;

   send(addr, message);
}

// =============================================================================
// Client::invoke_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::invoke_program(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Client::delete_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::delete_program(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

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
   // FIXME Generated Stub.
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
void Client::get_program_actions(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = GET_PROGRAM_ACTIONS_CMD;

   send(addr, message);
}
#endif
