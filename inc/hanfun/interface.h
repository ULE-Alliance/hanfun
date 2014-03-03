// =============================================================================
/*!
 * \file       inc/hanfun/interface.h
 *
 * This file contains the definitions common to all interfaces.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_INTERFACE_H
#define HF_INTERFACE_H

#include "hanfun/common.h"
#include "hanfun/attributes.h"

#include "hanfun/protocol.h"

namespace HF
{
   /*!
    * Common interface for all Interfaces.
    */
   struct Interface
   {
      /*!
       * Interface roles.
       */
      typedef enum
      {
         CLIENT_ROLE = 0,   //!< Client Role.
         SERVER_ROLE = 1,   //!< Server Role.
      } Role;

      /*!
       * Interfaces Unique Identifiers (UID).
       */
      typedef enum
      {
         /* Core Services */
         DEVICE_MANAGEMENT  = 0x0001,   //!< Device Management Interface UID.
         DEVICE_INFORMATION = 0x0005,   //!< Device Information Interface UID.

         /* Functional Interfaces. */
         ALERT              = 0x0100,  //!< Alert Interface UID
         ON_OFF             = 0x0200,  //!< ON-OFF Interface UID
         LEVEL_CONTROL      = 0x0201,  //!< Level Control Interface UID
         SIMPLE_POWER_METER = 0x0300,  //!< Simple Power Meter Interface UID

         /* Reserved */
         RESERVED = 0x7F00,           //!< Proprietary interfaces.
         MAX_UID  = 0x7FFF            //!< Max interface UID value.
      } UID;

      enum Commands
      {
         MAX_CMD_ID = 0xFF,           //! Maximum value for command ids.
      };

      // =============================================================================
      // API
      // =============================================================================

      /*!
       * This method returns the interface UID.
       *
       * Subclasses MUST override this method to provide a valid value
       * for the interface being implemented.
       *
       * This method returns a \c uint16_t and not a Interface::UID,
       * to provide support for proprietary interfaces.
       *
       * @return  the UID for the interface.
       */
      virtual uint16_t uid () const = 0;

      /*!
       * Return the Interface::Role this interface implements.
       *
       * @return  the Interface::Role implemented by the interface.
       */
      virtual Interface::Role role () const = 0;

      /*!
       * Handle incoming messages from the network.
       *
       * @param [in]    packet   the packet receive from the network.
       *
       * @param [in]    payload  the byte array containing the data received from the
       *                         network.
       *
       * @param [in]    offset   the offset the payload start at in the byte array.
       *
       * @return        the result of the message processing.
       */
      virtual Result handle (Protocol::Packet &packet, ByteArray &payload, size_t offset) = 0;

      /*!
       * Handle periodic processing.
       *
       * @param [in] time    current system clock value in seconds.
       */
      virtual void periodic (uint32_t time) = 0;

      /*!
       * Return a pointer to the interface attribute with the given \c uid.
       *
       * @param [in] uid   identifier of the attribute in the interface.
       *
       * @return     a pointer to the attribute if it exists,
       *             \c nullptr otherwise.
       */
      virtual IAttribute *attribute (uint8_t uid) = 0;
   };

   /*!
    * Common implementation of Interface functionality.
    */
   struct AbstractInterface:virtual public Interface
   {
      //! \see Interface::handle
      virtual Result handle (Protocol::Packet &packet, ByteArray &payload, size_t offset);

      //! \see Interface::periodic
      virtual void periodic (uint32_t time)
      {
         UNUSED (time);
      }

      //! \see Interface::attribute
      virtual IAttribute *attribute (uint8_t uid)
      {
         UNUSED (uid);
         return nullptr;
      }

      bool operator ==(AbstractInterface &other)
      {
         return uid () == other.uid ();
      }

      bool operator !=(AbstractInterface &other)
      {
         return !(*this == other);
      }

      protected:

      /*!
       * Send message \c msg to the network address given by \c addr.
       *
       * @param [in] addr        HF network address.
       * @param [in] message     pointer to the message to be sent to the network.
       */
      virtual void sendMessage (Protocol::Address &addr, Protocol::Message &message) = 0;

      /*!
       * Check if message has correct attributes to be processed by the interface.
       *
       * The attributes checked are :
       * - Interface UID;
       * - Interface Role;
       *
       * \see Interface::handle.
       */
      Result check_message (Protocol::Message &message, ByteArray &payload, size_t offset);

      /*!
       * Check if \c payload data size if sufficient for processing the \c message.
       *
       * \see Interface::handle.
       */
      Result check_payload_size (Protocol::Message &message, ByteArray &payload, size_t offset);

      /*!
       * Return the minimal payload size that should be present for the given
       * message.
       *
       * @param message message that was received.
       *
       * @return  the minimum size in bytes that the packet payload should hold.
       */
      virtual size_t payload_size (Protocol::Message &message) const;

      /*!
       * Return the minimal payload size that a message should hold when
       * addressed at the given interface.
       *
       * @param itf  the interface being address.
       *
       * @return  the minimum number of bytes for the message for the interface.
       */
      virtual size_t payload_size (Protocol::Message::Interface &itf) const
      {
         UNUSED (itf);
         return 0;
      }

      template<typename _Message_T>
      size_t payload_size_helper () const
      {
         static_assert (is_base_of <HF::Serializable, _Message_T>::value,
                        "_Message_T must be of type HF::Serializable");

         _Message_T message;
         return message.size ();
      }

      /*!
       * Handle command request/response messages, i.e. :
       *  - Protocol::Message:Type::COMMAND_REQ;
       *  - Protocol::Message:Type::COMMAND_RESP_REQ;
       *  - Protocol::Message:Type::COMMAND_RES;
       *
       * \see Interface::handle
       */
      virtual Result handle_command (Protocol::Packet &packet, ByteArray &payload, size_t offset);

      /*!
       * Handle attributes request/response messages, i.e. :
       *  - Protocol::Message:Type::GET_ATTR_REQ;
       *  - Protocol::Message:Type::SET_ATTR_REQ;
       *  - Protocol::Message:Type::SET_ATTR_RESP_REQ;
       *  - Protocol::Message:Type::GET_ATTR_PACK_REQ;
       *  - Protocol::Message:Type::SET_ATTR_PACK_REQ;
       *  - Protocol::Message:Type::SET_ATTR_PACK_RESP_REQ;
       *
       * \see Interface::handle
       */
      virtual Result handle_attribute (Protocol::Packet &packet, ByteArray &payload, size_t offset);

      /*!
       * Return a vector containing the attribute UIDs, for the given pack ID.
       *
       * @param [in] pack_id     the Attribute pack ID to get the attributes UIDs for.
       *
       * @return  vector containing the attributes UIDs.
       */
      virtual attribute_uids_t attributes (uint8_t pack_id = AttributePack::MANDATORY) const
      {
         UNUSED (pack_id);
         return attribute_uids_t ();
      }
   };

   /*!
    * This is the top-level namespace for all the Interfaces implemented.
    */
   namespace Interfaces
   {
      /*!
       * Class template for all interfaces implementations.
       */
      template<Interface::UID _uid>
      struct Base:public AbstractInterface
      {
         //! \see Interface::uid
         uint16_t uid () const
         {
            return _uid;
         }
      };

      /*!
       * Class template for all interfaces role implementations.
       */
      template<typename Itf, Interface::Role _role>
      struct InterfaceRole:public Itf
      {
         //! \see Interface::role
         Interface::Role role () const
         {
            return _role;
         }
      };

   }  // namespace Interfaces

}  // namespace HF

#endif /* HF_INTERFACE_H */
