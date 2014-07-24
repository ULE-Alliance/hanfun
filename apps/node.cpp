// =============================================================================
/*!
 * \file       apps/node.cpp
 *
 * This file contains the implementation of the Node class that represents the
 * HAN-FUN Node on the example application.
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "node.h"

#include "common.h"

// #define HF_LOG_LEVEL HF_LOG_LEVEL_TRACE
#include "application.h"

// =============================================================================
// Node
// =============================================================================

// =============================================================================
// Node::receive
// =============================================================================
/*!
 *
 */
// =============================================================================
void Node::receive (HF::Protocol::Packet &packet, HF::Common::ByteArray &payload, size_t offset)
{
   LOG (DEBUG) << ">>>>>>>>>>>>> Message Received <<<<<<<<<<<<<" << NL;

   LOG (TRACE) << "Payload : " << payload << NL;

   LOG (DEBUG) << packet << NL;

   HF::Devices::Node::Abstract <HF::Devices::Node::DefaultUnit0>::receive (packet, payload, offset);
}

// =============================================================================
// Simple Light
// =============================================================================

void SimpleLight::on ()
{
   LOG (INFO) << ">>>>>>>>>>>>> SimpleLight : ON <<<<<<<<<<<<<" << NL;
}

void SimpleLight::off ()
{
   LOG (INFO) << ">>>>>>>>>>>>> SimpleLight : OFF <<<<<<<<<<<<<" << NL;
}

void SimpleLight::toggle ()
{
   LOG (INFO) << ">>>>>>>>>>>>> SimpleLight : TOGGLE <<<<<<<<<<<<<" << NL;
}
