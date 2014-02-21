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
      virtual uint16_t interface () const       = 0;
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

      //! Attribute factory function type.
      typedef IAttribute * (*Factory)(uint8_t);
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

   /*!
    * This class represents the response sent when a
    * Protocol::Message::GET_ATTR_REQ request.
    */
   struct AttributeResponse:public Protocol::Response
   {
      IAttribute *attribute;

      AttributeResponse(IAttribute *attribute = nullptr):
         attribute (attribute) {}

      virtual ~AttributeResponse()
      {
         delete attribute;
      }

      //! \see HF::Serializable::size.
      size_t size () const
      {
         return Protocol::Response::size () + (attribute != nullptr ? attribute->size () : 0);
      }

      //! \see HF::Serializable::pack.
      size_t pack (ByteArray &array, size_t offset = 0) const
      {
         return Protocol::Response::pack (array, offset) +
                (attribute != nullptr ? attribute->pack (array, offset) : 0);
      }

      //! \see HF::Serializable::unpack.
      size_t unpack (const ByteArray &array, size_t offset = 0)
      {
         return Protocol::Response::unpack (array, offset) +
                (attribute != nullptr ? attribute->unpack (array, offset) : 0);
      }
   };

   /*!
    * This namespace contains the classes that implement the HF::Message::GET_ATTR_PACK_REQ
    * messages.
    */
   namespace GetAttributePack
   {
      /*!
       * Attribute Pack Special IDs.
       *
       * These IDs when received on a HF::Message::GET_ATTR_PACK_REQ, allow the return of all
       * mandatory/optional attributes, without the need to indicate all the attribute UIDs.
       */
      typedef enum
      {
         MANDATORY = 0x00, //!< Return all mandatory attributes for the interface.
         ALL       = 0xFE, //!< Return all mandatory and optional attributes for the interface.
         DYNAMIC   = 0xFF, //!< Return the attributes with the given attributes.
      } Type;

      /*!
       * This class represents the payload of a HF::Message::GET_ATTR_PACK_REQ request,
       * when the payload is Type::DYNAMIC.
       */
      struct Request:public Serializable
      {
         attribute_uids_t attributes;   //!< Vector containing the attributes UID's to get.

         /*!
          * Unpack attribute count.
          *
          * \warning This value will not be used as the number of attributes, when
          *          packing the request.
          */
         uint8_t count;

         Request():count (0) {}

         Request(attribute_uids_t &attributes):
            attributes (attributes), count (0)
         {}

         //! \see HF::Serializable::size.
         size_t size () const
         {
            return sizeof(uint8_t) + sizeof(uint8_t) * attributes.size ();
         }

         //! \see HF::Serializable::pack.
         size_t pack (ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += array.write (offset, (uint8_t) attributes.size ());

            /* *INDENT-OFF* */
            for_each (attributes.begin(), attributes.end(), [&array, &offset] (uint8_t uid)
            {
               offset += array.write( offset, uid );
            });
            /* *INDENT-ON* */

            return offset - start;
         }

         //! \see HF::Serializable::unpack.
         size_t unpack (const ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += array.read (offset, count);

            if (array.available (offset, count * sizeof(uint8_t)))
            {
               for (uint8_t i = 0; i < count; i++)
               {
                  uint8_t uid;
                  offset += array.read (offset, uid);
                  attributes.push_back (uid);
               }
            }

            return offset - start;
         }
      };

      /*!
       * This class represents the payload of a HF::Message::GET_ATTR_PACK_RES message.
       *
       * The payload contains the attribute values that were requested.
       */
      struct Response:public Protocol::Response
      {
         //! Pointer to the function to request the attribute instances to unpack the response.
         IAttribute::Factory attribute_factory;

         //!< List containing the attributes to send.
         AttributeList attributes;

         /*!
          * Unpack attribute count.
          *
          * \warning This value will not be used as the number of attributes, when
          *          packing the request.
          */
         uint8_t count;

         Response():attribute_factory (nullptr)
         {}

         Response(AttributeList &attributes):
            attribute_factory (nullptr), attributes (attributes)
         {}

         Response(IAttribute::Factory factory):
            attribute_factory (factory)
         {}

         virtual ~Response()
         {
            /* *INDENT-OFF* */
            for_each (attributes.begin (), attributes.end (), [](IAttribute *attr)
            {
               delete attr;
            });
            /* *INDENT-ON* */
         }

         //! \see HF::Serializable::size.
         size_t size () const
         {
            size_t result = Protocol::Response::size ();

            /* *INDENT-OFF* */
            for_each (attributes.begin(), attributes.end(), [&result](IAttribute *attr)
            {
               result += attr->size(true);
            });
            /* *INDENT-ON* */

            return result;
         }

         //! \see HF::Serializable::pack.
         size_t pack (ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += Protocol::Response::pack (array, offset);

            offset += array.write (offset, (uint8_t) attributes.size ());

            /* *INDENT-OFF* */
            for_each (attributes.begin(), attributes.end(), [&array, &offset] (IAttribute * attr)
            {
               offset += attr->pack(array, offset, true);
            });
            /* *INDENT-ON* */

            return offset - start;
         }

         //! \see HF::Serializable::unpack.
         size_t unpack (const ByteArray &array, size_t offset = 0);
      };

   }  // namespace GetAttributePack

   /*!
    * This namespace contains the classes that implement the HF::Message::SET_ATTR_PACK_REQ
    * and HF::Message::SET_ATTR_PACK_RESP_REQ messages.
    */
   namespace SetAttributePack
   {
      /*!
       * This class represents the message payload of a HF::Message::SET_ATTR_PACK_REQ
       * or HF::Message::SET_ATTR_PACK_RESP_REQ message.
       */
      struct Request:public Serializable
      {
         AttributeList attributes;   //!< List containing the attributes to send.

         /*!
          * Unpack attribute count.
          *
          * \warning This value will not be used as the number of attributes, when
          *          packing the request.
          */
         uint8_t count;

         virtual ~Request()
         {
            /* *INDENT-OFF* */
            for_each (attributes.begin (), attributes.end (), [](IAttribute *attr)
            {
               delete attr;
            });
            /* *INDENT-ON* */
         }

         //! \see Serializable::size
         size_t size () const
         {
            size_t result = sizeof(uint8_t);

            /* *INDENT-OFF* */
            for_each ( attributes.begin(), attributes.end(), [&result](IAttribute * attr)
            {
               result += attr->size(true);
            });
            /* *INDENT-ON* */

            return result;
         }

         //! \see Serializable::pack
         size_t pack (ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += array.write (offset, (uint8_t) attributes.size ());

            /* *INDENT-OFF* */
            for_each (attributes.begin (), attributes.end (), [&array,&offset](IAttribute * attr)
            {
               offset += attr->pack (array, offset, true);
            });
            /* *INDENT-ON* */

            return offset - start;
         }

         //! \see Serializable::unpack
         size_t unpack (const ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += array.read (offset, count);

            return offset - start;
         }
      };

      /*!
       * This class represents the payload of a HF::Message::SET_ATTR_PACK_RES message.
       */
      struct Response:public Serializable
      {
         /*!
          * Set attribute operation result.
          */
         struct Result:public Serializable
         {
            uint8_t    uid;  //!< Attribute UID.
            HF::Result code; //!< Command result.

            Result() {}

            Result(uint8_t uid, HF::Result code):
               uid (uid), code (code)
            {}

            //! \see Serializable::size
            static constexpr size_t Size = sizeof(uint8_t) + sizeof(uint8_t);

            //! \see Serializable::size
            size_t size () const
            {
               return Result::Size;
            }

            //! \see Serializable::pack
            size_t pack (ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, (uint8_t) uid);
               offset += array.write (offset, (uint8_t) code);

               return offset - start;
            }

            //! \see Serializable::unpack
            size_t unpack (const ByteArray &array, size_t offset = 0)
            {
               size_t  start = offset;

               uint8_t temp;
               offset += array.read (offset, temp);
               uid     = temp;

               offset += array.read (offset, temp);
               code    = static_cast <HF::Result>(temp);

               return offset - start;
            }
         };

         typedef vector <Result> results_t;

         results_t results;

         /*!
          * Unpack the results count.
          *
          * \warning This value will not be used as the number of results, when
          *          packing the response.
          */
         uint8_t count;

         //! \see Serializable::size
         size_t size () const
         {
            size_t result = sizeof(uint8_t); // Number of attribute results.

            result += results.size () * (sizeof(uint8_t) + sizeof(uint8_t));

            return result;
         }

         //! \see Serializable::pack
         size_t pack (ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += array.write (offset, (uint8_t) results.size ());

            /* *INDENT-OFF* */
            for_each (results.begin (), results.end (), [&array,&offset](Result result)
            {
               offset += result.pack (array, offset);
            });
            /* *INDENT-ON* */

            return offset - start;
         }

         //! \see Serializable::unpack
         size_t unpack (const ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += array.read (offset, count);

            for (int i = 0; i < count; i++)
            {
               if (!array.available (offset, Result::Size))
               {
                  break;
               }

               Result result;
               offset += result.unpack (array, offset);

               results.push_back (result);
            }

            return offset - start;
         }
      };

   }  // namespace SetAttributePack

}  // namespace HF

#endif /* HF_ATTRIBUTES_H */
