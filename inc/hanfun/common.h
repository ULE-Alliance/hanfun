// =============================================================================
/*!
 * \file       inc/hanfun/common.h
 *
 * This file contains the common defines for the HAN-FUN library.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2013 Bithium S.A.
 */
// =============================================================================

#ifndef HF_COMMON_H
#define HF_COMMON_H

#include <cstdlib>
#include <cstdint>
#include <cstddef>

#include <vector>

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
   /*!
    * This class represents a byte array.
    *
    * The method in this class are used to serialize the messages to be sent over the
    * network, converting between the host's endianness and the big-endian network format.
    */
   struct ByteArray:private vector <uint8_t>
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

      using vector::size;
      using vector::operator [];
      using vector::operator =;
      using vector::capacity;
      using vector::empty;
      using vector::clear;
      using vector::front;
      using vector::back;
      using vector::push_back;
      using vector::pop_back;
      using vector::begin;
      using vector::end;
      using vector::swap;
      using vector::data;
      using vector::reserve;

      bool operator ==(const ByteArray &other)
      {
         return (this->size () == other.size () &&
                 std::equal (this->begin (), this->end (), other.begin ()));
      }

      bool operator !=(const ByteArray &other)
      {
         return !(*this == other);
      }
   };

   /*!
    * This represents the common interface for message serialization.
    */
   class Serializable
   {
      public:

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

   // =============================================================================
   // API
   // =============================================================================

   /*!
    * Convert a \c Word from the host's format to the network format.
    *
    * @param value   word to convert.
    */
   void hf_hton (uint16_t &value);

   /*!
    * Convert a \c Double-Word from the host's format to the network format.
    *
    * @param value   double-word to convert.
    */
   void hf_hton (uint32_t &value);

   /*!
    * Convert a \c Word from the network format  to the host's format.
    *
    * @param value   word to convert.
    *
    * @return  the word converted.
    */
   void hf_ntoh (uint16_t &value);

   /*!
    * Convert a \c Double-Word from the network format to the host's format.
    *
    * @param value   double-word to convert.
    *
    * @return  the double-word converted.
    */
   void hf_ntoh (uint32_t &value);

}  // namespace HF

#endif /* HF_COMMON_H */
