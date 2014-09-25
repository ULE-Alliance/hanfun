// =============================================================================
/*!
 * \file       tests/core/test_device_information.cpp
 *
 * This file contains the implementation of the tests for Device Information Interface.
 *
 * \version    1.0.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
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

TEST_GROUP (DeviceInformation)
{
   struct TestDeviceInformationServer:public DeviceInformation::Server
   {
      TestDeviceInformationServer(HF::Core::Unit0 &unit):
         DeviceInformation::Server (unit)
      {}
   };

   struct TestDeviceManagementClient:public DeviceManagement::Client
   {
      TestDeviceManagementClient(HF::Core::Unit0 &unit):Client (unit)
      {}

      void address (uint16_t _address)
      {
         this->_address = _address;
      }

      using DeviceManagement::Client::address;
   };

   Testing::Device *device;

   TestDeviceInformationServer *dev_info;

   TEST_SETUP ()
   {
      device = new Testing::Device ();

      device->unit0 ()->device_management (new TestDeviceManagementClient (*device->unit0 ()));
      ((TestDeviceManagementClient *) device->unit0 ()->device_management ())->address (0x5A5A);

      dev_info             = new TestDeviceInformationServer (*device->unit0 ());

      dev_info->device_uid = new HF::UID::URI ("hf://device@example.com");

      mock ().ignoreOtherCalls ();
   }

   TEST_TEARDOWN ()
   {
      delete dev_info;

      delete device;

      mock ().clear ();
   }
};

TEST (DeviceInformation, Mandatory)
{
   Protocol::Message *msg = HF::Core::DeviceInformation::mandatory ();

   CHECK_FALSE (msg == nullptr);

   Protocol::Packet packet (*msg);

   delete msg;

   mock ("AbstractDevice").expectOneCall ("send");

   dev_info->handle (packet, packet.message.payload, 0);

   mock ("AbstractDevice").checkExpectations ();

   Protocol::Packet *packet_resp = device->packets.front ();

   Protocol::GetAttributePack::Response resp (HF::Core::DeviceInformation::create_attribute);

   resp.unpack (packet_resp->message.payload);

   LONGS_EQUAL (Result::OK, resp.code);
   LONGS_EQUAL (4, resp.attributes.size ())
}
