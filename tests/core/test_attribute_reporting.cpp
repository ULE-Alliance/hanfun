// =============================================================================
/*!
 * \file       tests/core/test_attribute_reporting.cpp
 *
 * This file contains the implementation of the unit tests for the Attribute
 * Reporting core service in HAN-FUN.
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#include "test_helper.h"

#include "hanfun/core/attribute_reporting.h"

using namespace HF;
using namespace HF::Core;
using namespace HF::Common;
using namespace HF::Core::AttributeReporting;

// =============================================================================
// AttributeReporting::Entry
// =============================================================================

TEST_GROUP (AttrReport_Entry)
{
   Entry entry;

   ByteArray expected;

   TEST_SETUP ()
   {
      entry    = Entry ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0x5A,                       // Unit ID
                            0x80, 0xAA,                 // Interface : Type + UID.
                            HF::Attributes::Pack::ALL,  // Attribute Pack ID.
                            0x00, 0x00, 0x00};
   }
};

TEST (AttrReport_Entry, Size)
{
   LONGS_EQUAL (1 + 2 + 1, entry.size ());
}

TEST (AttrReport_Entry, Pack)
{
   ByteArray result (expected.size ());

   entry.unit     = 0x5A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0xAA;
   entry.pack_id  = HF::Attributes::Pack::ALL;

   LONGS_EQUAL (entry.size (), entry.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Entry, Unpack)
{
   LONGS_EQUAL (entry.size (), entry.unpack (expected, 3));

   LONGS_EQUAL (0x5A, entry.unit);
   LONGS_EQUAL (HF::Interface::SERVER_ROLE, entry.itf.role);
   LONGS_EQUAL (0xAA, entry.itf.id);
   LONGS_EQUAL (HF::Attributes::Pack::ALL, entry.pack_id);
}

// =============================================================================
// AttributeReporting::Rule
// =============================================================================

TEST_GROUP (AttrReport_Rule)
{
   Rule rule;

   ByteArray expected;

   TEST_SETUP ()
   {
      rule     = Rule ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0x8A,                       // Report ID
                            0x85, 0xA5, 0xAA,           // Address : Type + UID.
                            0x00, 0x00, 0x00};
   }
};

TEST (AttrReport_Rule, Size)
{
   LONGS_EQUAL (1 + 2 + 1, rule.size ());
}

TEST (AttrReport_Rule, Pack)
{
   ByteArray result (expected.size ());

   rule.type               = EVENT;
   rule.id                 = 0x0A;

   rule.destination.mod    = HF::Interface::SERVER_ROLE;
   rule.destination.device = 0x05A5;
   rule.destination.unit   = 0xAA;

   LONGS_EQUAL (rule.size (), rule.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Rule, Unpack)
{
   LONGS_EQUAL (rule.size (), rule.unpack (expected, 3));

   LONGS_EQUAL (EVENT, rule.type);
   LONGS_EQUAL (0x0A, rule.id);

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, rule.destination.mod);
   LONGS_EQUAL (0x05A5, rule.destination.device);
   LONGS_EQUAL (0xAA, rule.destination.unit);
}

// =============================================================================
// AttributeReporting::Periodic
// =============================================================================

// =============================================================================
// AttributeReporting::Periodic::Entry
// =============================================================================

TEST_GROUP (AttrReport_Periodic_Entry)
{
   Periodic::Entry entry;

   ByteArray expected;

   TEST_SETUP ()
   {
      entry = Periodic::Entry ();
   }
};

TEST (AttrReport_Periodic_Entry, Size)
{
   entry.pack_id = HF::Attributes::Pack::MANDATORY;

   LONGS_EQUAL (1 + 2 + 1, entry.size ());

   entry.pack_id = HF::Attributes::Pack::ALL;

   LONGS_EQUAL (1 + 2 + 1, entry.size ());

   entry.pack_id = HF::Attributes::Pack::DYNAMIC;

   entry.attributes.push_back (0xA1);
   entry.attributes.push_back (0xA2);
   entry.attributes.push_back (0xA3);

   LONGS_EQUAL (1 + 2 + 1 + 1 + 3, entry.size ());
}

TEST (AttrReport_Periodic_Entry, Pack)
{
   expected = {0xFF, 0xFF, 0xFF,
               0x5A,                            // Unit ID
               0x80, 0xAA,                      // Interface : Type + UID.
               HF::Attributes::Pack::MANDATORY, // Attribute Pack ID.
               0xFF, 0xFF, 0xFF};

   ByteArray result (expected.size ());
   std::fill_n (result.begin (), expected.size (), 0xFF);

   entry.unit     = 0x5A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0xAA;
   entry.pack_id  = HF::Attributes::Pack::MANDATORY;

   LONGS_EQUAL (entry.size (), entry.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Periodic_Entry, Pack2)
{
   expected = {0xFF, 0xFF, 0xFF,
               0x5A,                      // Unit ID
               0x80, 0xAA,                // Interface : Type + UID.
               HF::Attributes::Pack::ALL, // Attribute Pack ID.
               0xFF, 0xFF, 0xFF};

   ByteArray result (expected.size ());
   std::fill_n (result.begin (), expected.size (), 0xFF);

   entry.unit     = 0x5A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0xAA;
   entry.pack_id  = HF::Attributes::Pack::ALL;

   LONGS_EQUAL (entry.size (), entry.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Periodic_Entry, Pack3)
{
   expected = {0xFF, 0xFF, 0xFF,
               0x5A,                          // Unit ID
               0x80, 0xAA,                    // Interface : Type + UID.
               HF::Attributes::Pack::DYNAMIC, // Attribute Pack ID.
               0x03, 0xA1, 0xA2,0xA3,         // Attributes UID's
               0xFF, 0xFF, 0xFF};

   ByteArray result (expected.size ());
   std::fill_n (result.begin (), expected.size (), 0xFF);

   entry.unit     = 0x5A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0xAA;
   entry.pack_id  = HF::Attributes::Pack::DYNAMIC;

   entry.attributes.push_back (0xA1);
   entry.attributes.push_back (0xA2);
   entry.attributes.push_back (0xA3);

   LONGS_EQUAL (entry.size (), entry.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Periodic_Entry, Unpack)
{
   expected = {0xFF, 0xFF, 0xFF,
               0x5A,                          // Unit ID
               0x80, 0xAA,                    // Interface : Type + UID.
               HF::Attributes::Pack::DYNAMIC, // Attribute Pack ID.
               0x03, 0xA1, 0xA2,0xA3,         // Attributes UID's
               0xFF, 0xFF, 0xFF};

   LONGS_EQUAL (entry.size (), entry.unpack (expected, 3));

   LONGS_EQUAL (0x5A, entry.unit);
   LONGS_EQUAL (HF::Interface::SERVER_ROLE, entry.itf.role);
   LONGS_EQUAL (0xAA, entry.itf.id);
   LONGS_EQUAL (HF::Attributes::Pack::DYNAMIC, entry.pack_id);

   LONGS_EQUAL (0x03, entry.attributes.size ());

   std::vector <uint8_t> temp = {0xA1, 0xA2, 0xA3};

   CHECK_EQUAL (temp, entry.attributes);
}

// =============================================================================
// AttributeReporting::Periodic::Rule
// =============================================================================

TEST_GROUP (AttrReport_Periodic_Rule)
{
   Periodic::Rule rule;

   ByteArray expected;

   TEST_SETUP ()
   {
      rule     = Periodic::Rule ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0x0A,                   // Report ID
                            0x85, 0xA5, 0xAA,       // Address : Type + UID.
                            0xAA, 0xBB, 0xCC, 0xDD, // Report period
                            0x03,                   // Number of entries.
                                                    // Entry 1
                            0x5A,                   // Unit ID.
                            0x85, 0x5A,             // Interface.
                            HF::Attributes::Pack::MANDATORY,
                            // Entry 2
                            0x6A,                       // Unit ID.
                            0x83, 0xA5,                 // Interface.
                            HF::Attributes::Pack::ALL,
                            // Entry 3
                            0x7A,                       // Unit ID.
                            0x83, 0xEE,                 // Interface.
                            HF::Attributes::Pack::DYNAMIC,
                            0x04, 0xD1, 0xD2, 0xD3, 0xD4,
                            0x00, 0x00, 0x00};
   }
};

TEST (AttrReport_Periodic_Rule, Size)
{
   LONGS_EQUAL (1 + 2 + 1 + 4 + 1, rule.size ());

   Periodic::Entry entry;

   rule.entries.push_back (entry);
   rule.entries.push_back (entry);
   rule.entries.push_back (entry);

   LONGS_EQUAL (1 + 2 + 1 + 4 + 1 + 3 * entry.size (), rule.size ());
}

TEST (AttrReport_Periodic_Rule, Pack)
{
   ByteArray result (expected.size ());

   rule.type               = PERIODIC;
   rule.id                 = 0x0A;

   rule.destination.mod    = HF::Protocol::Address::GROUP;
   rule.destination.device = 0x05A5;
   rule.destination.unit   = 0xAA;

   rule.interval           = 0xAABBCCDD;

   Periodic::Entry entry;

   entry.unit     = 0x5A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0x055A;
   entry.pack_id  = HF::Attributes::Pack::MANDATORY;

   rule.entries.push_back (entry);

   entry.unit     = 0x6A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0x03A5;
   entry.pack_id  = HF::Attributes::Pack::ALL;

   rule.entries.push_back (entry);

   entry.unit       = 0x7A;
   entry.itf.role   = HF::Interface::SERVER_ROLE;
   entry.itf.id     = 0x03EE;
   entry.pack_id    = HF::Attributes::Pack::DYNAMIC;
   entry.attributes = {0xD1, 0xD2, 0xD3, 0xD4};

   rule.entries.push_back (entry);

   LONGS_EQUAL (rule.size (), rule.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Periodic_Rule, Unpack)
{
   LONGS_EQUAL (rule.size (), rule.unpack (expected, 3));

   LONGS_EQUAL (PERIODIC, rule.type);
   LONGS_EQUAL (0x0A, rule.id);

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, rule.destination.mod);
   LONGS_EQUAL (0x05A5, rule.destination.device);
   LONGS_EQUAL (0xAA, rule.destination.unit);

   LONGS_EQUAL (0xAABBCCDD, rule.interval);

   expected[3] = 0x8A;

   LONGS_EQUAL (rule.size () - sizeof(rule.interval) - sizeof(uint8_t),
                rule.unpack (expected, 3));
}

// =============================================================================
// AttributeReporting::Event
// =============================================================================

// =============================================================================
// AttributeReporting::Event::Field
// =============================================================================

TEST_GROUP (AttrReport_Event_Field)
{
   Event::Field field;

   ByteArray    expected;

   TEST_SETUP ()
   {
      field    = Event::Field ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0x5A,                     // Attribute UID.
                            Event::EQ,                // Event Type.
                            0x04,                     // Field size.
                            0xA1, 0xA2, 0xA3, 0xA4,   // Field value.
                            0x00, 0x00, 0x00};
   }
};

TEST (AttrReport_Event_Field, Size)
{
   field.type = Event::EQ;

   LONGS_EQUAL (1 + 1 + 1, field.size ());

   field.value.push_back (0xA1);
   field.value.push_back (0xA2);
   field.value.push_back (0xA3);
   field.value.push_back (0xA4);

   LONGS_EQUAL (1 + 1 + 1 + 4, field.size ());

   field.type = Event::COV;

   LONGS_EQUAL (1 + 1 + 1, field.size ());
}

TEST (AttrReport_Event_Field, Pack)
{
   ByteArray result (expected.size ());

   field.type     = Event::EQ;
   field.attr_uid = 0x5A;

   field.value.push_back (0xA1);
   field.value.push_back (0xA2);
   field.value.push_back (0xA3);
   field.value.push_back (0xA4);

   LONGS_EQUAL (field.size (), field.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Event_Field, Pack2)
{
   expected[4] = Event::COV;
   expected[5] = 0xAA;
   expected[6] = 0x00;
   expected[7] = 0x00;
   expected[8] = 0x00;
   expected[9] = 0x00;

   ByteArray result (expected.size ());

   field.type     = Event::COV;
   field.attr_uid = 0x5A;

   field.value.push_back (0xAA);
   field.value.push_back (0xA2);
   field.value.push_back (0xA3);
   field.value.push_back (0xA4);

   LONGS_EQUAL (field.size (), field.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Event_Field, Unpack)
{
   LONGS_EQUAL (field.size (), field.unpack (expected, 3));

   LONGS_EQUAL (Event::EQ, field.type);
   LONGS_EQUAL (0x5A, field.attr_uid);

   LONGS_EQUAL (0x04, field.value.size ());

   std::vector <uint8_t> temp = {0xA1, 0xA2, 0xA3, 0xA4};

   CHECK_EQUAL (temp, field.value);
}

TEST (AttrReport_Event_Field, Unpack2)
{
   expected[4] = Event::COV;
   expected[5] = 0xAA;

   LONGS_EQUAL (field.size (), field.unpack (expected, 3));

   LONGS_EQUAL (Event::COV, field.type);
   LONGS_EQUAL (0x5A, field.attr_uid);

   LONGS_EQUAL (0x01, field.value.size ());

   LONGS_EQUAL (0xAA, field.value[0]);
}

// =============================================================================
// AttributeReporting::Event::Entry
// =============================================================================

TEST_GROUP (AttrReport_Event_Entry)
{
   Event::Entry entry;

   ByteArray    expected;

   TEST_SETUP ()
   {
      entry    = Event::Entry ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0x5A,                          // Unit ID
                            0x80, 0xAA,                    // Interface : Type + UID.
                            HF::Attributes::Pack::DYNAMIC, // Attribute Pack ID.
                            0x04,                          // Number of fields.
                                                           // Field 1.
                            0x5A,                          // Attribute UID.
                            Event::COV,                    // Event Type.
                            0x33,                          // Event Value.
                                                           // Field 2.
                            0x5A,                          // Attribute UID.
                            Event::HT,                     // Event Type.
                            0x01, 0xAA,                    // Event Value.
                                                           // Field 3.
                            0x5A,                          // Attribute UID.
                            Event::LT,                     // Event Type.
                            0x01, 0x55,                    // Event Value.
                                                           // Field 4.
                            0x5A,                          // Attribute UID.
                            Event::EQ,                     // Event Type.
                            0x01, 0x42,                    // Event Value.
                            0x00, 0x00, 0x00};
   }
};

TEST (AttrReport_Event_Entry, Size)
{
   Event::Field field;

   field.attr_uid = 0x5A;
   field.type     = Event::COV;
   field.value.push_back (0x55);

   entry.fields.push_back (field);

   LONGS_EQUAL (1 + 2 + 1 + 1 + 1, entry.size ());

   entry.pack_id = HF::Attributes::Pack::DYNAMIC;

   LONGS_EQUAL (1 + 2 + 1 + 1 + 1 + 1 + 1, entry.size ());

   entry.fields.push_back (field);
   entry.fields.push_back (field);

   LONGS_EQUAL (1 + 2 + 1 + 1 + 3 * field.size (), entry.size ());
}

TEST (AttrReport_Event_Entry, Pack)
{
   ByteArray result (expected.size ());

   entry.unit     = 0x5A;
   entry.itf.role = HF::Interface::SERVER_ROLE;
   entry.itf.id   = 0xAA;
   entry.pack_id  = HF::Attributes::Pack::DYNAMIC;

   Event::Field field;

   field.attr_uid = 0x5A;
   field.type     = Event::COV;
   field.value    = {0x33};

   entry.fields.push_back (field);

   field.type  = Event::HT;
   field.value = {0xAA};

   entry.fields.push_back (field);

   field.type  = Event::LT;
   field.value = {0x55};

   entry.fields.push_back (field);

   field.type  = Event::EQ;
   field.value = {0x42};

   entry.fields.push_back (field);

   LONGS_EQUAL (entry.size (), entry.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Event_Entry, Unpack)
{
   entry.unpack (expected, 3);

   LONGS_EQUAL (0x5A, entry.unit);
   LONGS_EQUAL (HF::Interface::SERVER_ROLE, entry.itf.role);
   LONGS_EQUAL (0xAA, entry.itf.id);
   LONGS_EQUAL (HF::Attributes::Pack::DYNAMIC, entry.pack_id);

   LONGS_EQUAL (0x04, entry.fields.size ());

   Event::Field &field = entry.fields[0];

   LONGS_EQUAL (0x5A, field.attr_uid);
   LONGS_EQUAL (Event::COV, field.type);
   LONGS_EQUAL (0x33, field.value[0]);

   field = entry.fields[1];

   LONGS_EQUAL (0x5A, field.attr_uid);
   LONGS_EQUAL (Event::HT, field.type);
   LONGS_EQUAL (0xAA, field.value[0]);

   field = entry.fields[2];

   LONGS_EQUAL (0x5A, field.attr_uid);
   LONGS_EQUAL (Event::LT, field.type);
   LONGS_EQUAL (0x55, field.value[0]);

   field = entry.fields[3];

   LONGS_EQUAL (0x5A, field.attr_uid);
   LONGS_EQUAL (Event::EQ, field.type);
   LONGS_EQUAL (0x42, field.value[0]);
}

TEST (AttrReport_Event_Entry, Unpack2)
{
   expected = {0x00, 0x00, 0x00,
               0x5A,                      // Unit ID
               0x80, 0xAA,                // Interface : Type + UID.
               HF::Attributes::Pack::ALL, // Attribute Pack ID.
               Event::COV,                // Event Type.
               0x33,                      // Event Value.
               0x00, 0x00, 0x00};

   entry.unpack (expected, 3);

   LONGS_EQUAL (0x5A, entry.unit);
   LONGS_EQUAL (HF::Interface::SERVER_ROLE, entry.itf.role);
   LONGS_EQUAL (0xAA, entry.itf.id);
   LONGS_EQUAL (HF::Attributes::Pack::ALL, entry.pack_id);

   LONGS_EQUAL (0x01, entry.fields.size ());

   Event::Field &field = entry.fields[0];

   LONGS_EQUAL (0x00, field.attr_uid);
   LONGS_EQUAL (Event::COV, field.type);
   LONGS_EQUAL (0x33, field.value[0]);
}

// =============================================================================
// AttributeReporting::Event::Rule
// =============================================================================

TEST_GROUP (AttrReport_Event_Rule)
{
   Event::Rule rule;

   ByteArray   expected;

   TEST_SETUP ()
   {
      rule     = Event::Rule ();

      expected = ByteArray {0x00, 0x00, 0x00,
                            0x0A,                          // Report ID
                            0x85, 0xA5, 0xAA,              // Address : Type + UID.
                            0x03,                          // Number of units.
                                                           // Entry 1
                            0x5A,                          // Unit ID.
                            0x80, 0xAA,                    // Interface : Type + UID.
                            HF::Attributes::Pack::DYNAMIC, // Attribute Pack ID.
                            0x01,                          // Number of fields.
                                                           // Entry 1 - Field 1.
                            0x10,                          // Attribute UID.
                            Event::COV,                    // Event Type.
                            0x33,                          // Event Value.
                                                           // Entry 2
                            0x5A,                          // Unit ID.
                            0x80, 0xAA,                    // Interface : Type + UID.
                            HF::Attributes::Pack::DYNAMIC, // Attribute Pack ID.
                            0x02,                          // Number of fields.
                                                           // Field 1.
                            0x11,                          // Attribute UID.
                            Event::HT,                     // Event Type.
                            0x01, 0xAA,                    // Event Value.
                                                           // Field 2.
                            0x12,                          // Attribute UID.
                            Event::LT,                     // Event Type.
                            0x01, 0x55,                    // Event Value.
                                                           // Entry 3
                            0x5A,                          // Unit ID.
                            0x80, 0xAA,                    // Interface : Type + UID.
                            HF::Attributes::Pack::ALL,     // Attribute Pack ID.
                            Event::COV,                    // Event Type.
                            0x88,                          // Event Value.
                            0x00, 0x00, 0x00};
   }

   Event::Entry create ()
   {
      Event::Entry entry;

      entry.unit     = 0x5A;
      entry.itf.role = HF::Interface::SERVER_ROLE;
      entry.itf.id   = 0xAA;
      entry.pack_id  = HF::Attributes::Pack::DYNAMIC;

      return entry;
   }

   size_t create_entry_1 ()
   {
      Event::Entry entry = create ();

      Event::Field field;
      field.attr_uid = 0x10;

      field.type     = Event::COV;
      field.value    = {0x33};

      entry.fields.push_back (field);

      rule.entries.push_back (entry);

      return entry.size ();
   }

   size_t create_entry_2 ()
   {
      Event::Entry entry = create ();

      Event::Field field;
      field.attr_uid = 0x11;

      field.type     = Event::HT;
      field.value    = {0xAA};

      entry.fields.push_back (field);

      field.attr_uid = 0x12;
      field.type     = Event::LT;
      field.value    = {0x55};

      entry.fields.push_back (field);

      rule.entries.push_back (entry);

      return entry.size ();
   }

   size_t create_entry_3 ()
   {
      Event::Entry entry = create ();

      entry.pack_id = HF::Attributes::Pack::ALL;

      Event::Field field;
      field       = Event::Field ();

      field.type  = Event::COV;
      field.value = {0x88};

      entry.fields.push_back (field);

      rule.entries.push_back (entry);

      return entry.size ();
   }

   void initialize ()
   {
      rule.type               = PERIODIC;
      rule.id                 = 0x0A;

      rule.destination.mod    = HF::Interface::SERVER_ROLE;
      rule.destination.device = 0x05A5;
      rule.destination.unit   = 0xAA;

      create_entry_1 ();
      create_entry_2 ();
      create_entry_3 ();
   }
};

TEST (AttrReport_Event_Rule, Size)
{
   size_t size = 1 + 2 + 1 + 1;

   LONGS_EQUAL (size, rule.size ());

   // Add an entry.
   size += create_entry_1 ();

   LONGS_EQUAL (size, rule.size ());

   // Add other entry.
   size += create_entry_2 ();

   LONGS_EQUAL (size, rule.size ());

   // Add yet another entry.
   size += create_entry_2 ();

   LONGS_EQUAL (size, rule.size ());
}

TEST (AttrReport_Event_Rule, Pack)
{
   ByteArray result (expected.size ());

   initialize ();

   LONGS_EQUAL (rule.size (), rule.pack (result, 3));

   CHECK_EQUAL (expected, result);
}

TEST (AttrReport_Event_Rule, Unpack)
{
   initialize ();

   size_t size = rule.size ();

   rule = Event::Rule ();

   LONGS_EQUAL (size, rule.unpack (expected, 3));

   LONGS_EQUAL (PERIODIC, rule.type);
   LONGS_EQUAL (0x0A, rule.id);

   LONGS_EQUAL (HF::Interface::SERVER_ROLE, rule.destination.mod);
   LONGS_EQUAL (0x05A5, rule.destination.device);
   LONGS_EQUAL (0xAA, rule.destination.unit);

   LONGS_EQUAL (0x03, rule.entries.size ());
}

// =============================================================================
// AttributeReporting - Tests
// =============================================================================

TEST_GROUP (AttributeReporting)
{};
