// =============================================================================
/*!
 * @file       src/core/suota_server.cpp
 *
 * This file contains the implementation of the SUOTA interface : Server role.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/suota.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::SUOTA;

// =============================================================================
// SUOTA Interface : Server Role
// =============================================================================

// =============================================================================
// Server::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Server::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case CHECK_VERSION_CMD:
      {
         check_version(packet.source);
         break;
      }

      case UPGRADE_COMPLETE_CMD:
      {
         upgrade_complete(packet.source);
         break;
      }

      default:
         return Common::Result::FAIL_SUPPORT;
   }

   return Common::Result::OK;
}

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Server::new_version_available
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::new_version_available(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SUOTA;
   message.itf.member = NEW_VERSION_AVAILABLE_CMD;

   send(addr, message);
}

// =============================================================================
// Server::check_version
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::check_version(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

// =============================================================================
// Server::upgrade_complete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Server::upgrade_complete(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}
