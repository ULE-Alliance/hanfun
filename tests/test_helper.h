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

#include "hanfun/common.h"
#include "hanfun/protocol.h"

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
         Message          *sendMsg;

         InterfaceHelper()
         {
            sendMsg = NULL;
            memset (&addr, 0, sizeof(Message::Address));
         }

         virtual ~InterfaceHelper()
         {
            if (sendMsg != NULL)
            {
               delete sendMsg;
            }
         }

         void sendMessage (Message::Address &addr, Message *msg)
         {
            this->addr    = addr;
            this->sendMsg = msg;
         }
      };

   }  // namespace Testing

}  // namespace HF


#endif /* HF_TEST_HELPER_H */
