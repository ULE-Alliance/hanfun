// =============================================================================
/*!
 * \file       src/protocol.cpp
 *
 * This file contains the implementation of the classes for the protocol layer
 * in the HAN-FUN specification.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/protocol.h"
#include "hanfun/interface.h"

using namespace HF;
using namespace HF::Protocol;

// =============================================================================
// Address
// =============================================================================

// =============================================================================
// Address::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Address::size () const
{
   return sizeof(uint16_t) + // Device Address + Flag.
          sizeof(uint8_t);   // Unit Address.
}

// =============================================================================
// Message::Address::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Address::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   uint16_t dev = ((this->mod & 0x01) << 15) | (this->device & BROADCAST_ADDR);

   offset += array.write (offset, dev);
   offset += array.write (offset, unit);

   return offset - start;
}

// =============================================================================
// Message::Address::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Address::unpack (const Common::ByteArray &array, size_t offset)
{
   uint16_t dev;
   size_t   start = offset;

   offset      += array.read (offset, dev);

   this->mod    = (dev & ~BROADCAST_ADDR) >> 15;
   this->device = dev & BROADCAST_ADDR;

   offset      += array.read (offset, this->unit);

   return offset - start;
}

// =============================================================================
// Message / Interface
// =============================================================================

// =============================================================================
// Message::Interface::::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Message::Interface::size () const
{
   return Common::Interface::size () + // Interface UID.
          sizeof(uint8_t);             // Interface Member.
}

// =============================================================================
// Message::Interface::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Message::Interface::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += Common::Interface::pack (array, offset);
   offset += array.write (offset, this->member);

   return offset - start;
}

// =============================================================================
// Message::Interface::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Message::Interface::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Common::Interface::unpack (array, offset);
   offset += array.read (offset, this->member);

   return offset - start;
}

// =============================================================================
// Message
// =============================================================================

Message::Message(Message &parent, size_t size):
   reference (parent.reference), itf (parent.itf), payload (Common::ByteArray (size)), length (0)
{
   switch (parent.type)
   {
      case COMMAND_REQ:
      case COMMAND_RESP_REQ:
         type = COMMAND_RES;
         break;
      case GET_ATTR_REQ:
         type = GET_ATTR_RES;
         break;
      case SET_ATTR_REQ:
      case SET_ATTR_RESP_REQ:
         type = SET_ATTR_RES;
         break;
      case GET_ATTR_PACK_REQ:
         type = GET_ATTR_PACK_RES;
         break;
      case SET_ATTR_PACK_REQ:
      case SET_ATTR_PACK_RESP_REQ:
         type = SET_ATTR_PACK_RES;
         break;
      case ATOMIC_SET_ATTR_PACK_REQ:
      case ATOMIC_SET_ATTR_PACK_RESP_REQ:
         type = ATOMIC_SET_ATTR_PACK_RES;
         break;
      case COMMAND_RES:
      case SET_ATTR_RES:
      case GET_ATTR_RES:
      case GET_ATTR_PACK_RES:
      case SET_ATTR_PACK_RES:
      case ATOMIC_SET_ATTR_PACK_RES:
      default:
         type = parent.type;
         break;
   }
}

// =============================================================================
// Message::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Message::size () const
{
   return sizeof(uint8_t) +    // Application Reference.
          sizeof(uint8_t) +    // Message Type.
          itf.size () +        // Interface Addr.
          sizeof(uint16_t) +   // Payload Length Value.
          payload.size ();     // Payload Length.
}

// =============================================================================
// Message::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Message::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   // Application Reference.
   offset += array.write (offset, this->reference);

   // Message Type.
   offset += array.write (offset, static_cast <uint8_t>(this->type));

   // Interface Address.
   offset += itf.pack (array, offset);

   // Payload Length.
   uint16_t length = payload.size () % MAX_PAYLOAD;

   offset += array.write (offset, length);

   array.extend (length);

   copy (payload.begin (), payload.end (), array.begin () + offset);

   offset += payload.size ();

   return offset - start;
}

// =============================================================================
// Message::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Message::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   // Application Reference.
   offset += array.read (offset, this->reference);

   // Message Type.
   uint8_t type = 0;
   offset    += array.read (offset, type);
   this->type = static_cast <Type>(type);

   // Interface Address
   offset += itf.unpack (array, offset);

   // Payload Length.
   offset       += array.read (offset, this->length);
   this->length &= MAX_PAYLOAD;

   return offset - start;
}

// =============================================================================
// Response
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
   return sizeof(uint8_t);
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

   offset += array.write (offset, static_cast <uint8_t>(this->code));

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
   size_t  start = offset;

   uint8_t code  = 0;
   offset    += array.read (offset, code);

   this->code = static_cast <Common::Result>(code);

   return offset - start;
}

// =============================================================================
// Packet
// =============================================================================

// =============================================================================
// Packet::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Packet::size () const
{
   return source.size () +       // Source Address.
          destination.size () +  // Destination Address.
          sizeof(uint16_t) +     // Transport header layer header.
          message.size ();       // Message payload size.
}

// =============================================================================
// Packet::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Packet::pack (Common::ByteArray &array, size_t offset) const
{
   uint16_t transport = 0;
   size_t   start     = offset;

   offset += source.pack (array, offset);
   offset += destination.pack (array, offset);

   offset += array.write (offset, transport);

   offset += message.pack (array, offset);

   return offset - start;
}

// =============================================================================
// Packet::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Packet::unpack (const Common::ByteArray &array, size_t offset)
{
   uint16_t transport = 0;
   size_t   start     = offset;

   offset += source.unpack (array, offset);
   offset += destination.unpack (array, offset);

   offset += array.read (offset, transport);

   offset += message.unpack (array, offset);

   return offset - start;
}

// =============================================================================
// Attributes API
// =============================================================================

// =============================================================================
// GetAtrributePack::Response
// =============================================================================

// =============================================================================
// GetAtrributePack::Response::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t GetAttributePack::Response::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += Protocol::Response::unpack (array, offset);

   if (!array.available (offset, sizeof(count)))
   {
      count = 0;
      goto _end;
   }

   offset += array.read (offset, count);

   if (attribute_factory == nullptr)
   {
      goto _end;
   }

   for (uint8_t i = 0; i < count; i++)
   {
      uint8_t uid;

      if (!array.available (offset, sizeof(uid)))
      {
         goto _end;
      }

      offset += array.read (offset, uid);

      HF::Attributes::IAttribute *attr = attribute_factory (uid);

      if (attr == nullptr)
      {
         goto _end;
      }

      if (!array.available (offset, attr->size ()))
      {
         goto _end;
      }

      offset += attr->unpack (array, offset);

      attributes.push_back (attr);
   }

   _end:
   return offset - start;
}

// =============================================================================
// Filters Classes
// =============================================================================

// =============================================================================
// Filters::Repeated::checksum
// =============================================================================
/*!
 *
 */
// =============================================================================
uint32_t Filters::Repeated::checksum (uint16_t const *data, size_t words)
{
   uint32_t sum1 = 0xFFFF, sum2 = 0xFFFF;

   while (words)
   {
      unsigned tlen = words > 359 ? 359 : words;
      words -= tlen;
      do
      {
         sum2 += sum1 += *data++;
      } while (--tlen);

      sum1 = (sum1 & 0xFFFF) + (sum1 >> 16);
      sum2 = (sum2 & 0xFFFF) + (sum2 >> 16);
   }

   /* Second reduction step to reduce sums to 16 bits */
   sum1 = (sum1 & 0xFFFF) + (sum1 >> 16);
   sum2 = (sum2 & 0xFFFF) + (sum2 >> 16);

   return sum2 << 16 | sum1;
}

// =============================================================================
// Filters::Repeated::operator ()
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Filters::Repeated::operator () (const HF::Protocol::Packet &packet, const HF::Common::ByteArray &payload)
{
#define MAX_TTL   std::numeric_limits<uint8_t>::max()

   bool result = false;

   Entry temp(packet.source.device, packet.message.reference);

   temp.checksum = checksum((uint16_t *) payload.data(), payload.size() / 2);

   auto it = std::lower_bound(db.begin(), db.end(), temp);

   // An entry was found.
   if (it != db.end() && it->address == temp.address)
   {
      if (it->reference != temp.reference || temp.checksum != it->checksum)
      {
         temp.ttl = (it->ttl + 1 < MAX_TTL ? it->ttl + 1 : MAX_TTL);
         *it = temp;
      }
      else
      {
         result = true;
      }
   }
   else
   {
      // Database full.
      if (db.size() == max_size)
      {
         // Find the oldest entry, i.e., the one with the lowest ttl value.
         auto oldest = std::min_element(db.begin(), db.end(), [](const Entry &lhs, const Entry &rhs){
            return lhs.ttl < rhs.ttl;
         });

         db.erase(oldest);
      }

      db.insert(it, temp);
   }

   return result;
}


// =============================================================================
// Filters::ResponseRequired::operator ()
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Filters::ResponseRequired::operator ()(const HF::Protocol::Packet &packet)
{
   if (response (packet.message.type))
   {
      Entry temp (packet);
      db.push_front (temp);
      return false;
   }

   /* *INDENT-OFF* */
   auto it = std::find_if (db.begin(), db.end(), [&packet](const Entry entry)
   {
      return (entry.address == packet.source.device) && (entry.itf == packet.message.itf) &&
              matches (entry.type, packet.message.type);
   });
   /* *INDENT-ON* */

   if (it != db.end ())
   {
      db.erase (it);
      return false;
   }

   if (request (packet.message.type, true))
   {
      return true;
   }

   // FIXME return HF::Interfaces::response(packet.message.itf);
   return false;
}

// =============================================================================
// Helper Functions
// =============================================================================

// =============================================================================
// Protocol::request
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Protocol::request (Message::Type type, bool response)
{
   switch (type)
   {
      case Message::Type::COMMAND_RESP_REQ:
      case Message::Type::SET_ATTR_RESP_REQ:
      case Message::Type::SET_ATTR_PACK_RESP_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RESP_REQ:
      {
         response = false;
      }
      case Message::Type::COMMAND_REQ:
      case Message::Type::GET_ATTR_REQ:
      case Message::Type::SET_ATTR_REQ:
      case Message::Type::GET_ATTR_PACK_REQ:
      case Message::Type::SET_ATTR_PACK_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_REQ:
      {
         return !response;
      }
      case Message::Type::COMMAND_RES:
      case Message::Type::SET_ATTR_RES:
      case Message::Type::GET_ATTR_RES:
      case Message::Type::GET_ATTR_PACK_RES:
      case Message::Type::SET_ATTR_PACK_RES:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RES:
      default:
         return false;
   }
}

// =============================================================================
// Protocol::responce
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Protocol::response (Message::Type type)
{
   switch (type)
   {
      case Message::Type::COMMAND_REQ:
      case Message::Type::COMMAND_RESP_REQ:
      case Message::Type::GET_ATTR_REQ:
      case Message::Type::SET_ATTR_REQ:
      case Message::Type::SET_ATTR_RESP_REQ:
      case Message::Type::GET_ATTR_PACK_REQ:
      case Message::Type::SET_ATTR_PACK_REQ:
      case Message::Type::SET_ATTR_PACK_RESP_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RESP_REQ:
      {
         return false;
      }
      case Message::Type::COMMAND_RES:
      case Message::Type::SET_ATTR_RES:
      case Message::Type::GET_ATTR_RES:
      case Message::Type::GET_ATTR_PACK_RES:
      case Message::Type::SET_ATTR_PACK_RES:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RES:
      {
         return true;
      }
      default:
         return false;
   }
}


// =============================================================================
// Protocol::response
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Protocol::matches (Message::Type lhs, Message::Type rhs)
{
   if (lhs == rhs)
   {
      return false;
   }

   switch (lhs)
   {
      case Message::Type::COMMAND_REQ:
      case Message::Type::COMMAND_RESP_REQ:
      {
         return (rhs == Message::Type::COMMAND_RES);
      }
      case Message::Type::COMMAND_RES:
      {
         return (rhs == Message::Type::COMMAND_REQ || rhs == Message::Type::COMMAND_RESP_REQ);
      }

      case Message::Type::GET_ATTR_REQ:
      {
         return (rhs == Message::Type::GET_ATTR_RES);
      }
      case Message::Type::GET_ATTR_RES:
      {
         return (rhs == Message::Type::GET_ATTR_REQ);
      }

      case Message::Type::SET_ATTR_REQ:
      case Message::Type::SET_ATTR_RESP_REQ:
      {
         return (rhs == Message::Type::SET_ATTR_RES);
      }
      case Message::Type::SET_ATTR_RES:
      {
         return (rhs == Message::Type::SET_ATTR_REQ || rhs == Message::Type::SET_ATTR_RESP_REQ);
      }

      case Message::Type::GET_ATTR_PACK_REQ:
      {
         return (rhs == Message::Type::GET_ATTR_PACK_RES);
      }
      case Message::Type::GET_ATTR_PACK_RES:
      {
         return (rhs == Message::Type::GET_ATTR_PACK_REQ);
      }

      case Message::Type::SET_ATTR_PACK_REQ:
      case Message::Type::SET_ATTR_PACK_RESP_REQ:
      {
         return (rhs == Message::Type::SET_ATTR_PACK_RES);
      }
      case Message::Type::SET_ATTR_PACK_RES:
      {
         return (rhs == Message::Type::SET_ATTR_PACK_REQ || rhs == Message::Type::SET_ATTR_PACK_RESP_REQ);
      }

      case Message::Type::ATOMIC_SET_ATTR_PACK_REQ:
      case Message::Type::ATOMIC_SET_ATTR_PACK_RESP_REQ:
      {
         return (rhs == Message::Type::ATOMIC_SET_ATTR_PACK_RES);
      }
      case Message::Type::ATOMIC_SET_ATTR_PACK_RES:
      {
         return (rhs == Message::Type::ATOMIC_SET_ATTR_PACK_REQ || rhs == Message::Type::ATOMIC_SET_ATTR_PACK_RESP_REQ);
      }
      default:
         return false;
   }
}
