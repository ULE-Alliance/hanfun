// =============================================================================
/*!
 * \file       inc/hanfun/transport.h
 *
 * This file contains the definition of the common transport layer API for
 * the HAN-FUN protocol.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#ifndef HF_TRANSPORT_H
#define HF_TRANSPORT_H

#include "hanfun/common.h"

#include "hanfun/uids.h"

#include "hanfun/device.h"

namespace HF
{
   /*!
    * This namespace contains the API for the transport layer.
    */
   namespace Transport
   {
      // Forward declaration.
      struct Layer;
      /*!
       * This interface represents a bidirectional link on the transport layer.
       */
      struct Link
      {
         /*!
          * Get the address of the device this link corresponds to.
          *
          * @return  the address of the device for this link.
          */
         virtual uint16_t address () const = 0;

         /*!
          * Set the address of this link corresponds to.
          *
          * @param [in] addr  the address value to set.
          */
         virtual void address (uint16_t addr) = 0;

         /*!
          * Send the data in the given \c ByteArray using the link to the remote end-point.
          *
          * @param [in] array   reference to the ByteArray containing the data to send.
          */
         virtual void send (Common::ByteArray &array) = 0;

         /*!
          * Return the end-point UID associated with this link.
          *
          * For example, for a ULE transport layer this would return the
          * RFPI/IPUI of the remote device.
          *
          * \warning This MUST not return \c nullptr.
          *
          * @return  the UID of the remote device of this link.
          */
         virtual HF::UID::UID const *uid () const = 0;

         /*!
          * Return the transport layer implementation this link belongs to.
          *
          * @return  pointer to the transport layer that created this link.
          */
         virtual Layer const *transport () const = 0;
      };

      /*!
       * This interface represents the API that a device using a HAN-FUN
       * transport layer MUST implement.
       */
      struct Endpoint
      {
         // ======================================================================
         // Events
         // ======================================================================
         //! \name Events
         //! @{

         /*!
          * Callback to report that a new transport \c link was been created to a
          * remote device.
          *
          * This link can then be used by the local end-point to send messages to
          * the remote end-point.
          *
          * @param [in] link    pointer to the created link.
          */
         virtual void connected (Link *link) = 0;

         /*!
          * Callback to the report that the given transport \c link no longer is
          * valid and MUST not be used to send messages to the remote end-point.
          *
          * At the time of this method call the link MUST not be used to send any
          * further messages to the remote end-point.
          *
          * @param [in] link    pointer to the link disconnected.
          */
         virtual void disconnected (Link *link) = 0;

         /*!
          * Callback to deliver a packet received from the transport layer.
          *
          * @param [in] packet   reference to the received packet.
          * @param [in] payload  reference a ByteArray containing the received data.
          * @param [in] offset   offset from where the received data starts on the \c payload
          *                      byte array buffer.
          */
         virtual void receive (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset) = 0;

         //! @}
         // ======================================================================
      };


      /*!
       * This class defines common API for all transport layers.
       */
      struct Layer
      {
         // ======================================================================
         // Commands
         // ======================================================================
         //! \name Commands
         //! @{

         /*!
          * Initialize the associated transport layer.
          *
          * This method should initialize the transport layer using it's default values.
          */
         virtual void initialize () = 0;

         /*!
          * Register the given \c Endpoint to receive events.
          *
          * This should be possible to register multiple end-points, to the transport
          * layer.
          *
          * @param [in] ep    pointer to the end-point to register in the transport layer.
          */
         virtual void add (Endpoint *ep) = 0;

         /*!
          * Free the system resources associated with the given \c Endpoint,
          * and remove it from receiving any further events.
          *
          * If \c ep is\c  nullptr then all end-points should be removed.
          *
          * When all end-points are removed all remaining system resources,
          * associated with this transport layer should be freed.
          *
          * @param ep   pointer to the endpoint to remove from the transport layer.
          */
         virtual void remove (Endpoint *ep = nullptr) = 0;

         /*!
          * Free the system resources associated with this transport layer.
          */
         virtual void destroy () = 0;

         /*!
          * Return the UID of the local device on this transport layer.
          *
          * For example on an ULE device this would return the IPUI of the device.
          *
          * @return  the UID of the local device on this transport layer.
          */
         virtual const HF::UID::UID *uid () const = 0;

         //! @}
         // ======================================================================
      };

      class AbstractLink:public Link
      {
         protected:

         uint16_t _address;

         public:

         AbstractLink(uint16_t _address = HF::Protocol::BROADCAST_ADDR):
            _address (_address)
         {}

         uint16_t address () const
         {
            return _address;
         }

         void address (uint16_t addr)
         {
            _address = addr;
         }
      };

   }  // namespace Transport

}  // namespace HF

#endif /* HF_TRANSPORT_H */
