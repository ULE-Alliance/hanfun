// =============================================================================
/*!
 * @file       src/core/group_table_server.cpp
 *
 * This file contains the implementation of the Group Table service : Server role.
 *
 * @version    1.5.3
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
         auto setter = (void (IServer::*)(uint8_t))nullptr;

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
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case ADD_CMD:
      {
         Entry entry;
         entry.unpack(payload, offset);

         Common::Result res = add(packet.source, entry);

         GroupTable::Response response(res, entry);
         Protocol::Message message(packet.message, response.size());

         response.pack(message.payload);

         send(packet.source, message);

         break;
      }

      case REMOVE_CMD:
      {
         Entry entry;
         entry.unpack(payload, offset);

         Common::Result res = remove(packet.source, entry);

         GroupTable::Response response(res, entry);
         Protocol::Message message(packet.message, response.size());

         response.pack(message.payload);

         send(packet.source, message);

         break;
      }

      case REMOVE_ALL_CMD:
      {
         Common::Result res = remove_all(packet.source);

         Protocol::Response response(res);
         Protocol::Message message(packet.message, response.size());

         response.pack(message.payload);

         send(packet.source, message);

         break;
      }

      case READ_ENTRIES_CMD:
      {
         ReadEntries params;
         params.unpack(payload, offset);

         ReadEntriesResponse response = read_entries(packet.source, params);
         Protocol::Message message(packet.message, response.size());

         response.pack(message.payload);

         send(packet.source, message);

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
Common::Result IServer::add(const Protocol::Address &addr, const Entry &entry)
{
   if (addr != Protocol::Address(0, 0))
   {
      return Common::Result::FAIL_AUTH;
   }

   if (entries().size() + 1 <= number_of_max_entries())
   {
      return entries().save(entry);
   }
   else
   {
      return Common::Result::FAIL_RESOURCES;
   }
}

// =============================================================================
// Server::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::remove(const Protocol::Address &addr, const Entry &entry)
{
   if (addr == Protocol::Address(0, 0))
   {
      return entries().destroy(entry);
   }
   else
   {
      return Common::Result::FAIL_AUTH;
   }
}

// =============================================================================
// Server::remove_all
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result IServer::remove_all(const Protocol::Address &addr)
{
   if (addr == Protocol::Address(0, 0))
   {
      entries().clear();

      return Common::Result::OK;
   }
   else
   {
      return Common::Result::FAIL_AUTH;
   }
}

// =============================================================================
// Server::read_entries
// =============================================================================
/*!
 *
 */
// =============================================================================
ReadEntriesResponse IServer::read_entries(const Protocol::Address &addr, const ReadEntries &params)
{
   UNUSED(addr);

   ReadEntriesResponse result;

   uint8_t offset = params.start;
   uint8_t count  = params.count;

   if (offset > entries().size())
   {
      result.code = Common::Result::FAIL_ARG;
      goto _end;
   }

   if ((offset + count) > entries().size())
   {
      count = entries().size() - offset;
   }

   result.entries.reserve(count * Entry::min_size);

   for (uint8_t i = 0; i < count; ++i)
   {
      result.entries.push_back(entries()[offset + i]);
   }

   _end:

   result.start = offset;

   return result;
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

// =============================================================================
// IServer::payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t IServer::payload_size(Protocol::Message::Interface &itf) const
{
   CMD cmd = static_cast<CMD>(itf.member);

   switch (cmd)
   {
      case ADD_CMD:
      case REMOVE_CMD:
      {
         return payload_size_helper<Entry>();
      }
      case READ_ENTRIES_CMD:
      {
         return payload_size_helper<ReadEntries>();
      }
      default:
      {
         return 0;
      }
   }
}
