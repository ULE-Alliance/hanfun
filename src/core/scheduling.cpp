// =============================================================================
/*!
 * @file       /HANFUN/src/core/scheduling.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Scheduling service.
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



#include "hanfun/core/scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(IServer *server, Interface::UID itf_uid,
                                                   uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return Scheduling::create_attribute(itf_uid, uid);
   }
}

HF::Attributes::IAttribute *Scheduling::create_attribute(Interface::UID itf_uid, uint8_t uid)
{
   using namespace HF::Core::Scheduling;

   Scheduling::Attributes attr = static_cast<Scheduling::Attributes>(uid);

   switch (attr)
   {
      case MAXIMUM_NUMBER_OF_ENTRIES_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(itf_uid, attr,
                                                       MaximumNumberOfEntries::WRITABLE);
      }

      case NUMBER_OF_ENTRIES_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(itf_uid, attr, NumberOfEntries::WRITABLE);
      }

      case STATUS_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(itf_uid, attr, Status::WRITABLE);
      }

      default:
         return nullptr;
   }
}

uint16_t ActivateScheduler::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   array.write(offset, status);

   return min_size;
}

uint16_t ActivateScheduler::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   array.read(offset, status);

   return min_size;
}

uint16_t UpdateStatus::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.write(offset, event_id);
   offset += array.write(offset, static_cast<uint8_t>((status << 7) & 0x80));
   return min_size;
}

uint16_t UpdateStatus::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   offset += array.read(offset, event_id);
   offset += array.read(offset, status);
   status  = (status >> 7) & 0x01;

   return min_size;
}

uint16_t DefineEventResponse::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint8_t start = offset;

   offset += Protocol::Response::pack(array, offset);

   if (code != Common::Result::OK)
   {
      goto _end;
   }

   offset += array.write(offset, event_id);

   _end:
   return offset - start;
}

uint16_t DefineEventResponse::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint8_t start = offset;

   offset += Protocol::Response::unpack(array, offset);

   if (code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, 1);

   offset += array.read(offset, event_id);

   _end:
   return offset - start;
}

uint16_t GetEntry::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   array.write(offset, event_id);

   return min_size;
}

uint16_t GetEntry::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   array.read(offset, event_id);

   return min_size;
}
