// =============================================================================
/*!
 * @file       src/core/group_table.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Group Table service.
 *
 * @version    1.5.0
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
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(IServer *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return GroupTable::create_attribute(uid);
   }
}

// =============================================================================
// GroupTable::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *GroupTable::create_attribute(uint8_t uid)
{
   using namespace HF::Core::GroupTable;

   GroupTable::Attributes attr = static_cast<GroupTable::Attributes>(uid);

   switch (attr)
   {
      case NUMBER_OF_ENTRIES_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(HF::Interface::GROUP_TABLE, attr,
                                                       NumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_MAX_ENTRIES_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(HF::Interface::GROUP_TABLE, attr,
                                                       NumberOfMaxEntries::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// GroupTable::Entry
// =============================================================================

// =============================================================================
// GroupTable::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entry::size() const
{
   return min_size;
}

// =============================================================================
// GroupTable::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entry::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   offset += array.write(offset, group);
   offset += array.write(offset, unit);

   return min_size;
}

// =============================================================================
// GroupTable::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entry::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   offset += array.read(offset, group);
   offset += array.read(offset, unit);

   return min_size;
}

// =============================================================================
// GroupTable::Entries
// =============================================================================

// =============================================================================
// Entries::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entries::size() const
{
   return this->db.size();
}

// =============================================================================
// Entries::save
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::save(const Entry &entry)
{
   if (!any_of(entry.group, entry.unit))
   {
      this->db.push_back(entry);
   }

   return Common::Result::OK;
}

// =============================================================================
// Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::destroy(const Entry &entry)
{
   uint8_t size = db.size();

   auto res     = std::remove_if(db.begin(), db.end(), [&entry](const Entry &e)
   {
      return entry == e;
   });

   db.erase(res, db.end());

   return size == db.size() ? Common::Result::FAIL_ARG : Common::Result::OK;
}

// =============================================================================
// Entries::clear
// =============================================================================
/*!
 *
 */
// =============================================================================
void Entries::clear()
{
   db.clear();
}

// =============================================================================
// Entries::any_of
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Entries::any_of(uint16_t group, uint8_t unit) const
{
   UNUSED(group);
   UNUSED(unit);

   Entry value(group, unit);

   /* *INDENT-OFF* */
   return std::find_if(this->db.cbegin(), this->db.cend(), [&value](const Entry &entry)
   {
      return value == entry;
   }) != this->db.cend();
   /* *INDENT-ON* */
}

// =============================================================================
// Entries::for_each
// =============================================================================
/*!
 *
 */
// =============================================================================
void Entries::for_each(uint16_t group, std::function<void(const Entry &)> func) const
{
   std::for_each(db.begin(), db.end(), [&group, &func](const Entry &e)
   {
      if (e.group == group)
      {
         func(e);
      }
   });
}
