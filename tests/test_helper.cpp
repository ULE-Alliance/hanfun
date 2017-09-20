// =============================================================================
/*!
 * @file       tests/test_helper.cpp
 *
 * This file contains helper functions used the tests.
 *
 * @version    1.4.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "CppUTest/CommandLineTestRunner.h"

#include "test_helper.h"

// =============================================================================
// Helper Functions
// =============================================================================

STRING_FROM(HF::Common::Interface)
STRING_FROM(HF::Attributes::IAttribute)

SimpleString StringFrom(const std::vector<uint8_t> &array)
{
   SimpleString result = "";

   for (uint16_t i = 0; i < array.size(); i++)
   {
      result += StringFromFormat("%02X ", array[i]);
   }

   return result;
}

SimpleString StringFrom(const HF::Common::Serializable &data)
{
   SimpleString result = "0x";

   Common::ByteArray array(data.size());

   uint16_t size = data.pack(array);

   result += HexStringFrom((long int) size);
   result += " : ";
   result += StringFrom(array);

   return result;
}

SimpleString StringFrom(const HF::Interfaces::ColourControl::HS_Colour &colour)
{
   SimpleString result = "";

   result += StringFromFormat("%02X ", colour.hue);
   result += StringFromFormat("%02X ", colour.saturation);

   return result;
}

SimpleString StringFrom(const HF::Interfaces::ColourControl::XY_Colour &colour)
{
   SimpleString result = "";

   result += StringFromFormat("%02X ", colour.X);
   result += StringFromFormat("%02X ", colour.Y);

   return result;
}

HF::Attributes::Factory HF::Testing::FactoryGetter(HF::Common::Interface itf)
{
   HF::Attributes::Factory result = HF::Attributes::get_factory(itf);

   if (result == nullptr)
   {
      result = TestInterface::create_attribute;
   }

   return result;
}

void HF::Testing::Assert(const char *expr, const char *file, int line)
{
   mock("support").actualCall("assert")
      .withParameter("expr", expr)
      .withParameter("file", file)
      .withParameter("line", line);
}

HF::Core::DeviceManagement::DevicePtr HF::Testing::DevMgt::create_device(Concentrator &base,
                                                                         uint16_t _dev_addr)
{
   HF::Core::DeviceManagement::Device device;
   device.address = _dev_addr;

   base.unit0()->device_management()->entries().save(device);

   return base.unit0()->device_management()->entries().find(_dev_addr);
}

HF::Core::DeviceManagement::UnitPtr HF::Testing::DevMgt::add_unit(
   HF::Core::DeviceManagement::DevicePtr &device,
   uint8_t unit_id, uint16_t profile)
{
   auto dev_helper = const_cast<HF::Core::DeviceManagement::Device *>(&(*(device)));

   dev_helper->units.emplace_back(unit_id, profile);

   return device->unit(unit_id);
}

HF::Core::DeviceManagement::UnitPtr HF::Testing::DevMgt::add_unit0(
   HF::Core::DeviceManagement::DevicePtr &device)
{
   return add_unit(device, 0, 0);
}

void HF::Testing::DevMgt::fill_unit(HF::Core::DeviceManagement::UnitPtr unit,
                                    HF::Interface::UID uid,
                                    HF::Interface::Role role)
{
   auto unit_helper = const_cast<HF::Core::DeviceManagement::Unit *>(&(*unit));

   unit_helper->interfaces.emplace_back(uid, role);
}

class IAttributeComparator: public MockNamedValueComparator
{
   public:

   bool isEqual(const void *object1, const void *object2)
   {
      return ((HF::Attributes::IAttribute *) object1)->compare(
         *((HF::Attributes::IAttribute *) object2))
             == 0;
   }

   SimpleString valueToString(const void *object)
   {
      return StringFrom(*((const HF::Attributes::IAttribute *) object));
   }
};

// =============================================================================
// Library Overrides
// =============================================================================

#if Backtrace_FOUND

extern const char *__progname;

void __assert_fail(const char *__assertion, const char *__file, unsigned int __line,
                   const char *__function)
{
   fprintf(stderr, "%s: %s:%d: %s Assertion `%s' failed.\n", __progname, __file, __line, __function,
           __assertion);
   // Backtrace.
   int nptrs;
   #define SIZE   100
   void *buffer[100];

   nptrs = backtrace(buffer, SIZE);

   backtrace_symbols_fd(buffer, nptrs, STDERR_FILENO);

   abort();
}

#endif

// =============================================================================
// Main
// =============================================================================

int main(int ac, char **av)
{
   IAttributeComparator iattr_comparator;

   mock().installComparator("IAttribute", iattr_comparator);

   return CommandLineTestRunner::RunAllTests(ac, av);
}
