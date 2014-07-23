// =============================================================================
/*!
 * \file       src/interfaces/alert_client.cpp
 *
 * This file contains the implementation of the Alert interface : Client role.
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/alert.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// Alert Interface : Client Role
// =============================================================================

// =============================================================================
// AlertClient::handle_commands
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Alert::Client::handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   if (packet.message.itf.member != Alert::STATUS_CMD)
   {
      return Common::Result::FAIL_SUPPORT;
   }

   Message alert_msg;

   alert_msg.unpack (payload, offset);

   status (alert_msg);

   return Common::Result::OK;
}
