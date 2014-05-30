// =============================================================================
/*!
 * \file       inc/hanfun/protocol.h
 *
 * This file contains the definitions for the HAN-FUN protocol messages.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================
#ifndef HF_PROTOCOL_H
#define HF_PROTOCOL_H

#include "hanfun/common.h"

// =============================================================================
// API
// =============================================================================

namespace HF
{
   // Forward declaration.
   namespace Transport
   {
      struct Link;
   }

   /*!
    * HAN-FUN Protocol implementation.
    */
   namespace Protocol
   {
      //! HAN-FUN Broadcast - device address.
      constexpr uint16_t BROADCAST_ADDR = 0x7FFF;

      //! HAN-FUN Broadcast - unit address.
      constexpr uint8_t BROADCAST_UNIT = 0xFF;

      //! HAN-FUN Network - Maximum application payload length.
      constexpr uint16_t MAX_PAYLOAD = 0x01FF;

      // =============================================================================
      // Classes
      // =============================================================================

      /*!
       * Network Message.
       */
      struct Message
      {
         /*!
          * Message types.
          */
         typedef enum
         {
            COMMAND_REQ                   = 0x01, //!< Command request
            COMMAND_RESP_REQ              = 0x02, //!< Command request with response required.
            COMMAND_RES                   = 0x03, //!< Command response.

            GET_ATTR_REQ                  = 0x04, //!< Get attributes request.
            GET_ATTR_RES                  = 0x05, //!< Get attributes response.

            SET_ATTR_REQ                  = 0x06, //!< Set attributes request.
            SET_ATTR_RESP_REQ             = 0x07, //!< Set attributes request with response required.
            SET_ATTR_RES                  = 0x08, //!< Set attributes response.

            GET_ATTR_PACK_REQ             = 0x09, //!< Get pack attributes request.
            GET_ATTR_PACK_RES             = 0x0A, //!< Get pack attributes response.

            SET_ATTR_PACK_REQ             = 0x0B, //!< Set pack attributes request.
            SET_ATTR_PACK_RESP_REQ        = 0x0C, //!< Set pack attributes request with response required.
            SET_ATTR_PACK_RES             = 0x0D, //!< Set pack attributes response.

            ATOMIC_SET_ATTR_PACK_REQ      = 0x0E, //!< Atomic set pack attributes request.
            ATOMIC_SET_ATTR_PACK_RESP_REQ = 0x0F, //!< Atomic set pack attributes request with response required.
            ATOMIC_SET_ATTR_PACK_RES      = 0x10, //!< Atomic set pack attributes response.
         } Type;

         /*!
          * Interface Address.
          */
         struct Interface:public Common::Interface
         {
            uint8_t member;            //!< Interface destination member.

            Interface(uint16_t uid = 0, uint16_t role = 0, uint8_t member = 0):Common::Interface(uid, role), member (member) {}

            //! \see HF::Serializable::size.
            size_t size () const;

            //! \see HF::Serializable::pack.
            size_t pack (Common::ByteArray &array, size_t offset = 0) const;

            //! \see HF::Serializable::unpack.
            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

         // =============================================================================
         // API
         // =============================================================================

         uint8_t   reference;          //!< Application reference.
         Type      type;               //!< Message type. \see Message::Type

         Interface itf;                //!< Interface Address.

         /*!
          * Message payload.
          */
         Common::ByteArray payload;

         //! The payload length value read when unpacking the message.
         uint16_t length;

         Message(size_t size = 0, Type _type = COMMAND_REQ):
            reference (0), type (_type), payload (Common::ByteArray (size)), length (0) {}

         Message(Message &parent, size_t size);

         //! \see HF::Serializable::size.
         size_t size () const;

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const;

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0);
      };

      /*!
       * Network Address.
       */
      struct Address
      {
         uint16_t mod    : 1;    //!< Address modifier.
         uint16_t device : 15;   //!< Device Address.

         uint8_t  unit;          //!< Source Unit.

         /*!
          * HAN-FUN Network Destination Address Types.
          */
         enum DestinationType
         {
            DEVICE_ADDR = 0,   //!< Destination address is for single device.
            GROUP_ADDR  = 1,   //!< Destination address is for a group of devices.
         };

         /*!
          * Create a new message address.
          *
          * @param _dev    device address. Default \c HF_BROADCAST_ADDR.
          * @param _unit   unit address. Default \c HF_BROADCAST_UNIT.
          * @param _mod    address modifier. Default \c DEVICE_ADDR.
          */
         Address(uint16_t _dev = BROADCAST_ADDR, uint8_t _unit = BROADCAST_UNIT,
                 DestinationType _mod = DEVICE_ADDR)
            :mod (_mod), device (_dev), unit (_unit)
         {}

         //! \see HF::Serializable::size.
         size_t size () const;

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const;

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0);

         bool   is_broadcast ()
         {
            return device == BROADCAST_ADDR && unit == BROADCAST_UNIT;
         }

         bool is_local (uint16_t address)
         {
            return this->device == address;
         }
      };

      /*!
       * HAN-FUN Protocol Packet.
       */
      struct Packet
      {
         Address source;           //!< Source Address.
         Address destination;      //!< Destination Address.

         /*!
          * Packet message payload;
          */
         Message         message;

         Transport::Link *link;     //! Link where this packet originated from.

         Packet() {}

         Packet(Message &message):message (message) {}

         Packet(Address &dst_addr, Message &message, uint8_t unit = BROADCAST_UNIT):
            destination (dst_addr), message (message)
         {
            source.mod    = Address::DEVICE_ADDR;
            source.device = BROADCAST_ADDR;
            source.unit   = unit;
         }

         Packet(Address &src_addr, Address &dst_addr, Message &message):
            source (src_addr), destination (dst_addr), message (message) {}

         //! \see HF::Serializable::size.
         size_t size () const;

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const;

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0);
      };

      /*!
       * HAN-FUN Response message.
       */
      struct Response
      {
         // =============================================================================
         // API
         // =============================================================================

         Common::Result code;

         Response(Common::Result code = Common::Result::OK):code (code) {}

         //! \see HF::Serializable::size.
         size_t size () const;

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const;

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0);
      };

   }  // namespace Protocol

}  // namespace HF

#endif /* HF_PROTOCOL_H */
