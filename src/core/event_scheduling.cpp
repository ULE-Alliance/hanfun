// =============================================================================
/*!
 * @file       src/core/event_scheduling.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Event Scheduling service.
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
#include "hanfun/core/event_scheduling.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::Scheduling;
using namespace HF::Core::Scheduling::Event;

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(Server *server, uint8_t uid)
{
   return create_attribute(static_cast<Scheduling::IServer *>(server),
                           HF::Interface::EVENT_SCHEDULING, uid);
}

HF::Attributes::IAttribute *Event::create_attribute(uint8_t uid)
{
   return Scheduling::create_attribute(HF::Interface::EVENT_SCHEDULING, uid);
}

uint16_t Interval::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.write(offset, start);
   offset += array.write(offset, end);
   offset += array.write(offset, repeat);
   return min_size;
}

uint16_t Interval::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   offset += array.read(offset, start);
   offset += array.read(offset, end);
   offset += array.read(offset, repeat);
   return min_size;
}
