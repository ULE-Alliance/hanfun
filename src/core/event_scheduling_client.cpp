// =============================================================================
/*!
 * @file       src/core/event_scheduling_client.cpp
 *
 * This file contains the implementation of the Event Scheduling service : Client role.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/event_scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;

// =============================================================================
// Event::IClient::define_event
// =============================================================================
/*!
 *
 */
// =============================================================================
void Event::IClient::define_event(const Protocol::Address &addr, uint8_t id, uint8_t status,
                                  Interval &time, uint8_t pid)
{
   DefineEvent<Interval> msg(id, status, time, pid);
   Protocol::Message message(msg.size());
   msg.pack(message.payload);

   /* *INDENT-OFF* */
   HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = ITF;
   message.itf.member = DEFINE_EVENT_CMD;

   send(addr, message);
}
