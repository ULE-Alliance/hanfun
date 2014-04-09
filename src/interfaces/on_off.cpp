// =============================================================================
/*!
 * \file       /HAN-FUN/src/interfaces/level_control.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Level Control interface.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interfaces/on_off.h"

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::OnOff;

// =============================================================================
// create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *create_attribute (uint8_t uid)
{
   return Interfaces::create_attribute ((OnOff::Server *) nullptr, uid);
}
