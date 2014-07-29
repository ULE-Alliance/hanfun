// =============================================================================
/*!
 * \file       src/interfaces/level_control.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Level Control interface.
 *
 * \version    0.3.2
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/interfaces/level_control.h"

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::LevelControl;

// =============================================================================
// create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *create_attribute (uint8_t uid)
{
   return Interfaces::create_attribute ((LevelControl::Server *) nullptr, uid);
}
