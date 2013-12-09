// =============================================================================
/*!
 * \file       tests/test_helper.cpp
 *
 * This file contains helper functions used the tests.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
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

SimpleString StringFrom (const HF::ByteArray &array)
{
   SimpleString result = "";

   for (size_t i = 0; i < array.size (); i++)
   {
      result += StringFromFormat ("%02X ", array[i]);
   }

   return result;
}

SimpleString StringFrom (const HF::Serializable &data)
{
   SimpleString result = "0x";

   ByteArray    array (data.size ());

   size_t size = data.pack (array);

   result += HexStringFrom (size);
   result += " : ";
   result += StringFrom (array);

   return result;
}

// =============================================================================
// Main
// =============================================================================

int main (int ac, char **av)
{
   return CommandLineTestRunner::RunAllTests (ac, av);
}
