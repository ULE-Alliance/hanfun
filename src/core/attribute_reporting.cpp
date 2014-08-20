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
// Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::size () const
{
   return sizeof(unit) + itf.size () + sizeof(unit);
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
   size_t  start = offset;

   uint8_t temp  = ((this->type & 0x01) << 7) | (this->id & 0x7F);

   offset += array.write (offset, temp);

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
   size_t  start = offset;

   uint8_t temp  = 0;
   offset    += array.read (offset, temp);

   this->type = ((temp & 0x80) != 0);
   this->id   = temp & 0x7F;

   offset    += destination.unpack (array, offset);

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
      result += (sizeof(uint8_t) + attributes.size ());
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
      offset += array.write (offset, (uint8_t) attributes.size ());

      /* *INDENT-OFF* */
      std::for_each(attributes.begin(), attributes.end(), [&array, &offset](uint8_t uid)
      {
         offset += array.write (offset, uid);
      });
      /* *INDENT-ON* */
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

   uint8_t _size = 0;
   offset += array.read (offset, _size);

   for (uint8_t i = 0; i < _size; i++)
   {
      uint8_t temp;

      offset += array.read (offset, temp);

      attributes.push_back (temp);
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
   size_t result = AttributeReporting::Rule::size () + sizeof(interval);

   result += sizeof(uint8_t);  // Number of entries.

   /* *INDENT-OFF* */
   std::for_each( entries.begin(), entries.end(), [&result](const Entry &entry)
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

   offset += array.write (offset, interval);

   offset += array.write (offset, (uint8_t) entries.size ());

   std::for_each (entries.begin (), entries.end (), [&offset, &array](const Entry &entry)
                  {
                     offset += entry.pack (array, offset);
                  }
                 );

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

   if (this->type != PERIODIC)
   {
      goto end;
   }

   offset += array.read (offset, interval);

   offset += array.read (offset, size);

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
size_t Event::Field::size (bool _attr_uid) const
{
   size_t result = sizeof(uint8_t);    // Field type.

   // Size of value holder or COV value.
   result += sizeof(uint8_t);

   // Number of bytes in value.
   result += (type != Event::COV ? value.size () : 0);

   // Attribute UID for dynamic entries.
   if (_attr_uid)
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
size_t Event::Field::pack (Common::ByteArray &array, size_t offset, bool _attr_uid) const
{
   size_t start = offset;

   if (_attr_uid)
   {
      offset += array.write (offset, attr_uid);
   }

   offset += array.write (offset, (uint8_t) type);

   if (type == Event::COV)
   {
      offset += array.write (offset, value.at (0));
   }
   else
   {
      offset += array.write (offset, (uint8_t) value.size ());

      auto dst = array.begin ();
      dst += offset;

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
size_t Event::Field::unpack (const Common::ByteArray &array, size_t offset, bool _attr_uid)
{
   size_t start = offset;

   if (_attr_uid)
   {
      offset += array.read (offset, attr_uid);
   }

   uint8_t temp = 0;
   offset += array.read (offset, temp);

   type    = static_cast <Event::Type>(temp);

   if (type == Event::COV)
   {
      temp = 1;
   }
   else
   {
      // Read value size.
      offset += array.read (offset, temp);
   }

   auto start_it = array.begin ();
   start_it += offset;

   auto end_it = start_it;
   end_it += temp;

   value   = Common::ByteArray (temp);

   std::copy (start_it, end_it, value.begin ());

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

   /* *INDENT-OFF* */
   std::for_each(fields.begin(), fields.end(), [&result, attr_uid](const Event::Field &field)
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

   /* *INDENT-OFF* */
   std::for_each(fields.begin(), fields.end(), [&offset, &array, attr_uid](const Event::Field &field)
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
         offset += field.unpack (array, offset);
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
   std::for_each(entries.begin(), entries.end(), [&result](const Entry &entry)
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

   offset += array.write (offset, (uint8_t) entries.size ());

   std::for_each (entries.begin (), entries.end (),
                  [&offset, &array](const Entry &entry)
                  {
                     offset += entry.pack (array, offset);
                  }
                 );

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
      entries.push_back (entry);
   }

   return offset - start;
}
