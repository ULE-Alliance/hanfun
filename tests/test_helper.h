// =============================================================================
/*!
 * \file       tests/test_helper.h
 *
 * This file contains the definition of helper classes used for testing.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
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

#define STRING_FROM(_T)                                                    \
   SimpleString StringFrom (const _T &data)                                \
   {                                                                       \
      Common::SerializableHelper <_T &> wrapper (const_cast <_T &>(data)); \
      return StringFrom (wrapper);                                         \
   }

SimpleString StringFrom (const HF::Common::ByteArray &array);

SimpleString StringFrom (const HF::Common::Serializable &data);

SimpleString StringFrom (const HF::Common::Interface &itf);

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

std::ostream &operator <<(std::ostream &os, std::uint8_t val);

// =============================================================================
// Helper Test Classes
// =============================================================================

namespace HF
{
   namespace Testing
   {
      struct Payload
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

         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            array.extend (fake_size);

            auto start = array.begin () + offset;

            array.insert (start, fake_size, 0x00);

            return fake_size;
         }

         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
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
         {}

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

      struct TestInterface:public InterfaceHelper <Interfaces::AbstractInterface>
      {
         static const uint16_t UID = 0x5A5A;

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

         TestInterface():_role (Interface::SERVER_ROLE), _uid (UID) {}

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

         HF::Attributes::IAttribute *attribute (uint8_t uid)
         {
            return create_attribute (this, uid);
         }

         static HF::Attributes::IAttribute *create_attribute (uint8_t uid)
         {
            return create_attribute (nullptr, uid);
         }

         static HF::Attributes::IAttribute *create_attribute (TestInterface *itf, uint8_t uid)
         {
            uint16_t itf_uid = (itf != nullptr ? itf->uid () : TestInterface::UID);

            switch (uid)
            {
               case ATTR1:
               {
                  if (itf == nullptr)
                  {
                     return new HF::Attributes::Attribute <uint16_t>(itf_uid, uid);
                  }
                  else
                  {
                     return new HF::Attributes::Attribute <uint16_t &>(itf_uid, uid, itf->attr1);
                  }

                  break;
               }

               case ATTR2:
               {
                  if (itf == nullptr)
                  {
                     return new HF::Attributes::Attribute <uint16_t>(itf_uid, uid);
                  }
                  else
                  {
                     return new HF::Attributes::Attribute <uint16_t &>(itf_uid, uid, itf->attr2);
                  }

                  break;
               }
               case ATTR3:
               {
                  if (itf == nullptr)
                  {
                     return new HF::Attributes::Attribute <uint16_t>(itf_uid, uid, true);
                  }
                  else
                  {
                     return new HF::Attributes::Attribute <uint16_t &>(itf_uid, uid, itf->attr3, true);
                  }

                  break;
               }

               default:
                  return nullptr;
            }
         }

         protected:

         Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            UNUSED (packet);
            UNUSED (payload);
            UNUSED (offset);

            mock ("Interface").actualCall ("handle_command").onObject (this);

            return Common::Result::OK;
         }

         //! \see AbstractInterface::attributes
         HF::Attributes::uids_t attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            HF::Attributes::uids_t result;

            if (pack_id == HF::Attributes::Pack::ALL)
            {
               result.push_back (ATTR1);
               result.push_back (ATTR2);
            }

            result.push_back (ATTR3);

            return result;
         }

         bool check_uid (uint16_t uid) const
         {
            return this->_uid == uid;
         }
      };

      struct Profile:public Profiles::IProfile, public TestInterface
      {
         uint16_t _uid;

         uint16_t uid () const
         {
            return _uid;
         }
      };

      struct Unit:public HF::Units::Unit <Profile>
      {
         Unit(uint16_t id, IDevice &device):
            HF::Units::Unit <Profile>(id, device)
         {}
      };

      template<class Parent>
      struct AbstractDevice:public Parent
      {
         uint16_t                    _address;

         vector <Protocol::Packet *> packets;

         AbstractDevice():
            _address (Protocol::BROADCAST_ADDR)
         {}

         virtual ~AbstractDevice()
         {
            /* *INDENT-OFF* */
            for_each (packets.begin (), packets.end (), [](Protocol::Packet *packet)
            {
               delete packet;
            });
            /* *INDENT-ON* */

            packets.clear ();
         }

         void connected (HF::Transport::Link *link)
         {
            UNUSED (link);
            mock ("AbstractDevice").actualCall ("connected");
         }

         void disconnected (HF::Transport::Link *link)
         {
            UNUSED (link);
            mock ("AbstractDevice").actualCall ("disconnected");
         }

         void send (Protocol::Packet &packet)
         {
            mock ("AbstractDevice").actualCall ("send");

            Protocol::Packet *temp = new Protocol::Packet (packet);

            packets.push_back (temp);
         }

         void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            UNUSED (packet);
            UNUSED (payload);
            UNUSED (offset);

            mock ("AbstractDevice").actualCall ("receive");
         }
      };

      struct DeviceUnit0 : public HF::Devices::Node::IUnit0
      {
         HF::Core::DeviceManagement::Client * dev_mgt;

         DeviceUnit0 (HF::IDevice &device) :
               HF::Devices::Node::IUnit0(device), dev_mgt(nullptr)
         {}

         virtual ~DeviceUnit0() {}

         HF::Core::DeviceManagement::Client *device_management ()
         {
            return dev_mgt;
         }

         HF::Core::DeviceManagement::Client *device_management () const
         {
            return dev_mgt;
         }

         Common::Result handle (HF::Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            UNUSED(packet);
            UNUSED(payload);
            UNUSED(offset);
            return Common::Result::FAIL_UNKNOWN;
         }
      };

      struct ConcentratorUnit0 : public HF::Devices::Concentrator::IUnit0
      {
         HF::Core::DeviceManagement::Server * dev_mgt;
         HF::Core::BindManagement::Server * bind_mgt;

         ConcentratorUnit0 (HF::IDevice &device) :
               HF::Devices::Concentrator::IUnit0(device), dev_mgt(nullptr), bind_mgt(nullptr)
         {}

         virtual ~ConcentratorUnit0() {}

         HF::Core::DeviceManagement::Server *device_management ()
         {
            return dev_mgt;
         }

         HF::Core::DeviceManagement::Server *device_management () const
         {
            return dev_mgt;
         }

         HF::Core::BindManagement::Server *bind_management ()
         {
            return bind_mgt;
         }

         HF::Core::BindManagement::Server *bind_management () const
         {
            return bind_mgt;
         }

         Common::Result handle (HF::Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            UNUSED(packet);
            UNUSED(payload);
            UNUSED(offset);
            return Common::Result::FAIL_UNKNOWN;
         }
      };

      struct Device:public AbstractDevice < HF::Devices::Node::Abstract < DeviceUnit0 >>
      {};

      struct Concentrator:public AbstractDevice < HF::Devices::Concentrator::Abstract < ConcentratorUnit0 >>
      {};

      struct Link:public HF::Transport::AbstractLink
      {
         HF::UID::UID         *_uid;
         HF::Transport::Layer *tsp;

         Common::ByteArray    *data;

         Link(HF::UID::UID *uid, HF::Transport::Layer *tsp):
            _uid (uid), tsp (tsp), data (nullptr)
         {}

         virtual ~Link()
         {
            delete _uid;
            delete data;
         }

         void send (Common::ByteArray &array)
         {
            this->data = new Common::ByteArray (array);
            mock ("Link").actualCall ("send");
         }

         HF::UID::UID const *uid () const
         {
            return _uid;
         }

         HF::Transport::Layer const *transport () const
         {
            return tsp;
         }
      };

   } // namespace Testing

} // namespace HF

#endif /* HF_TEST_HELPER_H */
