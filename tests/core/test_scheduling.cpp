// =============================================================================
/*!
 * @file       tests/core/test_scheduling.cpp
 *
 * This is file contains the unit tests for the Event Scheduling service
 * implementation.
 *
 * @version    1.5.0
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================

#include "hanfun/core/scheduling.h"

#include "test_helper.h"

using namespace HF;
using namespace HF::Core;

using namespace HF::Testing;

using namespace HF::Core::Scheduling;

// =============================================================================
// Scheduling common messages
// =============================================================================
//! Test Group for Common Scheduler service messages class.
TEST_GROUP(Scheduling_Messages)
{
   Common::ByteArray payload, expected;
   uint16_t size;
   TEST_SETUP()
   {
      size = 0;
      mock().ignoreOtherCalls();
   }

   TEST_TEARDOWN()
   {
      mock().clear();
   }
};

// ========== Activate Scheduler ==========
//! @test ActivateScheduler message size
TEST(Scheduling_Messages, ActivateScheduler_size)
{
   ActivateScheduler message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(1, message.size());
}

//! @test ActivateScheduler message pack
TEST(Scheduling_Messages, ActivateScheduler_pack)
{
   ActivateScheduler message(0x11);

   payload  = Common::ByteArray(message.size());

   size     = message.pack(payload);

   expected = Common::ByteArray {0x01};

   UNSIGNED_LONGS_EQUAL(1, size);
   CHECK_EQUAL(expected, payload);
}

//! @test ActivateScheduler message unpack
TEST(Scheduling_Messages, ActivateScheduler_unpack)
{
   ActivateScheduler message;

   payload = Common::ByteArray {0x11};

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(1, size);
   CHECK_TRUE(message.status);
}

// ========== Define Event Response ==========

//! @test  DefineEventResponse message size.
TEST(Scheduling_Messages, DefineEventResponse_size)
{
   DefineEventResponse message(Common::Result::FAIL_ARG);

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(1, message.size());

   message = DefineEventResponse();
   UNSIGNED_LONGS_EQUAL(2, message.size());
}

//! @test  DefineEventResponse message pack.
TEST(Scheduling_Messages, DefineEventResponse_pack)
{
   DefineEventResponse message(Common::Result::OK, 0x11);

   payload  = Common::ByteArray(message.size());

   size     = message.pack(payload);

   expected = Common::ByteArray {Common::Result::OK, 0x11};

   UNSIGNED_LONGS_EQUAL(2, size);
   CHECK_EQUAL(expected, payload);

   // ============

   message  = DefineEventResponse(Common::Result::FAIL_ARG);
   payload  = Common::ByteArray(message.size());

   size     = message.pack(payload);

   expected = Common::ByteArray {Common::Result::FAIL_ARG};

   UNSIGNED_LONGS_EQUAL(1, size);
   CHECK_EQUAL(expected, payload);
}

/*!
 * @test  DefineEventResponse message pack.
 * Fail because of insufficient space on the payload container.
 */
TEST(Scheduling_Messages, DefineEventResponse_pack_fail_no_space)
{
   DefineEventResponse message(Common::Result::OK, 0x11);

   payload = Common::ByteArray(message.size() - 1);

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

//! @test  DefineEventResponse message unpack.
TEST(Scheduling_Messages, DefineEventResponse_unpack)
{
   DefineEventResponse message;

   payload = Common::ByteArray {Common::Result::OK, 0x11};

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(2, size);
   UNSIGNED_LONGS_EQUAL(Common::Result::OK, message.code);
   UNSIGNED_LONGS_EQUAL(0x11, message.event_id);

   // ============

   message = DefineEventResponse(Common::Result::FAIL_ARG);
   payload = Common::ByteArray {Common::Result::FAIL_ARG};

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(1, size);
   UNSIGNED_LONGS_EQUAL(Common::Result::FAIL_ARG, message.code);
}

/*!
 * @test  DefineEventResponse message unpack.
 * Fail because of an incomplete message being received.
 */
TEST(Scheduling_Messages, DefineEventResponse_unpack_fail)
{
   DefineEventResponse message;

   payload = Common::ByteArray {Common::Result::OK};

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

// ========== Update Status ==========

//! @test  UpdateStatus message size
TEST(Scheduling_Messages, UpdateStatus_size)
{
   UpdateStatus message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(2, message.size());
}

//! @test  UpdateStatus message pack
TEST(Scheduling_Messages, UpdateStatus_pack)
{
   UpdateStatus message(0x11, 0x01);

   payload  = Common::ByteArray(message.size());

   size     = message.pack(payload);

   expected = Common::ByteArray {0x11, 0x80};

   UNSIGNED_LONGS_EQUAL(2, size);
   CHECK_EQUAL(expected, payload);
}

/*!
 * @test  UpdateStatus message pack.
 * Fail because of insufficient space on the payload container.
 */
TEST(Scheduling_Messages, UpdateStatus_pack_fail_no_size)
{
   UpdateStatus message(0x11, 0x01);

   payload = Common::ByteArray(message.size() - 1);

   size    = message.pack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

//! @test  UpdateStatus message unpack
TEST(Scheduling_Messages, UpdateStatus_unpack)
{
   UpdateStatus message;

   payload = Common::ByteArray {0x11, 0x81};

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(2, size);
   UNSIGNED_LONGS_EQUAL(0x11, message.event_id);
   CHECK_TRUE(message.status);
}

/*!
 * @test  UpdateStatus message unpack.
 * Fail because of an incomplete message being received.
 */
TEST(Scheduling_Messages, UpdateStatus_unpack_fail)
{
   UpdateStatus message;

   payload = Common::ByteArray {0x11};

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(0, size);
}

// ========== Get Entry ==========

//! @test  GetEntry message size.
TEST(Scheduling_Messages, GetEntry_size)
{
   GetEntry message;

   UNSIGNED_LONGS_EQUAL(message.min_size, message.size());
   UNSIGNED_LONGS_EQUAL(1, message.size());
}

//! @test  GetEntry message pack.
TEST(Scheduling_Messages, GetEntry_pack)
{
   GetEntry message(0x11);

   payload  = Common::ByteArray(message.size());

   size     = message.pack(payload);

   expected = Common::ByteArray {0x11};

   UNSIGNED_LONGS_EQUAL(1, size);
   CHECK_EQUAL(expected, payload);
}

//! @test  GetEntry message unpack.
TEST(Scheduling_Messages, GetEntry_unpack)
{
   GetEntry message;

   payload = Common::ByteArray {0x11};

   size    = message.unpack(payload);

   UNSIGNED_LONGS_EQUAL(1, size);
   UNSIGNED_LONGS_EQUAL(0x11, message.event_id);
}
