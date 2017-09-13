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

#include "../../inc/hanfun/core/batch_program_management.h"

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
HF::Attributes::UIDS Server::attributes(uint8_t pack_id) const
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
HF::Attributes::IAttribute *Server::attribute(uint8_t uid)
{
   using namespace HF::Core::BatchProgramManagement;

   BatchProgramManagement::Attributes attr =
      static_cast<BatchProgramManagement::Attributes>(uid);

   switch (attr)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, Server> Attribute;

         auto getter = (uint8_t (Server::*)(void) const) & Server::maximum_number_of_entries;
         auto setter = (void (Server::*)(uint8_t)) & Server::maximum_number_of_entries;

         return new Attribute(*this, attr, getter, setter, MaximumNumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, Server> Attribute;

         auto getter = (uint8_t (Server::*)(void) const) & Server::number_of_entries;
         auto setter = (void (Server::*)(uint8_t)) & Server::number_of_entries;

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
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case DEFINE_PROGRAM_CMD:
      {
         define_program(packet.source);
         break;
      }

      case DELETE_PROGRAM_CMD:
      {
         delete_program(packet.source);
         break;
      }

      case DELETE_ALL_PROGRAMS_CMD:
      {
         delete_all_programs(packet.source);
         break;
      }

      case GET_PROGRAM_ACTIONS_CMD:
      {
         get_program_actions(packet.source);
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
// Server::define_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::define_program(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::invoke_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::invoke_program(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::BATCH_PROGRAM_MANAGEMENT;
   message.itf.member = INVOKE_PROGRAM_CMD;

   send(addr, message);
}

// =============================================================================
// Server::delete_program
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::delete_program(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::delete_all_programs
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::delete_all_programs(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::get_program_actions
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::get_program_actions(const Protocol::Address &addr)
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
uint8_t Server::maximum_number_of_entries() const
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
void Server::maximum_number_of_entries(uint8_t __value)
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
uint8_t Server::number_of_entries() const
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
void Server::number_of_entries(uint8_t __value)
{
   HF_SETTER_HELPER(NumberOfEntries, _number_of_entries, __value);
}
