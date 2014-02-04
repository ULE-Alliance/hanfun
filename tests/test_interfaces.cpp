// =============================================================================
/*!
 * \file       tests/test_interfaces.cpp
 *
 * This file contains the implementation of the tests for the common functionalaty
 * of the interfaces.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/interface.h"

#include "test_helper.h"

using namespace HF;

// =============================================================================
// AbstractInterface
// =============================================================================

//! AbstractInterface test group.
TEST_GROUP (AbstractInterface)
{
   struct TestInterface: Testing::InterfaceHelper <AbstractInterface>
   {
      Role     _role;
      uint16_t _uid;

      Interface::Role role () const
      {
         return _role;
      }

      uint16_t uid () const
      {
         return _uid;
      }
   };
};

/*!
 * \test Should handle request message coming from the same interface and
 *       from the same role.
 */
TEST (AbstractInterface, Handle_Request)
{
   TestInterface itf;
   Protocol::Message message;
   ByteArray array;

   message.type     = Message::COMMAND_REQ;

   itf._uid         = message.itf.uid = 0x5AA5;
   message.itf.role = Interface::SERVER_ROLE;

   itf._role        = Interface::SERVER_ROLE;

   CHECK_EQUAL (Result::OK, itf.handle (message, array, 0));

   itf._role = Interface::CLIENT_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, itf.handle (message, array, 0));

   message.itf.role = Interface::SERVER_ROLE;
   message.itf.uid  = 0x7AAA;

   CHECK_EQUAL (Result::FAIL_ID, itf.handle (message, array, 0));
}

/*!
 * \test Should handle request message coming from the same interface and
 *       from the same role.
 */
TEST (AbstractInterface, Handle_RequestResp)
{
   TestInterface itf;
   Protocol::Message message;
   ByteArray array;

   message.type     = Message::COMMAND_RESP_REQ;

   itf._uid         = message.itf.uid = 0x5AA5;
   message.itf.role = Interface::SERVER_ROLE;

   itf._role        = Interface::SERVER_ROLE;

   CHECK_EQUAL (Result::OK, itf.handle (message, array, 0));

   itf._role = Interface::CLIENT_ROLE;

   CHECK_EQUAL (Result::FAIL_SUPPORT, itf.handle (message, array, 0));

   message.itf.role = Interface::SERVER_ROLE;
   message.itf.uid  = 0x7AAA;

   CHECK_EQUAL (Result::FAIL_ID, itf.handle (message, array, 0));
}
