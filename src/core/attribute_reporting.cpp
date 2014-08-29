// =============================================================================
/*!
 * \file       src/core/attribute_reporting.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Attribute Reporting service interface.
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/attribute_reporting.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::AttributeReporting;

// =============================================================================
// API
// =============================================================================

// =============================================================================
// Identifier::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Reference::size () const
{
   return sizeof(uint8_t);
}

// =============================================================================
// Identifier::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Reference::pack (Common::ByteArray &array, size_t offset) const
{
   size_t  start = offset;

   uint8_t temp  = ((type & 0x01) << 7) | (id & 0x7F);

   offset += array.write (offset, temp);

   return offset - start;
}

// =============================================================================
// Identifier::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Reference::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t  start = offset;

   uint8_t temp  = 0;
   offset += array.read (offset, temp);

   type    = ((temp & 0x80) != 0);
   id      = temp & 0x7F;

   return offset - start;
}

// =============================================================================
// Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::size () const
{
   return sizeof(unit) + itf.size () + sizeof(pack_id);
}

// =============================================================================
// Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, this->unit);

   offset += itf.pack (array, offset);

   offset += array.write (offset, this->pack_id);

   return offset - start;
}

// =============================================================================
// Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += array.read (offset, this->unit);

   offset += itf.unpack (array, offset);

   offset += array.read (offset, this->pack_id);

   return offset - start;
}

// =============================================================================
// Rule::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Rule::size () const
{
   return sizeof(uint8_t) + destination.size ();
}

// =============================================================================
// Rule::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Rule::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += report.pack (array, offset);

   offset += destination.pack (array, offset);

   return offset - start;
}

// =============================================================================
// Rule::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Rule::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += report.unpack (array, offset);

   offset += destination.unpack (array, offset);

   return offset - start;
}

// =============================================================================
// Periodic API
// =============================================================================

// =============================================================================
// Periodic::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Periodic::Entry::size () const
{
   size_t result = AttributeReporting::Entry::size ();

   if (pack_id == HF::Attributes::Pack::DYNAMIC)
   {
      result += uids.size ();
   }

   return result;
}

// =============================================================================
// Periodic::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Periodic::Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += AttributeReporting::Entry::pack (array, offset);

   if (pack_id == HF::Attributes::Pack::DYNAMIC)
   {
      offset += uids.pack (array, offset);
   }

   return offset - start;
}

// =============================================================================
// Periodic::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Periodic::Entry::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += AttributeReporting::Entry::unpack (array, offset);

   if (pack_id == HF::Attributes::Pack::DYNAMIC)
   {
      offset += uids.unpack (array, offset);
   }

   return offset - start;
}

// =============================================================================
// Periodic::Rule::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Periodic::Rule::size () const
{
   size_t result = AttributeReporting::Rule::size () + sizeof(_interval);

   result += sizeof(uint8_t);  // Number of entries.

   /* *INDENT-OFF* */
   std::for_each (entries.begin(), entries.end(), [&result](const Entry &entry)
   {
      result += entry.size();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Periodic::Rule::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Periodic::Rule::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += AttributeReporting::Rule::pack (array, offset);

   offset += array.write (offset, _interval);

   uint8_t _size = std::distance (entries.begin (), entries.end ());

   offset += array.write (offset, _size);

   /* *INDENT-OFF* */
   std::for_each (entries.begin (), entries.end (),
                  [&offset, &array](const Entry &entry)
   {
      offset += entry.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Periodic::Rule::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Periodic::Rule::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t  start = offset;
   uint8_t size  = 0;

   offset += AttributeReporting::Rule::unpack (array, offset);

   if (report.type != PERIODIC)
   {
      goto end;
   }

   offset += array.read (offset, _interval);

   clear ();

   offset += array.read (offset, size);

   for (uint8_t i = 0; i < size; i++)
   {
      Entry entry;
      offset += entry.unpack (array, offset);
      add (entry);
   }

   end:
   return offset - start;
}

// =============================================================================
// Event API.
// =============================================================================

// =============================================================================
// Event::Field::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Field::size (bool with_uid) const
{
   // Field type.
   size_t result = sizeof(uint8_t);

   // Size of value holder or COV value.
   result += sizeof(uint8_t);

   // Number of bytes in value.
   result += (type != Event::COV ? value.size () : 0);

   // Attribute UID for dynamic entries.
   if (with_uid)
   {
      result += sizeof(attr_uid);
   }

   return result;
}

// =============================================================================
// Event::Field::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Field::pack (Common::ByteArray &array, size_t offset, bool with_uid) const
{
   size_t start = offset;

   offset += array.write (offset, (uint8_t) type);

   if (with_uid)
   {
      offset += array.write (offset, attr_uid);
   }

   if (type == Event::COV)
   {
      uint8_t temp = (value.size () != 0 ? value.at(0) : 0);
      offset += array.write (offset, temp);
   }
   else
   {
      offset += array.write (offset, (uint8_t) value.size ());

      auto dst = array.begin ();
      std::advance (dst, offset);

      std::copy (value.begin (), value.end (), dst);

      offset += value.size ();
   }

   return offset - start;
}

// =============================================================================
// Event::Field::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Field::unpack (const Common::ByteArray &array, size_t offset, bool with_uid)
{
   size_t  start = offset;

   uint8_t temp  = 0;
   offset += array.read (offset, temp);

   type    = static_cast <Event::Type>(temp);

   if (with_uid)
   {
      offset += array.read (offset, attr_uid);
   }

   if (type == Event::COV)
   {
      temp = 1;
   }
   else
   {
      // Read value size.
      offset += array.read (offset, temp);
   }

   auto it = array.begin ();
   std::advance (it, offset);

   value = Common::ByteArray (temp);

   std::copy_n (it, temp, value.begin ());

   offset += value.size ();

   return offset - start;
}

// =============================================================================
// Event::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Entry::size () const
{
   bool attr_uid = false;

   size_t result = AttributeReporting::Entry::size ();

   if (this->pack_id == HF::Attributes::Pack::DYNAMIC)
   {
      attr_uid = true;
      result  += sizeof(uint8_t);
   }
   else if (fields.size() == 0)
   {
      result += (sizeof(uint8_t) + sizeof(uint8_t));
   }

   /* *INDENT-OFF* */
   std::for_each(fields.begin(), fields.end(),
                 [&result, attr_uid](const Event::Field &field)
   {
      result += field.size(attr_uid);
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Event::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start    = offset;

   bool   attr_uid = false;

   offset += AttributeReporting::Entry::pack (array, offset);

   if (this->pack_id == HF::Attributes::Pack::DYNAMIC)
   {
      attr_uid = true;
      offset  += array.write (offset, (uint8_t) fields.size ());
   }
   else if (fields.size () == 0)
   {
      offset  += array.write (offset, (uint8_t) COV);
      offset  += array.write (offset, (uint8_t) 0);
   }

   /* *INDENT-OFF* */
   std::for_each(fields.begin(), fields.end(),
                 [&offset, &array, attr_uid](const Event::Field &field)
   {
      offset += field.pack(array, offset, attr_uid);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Event::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Entry::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += AttributeReporting::Entry::unpack (array, offset);

   if (this->pack_id == HF::Attributes::Pack::DYNAMIC)
   {
      uint8_t _size = 0;
      offset += array.read (offset, _size);

      for (uint8_t i = 0; i < _size; i++)
      {
         Event::Field field;
         offset += field.unpack (array, offset, true);
         fields.push_back (field);
      }
   }
   else
   {
      Event::Field field;
      offset += field.unpack (array, offset, false);
      fields.push_back (field);
   }

   return offset - start;
}

// =============================================================================
// Event::Rule::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Rule::size () const
{
   size_t result = AttributeReporting::Rule::size ();

   result += sizeof(uint8_t);

   /* *INDENT-OFF* */
   std::for_each (entries.begin(), entries.end(), [&result](const Entry &entry)
   {
      result += entry.size();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Event::Rule::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Rule::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += AttributeReporting::Rule::pack (array, offset);

   uint8_t _size = std::distance (entries.begin (), entries.end ());

   offset += array.write (offset, _size);

   /* *INDENT-OFF* */
   std::for_each (entries.begin (), entries.end (), [&offset, &array](const Entry &entry)
   {
      offset += entry.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Event::Rule::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Event::Rule::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += AttributeReporting::Rule::unpack (array, offset);

   uint8_t _size = 0;

   offset += array.read (offset, _size);

   for (uint8_t i = 0; i < _size; i++)
   {
      Entry entry;
      offset += entry.unpack (array, offset);
      entries.push_front (entry);
   }

   return offset - start;
}

// =============================================================================
// Report API - Notification Messages
// =============================================================================

// =============================================================================
// Report::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Entry::size () const
{
   return sizeof(unit) + itf.size () + sizeof(uint8_t);
}

// =============================================================================
// Report::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, unit);

   offset += itf.pack (array, offset);

   offset += array.write (offset, count ());

   return offset - start;
}

// =============================================================================
// Report::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Entry::unpack (HF::Attributes::FactoryGetter get_factory,
                              const Common::ByteArray &array,
                              size_t offset)
{
   HF::Attributes::Factory factory = nullptr;
   uint8_t next                    = 0;

   size_t  start                   = offset;

   offset += array.read (offset, unit);

   offset += itf.unpack (array, offset);

   uint8_t _count = 0;
   offset += array.read (offset, _count);

   if (get_factory == nullptr)
   {
      goto end;
   }

   factory = get_factory (itf);

   if (factory == nullptr)
   {
      goto end;
   }

   next = count ();

   for (uint8_t i = 0; i < _count; i++, next = count ())
   {
      offset += unpack (factory, array, offset);

      // There was a problem reading entry field.
      if (next == count ())
      {
         break;
      }
   }

   end:
   return offset - start;
}

// =============================================================================
// Report::Periodic::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::Entry::size () const
{
   size_t result = Report::Entry::size ();

   /* *INDENT-OFF* */
   std::for_each(attributes.begin(), attributes.end(), [&result](const Attribute &attr)
   {
      result += attr->size(true);
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Report::Periodic::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::Entry::pack (array, offset);

   /* *INDENT-OFF* */
   std::for_each(attributes.begin(), attributes.end(), [&offset, &array](const Attribute &attr)
   {
      offset += attr->pack(array, offset, true);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Report::Periodic::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::Entry::unpack (HF::Attributes::Factory factory,
                                        const Common::ByteArray &array, size_t offset)
{
   size_t  start = offset;

   uint8_t uid   = 0;
   array.read (offset, uid);
   auto    attr  = factory (uid);

   if (attr != nullptr)
   {
      size_t _size = attr->unpack (array, offset, true);

      if (_size == attr->size (true))
      {
         add (attr);
         offset += _size;
      }
   }

   return offset - start;
}

// =============================================================================
// Report::Periodic::Entry::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void Report::Periodic::Entry::add (HF::Attributes::IAttribute * &attr)
{
   attributes.push_back (Attribute (std::move (attr)));
}

// =============================================================================
// Report::Periodic::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::size () const
{
   size_t result = Reference::size () +
                   sizeof(uint8_t);    // Number entries.

   /* *INDENT-OFF* */
   std::for_each(entries.begin(), entries.end(), [&result](const Entry &entry)
   {
      result += entry.size();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Report::Periodic::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::Abstract::pack (array, offset);

   uint8_t _count = std::distance (entries.begin (), entries.end ());

   offset += array.write (offset, _count);

   /* *INDENT-OFF* */
   std::for_each(entries.begin(), entries.end(), [&offset, &array](const Entry &entry)
   {
      offset += entry.pack(array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Report::Periodic::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::unpack (HF::Attributes::FactoryGetter get_factory,
                                 const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Report::Abstract::unpack (array, offset);

   uint8_t count = 0;

   offset += array.read (offset, count);

   for (uint8_t i = 0; i < count; i++)
   {
      Entry entry;
      offset += entry.unpack (get_factory, array, offset);
      add (entry);
   }

   return offset - start;
}

// =============================================================================
// Report::Periodic::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void Report::Periodic::add (Report::Periodic::Entry &entry)
{
   entries.push_front (std::move (entry));
}

// =============================================================================
// Report::Event
// =============================================================================

// =============================================================================
// Report::Event::Field::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::Field::size () const
{
   assert (attribute != nullptr);
   return sizeof(uint8_t) + attribute->size (true);
}

// =============================================================================
// Report::Event::Field::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::Field::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, (uint8_t) type);

   assert (attribute != nullptr);
   offset += attribute->pack (array, offset, true);

   return offset - start;
}

// =============================================================================
// Report::Event::Field::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::Field::unpack (HF::Attributes::Factory factory,
                                     const Common::ByteArray &array, size_t offset)
{
   size_t  start = offset;

   uint8_t temp  = 0xFF;

   offset += array.read (offset, temp);

   type    = (AttributeReporting::Event::Type) temp;

   array.read (offset, temp);
   auto attr = factory (temp);

   if (attr != nullptr)
   {
      size_t _size = attr->unpack (array, offset, true);

      if (_size == attr->size (true))
      {
         set_attribute (attr);
         offset += _size;
      }
   }

   return offset - start;
}

// =============================================================================
// Report::Event::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::Entry::size () const
{
   size_t result = Report::Entry::size ();

   std::for_each (fields.begin (), fields.end (), [&result](const Field &field)
                  {
                     result += field.size ();
                  }
                 );

   return result;
}

// =============================================================================
// Report::Event::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::Entry::pack (array, offset);

   /* *INDENT-OFF* */
   std::for_each (fields.begin (), fields.end (), [&offset, &array](const Field &field)
   {
      offset += field.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Report::Event::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::Entry::unpack (HF::Attributes::Factory factory,
                                     const Common::ByteArray &array, size_t offset)
{
   Field  field;
   size_t result = field.unpack (factory, array, offset);

   if (field.attribute != nullptr)
   {
      add (field);
   }

   return result;
}

// =============================================================================
// Report::Event::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::size () const
{
   size_t result = Reference::size () +
                   sizeof(uint8_t);    // Number entries.

   /* *INDENT-OFF* */
   std::for_each(entries.begin(), entries.end(), [&result](const Entry &entry)
   {
      result += entry.size();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Report::Event::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::Abstract::pack (array, offset);

   uint8_t _count = std::distance (entries.begin (), entries.end ());

   offset += array.write (offset, _count);

   /* *INDENT-OFF* */
   std::for_each(entries.begin(), entries.end(), [&offset, &array](const Entry &entry)
   {
      offset += entry.pack(array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Report::Event::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::unpack (HF::Attributes::FactoryGetter get_factory,
                              const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Report::Abstract::unpack (array, offset);

   uint8_t count = 0;

   offset += array.read (offset, count);

   for (uint8_t i = 0; i < count; i++)
   {
      Entry entry;
      offset += entry.unpack (get_factory, array, offset);
      add (entry);
   }

   return offset - start;
}

// =============================================================================
// Report - Message API
// =============================================================================

// =============================================================================
// Report::CreateMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::CreateMessage::size () const
{
   return destination.size ();
}

// =============================================================================
// Report::CreateMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::CreateMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += destination.pack (array, offset);

   return offset - start;
}

// =============================================================================
// Report::CreateMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::CreateMessage::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += destination.unpack (array, offset);

   return offset - start;
}

// =============================================================================
// Report::Periodic::CreateMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::CreateMessage::size () const
{
   return Report::CreateMessage::size () + sizeof(interval);
}

// =============================================================================
// Report::Periodic::CreateMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::CreateMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::CreateMessage::pack (array, offset);

   offset += array.write (offset, interval);

   return offset - start;
}

// =============================================================================
// Report::Periodic::CreateMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::CreateMessage::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Report::CreateMessage::unpack (array, offset);

   offset += array.read (offset, interval);

   return offset - start;
}

// =============================================================================
// Report::DeleteMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::DeleteMessage::size () const
{
   return report.size ();
}

// =============================================================================
// Report::DeleteMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::DeleteMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += report.pack (array, offset);

   return offset - start;
}

// =============================================================================
// Report::DeleteMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::DeleteMessage::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += report.unpack (array, offset);

   return offset - start;
}

// =============================================================================
// Report::AddEntryMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::AddEntryMessage::size () const
{
   return report.size () + sizeof(uint8_t);
}

// =============================================================================
// Report::AddEntryMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::AddEntryMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += report.pack (array, offset);

   offset += array.write (offset, count ());

   return offset - start;
}

// =============================================================================
// Report::AddEntryMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::AddEntryMessage::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += report.unpack (array, offset);

   uint8_t _count = 0;
   offset += array.read (offset, _count);

   for (uint8_t i = 0; i < _count; i++)
   {
      // TODO Add check if entry was read.
      offset += unpack_entry (array, offset);
   }

   return offset - start;
}

// =============================================================================
// Report::Periodic::AddEntryMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::AddEntryMessage::size () const
{
   size_t result = Report::AddEntryMessage::size ();

   /* *INDENT-OFF* */
   std::for_each (entries.begin(), entries.end(),
                  [&result](const AttributeReporting::Periodic::Entry &entry)
   {
      result += entry.size();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Report::Periodic::AddEntryMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::AddEntryMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::AddEntryMessage::pack (array, offset);

   /* *INDENT-OFF* */
   std::for_each (entries.begin(), entries.end(),
                  [&array, &offset](const AttributeReporting::Periodic::Entry &entry)
   {
      offset += entry.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Report::Periodic::AddEntryMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Periodic::AddEntryMessage::unpack_entry (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   AttributeReporting::Periodic::Entry entry;

   offset += entry.unpack (array, offset);

   entries.push_front (entry);

   return offset - start;
}

// =============================================================================
// Report::Event::AddEntryMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::AddEntryMessage::size () const
{
   size_t result = Report::AddEntryMessage::size ();

   /* *INDENT-OFF* */
   std::for_each (entries.begin(), entries.end(),
                  [&result](const AttributeReporting::Event::Entry &entry)
   {
      result += entry.size();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Report::Event::AddEntryMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::AddEntryMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Report::AddEntryMessage::pack (array, offset);

   /* *INDENT-OFF* */
   std::for_each (entries.begin(), entries.end(),
                  [&array, &offset](const AttributeReporting::Event::Entry &entry)
   {
      offset += entry.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Report::Event::AddEntryMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Report::Event::AddEntryMessage::unpack_entry (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   AttributeReporting::Event::Entry entry;

   offset += entry.unpack (array, offset);

   entries.push_front (entry);

   return offset - start;
}

// =============================================================================
// Message API
// =============================================================================

// =============================================================================
// Response::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Response::size () const
{
   return HF::Protocol::Response::size () + report.size ();
}

// =============================================================================
// Response::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Response::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += HF::Protocol::Response::pack (array, offset);

   offset += report.pack (array, offset);

   return offset - start;
}

// =============================================================================
// Response::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Response::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += HF::Protocol::Response::unpack (array, offset);

   offset += report.unpack (array, offset);

   return offset - start;
}

// =============================================================================
// API
// =============================================================================

// =============================================================================
// AttributeReporting::create
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *AttributeReporting::create (Protocol::Address &destination)
{
   Report::Event::CreateMessage *create_msg = new Report::Event::CreateMessage ();

   assert (create_msg != nullptr);

   if (create_msg == nullptr)
   {
      return nullptr;
   }

   create_msg->destination = destination;

   Protocol::Message *message = new Protocol::Message (create_msg->size ());

   assert (message != nullptr);

   if (message == nullptr)
   {
      return nullptr;
   }

   create_msg->pack (message->payload);

   delete create_msg;

   message->type       = Protocol::Message::COMMAND_REQ;
   message->itf.role   = HF::Interface::SERVER_ROLE;
   message->itf.id     = HF::Interface::ATTRIBUTE_REPORTING;
   message->itf.member = AttributeReporting::CREATE_EVENT_CMD;

   return message;
}

// =============================================================================
// AttributeReporting::create
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *AttributeReporting::create (Protocol::Address &destination, uint32_t interval)
{
   Report::Periodic::CreateMessage *create_msg = new Report::Periodic::CreateMessage ();

   assert (create_msg != nullptr);

   if (create_msg == nullptr)
   {
      return nullptr;
   }

   create_msg->destination = destination;
   create_msg->interval    = interval;

   Protocol::Message *message = new Protocol::Message (create_msg->size ());

   assert (message != nullptr);

   if (message == nullptr)
   {
      return nullptr;
   }

   create_msg->pack (message->payload);

   delete create_msg;

   message->type       = Protocol::Message::COMMAND_REQ;
   message->itf.role   = HF::Interface::SERVER_ROLE;
   message->itf.id     = HF::Interface::ATTRIBUTE_REPORTING;
   message->itf.member = AttributeReporting::CREATE_PERIODIC_CMD;

   return message;
}

// =============================================================================
// AttributeReporting::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *AttributeReporting::destroy (AttributeReporting::Type type, uint8_t report_id)
{
   Reference report (type, report_id);
   return destroy (report);
}

// =============================================================================
// AttributeReporting::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *AttributeReporting::destroy (Reference report)
{
   Report::DeleteMessage *delete_msg = new Report::DeleteMessage ();
   assert (delete_msg != nullptr);

   if (delete_msg == nullptr)
   {
      return nullptr;
   }

   delete_msg->report = report;

   Protocol::Message *message = new Protocol::Message (delete_msg->size ());
   assert (message != nullptr);

   if (message == nullptr)
   {
      return nullptr;
   }

   delete_msg->pack (message->payload);

   delete delete_msg;

   message->type       = Protocol::Message::COMMAND_REQ;
   message->itf.role   = HF::Interface::SERVER_ROLE;
   message->itf.id     = HF::Interface::ATTRIBUTE_REPORTING;
   message->itf.member = AttributeReporting::DELETE_REPORT_CMD;

   return message;
}

// =============================================================================
// AttributeReporting::add
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *AttributeReporting::add (Reference report,
                                              std::vector <Periodic::Entry>::iterator begin,
                                              std::vector <Periodic::Entry>::iterator end)
{
   if (report.type != PERIODIC)
   {
      return nullptr;
   }

   Report::Periodic::AddEntryMessage *add_msg = new Report::Periodic::AddEntryMessage ();
   assert (add_msg != nullptr);

   if (add_msg == nullptr)
   {
      return nullptr;
   }

   add_msg->report = report;
   /* *INDENT-OFF* */
   std::for_each (begin, end, [add_msg](Periodic::Entry &entry)
   {
      add_msg->add (entry);
   });
   /* *INDENT-ON* */

   Protocol::Message *message = new Protocol::Message (add_msg->size ());
   assert (message != nullptr);

   if (message == nullptr)
   {
      return nullptr;
   }

   add_msg->pack (message->payload);

   delete add_msg;

   message->type       = Protocol::Message::COMMAND_REQ;
   message->itf.role   = HF::Interface::SERVER_ROLE;
   message->itf.id     = HF::Interface::ATTRIBUTE_REPORTING;
   message->itf.member = AttributeReporting::ADD_PERIODIC_ENTRY_CMD;

   return message;
}

// =============================================================================
// AttributeReporting::add
// =============================================================================
/*!
 *
 */
// =============================================================================
Protocol::Message *AttributeReporting::add (Reference report,
                                              std::vector <Event::Entry>::iterator begin,
                                              std::vector <Event::Entry>::iterator end)
{
   if (report.type != EVENT)
   {
      return nullptr;
   }

   Report::Event::AddEntryMessage *add_msg = new Report::Event::AddEntryMessage ();
   assert (add_msg != nullptr);

   if (add_msg == nullptr)
   {
      return nullptr;
   }

   add_msg->report = report;
   std::for_each (begin, end, [add_msg](const Event::Entry &entry) {
                     add_msg->add (entry);
                  }
                 );

   Protocol::Message *message = new Protocol::Message (add_msg->size ());
   assert (message != nullptr);

   if (message == nullptr)
   {
      return nullptr;
   }

   add_msg->pack (message->payload);

   delete add_msg;

   message->type       = Protocol::Message::COMMAND_REQ;
   message->itf.role   = HF::Interface::SERVER_ROLE;
   message->itf.id     = HF::Interface::ATTRIBUTE_REPORTING;
   message->itf.member = AttributeReporting::ADD_EVENT_ENTRY_CMD;

   return message;
}
