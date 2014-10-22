// =============================================================================
/*!
 * \file       /inc/hanfun/core/attribute_reporting.h
 *
 * This file contains the definitions for the core Attribute Reporting Service
 * in HAN-FUN.
 *
 * \version    1.1.0
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
#include <memory>

#include "hanfun/common.h"
#include "hanfun/core.h"

#include "hanfun/attributes.h"

namespace HF
{
   namespace Core
   {
      // Forward declaration.
      namespace AttributeReporting
      {
         struct Server;
      }  // namespace AttributeReporting

      HF::Attributes::IAttribute *create_attribute (AttributeReporting::Server *server, uint8_t uid);

      /*!
       * This namespace contains the classes that implements
       * HAN-FUN's Core interface - Attribute Reporting.
       */
      namespace AttributeReporting
      {
         //! Commands.
         typedef enum CMD
         {
            PERIODIC_REPORT_CMD    = 0x01, //!< Periodic Report.
            EVENT_REPORT_CMD       = 0x02, //!< Event Report.
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
          * Report Identifier.
          */
         struct Reference
         {
            uint8_t type : 1; //!< Report type : PERIODIC or EVENT
            uint8_t id   : 7; //!< Report ID.

            Reference(uint8_t _type = 0, uint8_t _id = 0):
               type (_type), id (_id)
            {}

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         /*!
          * Parent class for all report rule entries.
          */
         struct Entry
         {
            uint8_t           unit;    //!< Unit ID this entry to.
            Common::Interface itf;     //!< Interface UID.

            uint8_t           pack_id; //!< Attribute's Pack ID.

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         /*!
          * Parent class for all report rules.
          */
         struct Rule
         {
            //! Report reference this rule generates.
            Reference report;

            //! Device/unit that will receive the report.
            Protocol::Address destination;

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };


         /*!
          * This namespace contains the classes to handle the attribute reporting
          * periodic functionality.
          */
         namespace Periodic
         {
            /*!
             * This class represents an entry in a periodic rule.
             */
            struct Entry:public AttributeReporting::Entry
            {
               HF::Attributes::UIDS uids;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);
            };

            /*!
             * This class represents a periodic rule for attribute reporting.
             */
            struct Rule:public AttributeReporting::Rule
            {
               typedef std::forward_list <Entry> Container;

               typedef Container::iterator iterator;

               typedef Container::const_iterator const_iterator;

               //! Attribute indicating the last time rule was activated.
               uint32_t last_time;

               Rule(uint32_t __interval = 0):
                  last_time (0), _interval (__interval)
               {}

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);

               /*!
                * Add the given \c entry to rule.
                *
                * @param [in] entry    reference to the entry to add to this rule.
                */
               void add (const Entry &entry)
               {
                  entries.push_front (entry);
               }

               /*!
                * Remove all entries in this rule.
                */
               void clear ()
               {
                  entries.clear ();
               }

               /*!
                * Number of entries present in this rule.
                *
                * @return  number of entries present in this rule.
                */
               size_t entries_count ()
               {
                  return std::distance (entries.begin (), entries.end ());
               }

               /*!
                * Time interval to generate a report based on this rule.
                *
                * @return  time interval in seconds to generate a report.
                */
               uint32_t interval ()
               {
                  return _interval;
               }

               iterator begin ()
               {
                  return entries.begin ();
               }

               iterator end ()
               {
                  return entries.end ();
               }

               const_iterator cbegin () const
               {
                  return entries.cbegin ();
               }

               const_iterator cend () const
               {
                  return entries.cend ();
               }

               protected:

               //! Attribute indicating the time interval rule needs to be activated.
               uint32_t  _interval;

               Container entries;
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
               //! Field type.
               Type type;

               //! Attribute UID.
               uint8_t attr_uid;

               //! Field value.
               Common::ByteArray value;

               Field(Type _type = COV, uint8_t _attr_uid = 0):
                  type (_type), attr_uid (_attr_uid)
               {}

               size_t size (bool with_uid) const;

               size_t pack (Common::ByteArray &array, size_t offset, bool with_uid) const;

               size_t unpack (const Common::ByteArray &array, size_t offset, bool with_uid);
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
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);
            };

            /*!
             * Report event rule.
             */
            struct Rule:public AttributeReporting::Rule
            {
               typedef std::forward_list <Entry> Container;

               typedef Container::iterator iterator;

               typedef Container::const_iterator const_iterator;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);

               /*!
                * Add the given \c entry to rule.
                *
                * @param [in] entry    reference to the entry to add to this rule.
                */
               void add (const Entry &entry)
               {
                  entries.push_front (entry);
               }

               /*!
                * Remove all entries in this rule.
                */
               void clear ()
               {
                  entries.clear ();
               }

               /*!
                * Number of entries present in this rule.
                *
                * @return  number of entries present in this rule.
                */
               size_t entries_count ()
               {
                  return std::distance (entries.begin (), entries.end ());
               }

               iterator begin ()
               {
                  return entries.begin ();
               }

               iterator end ()
               {
                  return entries.end ();
               }

               const_iterator cbegin () const
               {
                  return entries.cbegin ();
               }

               const_iterator cend () const
               {
                  return entries.cend ();
               }

               protected:

               Container entries;
            };

         }  // namespace Event

         /*!
          * This namespace contains the classes that implement the
          * attribute reporting notifications.
          */
         namespace Report
         {
            // =============================================================================
            // Notification API
            // =============================================================================

            typedef std::shared_ptr <HF::Attributes::IAttribute> Attribute;

            /*!
             * Top-level parent for all notification.
             */
            struct Abstract:public AttributeReporting::Reference
            {};

            /*!
             * Top-level parent for all notification entries.
             */
            struct Entry
            {
               uint8_t           unit;
               Common::Interface itf;

               Entry():unit (0) {}

               Entry(uint8_t _unit, Common::Interface _itf):
                  unit (_unit), itf (_itf)
               {}

               virtual ~Entry() {}

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (HF::Attributes::FactoryGetter get_factory,
                              const Common::ByteArray &array, size_t offset = 0);

               protected:

               virtual size_t unpack (HF::Attributes::Factory factory,
                                      const Common::ByteArray &array,
                                      size_t offset = 0)
               {
                  UNUSED (factory);
                  UNUSED (array);
                  UNUSED (offset);

                  return 0;
               }

               virtual uint8_t count () const
               {
                  return 0;
               }
            };

            // =============================================================================
            // Messages
            // =============================================================================

            /*!
             * This message is used to create a reporting rule.
             */
            struct CreateMessage
            {
               //! Device address to create the report rule for.
               Protocol::Address destination;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);
            };

            /*!
             * This message is used to delete a reporting rule.
             */
            struct DeleteMessage
            {
               //! Identification of the rule to delete.
               Reference report;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);
            };

            /*!
             * Parent class for the messages to create reporting
             * rule entries.
             */
            struct AddEntryMessage
            {
               //! Identification of the rule to add the entries to.
               Reference report;

               virtual ~AddEntryMessage() {}

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0);

               protected:

               virtual size_t unpack_entry (const Common::ByteArray &array, size_t offset = 0)
               {
                  UNUSED (array);
                  UNUSED (offset);

                  return 0;
               }

               virtual uint8_t count () const
               {
                  return 0;
               }
            };

            // =============================================================================
            // Periodic Report Support
            // =============================================================================

            /*!
             * Periodic report notification.
             */
            struct Periodic:public Abstract
            {
               /*!
                * Periodic notification entry.
                */
               struct Entry:public Report::Entry
               {
                  std::vector <Report::Attribute> attributes;

                  //! \see HF::Common::Serializable::size
                  size_t size () const;

                  //! \see HF::Common::Serializable::pack
                  size_t pack (Common::ByteArray &array, size_t offset = 0) const;

                  void add (HF::Attributes::IAttribute * &attr);

                  using Report::Entry::unpack;

                  protected:

                  //! \see HF::Common::Serializable::unpack
                  size_t unpack (HF::Attributes::Factory factory,
                                 const Common::ByteArray &array, size_t offset = 0);

                  uint8_t count () const
                  {
                     return attributes.size ();
                  }
               };

               //! Entries associated with this notification.
               std::forward_list <Entry> entries;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (HF::Attributes::FactoryGetter get_factory,
                              const Common::ByteArray &array, size_t offset = 0);

               void add (Entry &entry);

               // =============================================================================
               // Message API
               // =============================================================================

               struct CreateMessage:public Report::CreateMessage
               {
                  //! Time interval to send the periodic report.
                  uint32_t interval;

                  //! \see HF::Common::Serializable::size
                  size_t size () const;

                  //! \see HF::Common::Serializable::pack
                  size_t pack (Common::ByteArray &array, size_t offset = 0) const;

                  //! \see HF::Common::Serializable::unpack
                  size_t unpack (const Common::ByteArray &array, size_t offset = 0);
               };

               struct AddEntryMessage:public Report::AddEntryMessage
               {
                  typedef std::forward_list <AttributeReporting::Periodic::Entry> Container;

                  typedef Container::iterator iterator;

                  typedef Container::const_iterator const_iterator;

                  AddEntryMessage()
                  {
                     report.type = PERIODIC;
                  }

                  //! \see HF::Common::Serializable::size
                  size_t size () const;

                  //! \see HF::Common::Serializable::pack
                  size_t pack (Common::ByteArray &array, size_t offset = 0) const;

                  void add (const AttributeReporting::Periodic::Entry &entry)
                  {
                     entries.push_front (entry);
                  }

                  iterator begin ()
                  {
                     return entries.begin ();
                  }

                  iterator end ()
                  {
                     return entries.end ();
                  }

                  const_iterator cbegin () const
                  {
                     return entries.cbegin ();
                  }

                  const_iterator cend () const
                  {
                     return entries.cend ();
                  }

                  protected:

                  Container entries;

                  size_t unpack_entry (const Common::ByteArray &array, size_t offset = 0);

                  uint8_t count () const
                  {
                     return std::distance (entries.begin (), entries.end ());
                  }
               };
            };

            // =============================================================================
            // Event Report Support
            // =============================================================================

            /*!
             * Event type notification.
             */
            struct Event:public Abstract
            {
               /*!
                * Field for the entries in event notification.
                */
               struct Field
               {
                  //! Event type.
                  AttributeReporting::Event::Type type;

                  //! Attribute value.
                  Attribute attribute;

                  //! \see HF::Common::Serializable::size
                  size_t size () const;

                  //! \see HF::Common::Serializable::pack
                  size_t pack (Common::ByteArray &array, size_t offset = 0) const;

                  //! \see HF::Common::Serializable::unpack
                  size_t unpack (HF::Attributes::Factory factory, const Common::ByteArray &array,
                                 size_t offset = 0);

                  void set_attribute (HF::Attributes::IAttribute *attr)
                  {
                     Attribute temp (attr);
                     attribute = std::move (temp);
                  }
               };

               /*!
                * Event notification entry.
                */
               struct Entry:public Report::Entry
               {
                  std::vector <Field> fields;

                  //! \see HF::Common::Serializable::size
                  size_t size () const;

                  //! \see HF::Common::Serializable::pack
                  size_t pack (Common::ByteArray &array, size_t offset = 0) const;

                  void add (Field &field)
                  {
                     fields.push_back (std::move (field));
                  }

                  using Report::Entry::unpack;

                  protected:

                  //! \see HF::Common::Serializable::unpack
                  size_t unpack (HF::Attributes::Factory factory,
                                 const Common::ByteArray &array, size_t offset = 0);

                  uint8_t count () const
                  {
                     return fields.size ();
                  }
               };

               //! Entries for the event notification.
               std::forward_list <Entry> entries;

               //! \see HF::Common::Serializable::size
               size_t size () const;

               //! \see HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const;

               //! \see HF::Common::Serializable::unpack
               size_t unpack (HF::Attributes::FactoryGetter get_factory,
                              const Common::ByteArray &array, size_t offset = 0);

               void add (Entry &entry)
               {
                  entries.push_front (std::move (entry));
               }

               // =============================================================================
               // Message API
               // =============================================================================

               struct CreateMessage:public Report::CreateMessage
               {};

               struct AddEntryMessage:public Report::AddEntryMessage
               {
                  typedef std::forward_list <AttributeReporting::Event::Entry> Container;

                  typedef Container::iterator iterator;

                  typedef Container::const_iterator const_iterator;

                  AddEntryMessage()
                  {
                     report.type = EVENT;
                  }

                  //! \see HF::Common::Serializable::size
                  size_t size () const;

                  //! \see HF::Common::Serializable::pack
                  size_t pack (Common::ByteArray &array, size_t offset = 0) const;

                  void add (const AttributeReporting::Event::Entry &entry)
                  {
                     entries.push_front (entry);
                  }

                  iterator begin ()
                  {
                     return entries.begin ();
                  }

                  iterator end ()
                  {
                     return entries.end ();
                  }

                  const_iterator cbegin () const
                  {
                     return entries.cbegin ();
                  }

                  const_iterator cend () const
                  {
                     return entries.cend ();
                  }

                  protected:

                  Container entries;

                  size_t unpack_entry (const Common::ByteArray &array, size_t offset = 0);

                  uint8_t count () const
                  {
                     return std::distance (entries.begin (), entries.end ());
                  }
               };

               static Report::Event::Entry *process (const AttributeReporting::Event::Entry &entry,
                                                     const HF::Attributes::IAttribute &old_value,
                                                     const HF::Attributes::IAttribute &new_value);
            };

         }  // namespace Report

         // =============================================================================
         // Message API
         // =============================================================================

         struct Response:public HF::Protocol::Response
         {
            Reference report;

            //! \see HF::Common::Serializable::size
            size_t size () const;

            //! \see HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // API
         // =============================================================================

         //! Start value for report identifiers.
         static constexpr uint8_t START_ADDR = 0x01;

         //! Maximum value for report identifiers.
         static constexpr uint8_t MAX_ADDR = 0x7E;

         //! Value indicating all reports.
         static constexpr uint8_t ALL_ADDR = 0x7F;

         typedef std::vector <Periodic::Entry>::iterator periodic_iterator;
         typedef std::vector <Event::Entry>::iterator event_iterator;

         Protocol::Message *create (Protocol::Address &destination);

         Protocol::Message *create (Protocol::Address &destination, uint32_t interval);

         Protocol::Message *destroy (Type type, uint8_t report_id);

         Protocol::Message *destroy (Reference report);

         Protocol::Message *add (Reference report, periodic_iterator begin, periodic_iterator end);

         Protocol::Message *add (Reference report, event_iterator begin, event_iterator end);

         HF::Attributes::IAttribute *create_attribute (uint8_t uid);

         /*!
          * Attribute Reporting - Client Role.
          */
         struct Client:public HF::Interfaces::Base <HF::Interface::ATTRIBUTE_REPORTING>
         {
            // =============================================================================
            // Interface API
            // =============================================================================

            //! \see Interface::role
            Interface::Role role () const
            {
               return HF::Interface::CLIENT_ROLE;
            }

            // =============================================================================
            // API
            // =============================================================================

            // ======================================================================
            // Events
            // ======================================================================
            //! \name Events
            //! @{

            /*!
             * Event indicating a \c PERIODIC_REPORT_CMD or a \c EVENT_REPORT_CMD.
             *
             * @param [in] type     type of report.
             * @param [in] payload  data in the report.
             * @param [in] offset   offset from which to start reading the data of the
             *                      the report in the \c payload.
             */
            virtual void report (Type type, Common::ByteArray &payload, size_t offset)
            {
               UNUSED (type);
               UNUSED (payload);
               UNUSED (offset);
            }

            /*!
             * Event indicating the result of a \c CREATE_PERIODIC_CMD or
             * \c CREATE_EVENT_CMD.
             *
             * @param [in] address     device address that originated the event.
             * @param [in] response    result of the operation.
             */
            virtual void created (const Protocol::Address &address,
                                  const Response &response)
            {
               UNUSED (address);
               UNUSED (response);
            }

            /*!
             * Event indicating the result of a \c ADD_PERIODIC_ENTRY_CMD or
             * \c ADD_EVENT_ENTRY_CMD.
             *
             * @param [in] address     device address that originated the event.
             * @param [in] response    result of the operation.
             */
            virtual void added (const Protocol::Address &address,
                                const Response &response)
            {
               UNUSED (address);
               UNUSED (response);
            }

            /*!
             * Event indicating the result of a \c DELETE_REPORT_CMD.
             *
             * @param [in] address     device address that originated the event.
             * @param [in] response    result of the operation.
             */
            virtual void deleted (const Protocol::Address &address,
                                  const Response &response)
            {
               UNUSED (address);
               UNUSED (response);
            }

            //! @}
            // ======================================================================

            protected:

            //! \see AbstractInterface::payload_size
            size_t payload_size (Protocol::Message &message) const;

            //! \see AbstractInterface::payload_size
            size_t payload_size (Protocol::Message::Interface &itf) const;

            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                           size_t offset);

         };

         /*!
          * Attribute Reporting - Server Role.
          */
         struct Server:public AbstractService
         {
            std::forward_list <Periodic::Rule> periodic_rules;
            std::forward_list <Event::Rule>    event_rules;

            Server(Unit0 &unit):
               AbstractService (unit), last_time (0)
            {}

            virtual ~Server() {}

            // =============================================================================
            // API
            // =============================================================================

            /*!
             * Return a reference to the unit that this service belongs to.
             *
             * This is the same reference as AbstractService::unit, but static casted
             * to allow access to the the other interfaces.
             *
             * @return  a reference to the unit that holds this interface.
             */
            // HF::Devices::Concentrator::IUnit0 &unit0 ();

            //! \see Interface::uid
            uint16_t uid () const
            {
               return HF::Interface::ATTRIBUTE_REPORTING;
            }

            //! \see Interface::role
            Interface::Role role () const
            {
               return HF::Interface::SERVER_ROLE;
            }

            virtual Common::Result handle (const Report::Periodic::CreateMessage &message);

            virtual Common::Result handle (const Report::Periodic::AddEntryMessage &message);

            virtual Common::Result handle (const Report::Event::CreateMessage &message);

            virtual Common::Result handle (const Report::Event::AddEntryMessage &message);

            virtual Common::Result handle (const Report::DeleteMessage &message);

            using HF::Interfaces::AbstractInterface::handle;

            void periodic (uint32_t time);

            virtual void notify (uint8_t unit, const HF::Attributes::IAttribute &old_value,
                                 const HF::Attributes::IAttribute &new_value);

            size_t count (Type type) const;

            // =============================================================================
            // Interface Attribute API.
            // =============================================================================

            //! \see Interface::attribute
            HF::Attributes::IAttribute *attribute (uint8_t uid)
            {
               return Core::create_attribute (this, uid);
            }

            //! \see AbstractInterface::attributes
            HF::Attributes::UIDS attributes (uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
            {
               UNUSED (pack_id);
               return HF::Attributes::UIDS {REPORT_COUNT_ATTR, PERIODIC_REPORT_COUNT_ATTR,
                                            EVENT_REPORT_COUNT_ATTR};
            }

            protected:

            uint32_t last_time;

            using AbstractService::notify;

            //! \see AbstractInterface::payload_size
            size_t payload_size (Protocol::Message::Interface &itf) const;

            //! \see AbstractInterface::handle_command
            Common::Result handle_command (Protocol::Packet &packet, Common::ByteArray &payload,
                                           size_t offset);

            /*!
             * Send the response to a command with the given \c result.
             *
             * @param [in] packet   incoming packet to generate the response for.
             * @param [in] report   report identification.
             * @param [in] result   operation result.
             */
            void response (Protocol::Packet &packet, Reference &report, Common::Result result);

            /*!
             * Check if the given command is authorized for the device with
             * the given address.
             *
             * @param [in] member interface member to check.
             * @param [in] source device address of the incoming command.
             *
             * @retval  true  the command if authorized.
             * @retval  false otherwise.
             */
            virtual bool authorized (uint8_t member, Protocol::Address &source)
            {
               UNUSED (member);
               UNUSED (source);

               return true;
            }

            bool check_uid (uint16_t uid) const
            {
               return Server::uid () == uid;
            }

            void process_event (Report::Event::Entry &entry, const Event::Field &field,
                                const HF::Attributes::IAttribute &old_value,
                                const HF::Attributes::IAttribute &new_value) const;
         };

      }  // namespace AttributeReporting

   }  // namespace Core

}  // namespace HF

#endif /* HF_CORE_ATTRIBUTE_REPORTING_H */
