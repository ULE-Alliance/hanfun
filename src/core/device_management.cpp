// =============================================================================
/*!
 * @file       src/core/device_management.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Device Management core interface.
 *
 * @version    1.1.1
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/device_management.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::DeviceManagement;

// =============================================================================
// DeviceManagement::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *DeviceManagement::create_attribute (uint8_t uid)
{
   return Core::create_attribute ((DeviceManagement::IServer *) nullptr, uid);
}

// =============================================================================
// Unit Entry
// =============================================================================

// =============================================================================
// Unit::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Unit::size () const
{
   size_t result = sizeof(uint8_t) +   // Unit entry size.
                   sizeof(uint8_t) +   // Unit ID.
                   sizeof(uint16_t);   // Unit's profile UID.

   if (!interfaces.empty ())
   {
      Common::Interface temp;
      result += sizeof(uint8_t); // Number of optional units.
      result += (temp.size () * interfaces.size ());
   }

   return result;
}

// =============================================================================
// Unit::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Unit::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   size_t size  = this->size () - sizeof(uint8_t);

   offset += array.write (offset, (uint8_t) size); // Dummy write.
   offset += array.write (offset, this->id);       // Unit ID.
   offset += array.write (offset, this->profile);  // Unit's profile UID.

   // Pack the existing optional interfaces.
   if (!interfaces.empty ())
   {
      offset += array.write (offset, (uint8_t) interfaces.size ());

      /* *INDENT-OFF* */
      std::for_each(interfaces.begin (), interfaces.end (),
                    [&offset, &array](const HF::Common::Interface &itf)
      {
         offset += itf.pack (array, offset);
      });
      /* *INDENT-ON* */
   }

   return offset - start;
}

// =============================================================================
// Unit::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Unit::unpack (const Common::ByteArray &array, size_t offset)
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
         Common::Interface itf;
         offset += itf.unpack (array, offset);
         this->interfaces.push_back (itf);
      }
   }

   return offset - start;
}

// =============================================================================
// Unit::has_interface
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Unit::has_interface (uint16_t itf_uid, HF::Interface::Role role) const
{
   // Search the official interfaces.
   uint16_t count;
   const Common::Interface *itf = Profiles::interfaces (this->profile, count);

   Common::Interface temp (itf_uid, role);

   if (itf != nullptr)
   {
      for (uint16_t i = 0; i < count; ++i, ++itf)
      {
         if (*itf == temp)
         {
            return true;
         }
      }
   }
   else  // Search the optional interfaces.
   {
      /* *INDENT-OFF* */
      return std::any_of (interfaces.begin (), interfaces.end (),
                           [&temp](const Common::Interface &itf)
                           {
                              return temp == itf;
                           });
      /* *INDENT-ON* */
   }

   return false;
}

// =============================================================================
// Device Entry.
// =============================================================================

// =============================================================================
// Device::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Device::size () const
{
   size_t result = sizeof(uint16_t) +  // Device Address.
                   sizeof(uint8_t);    // Number of units.

   /* *INDENT-OFF* */
   std::for_each (units.begin (), units.end (), [&result](const Unit &unit)
   {
      result += unit.size ();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// Device::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Device::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, address);

   offset += array.write (offset, (uint8_t) units.size ());

   /* *INDENT-OFF* */
   std::for_each (units.begin (), units.end (), [&array, &offset](const Unit &unit)
   {
      offset += unit.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// Device::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Device::unpack (const Common::ByteArray &array, size_t offset)
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

RegisterMessage::~RegisterMessage()
{
   units.clear ();
}

// =============================================================================
// RegisterMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t RegisterMessage::size () const
{
   size_t result = uid.size ();  // UID Size.

   if (emc != 0x0000)
   {
      result += sizeof(uint16_t);
   }

   result += sizeof(uint8_t); // Number of units.

   /* *INDENT-OFF* */
   std::for_each (units.begin (), units.end (), [&result](const Unit &unit)
   {
      result += unit.size ();
   });
   /* *INDENT-ON* */

   return result;
}

// =============================================================================
// RegisterMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t RegisterMessage::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += uid.pack (array, offset);

   if (emc != 0x0000)
   {
      array[start] |= 0x80;
      offset       += array.write (offset, emc);
   }

   uint8_t temp = units.size ();
   offset += array.write (offset, temp);

   /* *INDENT-OFF* */
   std::for_each (units.begin (), units.end (), [&array, &offset](const Unit &unit)
   {
      offset += unit.pack (array, offset);
   });
   /* *INDENT-ON* */

   return offset - start;
}

// =============================================================================
// RegisterMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t RegisterMessage::unpack (const Common::ByteArray &array,
                                size_t offset)
{
   size_t start = offset;

   offset += uid.unpack (array, offset);

   if ((array[start] & 0x80) != 0)
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
// RegisterResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t RegisterResponse::size () const
{
   return Response::size () +                       // Parent Size.
          sizeof(uint16_t) +                        // Device Address.
          ((emc != 0x0000) ? sizeof(uint16_t) : 0); // EMC.
}

// =============================================================================
// RegisterResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t RegisterResponse::pack (Common::ByteArray &array, size_t offset) const
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
// RegisterResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t RegisterResponse::unpack (const Common::ByteArray &array,
                                 size_t offset)
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
// DeregisterMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeregisterMessage::size () const
{
   return sizeof(uint16_t);
}

// =============================================================================
// DeregisterMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeregisterMessage::pack (Common::ByteArray &array,
                                size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, address);

   return offset - start;
}

// =============================================================================
// DeregisterMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeregisterMessage::unpack (const Common::ByteArray &array,
                                  size_t offset)
{
   size_t start = offset;

   offset += array.read (offset, address);

   return offset - start;
}


// =============================================================================
// DeregisterResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeregisterResponse::size () const
{
   return Protocol::Response::size () + sizeof(address);
}

// =============================================================================
// DeregisterResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeregisterResponse::pack (Common::ByteArray &array,
                                 size_t offset) const
{
   size_t start = offset;

   offset += Protocol::Response::pack (array, offset);

   offset += array.write (offset, address);

   return offset - start;
}

// =============================================================================
// DeregisterResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t DeregisterResponse::unpack (const Common::ByteArray &array,
                                   size_t offset)
{
   size_t start = offset;

   offset  += Protocol::Response::unpack (array, offset);

   offset  += array.read (offset, address);

   address &= Protocol::BROADCAST_ADDR;

   return offset - start;
}

// =============================================================================
// IEntries API - Default Implementation
// =============================================================================

// =============================================================================
// Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
DevicePtr Entries::find (uint16_t address) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [address](const Device &device)
   {
      return device.address == address;
   });
   /* *INDENT-ON* */

   if (it == db.end ())
   {
      return std::move (DevicePtr ());
   }
   else
   {
      return std::move (DevicePtr (*(it.base ())));
   }
}

// =============================================================================
// Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
DevicePtr Entries::find (const HF::UID::UID &uid) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&uid](const Device &device)
   {
      return device.uid == uid;
   });
   /* *INDENT-ON* */

   if (it == db.end ())
   {
      return std::move (DevicePtr ());
   }
   else
   {
      return std::move (DevicePtr (*(it.base ())));
   }
}

// =============================================================================
// Entries::save
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::save (const Device &entry)
{
   if (entry.address == HF::Protocol::BROADCAST_ADDR)
   {
      return Common::Result::FAIL_UNKNOWN;
   }

   // Add new entry into the database.

   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&entry](const Device &other)
   {
      return entry.address == other.address;
   });
   /* *INDENT-ON* */

   if (it != db.end ()) // Update existing entry.
   {
      db.erase (it);
   }

   db.push_back (entry);

   return Common::Result::OK;
}

// =============================================================================
// Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::destroy (const Device &entry)
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&entry](const Device &other)
   {
      return entry.address == other.address;
   });
   /* *INDENT-ON* */

   if (it == db.end ())
   {
      return Common::Result::FAIL_ARG;
   }

   auto res = Common::Result::OK;

   db.erase (it);

   return res;
}

// =============================================================================
// DefaultServer::next_address
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entries::next_address () const
{
   uint16_t address    = START_ADDR;

   auto address_equals = [&address](const Device &device)
                         {
                            return device.address == address;
                         };

   while (std::any_of (db.begin (), db.end (), address_equals))
   {
      if (++address == Protocol::BROADCAST_ADDR)
      {
         break;
      }
   }

   return address;
}
