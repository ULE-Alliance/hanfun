// =============================================================================
/*!
 * @file       src/core/scheduling_client.cpp
 *
 * This file contains the implementation of the Client functionality for the
 * Scheduling service.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;

// =============================================================================
// Event Scheduling Interface : Client Role
// =============================================================================

// =============================================================================
// Commands
// =============================================================================

// =============================================================================
// Client::activate_scheduler
// =============================================================================
/*!
 *
 */
// =============================================================================
void IClient::activate_scheduler(Interface::UID itf_uid, const Protocol::Address &addr,
                                 uint8_t _status)
{
   ActivateScheduler msg(_status);

   Protocol::Message message(msg.size());
   msg.pack(message.payload);

   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = itf_uid;
   message.itf.member = ACTIVATE_SCHEDULER_CMD;

   send(addr, message);
}

#ifdef HF_CORE_EVENT_SCHEDULING_UPDATE_EVENT_STATUS_CMD
// =============================================================================
// Client::update_event_status
// =============================================================================
/*!
 *
 */
// =============================================================================
void IClient::update_event_status(Interface::UID itf_uid, const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = itf_uid;
   message.itf.member = UPDATE_STATUS_CMD;

   send(addr, message);
}
#endif

#ifdef HF_CORE_EVENT_SCHEDULING_GET_EVENT_ENTRY_CMD
// =============================================================================
// Client::get_event_entry
// =============================================================================
/*!
 *
 */
// =============================================================================
void IClient::get_event_entry(Interface::UID itf_uid, const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = itf_uid;
   message.itf.member = GET_ENTRY_CMD;

   send(addr, message);
}
#endif

// =============================================================================
// Client::delete_event
// =============================================================================
/*!
 *
 */
// =============================================================================
void IClient::delete_event(Interface::UID itf_uid, const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = itf_uid;
   message.itf.member = DELETE_CMD;

   send(addr, message);
}

#ifdef HF_CORE_EVENT_SCHEDULING_DELETE_ALL_EVENTS_CMD
// =============================================================================
// Client::delete_all_events
// =============================================================================
/*!
 *
 */
// =============================================================================
void IClient::delete_all_events(Interface::UID itf_uid, const Protocol::Address &addr)
{
   // FIXME Generated Stub.
   /* *INDENT-OFF* */
  HF_ASSERT(addr.unit == 0, { return; });
   /* *INDENT-ON* */

   Protocol::Message message;

   message.itf.role   = HF::Interface::SERVER_ROLE;
   message.itf.id     = itf_uid;
   message.itf.member = DELETE_ALL_CMD;

   send(addr, message);
}
#endif
