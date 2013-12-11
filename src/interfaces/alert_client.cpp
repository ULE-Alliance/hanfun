// =============================================================================
/*!
 * \file       src/interfaces/alert_server.cpp
 *
 * This file contains the implementation of the Alert interface : Client role.
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
// Alert Interface : Client Role
// =============================================================================

// =============================================================================
// AlertClient::handle
// =============================================================================
/*!
 *
 */
// =============================================================================
bool AlertClient::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   Message alert_msg;

   // Check for correct interface and command.
   if (!AbstractInterface::handle (message, payload, offset) ||
       message.itf.member != Alert::STATUS_CMD)
   {
      return false;
   }

   // Check payload size.
   if (message.length < alert_msg.size () || (payload.size () - offset) < alert_msg.size ())
   {
      return false;
   }

   alert_msg.unpack (payload, offset);

   status (alert_msg);

   return true;
}
