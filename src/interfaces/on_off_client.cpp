// =============================================================================
/*!
 * \file       src/interfaces/on_off_client.cpp
 *
 * This file contains the implementation of the On-Off interface : Client role.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/on_off.h"

using namespace HF;
using namespace HF::Interfaces;

// =============================================================================
// On-Off Interface : Client Role
// =============================================================================

// =============================================================================
// OnOffClient::on
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffClient::on (Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = uid ();
   message.itf.member = ON_CMD;

   sendMessage (addr, message);
}

// =============================================================================
// OnOffClient::off
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffClient::off (Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = uid ();
   message.itf.member = OFF_CMD;

   sendMessage (addr, message);
}

// =============================================================================
// OnOffClient::toggle
// =============================================================================
/*!
 *
 */
// =============================================================================
void OnOffClient::toggle (Protocol::Address &addr)
{
   Protocol::Message message;

   message.itf.role   = SERVER_ROLE;
   message.itf.uid    = uid ();
   message.itf.member = TOGGLE_CMD;

   sendMessage (addr, message);
}
