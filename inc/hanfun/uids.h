// =============================================================================
/*!
 * \file       inc/hanfun/uids.h
 *
 * This file contains the declaration of the classes that implement the HAN-FUN
 * UIDs.
 *
 * \version    0.4.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#ifndef HF_UIDS_H
#define HF_UIDS_H

#include "hanfun/common.h"

namespace HF
{
   // =============================================================================
   // UID implementation
   // =============================================================================

   /*!
    * This is the top-level namespace for HAN-FUN UIDs.
    */
   namespace UID
   {
      //! Types of UID available.
      enum Type
      {
         NONE_UID = 0x00,   //!< Empty UID.
         RFPI_UID = 0x01,   //!< Radio Fixed Part Identifier.
         IPUI_UID = 0x02,   //!< International Portable User Identifier.
         MAC_UID  = 0x03,   //!< Media Access Control (IEEE-MAC-48)
         URI_UID  = 0x04,   //!< Uniform Resource Identifier.
      };

      /*!
       * API for all UIDs.
       *
       * Parent UID class.
       */
      struct UID_T:public Common::Serializable, public Common::Cloneable <UID_T>
      {
         //! Type of the UID.
         virtual uint8_t type () const = 0;

         bool operator ==(const UID_T &other) const
         {
            return (this->compare (other) == 0);
         }

         bool operator !=(const UID_T &other) const
         {
            return !(*this == other);
         }

         /*!
          * Compare the current UID with the given UID.
          *
          * This function returns a value less that 0 if the current UID object
          * order is lower that the given UID, 0 if the UIDs represent the same
          * entity and a value greater that 0 if current UID object is above the given
          * UID.
          *
          * @param [in] other  a pointer to a UID object to compare to.
          *
          * @retval     < 0, the current UID is lower that the given UID.
          * @retval     0  , the current UID is the same as given UID.
          * @retval     > 0, the current UID is greater that the given UID.
          */
         virtual int compare (const UID_T &other) const
         {
            // return (other != nullptr ? this->type () - other.type () : this->type ());
            return (this->type () - other.type ());
         }

         //! \see HF::Common::Serializable::size.
         size_t size () const
         {
            return sizeof(uint8_t) + // UID Type.
                   sizeof(uint8_t);  // Size of the UID.
         }

         //! \see HF::Common::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;
            offset += array.write (offset, this->type ());
            return offset - start;
         }

         //! \see HF::Common::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            size_t  start = offset;

            uint8_t type;
            offset += array.read (offset, type);

            assert ((type & (~0x80)) == this->type ());

            return offset - start;
         }
      };

      template<uint8_t _type>
      struct Abstract:public UID_T
      {
         uint8_t type () const
         {
            return _type;
         }
      };

      /*!
       * This class represents an empty UID.
       */
      struct NONE:public Abstract <NONE_UID>
      {
         //! \see HF::Common::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += UID_T::pack (array, offset);
            offset += array.write (offset, (uint8_t) 0);

            return offset - start;
         }

         //! \see HF::Common::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += UID_T::unpack (array, offset);

            uint8_t size;
            offset += array.read (offset, size);

            assert (size == 0);

            return offset - start;
         }

         //! \see HF::Common::Clonable
         NONE *clone () const
         {
            return new NONE (*this);
         }
      };

      template<typename _Class, uint8_t _size, uint8_t _type>
      class ByteArray:public Abstract <_type>
      {
         protected:

         uint8_t value[_size];

         public:

         //! \see HF::Serializable::size.
         size_t size () const
         {
            return UID_T::size () + sizeof(value);
         }

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += UID_T::pack (array, offset);

            offset += array.write (offset, (uint8_t) sizeof(value));

            for (uint8_t i = 0; i < sizeof(value); i++)
            {
               offset += array.write (offset, value[i]);
            }

            return offset - start;
         }

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += UID_T::unpack (array, offset);

            uint8_t size;
            offset += array.read (offset, size);

            assert (size == sizeof(value));

            for (uint8_t i = 0; i < size; i++)
            {
               offset += array.read (offset, value[i]);
            }

            return offset - start;
         }

         // ===================================================================
         // API
         // ===================================================================

         int compare (const UID_T &other) const
         {
            int res = UID_T::compare (other);
            return (res == 0 ? memcmp (value, ((_Class *) &other)->value, sizeof(value)) : res);
         }

         // =============================================================================
         // Array style API.
         // =============================================================================

         uint8_t const &operator [](size_t index) const
         {
            return value[index];
         }

         uint8_t &operator [](size_t index)
         {
            return value[index];
         }

         uint8_t const &at (size_t index) const
         {
            assert (index < _size);
            return value[index];
         }

         uint8_t &at (size_t index)
         {
            assert (index < _size);
            return value[index];
         }

         // =============================================================================
         // Helper API
         // =============================================================================

         /*!
          * Fill the underlining byte array with the given value.
          *
          * @param byte    the value to be used to fill the byte array.
          */
         void fill (uint8_t byte)
         {
            memset (value, byte, sizeof(value));
         }

         /*!
          * Fill the underlining byte array with 0.
          */
         void clear ()
         {
            memset (value, 0, sizeof(value));
         }

         /*!
          * Return the number of bytes in the underlining byte array.
          *
          * @return  number of bytes in the underlining byte array.
          */
         static size_t length ()
         {
            return _size;
         }
      };

      /*!
       * RFPI UID class.
       */
      struct RFPI:public ByteArray <RFPI, 5, RFPI_UID>
      {
         // ===================================================================
         // Cloneable
         // ===================================================================

         RFPI *clone () const
         {
            return new RFPI (*this);
         }
      };

      /*!
       * IPUI UID class.
       */
      struct IPUI:public ByteArray <IPUI, 5, IPUI_UID>
      {
         // ===================================================================
         // Cloneable
         // ===================================================================

         IPUI *clone () const
         {
            return new IPUI (*this);
         }
      };

      /*!
       * IEEE MAC-48b UID class.
       */
      struct MAC:public ByteArray <MAC, 6, MAC_UID>
      {
         // ===================================================================
         // Cloneable
         // ===================================================================

         MAC *clone () const
         {
            return new MAC (*this);
         }
      };

      /*!
       * URI UID class.
       */
      struct URI:public Abstract <URI_UID>
      {
         std::string value;

         URI()
         {}

         URI(const std::string &value):
            value (value)
         {}

         //! \see HF::Serializable::size.
         size_t size () const
         {
            return UID_T::size () + value.size ();
         }

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;
            size_t size  = value.size ();

            offset += UID_T::pack (array, offset);

            offset += array.write (offset, (uint8_t) size);

            for (uint8_t i = 0; i < (size & 0xFF); i++)
            {
               offset += array.write (offset, (uint8_t) value[i]);
            }

            return offset - start;
         }

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            uint8_t size;
            size_t  start = offset;

            offset += UID_T::unpack (array, offset);

            offset += array.read (offset, size);

            value   = std::string (size, 0);

            for (uint8_t i = 0; i < size; i++)
            {
               uint8_t c;
               offset  += array.read (offset, c);
               value[i] = c;
            }

            return offset - start;
         }

         // ===================================================================
         // Cloneable
         // ===================================================================

         URI *clone () const
         {
            return new URI (*this);
         }

         // ===================================================================
         // API
         // ===================================================================

         int compare (const UID_T &other) const
         {
            int res = UID_T::compare (other);
            return (res == 0 ? value.compare (((URI *) &other)->value) : res);
         }

         std::string str () const
         {
            return value;
         }
      };

      // size_t pack (UID_T &uid, Common::ByteArray &array, size_t offset);

      // size_t unpack (UID_T * &uid, const Common::ByteArray &array, size_t offset);

      /*!
       * Wrapper around UID_T pointer's.
       *
       * This class can either wrap an existing pointer (i.e. a pointer to an object it doesn't own)
       * or wrap a pointer to an object that was created based on an existing object. In this
       * case the clone object is owned by this class instance.
       *
       * The destructor of this class will \c delete the object it owns.
       */
      class UID:public HF::Common::Serializable
      {
         UID_T *_raw;

         bool  owner;

         public:

         UID():_raw (new NONE ()), owner (true)
         {}

         UID(UID_T *_uid, bool _owner = false):
            _raw (_uid), owner (_owner)
         {
            assert (_uid != nullptr);
         }

         UID(UID &&other):_raw (other._raw), owner (other.owner)
         {
            other.owner = false;
         }

         UID(const UID &other):_raw (other._raw->clone ()), owner (true)
         {}

         virtual ~UID()
         {
            if (owner)
            {
               delete _raw;
            }
         }

         // =============================================================================
         // API
         // =============================================================================

         //! \see UID_T::type
         uint8_t type () const
         {
            return _raw->type ();
         }

         bool operator ==(const UID &other) const
         {
            return (this->_raw->compare (*(other._raw)) == 0);
         }

         bool operator !=(const UID &other) const
         {
            return !(*this == other);
         }

         size_t size () const
         {
            return _raw->size ();
         }

         size_t pack (Common::ByteArray &array, size_t offset) const
         {
            return _raw->pack (array, offset);
         }

         size_t unpack (const Common::ByteArray &array, size_t offset);

         // =============================================================================
         // Utils
         // =============================================================================

         // Copy
         UID &operator =(const UID &other)
         {
            if (this == &other)
            {
               return *this;
            }

            if (this->owner)
            {
               delete this->_raw;
            }

            this->_raw  = other._raw->clone ();
            this->owner = true;

            return *this;
         }

         // Move
         UID &operator =(UID &&other)
         {
            UID_T *temp;
            temp       = this->_raw;
            this->_raw = other._raw;
            other._raw = temp;

            bool btemp = this->owner;
            this->owner = other.owner;
            other.owner = btemp;

            return *this;
         }

         /*!
          * Use the given pointer \c as the pointer of the underlining UID to wrap around
          * of.
          *
          * In this case the class takes ownership of the pointer and will delete the
          * associated object when it is destructed.
          *
          * @param _uid    pointer the UID_T object instance to manage.
          *
          * @return  reference to self.
          */
         UID &operator =(UID_T *_uid)
         {
            if (owner)
            {
               delete this->_raw;
            }

            _raw  = _uid;
            owner = true;

            return *this;
         }

         int compare (const UID_T &other) const
         {
            return _raw->compare (other);
         }

         UID_T const *raw () const
         {
            return _raw;
         }
      };

   }  // namespace UID

}  // namespace HF

// =============================================================================
// Helper Functions
// =============================================================================

inline bool operator ==(const HF::UID::UID &lhs, const HF::UID::UID_T &rhs)
{
   return (lhs.compare (rhs) == 0);
}

inline bool operator ==(const HF::UID::UID_T &lhs, const HF::UID::UID &rhs)
{
   return (rhs.compare (lhs) == 0);
}

inline bool operator !=(const HF::UID::UID &lhs, const HF::UID::UID_T &rhs)
{
   return !(lhs == rhs);
}

inline bool operator !=(const HF::UID::UID_T &lhs, const HF::UID::UID &rhs)
{
   return !(lhs == rhs);
}

#endif /* HF_UIDS_H */
