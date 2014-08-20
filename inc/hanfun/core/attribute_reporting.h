// =============================================================================
/*!
 * \file       /inc/hanfun/core/attribute_reporting.h
 *
 * This file contains the definitions for the core Attribute Reporting Service
 * in HAN-FUN.
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
#ifndef HF_CORE_ATTRIBUTE_REPORTING_H
#define HF_CORE_ATTRIBUTE_REPORTING_H

#include <forward_list>

#include "hanfun/common.h"
#include "hanfun/core.h"

#include "hanfun/attributes.h"

namespace HF
{
   namespace Core
   {
      /*!
       * This namespace contains the classes that implements
       * HAN-FUN's Core interface - Attribute Reporting.
       */
      namespace AttributeReporting
      {
         //! Commands.
         typedef enum CMD
         {
            CREATE_PERIODIC_CMD    = 0x01, //!< Create Periodic Report.
            CREATE_EVENT_CMD       = 0x02, //!< Create Event Report.
            ADD_PERIODIC_ENTRY_CMD = 0x03, //!< Add periodic report entry.
            ADD_EVENT_ENTRY_CMD    = 0x04, //!< Add event report entry.
            DELETE_REPORT_CMD      = 0x05, //!< Delete report.
            GET_PERIODIC_ENTRIES   = 0x06, //!< Get periodic entries.
            GET_EVENT_ENTRIES      = 0x07, //!< Get event entries.
         } CMD;

         //! Attributes.
         typedef enum Attributes
         {
            REPORT_COUNT_ATTR          = 0x01, //!< Number of report entries attribute.
            PERIODIC_REPORT_COUNT_ATTR = 0x02, //!< Number of periodic report entries attribute.
            EVENT_REPORT_COUNT_ATTR    = 0x03, //!< Number of event report entries attribute.
         } Attributes;

         //! Types of reports send from the server to the client.
         typedef enum Type
         {
            PERIODIC = 0x00, //!< Periodic Report.
            EVENT    = 0x01, //!< Event Report.
         } Type;

         /*!
          * Parent class for all report rule entries.
          */
         struct Entry
         {
            uint8_t           unit;    //!< Unit ID this entry to.
            uint8_t           pack_id; //!< Attribute's Pack ID.

            Common::Interface itf;  //!< Interface UID.

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset);
         };

         /*!
          * Parent class for all report rules.
          */
         struct Rule
         {
            uint8_t type : 1; //!< Rule type : PERIODIC or EVENT
            uint8_t id   : 7; //!< Report ID.

            //! Device/unit that will receive the report.
            Protocol::Address destination;

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset);
         };

         /*!
          * Namespace for periodic Attribute Reporting.
          */
         namespace Periodic
         {
            /*!
             * Report periodic entry.
             */
            struct Entry:public AttributeReporting::Entry
            {
               //! Attributes UIDs when the pack_id is Attributes::Pack::Dynamic
               std::vector <uint8_t> attributes;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset);
            };

            /*!
             * Report periodic rule.
             */
            struct Rule:public AttributeReporting::Rule
            {
               uint32_t interval;   //!< Period to send reports at.

               //! Report entries.
               std::vector <Entry> entries;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset);
            };

         }  // namespace Periodic

         /*!
          * Namespace for Attribute Reporting based on events.
          */
         namespace Event
         {
            /*!
             * Types of events.
             */
            typedef enum Type
            {
               COV = 0x00, //!< Change of value event.
               HT  = 0x01, //!< High Threshold reached event.
               LT  = 0x02, //!< Low Threshold reached event.
               EQ  = 0x03, //!< Equal value event.
            } Type;

            /*!
             * Entry field for a given attribute.
             */
            struct Field
            {
               uint8_t           attr_uid; //!< Field Attribute UID.
               Type              type;     //!< Field type.
               Common::ByteArray value;    //!< Field value.

               Field():attr_uid (0)
               {}

               /*!
                * \see HF::Common::Serializable::size
                *
                * @param [in] _attr_uid   include the attr_uid field.
                */
               size_t size (bool _attr_uid = true) const;

               /*!
                * \see HF::Common::Serializable::pack
                *
                * @param [in] _attr_uid   include the attr_uid field.
                *
                */
               size_t pack (Common::ByteArray &array, size_t offset, bool _attr_uid = true) const;

               /*!
                * \see HF::Common::Serializable::unpack
                *
                * @param [in] _attr_uid   include the attr_uid field.
                *
                */
               size_t unpack (const Common::ByteArray &array, size_t offset, bool _attr_uid = true);
            };

            /*!
             * Report event entry.
             */
            struct Entry:public AttributeReporting::Entry
            {
               std::vector <Field> fields;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset);
            };

            /*!
             * Report event rule.
             */
            struct Rule:public AttributeReporting::Rule
            {
               std::vector <Entry> entries;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset);
            };

         }  // namespace Event

      }  // namespace AttributeReporting

   }  // namespace Core

}  // namespace HF

#endif /* HF_CORE_ATTRIBUTE_REPORTING_H */
