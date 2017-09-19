// =============================================================================
/*!
 * @file       src/core/batch_program_management_cpp
 *
 * This file contains the implementation of the common functionality for the
 * Batch Program Management interface.
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
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return BatchProgramManagement::create_attribute(uid);
   }
}

// =============================================================================
// BatchProgramManagement::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *BatchProgramManagement::create_attribute(uint8_t uid)
{
   using namespace HF::Core::BatchProgramManagement;

   BatchProgramManagement::Attributes attr =
      static_cast<BatchProgramManagement::Attributes>(uid);

   switch (attr)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(
            HF::Interface::BATCH_PROGRAM_MANAGEMENT, attr,
            MaximumNumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_ENTRIES_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(
            HF::Interface::BATCH_PROGRAM_MANAGEMENT, attr, NumberOfEntries::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// Batch Program Management
// =============================================================================

// =============================================================================
// action::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Action::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;
   uint16_t size = 0;

   size = Protocol::Message::unpack(array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size > 0, return 0;);
   /* *INDENT-ON* */
   offset += size;
   payload = Common::ByteArray(this->length);

   auto it = array.begin();
   std::advance(it, offset);
   std::copy_n(it, this->length, payload.begin());
   offset += this->length;

   return offset - start;
}


// =============================================================================
// entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entry::size() const
{
    Common::SerializableHelperVector<std::vector<Action> , uint8_t>
       helper (const_cast<std::vector<Action> &>(actions));

   return (min_size + name.length() + helper.size() -1); //-1 because the helper counts with the
                                                         //   number of entries already.
}

// =============================================================================
// entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entry::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   offset += array.write(offset, pid);

   size = HF::Common::SerializableHelper<std::string>::pack(name, array, offset);
   HF_ASSERT(size > 0, return 0;);
   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Action>, uint8_t> helper(
         const_cast<std::vector<Action> &>(actions));
   offset += helper.pack(array, offset);

   return offset - start;
}

// =============================================================================
// entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entry::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;
   uint16_t size;

   offset += array.read(offset, pid);

   size = HF::Common::SerializableHelper<std::string>::unpack(name, array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, return 0;);
   /* *INDENT-ON* */
   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Action>, uint8_t> helper(
         const_cast<std::vector<Action> &>(actions));
   size = helper.unpack(array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, return 0;);
   /* *INDENT-ON* */
   offset += size;

   return (offset - start);
}

// =============================================================================
// DefineProgramResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t DefineProgramResponse::size () const
{
   if(code != Common::Result::OK)
      return (min_size);
   else
      return (min_size + sizeof(uint8_t));
}

// =============================================================================
// DefineProgramResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t DefineProgramResponse::pack (Common::ByteArray& array,
                                      uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += Response::pack(array, offset);
   if (this->code != Common::Result::OK)
   {
      return min_size;
   }
   offset += array.write(offset, pid);
   return (offset - start);
}

// =============================================================================
// DefineProgramResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t DefineProgramResponse::unpack (
                                        const Common::ByteArray& array,
                                        uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;

   offset += Response::unpack(array, offset);

   this->pid = 0x00;

   if (this->code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, sizeof(uint8_t));

   offset += array.read(offset, pid);

   _end:
   return offset - start;
}

// =============================================================================
// InvokeProgram::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t InvokeProgram::size () const
{
   return(min_size);
}

// =============================================================================
// InvokeProgram::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t InvokeProgram::pack (Common::ByteArray& array,
                              uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   return (array.write(offset, pid));
}

// =============================================================================
// InvokeProgram::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t InvokeProgram::unpack (const Common::ByteArray& array,
                                uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);
   return (array.read(offset, pid));
}

// =============================================================================
// GetProgramActionsResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t GetProgramActionsResponse::size () const
{
   if (code != Common::Result::OK)
      return (min_size);
   else
      return (min_size + program.size());
}

// =============================================================================
// GetProgramActionsResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t GetProgramActionsResponse::pack (
                                          Common::ByteArray& array,
                                          uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += Response::pack(array, offset);
   if (this->code != Common::Result::OK)
   {
      return min_size;
   }

   offset += program.pack(array, offset);

   return (offset - start);
}

// =============================================================================
// GetProgramActionsResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t GetProgramActionsResponse::unpack (
                                            const Common::ByteArray& array,
                                            uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;
   uint16_t size;

   offset += Response::unpack(array, offset);

   if (this->code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, Entry::min_size);

   size += program.unpack(array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */
   offset += size;

   _end:
   return offset - start;
}

uint16_t Entries::size () const
{
   return db.size();
}

Common::Result Entries::save (const Entry& entry)
{
   db.insert(db.end(), std::pair<uint8_t, Entry>(entry.pid, entry));

   return Common::Result::OK;
}

Common::Result Entries::save (const uint8_t pid,
                              const std::string& name,
                              std::vector<Action>& actions)
{
   db.insert(db.end(), std::pair<uint8_t, Entry>(pid, Entry(pid, name, actions)));

   return Common::Result::OK;
}

Common::Result Entries::destroy (const uint8_t& pid)
{
   auto count = db.erase(pid);

      if (count != 1)
      {
         return Common::Result::FAIL_ARG;
      }
      else
      {
         return Common::Result::OK;
      }
}

Common::Result Entries::destroy (const Entry& entry)
{
   return destroy(entry.pid);
}

void Entries::clear()
{
   db.clear();
}

EntryPtr Entries::find (uint8_t pid) const
{
   auto it = db.find(pid);

   if (it == db.end())
   {
      return EntryPtr();
   }
   else
   {
      return EntryPtr(const_cast<Entry *>(&(*it).second));
   }
}

EntryPtr Entries::find (const std::string& name) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&name](const std::pair< const uint8_t, Entry> &prog)
   {
      return prog.second.name == name;
   });
   /* *INDENT-ON* */

   if (it == db.end())
   {
      return EntryPtr();
   }
   else
   {
      return EntryPtr(const_cast<Entry *>(&((it->second))));
   }

   return EntryPtr();
}

uint8_t Entries::next_pid () const
{
   uint8_t pid = 0;

   if (db.size() > Entry::MAX_PID)
   {
      return Entry::AVAILABLE_PID;
   }

   for (pid = Entry::START_PID; pid <= Entry::MAX_PID; ++pid)
   {
      if (db.find(pid) == db.end())
      {
         return pid;
      }
   }

   return Entry::AVAILABLE_PID;
}
