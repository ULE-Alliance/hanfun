// =============================================================================
/*!
 * \file       inc/hanfun/common.h
 *
 * This file contains the common defines for the HAN-FUN library.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.2.0
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#ifndef HF_COMMON_H
#define HF_COMMON_H

#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstring>

#include <string>
#include <vector>

#include "hanfun/version.h"
#include "hanfun/config.h"

#include "hanfun/gcc.h"

using namespace std;

// =============================================================================
// Defines
// =============================================================================

#define UNUSED(x)   (void) x

// =============================================================================
// API
// =============================================================================

/*!
 * Top-level namespace for the HAN-FUN library.
 */
namespace HF
{
   constexpr uint8_t CORE_VERSION       = 0;
   constexpr uint8_t PROFILES_VERSION   = 0;
   constexpr uint8_t INTERFACES_VERSION = 0;

   /*!
    * This namespace contains helper classes to be used though out the HAN-FUN
    * implementation.
    */
   namespace Common
   {
      // =============================================================================
      // Helper Classes.
      // =============================================================================

      /*!
       * These constants represent precisions that a measurement can be in.
       */
      typedef enum
      {
         BASE  = 0x00, //!< BASE
         MILI  = 0x10, //!< MILI
         MICRO = 0x11, //!< MICRO
         NANO  = 0x12, //!< NANO
         PICO  = 0x13, //!< PICO
         KILO  = 0x20, //!< KILO
         MEGA  = 0x21, //!< MEGA
         GIGA  = 0x22, //!< GIGA
         TERA  = 0x23, //!< TERA
      } Precision;

      /*!
       * This represents the type of time that is associated with a
       * time measurement.
       */
      typedef enum
      {
         UPTIME = 0x00, //!< Uptime.
         UTC    = 0x01, //!< UTC time.
      } Time;

      /*!
       * This class represents a byte array.
       *
       * The method in this class are used to serialize the messages to be sent over the
       * network, converting between the host's endianness and the big-endian network format.
       */
      struct ByteArray:public vector <uint8_t>
      {
         /*!
          * Create a byte array with the given initial size.
          *
          * @param size the initial size of the byte array.
          */
         ByteArray(size_t size = 0);

         /*!
          * Create a byte array with the given initial data.
          *
          * @param data    data to initialize the byte array with.
          * @param size    size in bytes of the data.
          */
         ByteArray(const uint8_t data[], const size_t size);

         /*!
          * Create byte array from the values in the given list.
          *
          * @param raw  values to add to the byte array.
          */
         ByteArray(initializer_list <uint8_t> raw):vector (raw)
         {}

         //! Destructor
         virtual ~ByteArray() {}

         /*!
          * Write a byte into the array at the given \c offset.
          *
          * @param [in] offset  offset to write the byte to.
          * @param [in] data    byte value to write to the array.
          *
          * @return  number of bytes written (1).
          */
         size_t write (size_t offset, uint8_t data);

         /*!
          * Write a word in the big endian format into the
          * array at the given \c offset.
          *
          * \warning If the host is NOT big-endian the value will
          *          be converted before being written.
          *
          * @param [in] offset  offset to write the word to.
          * @param [in] data    word value to write to the array.
          *
          * @return  number of bytes written (2).
          */
         size_t write (size_t offset, uint16_t data);

         /*!
          * Write a double-word in big endian format into the
          * array at the given \c offset.
          *
          * \warning If the host is NOT big-endian the value will
          *          be converted before being written.
          *
          * @param [in] offset  offset to write the double-word to.
          * @param [in] data    double-word value to write to the array.
          *
          * @return  number of bytes written (4).
          */
         size_t write (size_t offset, uint32_t data);

         //! \see  ByteArray::write (size_t, uint8_t)
         size_t write (size_t offset, bool data)
         {
            return write (offset, static_cast <uint8_t>(data));
         }

         /*!
          * Read the byte at \c offset into \c data.
          *
          * @param [in]  offset  offset to read the byte from.
          * @param [out] data    reference to save the read value to.
          *
          * @return  number of bytes read (1).
          */
         size_t read (size_t offset, uint8_t &data) const;

         /*!
          * Read the word in big-endian format at \c offset into \c data.
          *
          * \warning If the host is NOT big-endian the value will
          *          be converted to the host's Endianness.
          *
          * @param [in]  offset  offset to read the word from.
          * @param [out] data    reference to save the read value to.
          *
          * @return  number of bytes read (2).
          */
         size_t read (size_t offset, uint16_t &data) const;

         /*!
          * Read the double-word in big-endian format at \c offset into \c data.
          *
          * \warning If the host is NOT big-endian the value will
          *          be converted to the host's Endianness.
          *
          * @param [in]  offset  offset to read the double-word from.
          * @param [out] data    reference to save the read value to.
          *
          * @return  number of bytes read (4).
          */
         size_t read (size_t offset, uint32_t &data) const;

         //! \see  ByteArray::read (size_t, uint8_t)
         size_t read (size_t offset, bool &data) const
         {
            uint8_t temp;
            size_t  result = read (offset, temp);

            data = (temp & 0x01) != 0;

            return result;
         }

         /*!
          * Check if the array as at least \c expected bytes
          * available from the given \c offset.
          *
          * @param offset     the offset from where to start counting.
          * @param expected   the number of byte required.
          *
          * @retval  true if enough data is available,
          * @retval  false otherwise.
          */
         bool available (size_t offset, size_t expected) const
         {
            return expected <= available (offset);
         }

         /*!
          * Return the number of data bytes available from the given \c offset.
          *
          * @param offset     the offset from where to start counting.
          *
          * @return  number of data bytes available from the given \c offset.
          */
         size_t available (size_t offset) const
         {
            return (size () >= offset ? size () - offset : 0);
         }

         bool operator ==(const ByteArray &other)
         {
            return (this->size () == other.size () &&
                    std::equal (this->begin (), this->end (), other.begin ()));
         }

         bool operator !=(const ByteArray &other)
         {
            return !(*this == other);
         }

         /*!
          * Extend the byte array by the given size.
          *
          * This is the same as calling : array.reserve(array.size() + _size)
          *
          * @param [in] _size number of bytes to extent the array by.
          */
         void extend (size_t _size)
         {
            vector <uint8_t>::reserve (size () + _size);
         }
      };

      // =============================================================================
      // Common Interfaces
      // =============================================================================

      /*!
       * This represents the common interface for message serialization.
       */
      class Serializable
      {
         public:

         //! Destructor
         virtual ~Serializable() {}

         /*!
          * Number bytes needed to serialize the message.
          *
          * @return  number of bytes the message requires to be serialized.
          */
         virtual size_t size () const = 0;

         /*!
          * Write the object on to a ByteArray so it can be sent over the network.
          *
          * @param [inout] array   ByteArray reference to write the object to.
          * @param [in]    offset  offset to start writing to.
          *
          * @return  the number of bytes written.
          */
         virtual size_t pack (ByteArray &array, size_t offset = 0) const = 0;

         /*!
          * Read a message from a ByteArray.
          *
          * @param [inout] array   ByteArray reference to read the message from.
          * @param [in]    offset  offset to start reading from.
          *
          * @return  the number of bytes read.
          */
         virtual size_t unpack (const ByteArray &array, size_t offset = 0) = 0;
      };

      /*!
       * Wrapper for classes that implement the Serializable concept.
       */
      template<typename T, typename = void>
      struct SerializableHelper:public Serializable
      {
         T data;

         SerializableHelper()
         {
            memset (&data, 0, sizeof(T));
         }

         SerializableHelper(T data):data (data)
         {}
         size_t size () const
         {
            return data.size ();
         }

         size_t pack (ByteArray &array, size_t offset = 0) const
         {
            return data.pack (array, offset);
         }

         size_t unpack (const ByteArray &array, size_t offset = 0)
         {
            return data.unpack (array, offset);
         }
      };

      /*!
       * Wrapper to pointers for classes that implement the Serializable concept.
       */
      template<typename T>
      struct SerializableHelper <T, typename enable_if <is_pointer <T>::value>::type> :
         public Serializable
      {
         T data;

         SerializableHelper()
         {
            memset (&data, 0, sizeof(T));
         }

         SerializableHelper(T data):data (data)
         {}

         size_t size () const
         {
            return data->size ();
         }

         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            return data->pack (array, offset);
         }

         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            return data->unpack (array, offset);
         }
      };

      /*!
       * Wrapper for base integer types implementing the  Serializable API.
       */
      template<typename T>
      struct SerializableHelper <T, typename enable_if <is_integral <typename remove_reference <T>::type>::value>::type> :
         public Common::Serializable
      {
         T data;

         SerializableHelper()
         {
            memset (&data, 0, sizeof(T));
         }

         SerializableHelper(T data):data (data) {}

         size_t size () const
         {
            return sizeof(T);
         }

         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += array.write (offset, data);

            return offset - start;
         }

         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += array.read (offset, data);

            return offset - start;
         }
      };

      /*!
       * This class represents the interface that cloneable objects need
       * to implement.
       */
      struct Cloneable
      {
         /*!
          * Create a clone object of the object where this method is being called.
          *
          * @return  a new object that is a clone of this object.
          */
         virtual Cloneable *clone () const = 0;
      };

      /*!
       * Commands result codes.
       */
      typedef enum
      {
         OK                = 0x00, //!< Request OK
         FAIL_AUTH         = 0x01, //!< Fail - Not Authorized
         FAIL_ARG          = 0x02, //!< Fail - Invalid Argument
         FAIL_SUPPORT      = 0x03, //!< Fail - Not Supported
         FAIL_RO_ATTR      = 0x04, //!< Fail - Read only attribute.
         FAIL_READ_SESSION = 0x20, //!< Fail - Read Session not established
         FAIL_MODIFIED     = 0x21, //!< Fail - Entries table modified
         FAIL_ID           = 0x23, //!< Fail - ID Not Found
         FAIL_RESOURCES    = 0xFE, //!< Fail - Not enough resources
         FAIL_UNKNOWN      = 0xFF, //!< Fail - Unknown reason
      } Result;

      /*!
       * Interface UID.
       */
      struct Interface
      {
         uint16_t role : 1;         //!< Interface role : Server or Client.
         uint16_t id   : 15;        //!< Identifier of the interface. \see Interface::UID.

         Interface(uint16_t id = 0, uint16_t role = 0):
            role (role), id (id) {}

         //! \see HF::Serializable::size.
         size_t size () const;

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const;

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0);
      };

   }  // namespace Common

}  // namespace HF

// =============================================================================
// Helper Functions
// =============================================================================

inline bool operator ==(const HF::Common::Interface &lhs,
                        const HF::Common::Interface &rhs)
{
   return (lhs.role == rhs.role) && (lhs.id == rhs.id);
}

inline bool operator !=(const HF::Common::Interface &lhs,
                        const HF::Common::Interface &rhs)
{
   return !(lhs == rhs);
}

#endif /* HF_COMMON_H */
