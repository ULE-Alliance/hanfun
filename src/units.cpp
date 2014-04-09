// =============================================================================
/*!
 * \file       src/units.cpp
 *
 * This file contains the implementation of the common functionality for
 * the HAN-FUN Units.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include "hanfun/units.h"


// =============================================================================
// HF::Units::AbstractUnit::sendMessage
// =============================================================================
/*!
 *
 */
// =============================================================================
void HF::Units::AbstractUnit::sendMessage (Protocol::Address &addr, Protocol::Message &message,
                                                Transport::Link *link)
{
   Protocol::Packet packet(message);

   packet.destination = addr;
   packet.source.device = device()->address();
   packet.source.unit = id();
   packet.link = link;

   device()->send(packet);
}
