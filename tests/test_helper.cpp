// =============================================================================
/*!
 * \file       tests/test_helper.cpp
 *
 * This file contains helper functions used the tests.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.1.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include <stdlib.h>

#include "CppUTest/CommandLineTestRunner.h"

#include "test_helper.h"

#include "hanfun/common.h"

// =============================================================================
// Helper Functions
// =============================================================================

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

   ByteArray    array (data.size ());

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
