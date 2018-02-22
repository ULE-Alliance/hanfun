// =============================================================================
/*!
 * @file       src/interfaces/simple_keypad.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Simple Keypad interface.
 *
 * @version    1.5.1
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/interfaces/simple_keypad.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Interfaces;
using namespace HF::Interfaces::SimpleKeypad;

// =============================================================================
// Interfaces::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Interfaces::create_attribute(Server *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return SimpleKeypad::create_attribute(uid);
   }
}

// =============================================================================
// SimpleKeypad::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *SimpleKeypad::create_attribute(uint8_t uid)
{
   UNUSED(uid);
   return nullptr;
}

// =============================================================================
// KeyPressed::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t KeyPressed::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   array.write(offset, key_id);

   return min_size;
}

// =============================================================================
// KeyPressed::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t KeyPressed::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   array.read(offset, key_id);

   return min_size;
}
