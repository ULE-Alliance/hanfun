// =============================================================================
/*!
 * @file       src/core/event_scheduling.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Event Scheduling service.
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

#include "hanfun/core/scheduling.h"
#include "hanfun/core/event_scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(Event::IServer *server, uint8_t uid)
{
   return create_attribute(static_cast<Scheduling::IServer *>(server),
                           HF::Interface::EVENT_SCHEDULING, uid);
}

// =============================================================================
// Event::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Event::create_attribute(uint8_t uid)
{
   return Scheduling::create_attribute(HF::Interface::EVENT_SCHEDULING, uid);
}

// =============================================================================
// Event::Interval
// =============================================================================

// =============================================================================
// Event::Interval::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Event::Interval::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.write(offset, start);
   offset += array.write(offset, end);
   offset += array.write(offset, repeat);
   return min_size;
}

// =============================================================================
// Event::Interval::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Event::Interval::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.read(offset, start);
   offset += array.read(offset, end);
   offset += array.read(offset, repeat);
   return min_size;
}

// =============================================================================
// Event::IServer::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Event::IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                              uint16_t offset)
{
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case DEFINE_EVENT_CMD:
      {
         HF::Core::Scheduling::Entry<Interval> msg;
         msg.unpack(payload, offset);
         return define_event(packet, msg);
      }

      case UPDATE_STATUS_CMD:
      {
         UpdateStatus msg;
         msg.unpack(payload, offset);
         return update_event_status(packet, msg);
      }

      case GET_ENTRY_CMD:
      {
         GetEntry msg;
         msg.unpack(payload, offset);
         return get_event_entry(packet, msg);
      }

      case DELETE_CMD:
      {
         DeleteEvent msg;
         msg.unpack(payload, offset);
         return delete_event(packet, msg);
      }

      case DELETE_ALL_CMD:
      {
         return delete_all_events(packet);
      }

      default:
         return Scheduling::IServer::handle_command(packet, payload, offset);
   }

   return Common::Result::OK;
}
