// =============================================================================
/*!
 * \file       src/interfaces/alert.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Alert interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#include "hanfun/interfaces/alert.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// Alert / Message
// =============================================================================

Alert::Message::Message(uint16_t type, uint32_t state):
   type (type), state (state)
{}

// =============================================================================
// Alert::Message::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Alert::Message::size () const
{
   return sizeof(uint16_t) + // Profile UID.
          sizeof(uint32_t);  // State.
}

// =============================================================================
// Alert::Message::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Alert::Message::pack (ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += array.write (offset, this->type);
   offset += array.write (offset, this->state);

   return offset - start;
}

// =============================================================================
// Alert::Message::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Alert::Message::unpack (const ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += array.read (offset, this->type);
   offset += array.read (offset, this->state);

   return offset - start;
}
