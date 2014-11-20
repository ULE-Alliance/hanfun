// =============================================================================
/*!
 * @file       src/uids.cpp
 *
 * This file contains the implementation of the UIDs classes in HAN-FUN.
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

#include "hanfun/common.h"

#include "hanfun/uids.h"

using namespace HF::UID;

// =============================================================================
// API
// =============================================================================

// =============================================================================
// UID::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t UID::unpack (const Common::ByteArray &array, uint16_t offset)
{
   uint16_t start = offset;

   if (owner)
   {
      delete _raw;
   }

   _raw  = nullptr;
   owner = true;

   uint8_t type = Type::NONE_UID;

   array.read (offset, type);

   // Remove upper bit so it can be used for other purposes.
   type &= (~0x80);

   switch (type)
   {
      case HF::UID::NONE_UID:
         _raw = new HF::UID::NONE ();
         break;

      case HF::UID::DECT_UID:
         _raw = new HF::UID::DECT ();
         break;

      case HF::UID::MAC_UID:
         _raw = new HF::UID::MAC ();
         break;

      case HF::UID::URI_UID:
         _raw = new HF::UID::URI ();
         break;
   }

   offset += _raw->unpack (array, offset);

   return offset - start;
}
