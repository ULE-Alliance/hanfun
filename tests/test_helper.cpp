// =============================================================================
/*!
 * \file       tests/test_helper.cpp
 *
 * This file contains helper functions used the tests.
 *
 * \version    0.3.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include <stdlib.h>

#include "CppUTest/CommandLineTestRunner.h"

#include "test_helper.h"

// =============================================================================
// Helper Functions
// =============================================================================

STRING_FROM (HF::Common::Interface)

SimpleString StringFrom (const HF::Common::ByteArray &array)
{
   SimpleString result = "";

   for (size_t i = 0; i < array.size (); i++)
   {
      result += StringFromFormat ("%02X ", array[i]);
   }

   return result;
}

SimpleString StringFrom (const HF::Common::Serializable &data)
{
   SimpleString result = "0x";

   Common::ByteArray array (data.size ());

   size_t size = data.pack (array);

   result += HexStringFrom (size);
   result += " : ";
   result += StringFrom (array);

   return result;
}

std::ostream &operator <<(std::ostream &os, std::uint8_t val)
{
   return os << static_cast <int>(val);
}

// =============================================================================
// Main
// =============================================================================

int main (int ac, char **av)
{
   return CommandLineTestRunner::RunAllTests (ac, av);
}
