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

#include "protocol.h"

using namespace HF::Protocol;

namespace HF
{
   /*!
    * Common interface for all Interfaces.
    */
   class Interface
   {
      public:

      /*!
       * Interface roles.
       */
      enum Role
      {
         CLIENT_ROLE = 0,   //!< Client Role.
         SERVER_ROLE = 1,   //!< Server Role.
      };

      /*!
       * Interfaces Unique Identifiers (UID).
       */
      enum UID
      {
         /* Functional Interfaces. */
         ALERT              = 0x0100, //!< Alert Interface UID

         /* Reserved */
         RESERVED = 0x7F00,           //!< Proprietary interfaces.
         MAX_UID  = 0x7FFF            //!< Max interface UID value.
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
       * @param [inout] message  the message receive from the network.
       *                         This will updated if a response is to send back.
       *
       * @param [in]    payload  the byte array containing the data received from the
       *                         network.
       *
       * @param [in]    offset   the offset the payload start at in the byte array.
       *
       * @retval  true     if the message was handled by this interface,
       * @retval  false    otherwise.
       */
      virtual bool handle (Protocol::Message &message, ByteArray &payload, size_t offset) = 0;

      /*!
       * Handle periodic processing.
       *
       * @param [in] time    current system clock value in seconds.
       */
      virtual void periodic (uint32_t time) = 0;

      protected:

      /*!
       * Send message \c msg to the network address given by \c addr.
       *
       * @param [in] addr        HF network address.
       * @param [in] message     pointer to the message to be sent to the network.
       */
      virtual void sendMessage (Message::Address &addr, Message &message) = 0;
   };

   /*!
    * Common implementation of Interface functionality.
    */
   struct AbstractInterface:public Interface
   {
      bool handle (Message &message, ByteArray &payload, size_t offset)
      {
         UNUSED (payload);
         UNUSED (offset);

         // Only handle message that are for this interface and are
         // from the complementing role.
         if (uid () == message.itf.uid && role () != message.itf.role)
         {
            return true;
         }
         else
         {
            return false;
         }
      }

      //! \see Interface::periodic
      void periodic (uint32_t time)
      {
         UNUSED (time);
      }

      bool operator ==(AbstractInterface &other)
      {
         return uid () == other.uid ();
      }

      bool operator !=(AbstractInterface &other)
      {
         return !(*this == other);
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
