// =============================================================================
/*!
 * \file       inc/hanfun/transport.h
 *
 * This file contains the definition of the common transport layer API for
 * the HAN-FUN protocol.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#ifndef HF_TRANSPORT_H
#define HF_TRANSPORT_H

#include "hanfun/common.h"

namespace HF
{
   /*!
    * This class defines common API for all transport layers.
    */
   struct Transport
   {
      /*!
       * This interface represents a bidirectional link on the transport layer.
       */
      struct Link
      {
         /*!
          * Write the data present in the given ByteArray to the network.
          *
          * @param [in] array   a pointer to the ByteArray that contains the data to be sent.
          * @param [in] offset  index to start reading the data to be sent.
          * @param [in] size    number of bytes to send to the network.
          *
          * @return             number of bytes that were read from the ByteArray to be sent.
          */
         virtual size_t write (ByteArray * array, size_t offset, size_t size) = 0;

         /*!
          * Helper method : \see Link::write(ByteArray *, size_t, size_t)
          *
          * This is the same as calling :
          *
          * \code{.cpp}
          *
          * Link        link;    // The link to write to.
          * ByteArray   array;   // The ByteArray containing the data to be sent.
          *
          * size_t offset;       // Start offset to get the data to be sent.
          *
          * link.write( &array, offset ); // is the same as link.write( &array, offset, array.size() - offset );
          *
          * \endcode
          */
         virtual size_t write (ByteArray * array, size_t offset = 0)
         {
            return this->write(array, offset, array->size() - offset);
         }

         /*!
          * Read the available data from the network into the given ByteArray.
          *
          * @param [in] array   pointer to the ByteArray to place the incoming data.
          * @param [in] offset  index to start placing the data into the array.
          * @param [in] size    number of bytes to read from the link.
          *
          * @return             number of bytes placed in the ByteArray.
          */
         virtual size_t read (ByteArray * array, size_t offset, size_t size) = 0;

         /*!
          * Helper method : \see Link::read(ByteArray *, size_t, size_t)
          *
          * This is the same as calling :
          *
          * \code{.cpp}
          *
          * Link        link;    // The link to read from.
          * ByteArray   array;   // The ByteArray that will hold the data read.
          *
          * size_t offset;       // Start offset where the data should be read to.
          *
          * link.read( &array, offset ); // is the same as link.read( &array, offset, link.availabe() );
          *
          * \endcode
          */
         virtual size_t read (ByteArray * array, size_t offset = 0)
         {
            return this->read(array, offset, this->available());
         }

         /*!
          * Retrieve the number of byte available to be read from the network.
          *
          * @return  the number of bytes available to be read.
          */
         virtual size_t available () = 0;

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
         virtual UID uid () = 0;

         /*!
          * Return the transport layer implementation this link belongs to.
          *
          * @return  pointer to the transport layer that created this link.
          */
         virtual HF::Transport * transport () = 0;
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
          * @param link    pointer to the created link.
          */
         virtual void connected (Link * link) = 0;

         /*!
          * Callback to the report that the given transport \c link no longer is
          * valid and MUST not be used to send messages to the remote end-point.
          *
          * At the time of this method call the link MUST not be used to send any
          * further messages to the remote end-point.
          *
          * @param link    pointer to the link disconnected.
          */
         virtual void disconnected (Link * link) = 0;

         /*!
          * Callback to report that the given transport \c link has data available
          * to be read.
          *
          * @param link    pointer to the link were data is available.
          */
         virtual void receive (Link * link) = 0;

         //! @}
         // ======================================================================
      };

      // ======================================================================
      // Commands
      // ======================================================================
      //! \name Commands
      //! @{

      /*!
       * Initialize the associated transport layer and register the given
       * \c Endpoint to receive events.
       *
       * This should be possible to register multiple end-points, to the transport
       * layer.
       *
       * @param [in] ep    end-point to register in the transport layer.
       */
      virtual void initialize (Endpoint * ep) = 0;

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
      virtual void destroy (Endpoint * ep = nullptr) = 0;

      /*!
       * Return the UID of the local device on this transport layer.
       *
       * For example on an ULE device this would return the IPUI of the device.
       *
       * @return  the UID of the local device on this transport layer.
       */
      virtual UID uid () const = 0;

      //! @}
      // ======================================================================
   };

}  // namespace HF

#endif /* HF_TRANSPORT_H */
