// =============================================================================
/*!
 * @file       tests/core/test_device_information.cpp
 *
 * This file contains the implementation of the tests for Device Information Interface.
 *
 * @version    1.4.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/device_information.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Common;
using namespace HF::Core;

// =============================================================================
// Helper Classes
// =============================================================================

// =============================================================================
// API
// =============================================================================

// =============================================================================
// Tests
// =============================================================================

TEST_GROUP(DeviceInformation)
{
   struct TestDeviceInformationServer: public DeviceInformation::Server
   {
      TestDeviceInformationServer(HF::Core::Unit0 &unit):
         DeviceInformation::Server(unit)
      {}
   };

   struct TestDeviceManagementClient: public DeviceManagement::Client
   {
      TestDeviceManagementClient(HF::Core::Unit0 &unit): Client(unit)
      {}

      void address(uint16_t _address)
      {
         this->_address = _address;
      }

      using DeviceManagement::Client::address;
   };

   Testing::Device *device;

   TestDeviceInformationServer *dev_info;

   TEST_SETUP()
   {
      device = new Testing::Device();

      device->unit0()->device_management(new TestDeviceManagementClient(*device->unit0()));
      ((TestDeviceManagementClient *) device->unit0()->device_management())->address(0x5A5A);

      dev_info             = new TestDeviceInformationServer(*device->unit0());

      dev_info->device_uid = new HF::UID::URI("hf://device@example.com");

      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      delete dev_info;

      delete device;

      mock().clear();
   }
};

TEST(DeviceInformation, Mandatory)
{
   Protocol::Message *msg = HF::Core::DeviceInformation::mandatory();

   CHECK_FALSE(msg == nullptr);

   Protocol::Packet packet(*msg);

   delete msg;

   mock("AbstractDevice").expectOneCall("send");

   dev_info->handle(packet, packet.message.payload, 0);

   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet_resp = device->packets.front();

   Protocol::GetAttributePack::Response resp(HF::Core::DeviceInformation::create_attribute);

   resp.unpack(packet_resp->message.payload);

   LONGS_EQUAL(Result::OK, resp.code);
   LONGS_EQUAL(4, resp.attributes.size())
}

TEST(DeviceInformation, All)
{
   Protocol::Message *msg = HF::Core::DeviceInformation::all();

   CHECK_FALSE(msg == nullptr);

   Protocol::Packet packet(*msg);

   delete msg;

   mock("AbstractDevice").expectOneCall("send");

   dev_info->handle(packet, packet.message.payload, 0);

   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet_resp = device->packets.front();

   Protocol::GetAttributePack::Response resp(HF::Core::DeviceInformation::create_attribute);

   resp.unpack(packet_resp->message.payload);

   LONGS_EQUAL(Result::OK, resp.code);
   LONGS_EQUAL(8, resp.attributes.size())
}

TEST(DeviceInformation, ExtraCapabilities)
{
   LONGS_EQUAL(0x00, dev_info->capabilities());

   dev_info->paging(true);

   LONGS_EQUAL(0x01, dev_info->capabilities());

   CHECK_TRUE(dev_info->has_paging());

   dev_info->broadcast(true);

   LONGS_EQUAL(0x03, dev_info->capabilities());

   CHECK_TRUE(dev_info->has_broadcast());

   dev_info->paging(false);

   LONGS_EQUAL(0x02, dev_info->capabilities());

   CHECK_FALSE(dev_info->has_paging());
   CHECK_TRUE(dev_info->has_broadcast());

   dev_info->paging(true);
   dev_info->broadcast(false);

   LONGS_EQUAL(0x01, dev_info->capabilities());

   CHECK_TRUE(dev_info->has_paging());
   CHECK_FALSE(dev_info->has_broadcast());
}

TEST(DeviceInformation, ApplicationVersion)
{
   STRCMP_EQUAL(HF_APPLICATION_VERSION, dev_info->application_version().c_str());

   auto attr = dev_info->attribute(DeviceInformation::APP_VERSION_ATTR);
   CHECK_TRUE(attr != nullptr);

   typedef HF::Attributes::Attribute<std::string, DeviceInformation::Server> __Attribute;

   auto attr_impl = static_cast<__Attribute *>(attr);

   STRCMP_EQUAL(HF_APPLICATION_VERSION, attr_impl->value().c_str());

   delete attr;
}

TEST(DeviceInformation, HardwareVersion)
{
   STRCMP_EQUAL(HF_HARDWARE_VERSION, dev_info->hardware_version().c_str());

   auto attr = dev_info->attribute(DeviceInformation::HW_VERSION_ATTR);
   CHECK_TRUE(attr != nullptr);

   typedef HF::Attributes::Attribute<std::string, DeviceInformation::Server> __Attribute;

   auto attr_impl = static_cast<__Attribute *>(attr);

   STRCMP_EQUAL(HF_HARDWARE_VERSION, attr_impl->value().c_str());

   delete attr;
}

TEST(DeviceInformation, ManufacturerName)
{
   STRCMP_EQUAL(HF_MANUFACTURER_NAME, dev_info->manufacturer_name().c_str());

   auto attr = dev_info->attribute(DeviceInformation::MANUFACTURER_NAME_ATTR);
   CHECK_TRUE(attr != nullptr);

   typedef HF::Attributes::Attribute<std::string, DeviceInformation::Server> __Attribute;

   auto attr_impl = static_cast<__Attribute *>(attr);

   STRCMP_EQUAL(HF_MANUFACTURER_NAME, attr_impl->value().c_str());

   delete attr;
}

TEST(DeviceInformation, Not_Supported_Attr)
{
   using namespace HF::Core::DeviceInformation;
   Protocol::Message *msg = get(__LAST_ATTR__ + 1);

   CHECK_FALSE(msg == nullptr);

   Protocol::Packet packet(*msg);

   delete msg;

   mock("AbstractDevice").expectOneCall("send");

   dev_info->handle(packet, packet.message.payload, 0);

   mock("AbstractDevice").checkExpectations();

   Protocol::Packet *packet_resp = device->packets.front();

   Protocol::Response resp;

   resp.unpack(packet_resp->message.payload);

   LONGS_EQUAL(Result::FAIL_SUPPORT, resp.code);
}
