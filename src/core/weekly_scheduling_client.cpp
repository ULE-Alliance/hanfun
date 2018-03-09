// =============================================================================
/*!
 * @file       src/core/weekly_scheduling_client.cpp
 *
 * This file contains the implementation of the Weekly Scheduling service : Client role.
 *
 * @version    1.5.2
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/weekly_scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;

// =============================================================================
// Weekly::IClient::define_event
// =============================================================================
/*!
 *
 */
// =============================================================================
void Weekly::IClient::define_event(const Protocol::Address &addr, uint8_t id, uint8_t status,
                                   Day &time, uint8_t pid)
{
   HF::Core::Scheduling::Entry<Day> msg(id, status, time, pid);
   Protocol::Message message(msg.size());
   msg.pack(message.payload);

   HF_ASSERT(addr.unit == 0, {return;});

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = ITF;
   message.itf.member = DEFINE_EVENT_CMD;

   send(addr, message);
}
