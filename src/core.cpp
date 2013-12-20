// =============================================================================
/*!
 * \file       src/core.cpp
 *
 * This file contains the implementation of common functionality in the core
 * services and interfaces of the HAN-FUN protocol.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"
#include "hanfun/core.h"

#include "hanfun/devices.h"

// =============================================================================
// AbstractService
// =============================================================================

// =============================================================================
// AbstractService::sendMessage
// =============================================================================
/*!
 *
 */
// =============================================================================
void AbstractService::sendMessage (Message::Address &addr, Message &message)
{
   _device->sendMessage( *this, addr, message);
}
