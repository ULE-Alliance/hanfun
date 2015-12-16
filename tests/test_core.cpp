// =============================================================================
/*!
 * @file       tests/test_core.cpp
 *
 * This file contains the implementation of the tests for the code that implements
 * core functionality of HAN-FUN.
 *
 * @version    1.4.0
 *
 * @copyright  Copyright &copy; &nbsp; 2015 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/common.h"

#include "hanfun/core.h"
#include "hanfun/core/device_management.h"
#include "hanfun/core/suota.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Common;
using namespace HF::Core;
using namespace HF::Testing;

// =============================================================================
// Helper Classes
// =============================================================================


namespace
{
   using Interface_1 = HF::Core::Service<HF::Interface::POWER>;
   using Interface_2 = HF::Core::Service<HF::Interface::RSSI>;
   using Interface_3 = HF::Core::Service<HF::Interface::TIME>;

   template<class Base>
   struct ServiceParentHelper: public Base
   {
      ServiceParentHelper(HF::Core::Unit0 &unit0): Base(unit0)
      {}

      HF::Interface::Role role() const
      {
         return HF::Interface::SERVER_ROLE;
      }
   };

   struct TestInterface_1: public ServiceParentHelper<Interface_1>
   {
      TestInterface_1(HF::Core::Unit0 &unit0):
         ServiceParentHelper<Interface_1>(unit0)
      {}
   };

   struct TestInterface_2: public ServiceParentHelper<Interface_2>
   {
      TestInterface_2(HF::Core::Unit0 &unit0):
         ServiceParentHelper<Interface_2>(unit0)
      {}
   };

   struct TestInterface_3: public ServiceParentHelper<Interface_3>
   {
      TestInterface_3(HF::Core::Unit0 &unit0):
         ServiceParentHelper<Interface_3>(unit0)
      {}
   };

   using UnitImpl = HF::Unit0<HF::Core::Unit0, HF::Core::DeviceInformation::Server,
                              HF::Core::DeviceManagement::Client,
                              HF::Core::AttributeReporting::Server,
                              TestInterface_1, TestInterface_2, TestInterface_3>;

   struct TestUnit0: public UnitImpl, public HF::IDevice::IUnit0
   {
      TestUnit0(HF::IDevice &device): UnitImpl(device)
      {}

      DeviceInfo *device_info() const
      {
         return UnitImpl::device_info();
      }

      DeviceInfo *device_info()
      {
         return UnitImpl::device_info();
      }

      AttrReporting *attribute_reporting() const
      {
         return UnitImpl::attribute_reporting();
      }

      AttrReporting *attribute_reporting()
      {
         return UnitImpl::attribute_reporting();
      }
   };

   typedef HF::Testing::AbstractDevice<HF::Devices::Node::Abstract<TestUnit0>> TestDevice;

}  // namespace

// =============================================================================
// API
// =============================================================================

TEST_GROUP(Unit0)
{
   TestDevice *device;

   TEST_SETUP()
   {
      device = new TestDevice();
   }

   TEST_TEARDOWN()
   {
      delete device;
   }
};

TEST(Unit0, OptionalInterfaces)
{
   auto itfs = device->unit0()->interfaces();

   LONGS_EQUAL(3, itfs.size());

   CHECK_TRUE(std::any_of(itfs.begin(), itfs.end(), [](const Common::Interface &itf) {
      return itf.id == HF::Interface::POWER;
   }));

   CHECK_TRUE(std::any_of(itfs.begin(), itfs.end(), [](const Common::Interface &itf) {
      return itf.id == HF::Interface::RSSI;
   }));

   CHECK_TRUE(std::any_of(itfs.begin(), itfs.end(), [](const Common::Interface &itf) {
      return itf.id == HF::Interface::TIME;
   }));
}
