// =============================================================================
/*!
 * \file       /HAN-FUN/apps/base.h
 *
 * This file contains the definition of a HAN-FUN base.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#ifndef HF_APP_BASE_H
#define HF_APP_BASE_H

#include <string>

#include "hanfun.h"

class Base:public HF::Devices::Concentrator::Base <HF::Devices::Concentrator::DefaultUnit0>
{
   HF::UID::URI _uid;

   public:

   Base(std::string uid):_uid (HF::UID::URI (uid))
   {}

   HF::UID::UID *uid () const
   {
      return (HF::UID::UID *) (&_uid);
   }

   void send (HF::Protocol::Packet &packet)
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Devices::Concentrator::Base <HF::Devices::Concentrator::DefaultUnit0>::send (packet);
   }

   void receive (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset)
   {
      cout << __PRETTY_FUNCTION__ << endl;
      HF::Devices::Concentrator::Base <HF::Devices::Concentrator::DefaultUnit0>::receive (packet, payload, offset);
   }

   protected:

   void route_packet (HF::Protocol::Packet &packet, HF::ByteArray &payload, size_t offset)
   {
      cout << __PRETTY_FUNCTION__ << endl;

      UNUSED (packet);
      UNUSED (payload);
      UNUSED (offset);

      // XXX This will be replaced by the binding core service.
      if (packet.destination.device == HF::Protocol::BROADCAST_ADDR && packet.source.device == 1)
      {
         packet.destination.device = 2;
         packet.destination.unit   = 1;

         packet.link               = nullptr;

         send (packet);
      }
   }
};

#endif /* HF_APP_BASE_H */