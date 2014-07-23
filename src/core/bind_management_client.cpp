// =============================================================================
/*!
 * \file       src/core/bind_management_client.cpp
 *
 * This file contains the implementation of the Bind Management : Client Role.
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================
#include "hanfun/core/bind_management.h"

using namespace HF;
using namespace HF::Core;

// =============================================================================
// BindManagement::Client.
// =============================================================================

// =============================================================================
// BindManagement::Client::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindManagement::Client::add (const Protocol::Address &source,
                                  const Protocol::Address &destination,
                                  const Common::Interface &itf)
{
   Protocol::Address addr (0, 0);

   BindManagement::Entry *payload = new BindManagement::Entry ();

   payload->source      = source;
   payload->destination = destination;
   payload->itf         = itf;

   Protocol::Message message (payload->size ());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = BindManagement::Client::uid ();
   message.itf.member = ADD_BIND_CMD;

   payload->pack (message.payload);

   sendMessage (addr, message);

   delete payload;
}

// =============================================================================
// BindManagement::Client::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindManagement::Client::remove (const Protocol::Address &source,
                                     const Protocol::Address &destination,
                                     const Common::Interface &itf)
{
   Protocol::Address addr (0, 0);

   BindManagement::Entry *payload = new BindManagement::Entry ();

   payload->source      = source;
   payload->destination = destination;
   payload->itf         = itf;

   Protocol::Message message (payload->size ());

   message.itf.role   = SERVER_ROLE;
   message.itf.id     = BindManagement::Client::uid ();
   message.itf.member = REMOVE_BIND_CMD;

   payload->pack (message.payload);

   sendMessage (addr, message);

   delete payload;
}


// =============================================================================
// BindManagement::Client::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result BindManagement::Client::handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
{
   Common::Result result   = Common::Result::FAIL_UNKNOWN;

   BindManagement::CMD cmd = static_cast <BindManagement::CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case ADD_BIND_CMD:
      case REMOVE_BIND_CMD:
      {
         Protocol::Response response;
         response.unpack (payload, offset);

         this->response (cmd, response);
         result = Common::Result::OK;
         break;
      }
      default:
         break;
   }

   return result;
}

// =============================================================================
// BindManagement::Client::response
// =============================================================================
/*!
 *
 */
// =============================================================================
void BindManagement::Client::response (const BindManagement::CMD cmd, const Protocol::Response &response)
{
   UNUSED (cmd);
   UNUSED (response);
}
