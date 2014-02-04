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
using namespace HF::Protocol;

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
Result AbstractInterface::handle (Message &message, ByteArray &payload, size_t offset)
{
   if (uid () != message.itf.uid)
   {
      return Result::FAIL_ID;
   }

   switch (message.type)
   {
      case Message::COMMAND_REQ:
      case Message::COMMAND_RESP_REQ:
      case Message::GET_ATTR_REQ:
      case Message::SET_ATTR_REQ:
      case Message::SET_ATTR_RESP_REQ:
      case Message::SET_ATTR_PACK_REQ:
      case Message::SET_ATTR_PACK_RESP_REQ:
      {
         if (role () != message.itf.role)
         {
            return Result::FAIL_SUPPORT;
         }

         break;
      }
      case Message::COMMAND_RES:
      {
         if (role () == message.itf.role)
         {
            return Result::FAIL_SUPPORT;
         }

         break;
      }
      default:
         break;
   }

   return check_payload_size (message, payload, offset);
}

// =============================================================================
// AbstractInterface::check_payload_size
// =============================================================================
/*!
 *
 */
// =============================================================================
Result AbstractInterface::check_payload_size (Message &message, ByteArray &payload, size_t offset)
{
   size_t _payload_size = payload_size (message.itf);

   if (_payload_size != 0)
   {
      if (message.length < _payload_size || payload.size () < offset ||
          (payload.size () - offset) < _payload_size)
      {
         return Result::FAIL_ARG;
      }
   }

   return Result::OK;
}
