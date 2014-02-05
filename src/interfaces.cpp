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
Result AbstractInterface::handle (Packet &packet, ByteArray &payload, size_t offset)
{
   Result result = check_message (packet.message, payload, offset);

   if (result != Result::OK)
   {
      return result;
   }

   result = check_payload_size (packet.message, payload, offset);

   if (result != Result::OK)
   {
      return result;
   }

   if (packet.message.type >= Message::COMMAND_REQ && packet.message.type <= Message::COMMAND_RES)
   {
      return handle_command (packet, payload, offset);
   }
   else if (packet.message.type >= Message::GET_ATTR_REQ && packet.message.type <= Message::SET_ATTR_PACK_RES)
   {
      return handle_attribute (packet, payload, offset);
   }

   return Result::FAIL_UNKNOWN;
}


// =============================================================================
// AbstractInterface::check_message
// =============================================================================
/*!
 *
 */
// =============================================================================
Result AbstractInterface::check_message (Message &message, ByteArray &payload, size_t offset)
{
   UNUSED (payload);
   UNUSED (offset);

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

   return Result::OK;
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


// =============================================================================
// AbstractInterface::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Result AbstractInterface::handle_command (Packet &packet, ByteArray &payload, size_t offset)
{
   UNUSED (packet);
   UNUSED (payload);
   UNUSED (offset);

   return Result::FAIL_UNKNOWN;
}

// =============================================================================
// AbstractInterface::handle_attributes
// =============================================================================
/*!
 *
 *
 */
// =============================================================================
Result AbstractInterface::handle_attribute (Packet &packet, ByteArray &payload, size_t offset)
{
   UNUSED (packet);
   UNUSED (payload);
   UNUSED (offset);

   return Result::FAIL_UNKNOWN;
}
