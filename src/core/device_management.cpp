// =============================================================================
/*!
 * \file       src/core/device_management.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Device Management core interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/core/device_management.h"

using namespace HF;
using namespace HF::Core;

// =============================================================================
// Optional Interface.
// =============================================================================

// =============================================================================
// DeviceManagement::Interface::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Interface::size () const
{
   return sizeof(uint16_t);
}

// =============================================================================
// DeviceManagement::Interface::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Interface::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   uint16_t itf = ((this->role & 0x01) << 15) | (this->uid & HF::Interface::MAX_UID);

   offset += array.write (offset, itf);

   return offset - start;
}

// =============================================================================
// DeviceManagement::Interface::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Interface::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;
   uint16_t itf;

   offset    += array.read (offset, itf);

   this->role = (itf & ~HF::Interface::MAX_UID) >> 15;
   this->uid  = itf & HF::Interface::MAX_UID;

   return offset - start;

}

// =============================================================================
// Unit Entry
// =============================================================================

// =============================================================================
// DeviceManagement::Unit::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Unit::size () const
{
   size_t result = sizeof(uint8_t) +   // Unit entry size.
                   sizeof(uint8_t) +   // Unit ID.
                   sizeof(uint16_t);   // Unit's profile UID.

   if (!opt_ift.empty ())
   {
      Interface temp;
      result += sizeof(uint8_t); // Number of optional units.
      result += (temp.size () * opt_ift.size ());
   }

   return result;
}

// =============================================================================
// DeviceManagement::Unit::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Unit::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   size_t size  = this->size () - sizeof(uint8_t);

   offset += array.write (offset, (uint8_t) size); // Dummy write.
   offset += array.write (offset, this->id);       // Unit ID.
   offset += array.write (offset, this->profile);  // Unit's profile UID.

   // Pack the existing optional interfaces.
   if (!opt_ift.empty ())
   {
      offset += array.write (offset, (uint8_t) opt_ift.size ());

      for (vector <Interface>::const_iterator itf = opt_ift.begin (); itf != opt_ift.end (); ++itf)
      {
         offset += itf->pack (array, offset);
      }
   }

   return offset - start;
}

// =============================================================================
// DeviceManagement::Unit::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Unit::unpack (const ByteArray &array, size_t offset)
{
   size_t  start = offset;

   uint8_t size  = 0;
   offset += array.read (offset, size);

   offset += array.read (offset, this->id);
   offset += array.read (offset, this->profile);

   // Unpack the existing optional interfaces.
   if (offset - start < size)
   {
      uint8_t count = 0;

      offset += array.read (offset, count);   // Number of optional interfaces.

      for (uint8_t i = 0; i < count; i++)
      {
         Interface itf;
         offset += itf.unpack (array, offset);
         this->opt_ift.push_back (itf);
      }
   }

   return offset - start;
}

// =============================================================================
// Device Entry.
// =============================================================================

// =============================================================================
// DeviceManagement::Device::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Device::size () const
{
   size_t result = sizeof(uint16_t) +  // Device Address.
                   sizeof(uint8_t);    // Number of units.

   for (vector <Unit>::const_iterator unit = units.begin (); unit != units.end (); ++unit)
   {
      result += unit->size ();
   }

   return result;
}

// =============================================================================
// DeviceManagement::Device::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Device::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, address);

   offset += array.write (offset, (uint8_t) units.size ());

   for (vector <Unit>::const_iterator unit = units.begin (); unit != units.end (); ++unit)
   {
      offset += unit->pack (array, offset);
   }

   return offset - start;
}

// =============================================================================
// DeviceManagement::Device::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::Device::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += array.read (offset, address);

   uint8_t count = 0;
   offset += array.read (offset, count);

   for (uint8_t i = 0; i < count; i++)
   {
      Unit unit;
      offset += unit.unpack (array, offset);
      units.push_back (unit);
   }

   return offset - start;
}

// =============================================================================
// Register Command Messages
// =============================================================================

DeviceManagement::RegisterMessage::~RegisterMessage()
{
   units.clear ();

   if (_uid != nullptr)
   {
      delete _uid;
   }
}

// =============================================================================
// DeviceManagement::RegisterMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::RegisterMessage::size () const
{
   size_t result = sizeof(uint8_t);    // Discriminator Type.

   result += (_uid ? _uid->size () : 1);

   if (emc != 0x0000)
   {
      result += sizeof(uint16_t);
   }

   result += sizeof(uint8_t); // Number of units.

   for (vector <Unit>::const_iterator unit = units.begin (); unit != units.end (); ++unit)
   {
      result += unit->size ();
   }

   return result;
}

// =============================================================================
// DeviceManagement::RegisterMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::RegisterMessage::pack (ByteArray &array, size_t offset) const
{
   size_t  start = offset;

   uint8_t temp  = (_uid != nullptr ? _uid->type () : 0);

   if (emc != 0x0000)
   {
      temp |= 0x80;
   }

   offset += array.write (offset, temp);

   if (_uid != nullptr)
   {
      offset += _uid->pack (array, offset);
   }
   else
   {
      HF::UID::NONE none_uid;
      offset += none_uid.pack (array, offset);
   }

   if (emc != 0x0000)
   {
      offset += array.write (offset, emc);
   }

   temp    = units.size ();
   offset += array.write (offset, temp);

   for (vector <Unit>::const_iterator unit = units.begin (); unit != units.end (); ++unit)
   {
      offset += unit->pack (array, offset);
   }

   return offset - start;
}

// =============================================================================
// DeviceManagement::RegisterMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::RegisterMessage::unpack (const ByteArray &array, size_t offset)
{
   size_t  start = offset;

   uint8_t type  = 0;
   offset += array.read (offset, type);

   bool emc = (type & 0x80) != 0;

   type &= ~0x80;

   switch (type)
   {
      case HF::UID::NONE_UID:
         _uid = new HF::UID::NONE ();
         break;
      case HF::UID::RFPI_UID:
         _uid = new HF::UID::RFPI ();
         break;
      case HF::UID::IPUI_UID:
         _uid = new HF::UID::IPUI ();
         break;
      case HF::UID::MAC_UID:
         _uid = new HF::UID::MAC ();
         break;
      case HF::UID::URI_UID:
         _uid = new HF::UID::URI ();
         break;
      default:
         break;
   }

   if (_uid != nullptr)
   {
      offset += _uid->unpack (array, offset);
   }

   if (emc)
   {
      offset += array.read (offset, this->emc);
   }

   uint8_t count = 0;
   offset += array.read (offset, count);

   for (uint8_t i = 0; i < count; i++)
   {
      Unit unit;
      offset += unit.unpack (array, offset);
      units.push_back (unit);
   }

   return offset - start;
}

// =============================================================================
// DeviceManagement::RegisterResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::RegisterResponse::size () const
{
   return Response::size () +                       // Parent Size.
          sizeof(uint16_t) +                        // Device Address.
          ((emc != 0x0000) ? sizeof(uint16_t) : 0); // EMC.
}

// =============================================================================
// DeviceManagement::RegisterResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::RegisterResponse::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Response::pack (array, offset);

   uint16_t temp = address;

   if (emc != 0x0000)
   {
      temp |= 0x8000;
   }

   offset += array.write (offset, temp);

   if (emc != 0x0000)
   {
      offset += array.write (offset, emc);
   }

   return offset - start;
}

// =============================================================================
// DeviceManagement::RegisterResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::RegisterResponse::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Response::unpack (array, offset);

   uint16_t temp;
   offset += array.read (offset, temp);

   address = temp & ~0x8000;

   if ((temp & 0x8000) != 0)
   {
      offset += array.read (offset, emc);
   }

   return offset - start;
}

// =============================================================================
// De-register Command Messages.
// =============================================================================

// =============================================================================
// DeviceManagement::DeregisterMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::DeregisterMessage::size () const
{
   return sizeof(uint16_t);
}

// =============================================================================
// DeviceManagement::DeregisterMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::DeregisterMessage::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, address);

   return offset - start;
}

// =============================================================================
// DeviceManagement::DeregisterMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::DeregisterMessage::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += array.read (offset, address);

   return offset - start;
}

// =============================================================================
// Read Session Messages
// =============================================================================

// =============================================================================
// DeviceManagement::StartSessionResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::StartSessionResponse::size () const
{
   return Response::size () + // Parent size.
          sizeof(uint16_t);   // Number of entries field.
}

// =============================================================================
// DeviceManagement::StartSessionResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::StartSessionResponse::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Response::pack (array, offset);
   offset += array.write (offset, count);

   return offset - start;
}

// =============================================================================
// DeviceManagement::StartSessionResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::StartSessionResponse::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Response::unpack (array, offset);
   offset += array.read (offset, count);

   return offset - start;
}

// =============================================================================
// DeviceManagement::GetEntriesMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::GetEntriesMessage::size () const
{
   return sizeof(uint16_t) + // Offset
          sizeof(uint8_t);   // Count
}

// =============================================================================
// DeviceManagement::GetEntriesMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::GetEntriesMessage::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, this->offset);
   offset += array.write (offset, this->count);

   return offset - start;
}

// =============================================================================
// DeviceManagement::GetEntriesMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::GetEntriesMessage::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += array.read (offset, this->offset);
   offset += array.read (offset, this->count);

   return offset - start;
}

// =============================================================================
// DeviceManagement::GetEntriesResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::GetEntriesResponse::size () const
{
   size_t result = Response::size () + // Parent size.
                   sizeof(uint8_t);    // Number of entries.

   for (vector <Device>::const_iterator device = entries.begin (); device != entries.end (); ++device)
   {
      result += device->size ();
   }

   return result;
}

// =============================================================================
// DeviceManagement::GetEntriesResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::GetEntriesResponse::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Response::pack (array, offset);
   offset += array.write (offset, (uint8_t) entries.size ());

   for (vector <Device>::const_iterator device = entries.begin (); device != entries.end (); ++device)
   {
      offset += device->pack (array, offset);
   }

   return offset - start;
}

// =============================================================================
// DeviceManagement::GetEntriesResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeviceManagement::GetEntriesResponse::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Response::unpack (array, offset);

   uint8_t size;
   offset += array.read (offset, size);

   for (uint8_t i = 0; i < size; i++)
   {
      Device device;
      offset += device.unpack (array, offset);
      entries.push_back (device);
   }

   return offset - start;
}
