// =============================================================================
/*!
 * \file       src/uids.cpp
 *
 * This file contains the implementation of the UIDs classes in HAN-FUN.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"

#include "hanfun/uids.h"

using namespace HF;

// =============================================================================
// API
// =============================================================================

// =============================================================================
// UID::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t UID::pack (UID &uid, Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   if (!array.available (offset + uid.size (), sizeof(uint8_t)))
   {
      array.extend (sizeof(uint8_t));
      array.push_back (0);
   }

   offset += array.write (offset, uid.type ());
   offset += uid.pack (array, offset);

   return offset - start;
}

// =============================================================================
// UID::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t UID::unpack (UID * &uid, Common::ByteArray &array, size_t offset)
{
   size_t  start = offset;

   uint8_t type  = Type::NONE_UID;

   offset += array.read (offset, type);

   switch (type)
   {
      case HF::UID::NONE_UID:
         uid = new HF::UID::NONE ();
         break;

      case HF::UID::IPUI_UID:
         uid = new HF::UID::IPUI ();
         break;

      case HF::UID::RFPI_UID:
         uid = new HF::UID::RFPI ();
         break;

      case HF::UID::URI_UID:
         uid = new HF::UID::URI ();
         break;
   }

   uid->unpack (array, offset);

   return offset - start;
}
