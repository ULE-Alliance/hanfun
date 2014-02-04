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
Result AlertClient::handle (Protocol::Message &message, ByteArray &payload, size_t offset)
{
   // Check for correct interface and command.
   Result result = AbstractInterface::handle (message, payload, offset);

   if (result != Result::OK)
   {
      return result;
   }

   if (message.itf.member != Alert::STATUS_CMD)
   {
      return Result::FAIL_SUPPORT;
   }

   Message alert_msg;

   alert_msg.unpack (payload, offset);

   status (alert_msg);

   return Result::OK;
}
