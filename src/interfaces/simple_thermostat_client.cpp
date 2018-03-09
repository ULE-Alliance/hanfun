// =============================================================================
/*!
 * @file       src/interfaces/simple_thermostat_client.cpp
 *
 * This file contains the implementation of the Simple Thermostat interface :
 * Client role.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2015 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/simple_thermostat.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleThermostat;

// =============================================================================
// Simple Thermostat Interface : Client Role
// =============================================================================

#if HF_ITF_STS_BOOST_CMD
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
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   if (!packet.message.isCommandResponse())
   {
      return Common::Result::FAIL_SUPPORT;
   }

   switch (cmd)
   {
      case BOOST_START_CMD:
      {
         Protocol::Response response;
         response.unpack(payload, offset);
         boost_start(packet.source, response.code);
         break;
      }
      case BOOST_STOP_CMD:
      {
         Protocol::Response response;
         response.unpack(payload, offset);
         boost_stop(packet.source, response.code);
         break;
      }
      default:
         return Common::Result::FAIL_ARG;
   }

   return Common::Result::OK;
}

// =============================================================================
// Client::boost_start
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::boost_start(const Protocol::Address &address)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Client::uid();
   message.itf.member = BOOST_START_CMD;

   send(address, message);
}

// =============================================================================
// Client::boost_stop
// =============================================================================
/*!
 *
 */
// =============================================================================
void Client::boost_stop(const Protocol::Address &address)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = Client::uid();
   message.itf.member = BOOST_STOP_CMD;

   send(address, message);
}
#endif
