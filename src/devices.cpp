// =============================================================================
/*!
 * \file       src/devices.cpp
 *
 * This file contains the implementation of the Devices API.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include <stdlib.h>

#include "hanfun/devices.h"

using namespace HF;

// =============================================================================
// AbstractDevice
// =============================================================================

// =============================================================================
// AbstractDevice::add
// =============================================================================
/*!
 *
 */
// =============================================================================
void AbstractDevice::add (IUnit *unit)
{
   _units.push_back (unit);
}

// =============================================================================
// AbstractDevice::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
void AbstractDevice::remove (IUnit *unit)
{
   for (std::vector <IUnit *>::iterator it = _units.begin (); it != _units.end (); ++it)
   {
      if (*it == unit)
      {
         _units.erase (it);
         break;
      }
   }
}

// =============================================================================
// AbstractDevice::sendMessage
// =============================================================================
/*!
 *
 */
// =============================================================================
void AbstractDevice::sendMessage (IUnit &unit, Protocol::Address &addr, Protocol::Message &message)
{
   UNUSED (unit);
   UNUSED (addr);
   UNUSED (message);
}
