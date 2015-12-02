// =============================================================================
/*!
 * @file       src/core/suota_client.cpp
 *
 * This file contains the implementation of the SUOTA interface : Client role.
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
// SUOTA Interface : Client Role
// =============================================================================

// =============================================================================
// Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Client::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                      uint16_t offset)
{
   UNUSED(payload);
   UNUSED(offset);

   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case NEW_VERSION_AVAILABLE_CMD:
      {
         new_version_available(packet.source);
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
// Client::new_version_available
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::new_version_available(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   UNUSED(addr);
}

#ifdef HF_CORE_SUOTA_CHECK_VERSION_CMD
// =============================================================================
// Client::check_version
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::check_version(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SUOTA;
   message.itf.member = CHECK_VERSION_CMD;

   send(addr, message);
}
#endif

// =============================================================================
// Client::upgrade_complete
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::upgrade_complete(const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   Protocol::Message message;

   message.itf.role   = CLIENT_ROLE;
   message.itf.id     = Interface::SUOTA;
   message.itf.member = UPGRADE_COMPLETE_CMD;

   send(addr, message);
}
