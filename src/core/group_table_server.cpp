// =============================================================================
/*!
 * @file       src/core/group_table_server.cpp
 *
 * This file contains the implementation of the Group Table service : Server role.
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

#include "hanfun/core/group_table.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::GroupTable;

// =============================================================================
// Group Table Interface : Server Role
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
   HF::Attributes::UIDS result({NUMBER_OF_ENTRIES_ATTR, NUMBER_OF_MAX_ENTRIES_ATTR});

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
   using namespace HF::Core::GroupTable;

   GroupTable::Attributes attr = static_cast<GroupTable::Attributes>(uid);

   switch (attr)
   {
      case NUMBER_OF_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::number_of_entries;
         auto setter = (void (IServer::*)(uint8_t)) nullptr;

         return new Attribute(*this, attr, getter, setter, NumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_MAX_ENTRIES_ATTR:
      {
         typedef HF::Attributes::Attribute<uint8_t, IServer> Attribute;

         auto getter = (uint8_t (IServer::*)(void) const) & IServer::number_of_max_entries;
         auto setter = (void (IServer::*)(uint8_t)) & IServer::number_of_max_entries;

         return new Attribute(*this, attr, getter, setter, NumberOfMaxEntries::WRITABLE);
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
      case ADD_CMD:
      {
         add(packet.source);
         break;
      }

      case REMOVE_CMD:
      {
         remove(packet.source);
         break;
      }

      case REMOVE_ALL_CMD:
      {
         remove_all(packet.source);
         break;
      }

      case READ_ENTRIES_CMD:
      {
         read_entries(packet.source);
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
// Server::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::add(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::remove(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::remove_all
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::remove_all(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::read_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::read_entries(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}


// =============================================================================
// Get/Set Attributes
// =============================================================================

// =============================================================================
// Server::number_of_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::number_of_entries() const
{
   return (uint8_t) entries().size();
}

// =============================================================================
// Server::number_of_max_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
uint8_t IServer::number_of_max_entries() const
{
   return _number_of_max_entries;
}

// =============================================================================
// Server::number_of_max_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
void IServer::number_of_max_entries(uint8_t __value)
{
   HF_SETTER_HELPER(NumberOfMaxEntries, _number_of_max_entries, __value);
}
