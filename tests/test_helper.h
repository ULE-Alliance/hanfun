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

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "hanfun/common.h"
#include "hanfun/protocol.h"

#include "hanfun/interface.h"
#include "hanfun/profiles.h"
#include "hanfun/devices.h"

#include "hanfun/units.h"

using namespace HF;
using namespace HF::Protocol;

// =============================================================================
// Helper Test Functions
// =============================================================================

SimpleString StringFrom (const HF::ByteArray &array);

SimpleString StringFrom (const HF::Serializable &data);

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

         Payload(size_t fake_size = 0):fake_size (fake_size & Protocol::MAX_PAYLOAD) {}

         virtual ~Payload() {}

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
         Message::Address addr;
         Message          sendMsg;

         InterfaceHelper()
         {
            memset (&sendMsg, 0, sizeof(Message));
            memset (&addr, 0, sizeof(Message::Address));
         }

         virtual ~InterfaceHelper()
         {
            if (sendMsg.payload != nullptr)
            {
               delete sendMsg.payload;
            }
         }

         void sendMessage (Message::Address &addr, Protocol::Message &message)
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

      struct TestInterface:public AbstractInterface
      {
         uint16_t        _uid;
         Interface::Role _role;

         uint16_t uid () const
         {
            return _uid;
         }

         Interface::Role role () const
         {
            return Interface::SERVER_ROLE;
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
         Unit(uint16_t id, AbstractDevice *device):
            HF::Unit <Profile>(id, device) {}
      };

      struct Device:public AbstractDevice
      {
         Message::Address msg_addr;
         Message          sendMsg;
         IUnit            *unit;

         Device()
         {
            memset (&sendMsg, 0, sizeof(Message));
            memset (&msg_addr, 0, sizeof(Message::Address));
         }

         virtual ~Device()
         {
            if (sendMsg.payload != nullptr)
            {
               delete sendMsg.payload;
            }
         }

         void sendMessage (IUnit &unit, Message::Address &addr, Protocol::Message &message)
         {
            mock ("Device").actualCall ("sendMessage");

            this->msg_addr = addr;
            this->sendMsg  = message;
            this->unit     = &unit;
         }
      };

   }  // namespace Testing

}  // namespace HF


#endif /* HF_TEST_HELPER_H */
