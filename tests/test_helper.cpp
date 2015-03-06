// =============================================================================
/*!
 * @file       tests/test_helper.cpp
 *
 * This file contains helper functions used the tests.
 *
 * @version    1.2.2
 *
 * @copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CppUTest/CommandLineTestRunner.h"

#include "test_helper.h"

// =============================================================================
// Helper Functions
// =============================================================================

STRING_FROM (HF::Common::Interface)

SimpleString StringFrom (const std::vector <uint8_t> &array)
{
   SimpleString result = "";

   for (uint16_t i = 0; i < array.size (); i++)
   {
      result += StringFromFormat ("%02X ", array[i]);
   }

   return result;
}

#ifdef HF_USE_EASTL
SimpleString StringFrom (const __std::vector <uint8_t> &array)
{
   SimpleString result = "";

   for (uint16_t i = 0; i < array.size (); i++)
   {
      result += StringFromFormat ("%02X ", array[i]);
   }

   return result;
}

bool operator ==(const std::vector <uint8_t> &lhs, const __std::vector <uint8_t> &rhs)
{
   if (lhs.size() == rhs.size())
   {
      return std::equal(lhs.begin(), lhs.end(), rhs.begin());
   }
   else
   {
      return false;
   }
}
#endif

SimpleString StringFrom (const HF::Common::Serializable &data)
{
   SimpleString result = "0x";

   Common::ByteArray array (data.size ());

   uint16_t size = data.pack (array);

   result += HexStringFrom ((long int) size);
   result += " : ";
   result += StringFrom (array);

   return result;
}

HF::Attributes::Factory HF::Testing::FactoryGetter (HF::Common::Interface itf)
{
   HF::Attributes::Factory result = HF::Attributes::get_factory (itf);

   if (result == nullptr)
   {
      result = TestInterface::create_attribute;
   }

   return result;
}

// =============================================================================
// Library Overrides
// =============================================================================

extern const char *__progname;

void __assert_fail (const char *__assertion, const char *__file, unsigned int __line,
                    const char *__function)
{
   fprintf (stderr, "%s: %s:%d: %s Assertion `%s' failed.\n", __progname, __file, __line, __function, __assertion);
   // Backtrace.
   int nptrs;
#define SIZE   100
   void *buffer[100];

   nptrs = backtrace (buffer, SIZE);

   backtrace_symbols_fd (buffer, nptrs, STDERR_FILENO);

   abort ();
}

// =============================================================================
// STL - EASTL
// =============================================================================

#ifdef HF_USE_EASTL

#ifdef CPPUTEST_USE_MEM_LEAK_DETECTION
#undef new
#include "CppUTest/TestMemoryAllocator.h"
#include "CppUTest/MemoryLeakDetector.h"
#endif

// EASTL expects us to define these, see allocator.h line 194
void* operator new[] (size_t size, const char* pName, int flags, unsigned debugFlags,
                      const char* file, int line)
{
   UNUSED (pName);
   UNUSED (flags);
   UNUSED (debugFlags);

#if CPPUTEST_USE_MEM_LEAK_DETECTION
   void* memory = MemoryLeakWarningPlugin::getGlobalDetector()->allocMemory(getCurrentNewArrayAllocator(),
                                                                            size, (char*) file, line);
   if (memory == NULL) std::__throw_bad_alloc();

   return memory;
#else
   UNUSED (file);
   UNUSED (line);

   return ::malloc(size);
#endif
}

void* operator new[] (size_t size, size_t alignment, size_t alignmentOffset, const char* pName,
                      int flags, unsigned debugFlags, const char* file, int line)
{
   UNUSED (pName);
   UNUSED (flags);
   UNUSED (debugFlags);
   UNUSED (alignment);
   UNUSED (alignmentOffset);

#if CPPUTEST_USE_MEM_LEAK_DETECTION
   void* memory = MemoryLeakWarningPlugin::getGlobalDetector()->allocMemory(getCurrentNewArrayAllocator(),
                                                                            size, (char*) file, line);
   if (memory == NULL) std::__throw_bad_alloc();

   return memory;
#else
   UNUSED (file);
   UNUSED (line);

  return ::malloc(size);
#endif
}

#endif

// =============================================================================
// Main
// =============================================================================

int main (int ac, char **av)
{
   return CommandLineTestRunner::RunAllTests (ac, av);
}
