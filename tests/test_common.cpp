// =============================================================================
/*!
 * \file       tests/test_common.cpp
 *
 * This file contains the implementation of the unit tests for the common
 * functions and classes for the HAN-FUN library.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#include "hanfun/common.h"
#include "hanfun/uids.h"

#include "test_helper.h"

using namespace std;
using namespace HF;

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
   uint16_t  value = 0x5AA5;

   ByteArray expected {0x5A, 0xA5, 0x00, 0x00, 0x00, 0x5A, 0xA5, 0x00, 0x00, 0x00};

   LONGS_EQUAL (sizeof(uint16_t), array->write (0, value));

   LONGS_EQUAL (sizeof(uint16_t), array->write (5, value));

   CHECK_EQUAL (expected, (*array));
}

TEST (ByteArray, Write_DWord)
{
   uint32_t  value = 0xFF5AA5CC;

   ByteArray expected {0xFF, 0x5A, 0xA5, 0xCC, 0x00, 0xFF, 0x5A, 0xA5, 0xCC, 0x00};

   LONGS_EQUAL (sizeof(uint32_t), array->write (0, value));

   LONGS_EQUAL (sizeof(uint32_t), array->write (5, value));

   CHECK_EQUAL (expected, (*array));
}

TEST_GROUP (ByteArray_Read)
{
   ByteArray array;

   TEST_SETUP ()
   {
      array = ByteArray {0x00, 0x00, 0x00, 0x00, 0xFF, 0x5A, 0xA5, 0xCC, 0x00, 0x00};
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


TEST (ByteArray_Read, Available)
{
   LONGS_EQUAL (10, array.available (0));
   LONGS_EQUAL (5, array.available (5));

   LONGS_EQUAL (1, array.available (9));
   LONGS_EQUAL (0, array.available (10));

   LONGS_EQUAL (0, array.available (11));

   CHECK_TRUE (array.available (0, 5));
   CHECK_TRUE (array.available (4, 5));

   CHECK_FALSE (array.available (6, 5));

   CHECK_FALSE (array.available (10, 1));
}

// =============================================================================
// HF::UID
// =============================================================================

using namespace HF::UID;

TEST_GROUP (UID)
{};

TEST (UID, NONE)
{
   NONE uid;

   LONGS_EQUAL (NONE_UID, uid.type ());

   size_t size = uid.size ();

   LONGS_EQUAL (1, size);

   ByteArray expected ({0xAA, 0xAA, 0xAA,
                        0x00, // UID size.
                        0xAA, 0xAA, 0xAA}
                      );
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

   LONGS_EQUAL (IPUI_UID, uid.type ());

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

   LONGS_EQUAL (MAC_UID, uid.type ());

   size_t size = uid.size ();

   LONGS_EQUAL (1 + 6, size);

   ByteArray expected ({0x00, 0x00, 0x00,
                        0x06,                               // UID size.
                        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, // MAC value.
                        0x00, 0x00, 0x00}
                      );
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

   LONGS_EQUAL (URI_UID, uid.type ());

   uid.value = "Hello World !";

   size_t size = uid.size ();

   LONGS_EQUAL (1 + 13, size);

   ByteArray expected ({0x00, 0x00, 0x00,
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
                        0x00, 0x00, 0x00}
                      );
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
   UID::UID *temp;

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

TEST (UID, Order)
{
   NONE none;
   NONE none2;

   RFPI rfpi;
   RFPI rfpi2;

   IPUI ipui;
   IPUI ipui2;

   MAC  mac;
   MAC  mac2;

   URI  uri;
   URI  uri2;

   rfpi.value[0]  = rfpi2.value[0] = 0x12;
   rfpi.value[1]  = rfpi2.value[1] = 0x23;
   rfpi.value[2]  = rfpi2.value[2] = 0x45;
   rfpi.value[3]  = rfpi2.value[3] = 0x67;

   rfpi.value[4]  = 0x89;

   rfpi2.value[4] = rfpi.value[4] + 2;

   ipui.value[0]  = ipui2.value[0] = 0x12;
   ipui.value[1]  = ipui2.value[1] = 0x23;
   ipui.value[2]  = ipui2.value[2] = 0x45;
   ipui.value[3]  = ipui2.value[3] = 0x67;

   ipui.value[4]  = 0x89;

   ipui2.value[4] = ipui.value[4] + 3;

   mac.value[0]   = mac2.value[0] = 0x12;
   mac.value[1]   = mac2.value[1] = 0x23;
   mac.value[2]   = mac2.value[2] = 0x45;
   mac.value[3]   = mac2.value[3] = 0x67;
   mac.value[4]   = mac2.value[4] = 0x89;
   mac.value[5]   = 0x9A;

   mac2.value[5]  = mac.value[5] + 4;

   uri.value      = "hf://www.example.com";

   uri2.value     = "hf://www.example1.com";

   // Check if operation works.

   less <UID::UID *> comp;

   CHECK_FALSE (comp (&none, &none));
   CHECK_FALSE (comp (&rfpi, &rfpi));
   CHECK_FALSE (comp (&ipui, &ipui));
   CHECK_FALSE (comp (&mac, &mac));
   CHECK_FALSE (comp (&uri, &uri));

   CHECK_TRUE (comp (&none, &rfpi));
   CHECK_TRUE (comp (&none, &ipui));
   CHECK_TRUE (comp (&none, &mac));
   CHECK_TRUE (comp (&none, &uri));

   CHECK_FALSE (comp (&rfpi, &none));
   CHECK_TRUE (comp (&rfpi, &ipui));
   CHECK_TRUE (comp (&rfpi, &mac));
   CHECK_TRUE (comp (&rfpi, &uri));

   CHECK_FALSE (comp (&ipui, &none));
   CHECK_FALSE (comp (&ipui, &rfpi));
   CHECK_TRUE (comp (&ipui, &mac));
   CHECK_TRUE (comp (&ipui, &uri));

   CHECK_FALSE (comp (&mac, &none));
   CHECK_FALSE (comp (&mac, &rfpi));
   CHECK_FALSE (comp (&mac, &ipui));
   CHECK_TRUE (comp (&mac, &uri));

   CHECK_FALSE (comp (&uri, &none));
   CHECK_FALSE (comp (&uri, &rfpi));
   CHECK_FALSE (comp (&uri, &ipui));
   CHECK_FALSE (comp (&uri, &mac));

   // Using it on an ordered collection.

   map <UID::UID *, string> test_db;

   // Only one NONE UID is possible.

   string temp;
   test_db[&none] = temp = "This is the NONE UID.";

   LONGS_EQUAL (1, test_db.size ());

   test_db[&none2] = temp = "This is the Other NONE UID.";

   LONGS_EQUAL (1, test_db.size ());

   STRCMP_EQUAL (temp.c_str (), test_db[&none].c_str ());

   // All other UID should increase size value.

   uint8_t size = test_db.size ();

   test_db[&rfpi] = "This is the RFPI UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&rfpi2] = "This is the other RFPI UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&ipui] = "This is the IPUI UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&ipui2] = "This is the other IPUI UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&mac] = "This is the MAC UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&mac2] = "This is the other MAC UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&uri] = "This is the URI UID.";
   LONGS_EQUAL (++size, test_db.size ());

   test_db[&uri2] = "This is the other URI UID.";
   LONGS_EQUAL (++size, test_db.size ());

   RFPI rfpi3;

   rfpi3.value[0] = rfpi.value[0];
   rfpi3.value[1] = rfpi.value[1];
   rfpi3.value[2] = rfpi.value[2];
   rfpi3.value[3] = rfpi.value[3];
   rfpi3.value[4] = rfpi.value[4];

   STRCMP_EQUAL ("This is the RFPI UID.", test_db[&rfpi3].c_str ());

   rfpi3.value[0] = rfpi2.value[0];
   rfpi3.value[1] = rfpi2.value[1];
   rfpi3.value[2] = rfpi2.value[2];
   rfpi3.value[3] = rfpi2.value[3];
   rfpi3.value[4] = rfpi2.value[4];

   STRCMP_EQUAL ("This is the other RFPI UID.", test_db[&rfpi3].c_str ());

   IPUI ipui3;

   ipui3.value[0] = ipui.value[0];
   ipui3.value[1] = ipui.value[1];
   ipui3.value[2] = ipui.value[2];
   ipui3.value[3] = ipui.value[3];
   ipui3.value[4] = ipui.value[4];

   STRCMP_EQUAL ("This is the IPUI UID.", test_db[&ipui3].c_str ());

   ipui3.value[0] = ipui2.value[0];
   ipui3.value[1] = ipui2.value[1];
   ipui3.value[2] = ipui2.value[2];
   ipui3.value[3] = ipui2.value[3];
   ipui3.value[4] = ipui2.value[4];

   STRCMP_EQUAL ("This is the other IPUI UID.", test_db[&ipui3].c_str ());

   MAC mac3;

   mac3.value[0] = mac.value[0];
   mac3.value[1] = mac.value[1];
   mac3.value[2] = mac.value[2];
   mac3.value[3] = mac.value[3];
   mac3.value[4] = mac.value[4];
   mac3.value[5] = mac.value[5];

   STRCMP_EQUAL ("This is the MAC UID.", test_db[&mac3].c_str ());

   mac3.value[0] = mac2.value[0];
   mac3.value[1] = mac2.value[1];
   mac3.value[2] = mac2.value[2];
   mac3.value[3] = mac2.value[3];
   mac3.value[4] = mac2.value[4];
   mac3.value[5] = mac2.value[5];

   STRCMP_EQUAL ("This is the other MAC UID.", test_db[&mac3].c_str ());

   URI uri3;

   uri3.value = uri.value;

   STRCMP_EQUAL ("This is the URI UID.", test_db[&uri3].c_str ());

   uri3.value = uri2.value;

   STRCMP_EQUAL ("This is the other URI UID.", test_db[&uri3].c_str ());
}

TEST (UID, Order_NULL)
{
   NONE uid;

   less <UID::UID *> comp;

   CHECK_TRUE (comp (nullptr, &uid));

   CHECK_TRUE (comp (nullptr, nullptr));

   CHECK_FALSE (comp (&uid, nullptr));
}

// =============================================================================
// Attributes
// =============================================================================

TEST_GROUP (Attributes)
{
   struct TestInterface:public Testing::InterfaceHelper <Testing::TestInterface>
   {};

   struct TestMeasure:public Serializable
   {
      uint8_t  type;
      uint16_t value;

      size_t size () const
      {
         return sizeof(type) + sizeof(value);
      }

      size_t pack (ByteArray &array, size_t offset = 0) const
      {
         size_t start = offset;

         offset += array.write (offset, type);
         offset += array.write (offset, value);

         return offset - start;
      }

      size_t unpack (const ByteArray &array, size_t offset = 0)
      {
         size_t start = offset;

         offset += array.read (offset, type);
         offset += array.read (offset, value);

         return offset - start;
      }
   };
};

TEST (Attributes, API)
{
   uint8_t  data  = 0x5A;
   uint16_t data2 = 0x5A50;
   uint32_t data3 = 0x5A50;

   TestInterface itf;
   HF::Attributes::Attribute <uint8_t &>  attr (itf.uid (), 0x5B, data);
   HF::Attributes::Attribute <uint16_t &> attr2 (itf.uid (), 0x5A, data2);
   HF::Attributes::Attribute <uint32_t &> attr3 (itf.uid (), 0x5C, data3);

   LONGS_EQUAL (data2, attr2.value ());

   data2 = 0x1234;

   LONGS_EQUAL (0x1234, attr2.value ());
   LONGS_EQUAL (0x1234, attr2.get ());

   LONGS_EQUAL (1, attr.size ());
   LONGS_EQUAL (2, attr2.size ());
   LONGS_EQUAL (4, attr3.size ());
}

TEST (Attributes, API2)
{
   TestMeasure data;

   TestInterface itf;
   HF::Attributes::Attribute <TestMeasure &> attr (itf.uid (), 0x5A, data, true);

   data.type  = 0x55;
   data.value = 0x5A5A;

   LONGS_EQUAL (data.type, attr.value ().type);
   LONGS_EQUAL (data.value, attr.value ().value);
}

TEST (Attributes, Serialize_Pack)
{
   ByteArray expected {0x00, 0x00, 0x00,
                       0x12, 0x34,
                       0x00, 0x00, 0x00};

   uint16_t attr = 0x1234;

   TestInterface itf;
   HF::Attributes::Attribute <uint16_t &> attr_wrapper (itf.uid (), 0x5B, attr);

   ByteArray result (expected.size ());

   fill (result.begin (), result.end (), 0);

   size_t w_size = attr_wrapper.pack (result, 3);
   LONGS_EQUAL (sizeof(attr), w_size);

   CHECK_EQUAL (expected, result);
}

TEST (Attributes, Serialize_Unpack)
{
   ByteArray expected {0x00, 0x00, 0x00,
                       0x12, 0x34,
                       0x00, 0x00, 0x00};

   uint16_t attr = 0x6666;

   TestInterface itf;
   HF::Attributes::Attribute <uint16_t &> attr_wrapper (itf.uid (), 0x5B, attr);


   size_t r_size = attr_wrapper.unpack (expected, 3);
   LONGS_EQUAL (sizeof(attr), r_size);

   CHECK_EQUAL (0x1234, attr);
}
