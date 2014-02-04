// =============================================================================
/*!
 * \file       src/interfaces.cpp
 *
 * This file contains the implementation of the interfaces common code.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"
#include "hanfun/interface.h"

using namespace HF;

// =============================================================================
// AbstractInterface API.
// =============================================================================

// =============================================================================
// AbstractInterface::handle
// =============================================================================
/*!
 *
 */
// =============================================================================
Result AbstractInterface::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   // Only handle message that are for this interface and are
   // from the complementing role.
   if (uid() == message.itf.uid && role() != message.itf.role)
   {
      return check_payload_size(message, payload, offset);
   }
   else
   {
      return (uid() != message.itf.uid ? Result::FAIL_ID : Result::FAIL_SUPPORT);
   }
}

// =============================================================================
// AbstractInterface::check_payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
Result AbstractInterface::check_payload_size (Protocol::Message &message, ByteArray &payload,
                                                size_t offset)
{
   size_t _payload_size = payload_size(message.itf);

   if (_payload_size != 0)
   {
      if (message.length < _payload_size || payload.size() < offset ||
          (payload.size() - offset) < _payload_size)
      {
         return Result::FAIL_ARG;
      }
   }

   return Result::OK;
}
