// =============================================================================
/*!
 * \file       src/units.cpp
 *
 * This file contains the implementation of the common functionality for
 * the HAN-FUN Units.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
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
   Protocol::Packet * packet = new Protocol::Packet (message);

   packet->destination   = addr;
   packet->source.device = device ().address ();
   packet->source.unit   = id ();
   packet->link          = link;

   device ().send (*packet);

   message.reference = packet->message.reference;

   delete packet;
}
