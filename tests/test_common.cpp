// =============================================================================
/*!
 * \file       tests/test_common.cpp
 *
 * This file contains the implementation of the unit tests for the common
 * functions and classes for the HAN-FUN library.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"

#include "test_helper.h"

using namespace std;
using namespace HF;

// =============================================================================
// Functions
// =============================================================================

#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__

TEST_GROUP (Endianness)
{};

   #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

TEST (Endianness, Network2Host_Integer)
{
   uint16_t value16 = 0x3412;

   hf_ntoh (value16);
   LONGS_EQUAL (0x1234, value16);

   uint32_t value32 = 0x0A0B0C0D;

   hf_ntoh (value32);
   LONGS_EQUAL (0x0D0C0B0A, value32);
}

TEST (Endianness, Host2Network_Integer)
{
   uint16_t value16 = 0x3412;

   hf_hton (value16);
   LONGS_EQUAL (0x1234, value16);

   uint32_t value32 = 0x0D0C0B0A;

   hf_hton (value32);
   LONGS_EQUAL (0x0A0B0C0D, value32);
}

   #endif

#endif

// =============================================================================
// Byte Array
// =============================================================================

TEST_GROUP (ByteArray)
{
   ByteArray *array;

   TEST_SETUP ()
   {
      array = new ByteArray (10);
   }

   TEST_TEARDOWN ()
   {
      delete array;
   }
};

TEST (ByteArray, Create)
{
   LONGS_EQUAL (10, array->size ());

   LONGS_EQUAL (10, array->capacity ());
}

TEST (ByteArray, Write_Byte)
{
   uint8_t value = 42;

   LONGS_EQUAL (sizeof(uint8_t), array->write (0, value));

   BYTES_EQUAL (value, (*array)[0]);

   LONGS_EQUAL (sizeof(uint8_t), array->write (5, value));

   BYTES_EQUAL (value, (*array)[5]);
}

TEST (ByteArray, Write_Word)
{
   uint16_t  value  = 0x5AA5;

   uint8_t   data[] = {0x5A, 0xA5, 0x00, 0x00, 0x00, 0x5A, 0xA5, 0x00, 0x00, 0x00};
   ByteArray expected (data, sizeof(data));

   LONGS_EQUAL (sizeof(uint16_t), array->write (0, value));

   LONGS_EQUAL (sizeof(uint16_t), array->write (5, value));

   CHECK_EQUAL (expected, (*array));
}

TEST (ByteArray, Write_DWord)
{
   uint32_t  value  = 0xFF5AA5CC;

   uint8_t   data[] = {0xFF, 0x5A, 0xA5, 0xCC, 0x00, 0xFF, 0x5A, 0xA5, 0xCC, 0x00};
   ByteArray expected (data, sizeof(data));

   LONGS_EQUAL (sizeof(uint32_t), array->write (0, value));

   LONGS_EQUAL (sizeof(uint32_t), array->write (5, value));

   CHECK_EQUAL (expected, (*array));
}

TEST_GROUP (ByteArray_Read)
{
   ByteArray array;

   TEST_SETUP ()
   {
      uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x5A, 0xA5, 0xCC, 0x00, 0x00};
      array = ByteArray (data, sizeof(data));
   }
};

TEST (ByteArray, Read_Byte)
{
   uint8_t temp;
   uint8_t value = 42;

   (*array)[5] = value;

   LONGS_EQUAL (sizeof(uint8_t), array->read (0, temp));

   LONGS_EQUAL (0, temp);

   LONGS_EQUAL (sizeof(uint8_t), array->read (5, temp));

   LONGS_EQUAL (value, temp);

   temp = 0;

   temp = (*array)[5];

   LONGS_EQUAL (value, temp);
}

TEST (ByteArray_Read, Word)
{
   uint16_t temp = 0xFF;

   LONGS_EQUAL (sizeof(uint16_t), array.read (0, temp));

   LONGS_EQUAL (0, temp);

   LONGS_EQUAL (sizeof(uint16_t), array.read (5, temp));

   LONGS_EQUAL (0x5AA5, temp);
}

TEST (ByteArray_Read, DWord)
{
   uint32_t temp = 0xBB;

   LONGS_EQUAL (sizeof(uint32_t), array.read (0, temp));

   LONGS_EQUAL (0, temp);

   LONGS_EQUAL (sizeof(uint32_t), array.read (4, temp));

   LONGS_EQUAL (0xFF5AA5CC, temp);
}

// =============================================================================
// HF::UID
// =============================================================================

TEST_GROUP (UID)
{};

TEST (UID, NONE)
{
   UID uid;

   LONGS_EQUAL (UID::NONE, uid.type ());

   size_t size = uid.size ();

   LONGS_EQUAL (1, size);

   uint8_t data[] = {0xAA, 0xAA, 0xAA,
                     0x00, // UID size.
                     0xAA, 0xAA, 0xAA};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   for (int i = 0; i < 3; i++)
   {
      array[i] = 0xAA;
   }

   for (int i = 4; i < 7; i++)
   {
      array[i] = 0xAA;
   }

   size_t wsize = uid.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   size_t rsize = uid.unpack (array, 3);
   LONGS_EQUAL (size, rsize);
}

TEST (UID, IPUI)
{
   IPUI uid;

   LONGS_EQUAL (UID::IPUI, uid.type ());

   size_t size = uid.size ();

   LONGS_EQUAL (1 + 5, size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x05,                         // UID size.
                     0x00, 0x73, 0x70,0x5A,  0xA5, // IPUI value.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   uid.value[0] = 0x00;
   uid.value[1] = 0x73;
   uid.value[2] = 0x70;
   uid.value[3] = 0x5A;
   uid.value[4] = 0xA5;

   size_t wsize = uid.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   memset (uid.value, 0xFF, sizeof(uid.value));

   size_t rsize = uid.unpack (array, 3);
   LONGS_EQUAL (size, rsize);

   BYTES_EQUAL (0x00, uid.value[0]);
   BYTES_EQUAL (0x73, uid.value[1]);
   BYTES_EQUAL (0x70, uid.value[2]);
   BYTES_EQUAL (0x5A, uid.value[3]);
   BYTES_EQUAL (0xA5, uid.value[4]);
}

TEST (UID, MAC)
{
   MAC uid;

   LONGS_EQUAL (UID::MAC, uid.type ());

   size_t size = uid.size ();

   LONGS_EQUAL (1 + 6, size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x06,                               // UID size.
                     0x12, 0x34, 0x56,0x78,  0x9A, 0xBC, // MAC value.
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   uid.value[0] = 0x12;
   uid.value[1] = 0x34;
   uid.value[2] = 0x56;
   uid.value[3] = 0x78;
   uid.value[4] = 0x9A;
   uid.value[5] = 0xBC;

   size_t wsize = uid.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   memset (uid.value, 0xFF, sizeof(uid.value));

   size_t rsize = uid.unpack (array, 3);
   LONGS_EQUAL (size, rsize);

   BYTES_EQUAL (0x12, uid.value[0]);
   BYTES_EQUAL (0x34, uid.value[1]);
   BYTES_EQUAL (0x56, uid.value[2]);
   BYTES_EQUAL (0x78, uid.value[3]);
   BYTES_EQUAL (0x9A, uid.value[4]);
   BYTES_EQUAL (0xBC, uid.value[5]);
}

TEST (UID, URI)
{
   URI uid;

   LONGS_EQUAL (UID::URI, uid.type ());

   uid.value = "Hello World !";

   size_t size = uid.size ();

   LONGS_EQUAL (1 + 13, size);

   uint8_t data[] = {0x00, 0x00, 0x00,
                     0x0D, // UID size.
                     0x48, // H
                     0x65, // e
                     0x6c, // l
                     0x6c, // l
                     0x6f, // o
                     0x20, //
                     0x57, // W
                     0x6f, // o
                     0x72, // r
                     0x6c, // l
                     0x64, // d
                     0x20, //
                     0x21, // !
                     0x00, 0x00, 0x00};

   ByteArray expected (data, sizeof(data));
   ByteArray array (size + 6);

   size_t    wsize = uid.pack (array, 3);
   LONGS_EQUAL (size, wsize);

   CHECK_EQUAL (expected, array);

   uid.value = "";

   size_t rsize = uid.unpack (array, 3);
   LONGS_EQUAL (size, rsize);

   CHECK_EQUAL ("Hello World !", uid.value);
}

TEST (UID, Equals)
{
   UID  *temp;

   IPUI ipui;
   IPUI ipui2;

   ipui.value[0] = 0x00;
   ipui.value[1] = 0x73;
   ipui.value[2] = 0x70;
   ipui.value[3] = 0xAA;
   ipui.value[4] = 0xBB;

   temp          = &ipui;
   ipui2         = ipui;

   CHECK_EQUAL (ipui, *temp);
   CHECK_EQUAL (ipui, ipui2);

   MAC mac;
   MAC mac2;

   ipui.value[0] = 0x00;
   ipui.value[1] = 0x73;
   ipui.value[2] = 0x70;
   ipui.value[3] = 0xAA;
   ipui.value[4] = 0xBB;
   ipui.value[4] = 0xCC;

   temp          = &mac;
   mac2          = mac;

   CHECK_EQUAL (mac, *temp);
   CHECK_EQUAL (mac, mac2);

   URI uri;
   URI uri2;

   uri.value = "sn://1234567890";

   temp      = &uri;
   uri2      = uri;

   CHECK_EQUAL (uri, *temp);
   CHECK_EQUAL (uri, uri2);
}
