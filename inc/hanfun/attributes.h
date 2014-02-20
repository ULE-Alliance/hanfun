// =============================================================================
/*!
 * \file       inc/hanfun/attributes.h
 *
 * This file contains the API definitions for the Attributes.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    x.x.x
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#ifndef HF_ATTRIBUTES_H
#define HF_ATTRIBUTES_H

#include <list>
#include <type_traits>
#include <algorithm>

#include "hanfun/common.h"
#include "hanfun/protocol.h"

namespace HF
{
   struct Interface;

   /*!
    * Interface/Service Attribute's API.
    */
   struct IAttribute:public Serializable
   {
      /*!
       * Attribute's UID.
       *
       * @return  the attribute's UID value.
       */
      virtual uint8_t uid () const = 0;

      /*!
       * Indicate if the attribute is writable.
       *
       * @retval  true if the attribute is writable,
       * @retval  false otherwise.
       */
      virtual bool isWritable () const = 0;

      /*!
       * Return the UID of the interface the attribute belongs to.
       *
       * @return  UID of the interface the attribute belongs to.
       */
      virtual uint16_t interface() const = 0;
      /*!
       * \see Serializable::size
       *
       * @param [in] with_uid    include uid() size in the calculation.
       */
      virtual size_t size (bool with_uid) const = 0;

      //! \see Serializable::size
      size_t size () const = 0;

      /*!
       * \see Serializable::pack
       *
       * @param [in] with_uid    include uid() size in the calculation.
       */
      virtual size_t pack (ByteArray &array, size_t offset, bool with_uid) const = 0;

      //! \see Serializable::pack
      virtual size_t pack (ByteArray &array, size_t offset) const = 0;

      /*!
       * \see Serializable::unpack
       *
       * \warning If \c with_uid == \c true, then if the value read from the
       *          array does not match the attribute's UID, no more data will be read.
       *
       * @param [in] with_uid    include uid() size in the calculation.
       */
      virtual size_t unpack (const ByteArray &array, size_t offset, bool with_uid) = 0;

      //! \see Serializable::unpack
      virtual size_t unpack (const ByteArray &array, size_t offset) = 0;
   };

   typedef vector <uint8_t> attribute_uids_t;

   class AbstractAttribute:public IAttribute
   {
      protected:

      const uint16_t _itf_uid;     //! Interface this attribute belongs to.
      const uint8_t  _uid;         //! Attribute unique identifier.
      const bool _writable;        //! Attribute access mode.

      AbstractAttribute(const uint16_t itf_uid, const uint8_t uid, const bool writable = false):
         _itf_uid (itf_uid), _uid (uid), _writable (writable)
      {}

      public:

      uint8_t uid () const
      {
         return _uid;
      }

      bool isWritable () const
      {
         return _writable;
      }

      uint16_t interface () const
      {
         return _itf_uid;
      }
   };

   template<typename T, typename = void>
   struct SerializableHelper:public Serializable
   {

      static_assert (is_same <bool, T>::value || is_same <uint8_t, T>::value ||
                     is_same <uint16_t, T>::value || is_same <uint32_t, T>::value ||
                     is_base_of <HF::Serializable, T>::value,
                     "T must be a uint8_t or uint16_t or uint32_t or Serializable.");

      size_t size () const
      {
         return 0;
      }

      size_t pack (ByteArray &array, size_t offset = 0) const
      {
         UNUSED (array);
         UNUSED (offset);
         return 0;
      }

      size_t unpack (const ByteArray &array, size_t offset = 0)
      {
         UNUSED (array);
         UNUSED (offset);
         return 0;
      }

      private:

      SerializableHelper()
      {}
   };

   template<typename T>
   struct SerializableHelper <T, typename enable_if <is_integral <typename remove_reference <T>::type>::value>::type> :
      public Serializable
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

      size_t pack (ByteArray &array, size_t offset = 0) const
      {
         size_t start = offset;

         offset += array.write (offset, data);

         return offset - start;
      }

      size_t unpack (const ByteArray &array, size_t offset = 0)
      {
         size_t start = offset;

         offset += array.read (offset, data);

         return offset - start;
      }
   };

   template<typename T>
   struct SerializableHelper <T, typename enable_if <is_base_of <Serializable, typename remove_reference <T>::type>::value>::type> :
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

   template<typename T, typename = void>
   struct Attribute:public AbstractAttribute
   {
      Attribute(const uint16_t interface, const uint8_t uid, T data, bool writable = false):
         AbstractAttribute (interface, uid, writable), helper (data)
      {}

      Attribute(const uint16_t interface, const uint8_t uid, bool writable = false):
         AbstractAttribute (interface, uid, writable)
      {}

      typedef typename remove_reference <T>::type value_type;

      // =============================================================================
      // API
      // =============================================================================

      void set (value_type value)
      {
         helper.data = value;
      }

      value_type get () const
      {
         return value ();
      }

      value_type value () const
      {
         return helper.data;
      }

      size_t size (bool with_uid) const
      {
         return size () + (with_uid ? sizeof(uint8_t) : 0);
      }

      size_t size () const
      {
         return helper.size ();
      }
      size_t pack (ByteArray &array, size_t offset, bool with_uid) const
      {
         size_t start = offset;

         if (with_uid)
         {
            offset += array.write (offset, uid ());
         }

         offset += helper.pack (array, offset);

         return offset - start;
      }

      size_t pack (ByteArray &array, size_t offset = 0) const
      {
         return helper.pack (array, offset);
      }
      size_t unpack (const ByteArray &array, size_t offset, bool with_uid)
      {
         size_t start = offset;

         if (with_uid)
         {
            uint8_t temp;
            offset += array.read (offset, temp);

            if (temp != uid ())
            {
               goto _end;
            }
         }

         offset += helper.unpack (array, offset);

         _end:
         return offset - start;
      }

      size_t unpack (const ByteArray &array, size_t offset = 0)
      {
         return helper.unpack (array, offset);
      }

      protected:

      SerializableHelper <T> helper;

      private:

      Attribute(const Attribute &other) = delete;
   };

   /*!
    * This class has the same behavior has a \c list, however
    * the \c list element access methods where overwritten to
    * allow using the attribute UIDs as indexes.
    *
    * The difference between the \c list indexes and the \c Attribute
    * indexes is that the former start at 0 and the latter at 1.
    */
   struct AttributeList:public list <IAttribute *>
   {
      IAttribute *operator [](uint8_t uid)
      {
         for (iterator it = begin (); it != end (); ++it)
         {
            if ((*it)->uid () == uid)
            {
               return *it;
            }
         }

         return nullptr;
      }
   };

}  // namespace HF

#endif /* HF_ATTRIBUTES_H */
