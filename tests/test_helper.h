// =============================================================================
/*!
 * \file       tests/test_helper.h
 *
 * This file contains the definition of helper classes used for testing.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_TEST_HELPER_H
#define HF_TEST_HELPER_H

#include <string.h>
#include <sstream>
#include <algorithm>
#include <iostream>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "hanfun/common.h"
#include "hanfun/attributes.h"
#include "hanfun/protocol.h"

#include "hanfun/interface.h"
#include "hanfun/profiles.h"
#include "hanfun/devices.h"

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"

#include "hanfun/units.h"

using namespace HF;
using namespace HF::Protocol;

// =============================================================================
// Helper Test Functions
// =============================================================================

SimpleString StringFrom (const HF::ByteArray &array);

SimpleString StringFrom (const HF::Serializable &data);

template<typename _type>
void check_index (_type expected, _type actual, uint32_t index, const char *header,
                  const char *fileName,
                  int lineNumber)
{
   if (actual != expected)
   {
      std::ostringstream error;
      error << header << " at index " << index << std::endl
            << "\t\t expected : "
            << expected << std::endl
            << "\t\t but was  : "
            << actual;

      FAIL_TEST_LOCATION (error.str ().c_str (), fileName, lineNumber);
   }
}

// =============================================================================
// Helper Test Classes
// =============================================================================

namespace HF
{
   namespace Testing
   {
      struct Payload:public Serializable
      {
         size_t fake_size;

         Payload(size_t fake_size = 0):
            fake_size (fake_size & Protocol::MAX_PAYLOAD)
         {}

         virtual ~Payload()
         {}

         size_t size () const
         {
            return fake_size;
         }

         size_t pack (ByteArray &array, size_t offset = 0) const
         {
            UNUSED (array);
            UNUSED (offset);
            return fake_size;
         }

         size_t unpack (const ByteArray &array, size_t offset = 0)
         {
            UNUSED (array);
            UNUSED (offset);
            return fake_size;
         }
      };

      //! Test Interface.
      template<class Base>
      struct InterfaceHelper:public Base
      {
         Protocol::Address addr;
         Protocol::Message sendMsg;

         InterfaceHelper()
         {
            memset (&sendMsg, 0, sizeof(Protocol::Message));
            memset (&addr, 0, sizeof(Protocol::Address));
         }

         virtual ~InterfaceHelper()
         {
            if (sendMsg.payload != nullptr)
            {
               delete sendMsg.payload;
            }
         }

         void sendMessage (Protocol::Address &addr, Protocol::Message &message)
         {
            mock ("Interface").actualCall ("sendMessage");

            this->addr    = addr;
            this->sendMsg = message;
         }
      };

      template<class Base>
      struct InterfaceParentHelper:public InterfaceHelper <Base>
      {
         Interface::Role role () const
         {
            return Interface::SERVER_ROLE;
         }
      };

      struct TestInterface:public InterfaceHelper <AbstractInterface>
      {
         typedef enum
         {
            ATTR1 = 0x01,
            ATTR2 = 0x02,
            ATTR3 = 0x03,
         } Attributes;

         Interface::Role _role;
         uint16_t        _uid;

         uint16_t        attr1;
         uint16_t        attr2;
         uint16_t        attr3;

         TestInterface() {}

         TestInterface(Role role, uint16_t _uid):
            _role (role), _uid (_uid), attr1 (0x5A51), attr2 (0x5A52), attr3 (0x5A53)
         {}

         uint16_t uid () const
         {
            return _uid;
         }

         Interface::Role role () const
         {
            return _role;
         }

         IAttribute *attribute (uint8_t uid)
         {
            switch (uid)
            {
               case ATTR1:
                  return new Attribute <uint16_t >(this->uid(), ATTR1, attr1);

               case ATTR2:
                  return new Attribute <uint16_t >(this->uid(), ATTR2, attr2);

                  break;
               case ATTR3:
                  return new Attribute <uint16_t &>(this->uid(), ATTR3, attr3, true);

               default:
                  return nullptr;
            }
         }

         protected:

         Result handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset)
         {
            UNUSED (packet);
            UNUSED (payload);
            UNUSED (offset);

            mock ("Interface").actualCall ("handle_command").onObject (this);

            return Result::OK;
         }
      };

      struct Profile:public IProfile, public TestInterface
      {
         uint16_t _uid;

         uint16_t uid () const
         {
            return _uid;
         }
      };

      struct Unit:public HF::Unit <Profile>
      {
         Unit(uint16_t id, IDevice *device):
            HF::Unit <Profile>(id, device)
         {}
      };

      template<class Parent>
      struct AbstractDevice:public Parent
      {
         uint16_t                    _address;

         vector <Protocol::Packet *> packets;

         AbstractDevice():
            _address (Protocol::BROADCAST_ADDR) // , packet (nullptr)
         {}

         virtual ~AbstractDevice()
         {
            /* *INDENT-OFF* */
            for_each (packets.begin (), packets.end (), [](Protocol::Packet *packet)
            {
               if (packet->message.payload != nullptr)
               {
                  delete packet->message.payload;
               }

               delete packet;
            });
            /* *INDENT-ON* */

            packets.clear ();
         }

         void send (Protocol::Packet *packet)
         {
            mock ("AbstractDevice").actualCall ("send");
            packets.push_back (packet);
         }

         void receive (Protocol::Packet *packet)
         {
            mock ("AbstractDevice").actualCall ("receive");
            packets.push_back (packet);
         }
      };

      struct Device:public AbstractDevice <
         HF::Device < HF::Core::DefaultDeviceInformation, HF::Core::DeviceManagementClient >>
      {};

      struct Concentrator:public AbstractDevice <
         HF::Concentrator < HF::Core::DefaultDeviceInformation, HF::Core::DefaultDeviceManagementServer >>
      {};

      struct Link:public HF::Transport::Link
      {
         HF::UID       *_uid;
         HF::Transport *tsp;

         Link(HF::UID *uid, HF::Transport *tsp):
            _uid (uid), tsp (tsp)
         {}

         virtual ~Link()
         {
            delete _uid;
         }

         virtual size_t write (ByteArray *array, size_t offset, size_t size)
         {
            UNUSED (array);
            UNUSED (offset);
            UNUSED (size);

            return mock ("Link").actualCall ("write").returnValue ().getIntValue ();
         }

         virtual size_t read (ByteArray *array, size_t offset, size_t size)
         {
            UNUSED (array);
            UNUSED (offset);
            UNUSED (size);

            return mock ("Link").actualCall ("read").returnValue ().getIntValue ();
         }

         virtual size_t available ()
         {
            return mock ("Link").actualCall ("available").returnValue ().getIntValue ();
         }

         virtual HF::UID *uid ()
         {
            return _uid;
         }

         virtual HF::Transport *transport ()
         {
            return tsp;
         }
      };

   } // namespace Testing

} // namespace HF

#endif /* HF_TEST_HELPER_H */
