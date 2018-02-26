// =============================================================================
/*!
 * @file       src/core/weekly_scheduling.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Weekly Scheduling service.
 *
 * @version    1.5.1
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/scheduling.h"
#include "hanfun/core/weekly_scheduling.h"

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
HF::Attributes::IAttribute *Core::create_attribute(Weekly::IServer *server, uint8_t uid)
{
   return create_attribute(static_cast<Scheduling::IServer *>(server),
                           HF::Interface::WEEKLY_SCHEDULING, uid);
}

// =============================================================================
// Weekly::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Weekly::create_attribute(uint8_t uid)
{
   return Scheduling::create_attribute(HF::Interface::WEEKLY_SCHEDULING, uid);
}

// =============================================================================
// Weekly::Day
// =============================================================================

// =============================================================================
// Weekly::Day::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Weekly::Day::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.write(offset, day);
   offset += array.write(offset, hour);
   offset += array.write(offset, minute);
   return min_size;
}

// =============================================================================
// Weekly::Day::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Weekly::Day::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.read(offset, day);
   offset += array.read(offset, hour);
   offset += array.read(offset, minute);
   return min_size;
}

// =============================================================================
// Weekly::IServer::handle_command
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Weekly::IServer::handle_command(Protocol::Packet &packet, Common::ByteArray &payload,
                                               uint16_t offset)
{
   CMD cmd = static_cast<CMD>(packet.message.itf.member);

   switch (cmd)
   {
      case DEFINE_EVENT_CMD:
      {
         HF::Core::Scheduling::Entry<Day> msg;
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
