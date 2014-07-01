// =============================================================================
/*!
 * \file       apps/node.h
 *
 * This file contains the definition of the Node class that represents the
 * HAN-FUN Node on the application.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#ifndef HF_APP_BASE_H
#define HF_APP_BASE_H

#include "hanfun.h"

/*!
 * This class represents a node in the HAN-FUN network.
 */
struct Node:public HF::Devices::Node::Abstract <HF::Devices::Node::DefaultUnit0>
{
   void receive (HF::Protocol::Packet &packet, HF::Common::ByteArray &payload, size_t offset);
};

struct SimpleLight:public HF::Units::Unit <HF::Profiles::SimpleLight>
{
   SimpleLight(uint8_t index, HF::IDevice &device):
      HF::Units::Unit <HF::Profiles::SimpleLight>(index, device)
   {}

   void on ();

   void off ();

   void toggle ();
};

struct SimpleSwitch:public HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>
{
   SimpleSwitch(uint8_t index, HF::IDevice &device):
      HF::Units::Unit <HF::Profiles::SimpleOnOffSwitch>(index, device)
   {}
};

#endif /* HF_APP_BASE_H */
