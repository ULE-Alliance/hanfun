// =============================================================================
/*!
 * \file       inc/hanfun/uids.h
 *
 * This file contains the declaration of the classes that implement the HAN-FUN
 * UIDs.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
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
         NONE_UID = 0x00,    //!< Empty UID.
         RFPI_UID = 0x01,    //!< Radio Fixed Part Identifier.
         IPUI_UID = 0x02,    //!< International Portable User Identifier.
         MAC_UID  = 0x03,    //!< Media Access Control (IEEE-MAC-48)
         URI_UID  = 0x04,    //!< Uniform Resource Identifier.
      };

      /*!
       * API for all UIDs.
       *
       * Parent UID class.
       */
      struct UID:public Common::Serializable, public Common::Cloneable
      {
         //! Type of the UID.
         virtual uint8_t type () const = 0;

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
         virtual int compare (const UID *other) const
         {
            return (other != nullptr ? this->type () - other->type () : this->type ());
         }

         bool operator ==(const UID &other) const
         {
            return (this->compare (&other) == 0);
         }

         bool operator !=(const UID &other) const
         {
            return !(*this == other);
         }

         virtual UID *clone () const = 0;

         //! \see HF::Serializable::size.
         size_t size () const
         {
            return 1;
         }
      };

      /*!
       * This class represents an empty UID.
       */
      struct NONE:public UID
      {
         //! Type of the UID.
         uint8_t type () const
         {
            return NONE_UID;
         }

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;
            offset += array.write (offset, (uint8_t) 0);
            return offset - start;
         }

         //! \see HF::Serializable::unpack.
         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            uint8_t size;
            size_t  start = offset;
            offset += array.read (offset, size);
            return offset - start;
         }

         NONE *clone () const
         {
            return new NONE (*this);
         }
      };

      template<uint8_t _type>
      struct AbstractUID:public UID
      {
         uint8_t type () const
         {
            return _type;
         }
      };

      template<typename _Class, uint8_t _size, uint8_t _type>
      struct ByteArrayUID:public AbstractUID <_type>
      {
         uint8_t value[_size];

         //! \see HF::Serializable::size.
         size_t size () const
         {
            return UID::size () + sizeof(value);
         }

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

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
            uint8_t size;
            size_t  start = offset;

            offset += array.read (offset, size);

            for (uint8_t i = 0; size == sizeof(value) && i < size; i++)
            {
               offset += array.read (offset, value[i]);
            }

            return offset - start;
         }

         // ===================================================================
         // API
         // ===================================================================

         int compare (const UID *other) const
         {
            int res = UID::compare (other);
            return (res == 0 ? memcmp (value, ((_Class *) other)->value, sizeof(value)) : res);
         }
      };

      /*!
       * RFPI UID class.
       */
      struct RFPI:public ByteArrayUID <RFPI, 5, RFPI_UID>
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
      struct IPUI:public ByteArrayUID <IPUI, 5, IPUI_UID>
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
      struct MAC:public ByteArrayUID <MAC, 6, MAC_UID>
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
      struct URI:public AbstractUID <URI_UID>
      {
         string value;

         URI(string value = ""):
            value (value)
         {}

         //! \see HF::Serializable::size.
         size_t size () const
         {
            return UID::size () + value.size ();
         }

         //! \see HF::Serializable::pack.
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;
            size_t size  = value.size ();

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

            offset += array.read (offset, size);

            value   = string (size, 0);

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

         int compare (const UID *other) const
         {
            int res = UID::compare (other);
            return (res == 0 ? value.compare (((URI *) other)->value) : res);
         }
      };

      size_t pack(UID &uid, Common::ByteArray &array, size_t offset);

      size_t unpack(UID *&uid, Common::ByteArray &array, size_t offset);

   }  // namespace UID

}  // namespace HF

/*
 * This provides a template specialization to use in ordered collections
 * containing pointer to UID classes.
 */
namespace std
{
   template<>
   struct less <HF::UID::UID *> :public binary_function <HF::UID::UID *, HF::UID::UID *, bool>
   {
      bool operator ()(HF::UID::UID *lhs, HF::UID::UID *rhs) const
      {
         if (lhs == nullptr)
         {
            return true;
         }
         else if (rhs == nullptr)
         {
            return false;
         }
         else
         {
            return lhs->compare (rhs) < 0;
         }
      }
   };

   template<>
   struct less <HF::UID::UID const *> :public binary_function <HF::UID::UID const *, HF::UID::UID const *, bool>
   {
      bool operator ()(HF::UID::UID const *lhs, HF::UID::UID const *rhs) const
      {
         if (lhs == nullptr)
         {
            return true;
         }
         else if (rhs == nullptr)
         {
            return false;
         }
         else
         {
            return lhs->compare (rhs) < 0;
         }
      }
   };
}

#endif /* HF_UIDS_H */
