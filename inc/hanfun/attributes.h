// =============================================================================
/*!
 * @file       inc/hanfun/attributes.h
 *
 * This file contains the definitions for the attribute handling API in HAN-FUN.
 *
 * @version    1.1.1
 *
 * @copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
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
    * This is the top-level namespace for the classes that implement
    * the interface's attribute manipulation functionality.
    */
   namespace Attributes
   {
      /*!
       * @addtogroup attributes  Attributes
       * @ingroup protocol
       *
       * This module contains the classes that define and implement the common API for
       * handling attributes.
       * @{
       */

      /*!
       * %Interface/%Service %Attribute API.
       */
      struct IAttribute:public Common::Serializable, public Common::Cloneable <IAttribute>
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
         virtual uint16_t interface () const = 0;

         /*!
          * Pointer to the interface that owns this attribute.
          *
          * This can return nullptr if the attribute was created for a remote interface.
          *
          * @return  pointer to the interface that owns this attribute,
          *          or nullptr if owner is a remote object.
          */
         virtual HF::Interface const *owner () const = 0;

         /*!
          * @copydoc HF::Common::Serializable::size
          *
          * @param [in] with_uid    include uid() size in the calculation.
          */
         virtual size_t size (bool with_uid) const = 0;

         //! @copydoc HF::Common::Serializable::size
         virtual size_t size () const = 0;

         /*!
          * @copydoc HF::Common::Serializable::pack
          *
          * @param [in] with_uid    include uid() in the serialization.
          */
         virtual size_t pack (Common::ByteArray &array, size_t offset, bool with_uid) const = 0;

         //! @copydoc HF::Common::Serializable::pack
         virtual size_t pack (Common::ByteArray &array, size_t offset) const = 0;

         /*!
          * @copydoc HF::Common::Serializable::unpack
          *
          * @warning If @c with_uid == @c true, then if the value read from the
          *          array does not match the attribute's UID, no more data will be read.
          *
          * @param [in] with_uid    attribute %UID is included in the serialization.
          */
         virtual size_t unpack (const Common::ByteArray &array, size_t offset, bool with_uid) = 0;

         //! @copydoc HF::Common::Serializable::unpack
         virtual size_t unpack (const Common::ByteArray &array, size_t offset) = 0;

         virtual bool operator ==(const IAttribute &other) const               = 0;

         virtual bool operator <(const IAttribute &other) const                = 0;

         virtual bool operator >(const IAttribute &other) const                = 0;

         /*!
          * This method is used to get the percentage of change that the
          * attribute has in relation to the value present in @c other.
          *
          * @param [in] other attribute holding a previous value.
          *
          * @return  float indicating the percentage of change.
          */
         virtual float changed (const IAttribute &other) const = 0;

         /*!
          * Compare this attribute with the given attribute in @c other.
          *
          * This method should return < 0 if this attribute is less that,
          * 0 if it is equal and > 0 if greater that the @c other attribute.
          *
          * @param [in] other attribute to compare to.
          *
          * @retval  <0 if attribute less than @c other;
          * @retval  0  if attribute equal to @c other;
          * @retval  >0 if attribute greater than @c other.
          */
         virtual int compare (const IAttribute &other) const = 0;
      };

      //! Attribute factory function type.
      typedef IAttribute * (*Factory)(uint8_t);

      /*!
       * Function pointer to a function that returns the attribute
       * factory associated with a given interface.
       */
      typedef Attributes::Factory (*FactoryGetter)(Common::Interface);

      /*!
       * Return the attribute factory associated with the given
       * interface identifier.
       *
       * @warning This function only returns the factories for the
       *          build in interfaces.
       *
       * @param itf  interface identifier.
       *
       * @return  the factory associated with the interface, or
       *          @c nullptr if the interface is unknown.
       */
      Factory get_factory (Common::Interface itf);

      /*!
       * List of attributes UIDs.
       */
      struct UIDS:public std::vector <uint8_t>
      {
         UIDS():std::vector <uint8_t>()
         {}

         /*!
          * Constructor
          *
          * @param [in] uids attributes UIDs list.
          */
         UIDS(std::initializer_list <uint8_t> uids):vector <uint8_t>(uids)
         {}

         /*!
          * Number of elements in the list.
          *
          * @note this is used as an alias to the underling method,
          * @c std::vector<uint8_t>::size, as the size method signature
          * is used to provide a Serializable type interface.
          *
          * @return  number of elements.
          */
         vector <uint8_t>::size_type length () const
         {
            return vector <uint8_t>::size ();
         }

         //! @copydoc HF::Common::Serializable::size
         size_t size () const
         {
            return sizeof(uint8_t) + sizeof(uint8_t) * vector <uint8_t>::size ();
         }

         //! @copydoc HF::Common::Serializable::pack
         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t  start = offset;

            uint8_t count = length ();
            offset += array.write (offset, count);

            /* *INDENT-OFF* */
            std::for_each (vector<uint8_t>::begin(), vector<uint8_t>::end(),
                           [&offset,&array](uint8_t uid)
            {
               offset += array.write (offset, uid);
            });
            /* *INDENT-ON* */

            return offset - start;
         }

         //! @copydoc HF::Common::Serializable::unpack
         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            uint8_t count = 0;
            return unpack (array, offset, count);
         }

         /*!
          * @copydoc HF::Common::Serializable::unpack
          *
          * @param [out] count   reference to a variable that will hold
          *                      the count value read from the array.
          */
         size_t unpack (const Common::ByteArray &array, size_t offset, uint8_t &count)
         {
            size_t start = offset;

            offset += array.read (offset, count);

            for (uint8_t i = 0; i < count; i++)
            {
               uint8_t uid;
               offset += array.read (offset, uid);
               vector <uint8_t>::push_back (uid);
            }

            return offset - start;
         }

      };

      /*!
       * Parent class for Attribute API implementation.
       */
      class AbstractAttribute:public IAttribute
      {
         protected:

         const uint16_t _itf_uid;      //!< Interface this attribute belongs to.
         const uint8_t  _uid;          //!< Attribute unique identifier.
         const bool _writable;         //!< Attribute access mode.

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

         bool operator ==(const IAttribute &other) const
         {
            return this->compare (other) == 0;
         }

         bool operator ==(IAttribute &other) const
         {
            return this->compare (other) == 0;
         }

         bool operator <(const IAttribute &other) const
         {
            return this->compare (other) < 0;
         }

         bool operator <(IAttribute &other) const
         {
            return this->compare (other) < 0;
         }

         bool operator >(const IAttribute &other) const
         {
            return this->compare (other) > 0;
         }

         bool operator >(IAttribute &other) const
         {
            return this->compare (other) > 0;
         }

         int compare (const IAttribute &other) const
         {
            int res = this->interface () - other.interface ();

            if (res != 0)
            {
               return res;
            }

            return this->uid () - other.uid ();
         }
      };

      /*!
       * Helper template class to declare an attribute with the given @c T type.
       *
       * @tparam T underling data type for the attribute.
       */
      template<typename T, typename _Owner = void, typename = void>
      struct Attribute:public AbstractAttribute
      {
         /*!
          * Attribute template constructor.
          *
          * @param [in] interface   attribute's interface UID.
          * @param [in] uid         attribute's UID.
          * @param [in] data        attribute's value.
          * @param [in] __owner     pointer to attribute's interface owner object.
          * @param [in] writable    attribute's writable information.
          */
         Attribute(const uint16_t interface, const uint8_t uid, const HF::Interface *__owner, T data,
                   bool writable = false):
            AbstractAttribute (interface, uid, writable), helper (data), _owner (__owner)
         {}

         /*!
          * Attribute template constructor.
          *
          * @param [in] interface   attribute's interface UID.
          * @param [in] uid         attribute's UID.
          * @param [in] writable    attribute's writable information.
          */
         Attribute(const uint16_t interface, const uint8_t uid, bool writable = false):
            AbstractAttribute (interface, uid, writable), _owner (nullptr)
         {}

         /*!
          * Attribute template constructor.
          *
          * @param [in] interface   attribute's interface UID.
          * @param [in] uid         attribute's UID.
          * @param [in] data        attribute's value.
          * @param [in] writable    attribute's writable information.
          */
         Attribute(const uint16_t interface, const uint8_t uid, T data, bool writable = false):
            AbstractAttribute (interface, uid, writable), helper (data), _owner (nullptr)
         {}

         typedef typename std::remove_reference <T>::type value_type;

         // =============================================================================
         // API
         // =============================================================================

         virtual void set (value_type value)
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

         HF::Interface const *owner () const
         {
            return _owner;
         }

         size_t size (bool with_uid) const
         {
            return size () + (with_uid ? sizeof(uint8_t) : 0);
         }

         size_t size () const
         {
            return helper.size ();
         }

         size_t pack (Common::ByteArray &array, size_t offset, bool with_uid) const
         {
            size_t start = offset;

            if (with_uid)
            {
               offset += array.write (offset, uid ());
            }

            offset += helper.pack (array, offset);

            return offset - start;
         }

         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            return helper.pack (array, offset);
         }

         size_t unpack (const Common::ByteArray &array, size_t offset, bool with_uid)
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

         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            return helper.unpack (array, offset);
         }

         IAttribute *clone () const
         {
            return new HF::Attributes::Attribute <T>(this->_itf_uid, this->_uid, this->_owner,
                                                     this->helper.data, this->_writable);
         }

         int compare (const IAttribute &other) const
         {
            int res = AbstractAttribute::compare (other);

            if (res == 0)
            {
               Attribute <T> *temp = (Attribute <T> *) & other;
               res = helper.compare (temp->helper);
            }

            return res;
         }

         float changed (const IAttribute &other) const
         {
            int res = AbstractAttribute::compare (other);

            if (res == 0)
            {
               Attribute <T> *temp = (Attribute <T> *) & other;
               return helper.changed (temp->helper);
            }

            return 0.0;
         }

         protected:

         Common::SerializableHelper <T> helper;

         const HF::Interface            *_owner;

         private:

         // Don't allow copy.
         Attribute(const Attribute &other) = delete;
      };

      /*!
       * Helper template class to declare an attribute with the given @c T type.
       *
       * @tparam T underling data type for the attribute.
       */
      template<typename T, typename _Owner>
      struct Attribute <T, _Owner, typename std::enable_if <std::is_base_of <HF::Interface, _Owner>::value>::type> :
         public AbstractAttribute
      {
         typedef typename std::remove_reference <T>::type value_type;

         typedef typename std::function <value_type (_Owner &)> getter_t;
         typedef typename std::function <void (_Owner &, T)> setter_t;

         /*!
          * Attribute template constructor.
          *
          * @param [in] __owner   reference to attribute's interface owner object.
          * @param [in] uid       attribute's UID.
          * @param [in] _getter   owner's member function to get the value of the attribute.
          * @param [in] _setter   owner's member function to set the value of the attribute.
          * @param [in] writable  attribute's writable information.
          */
         Attribute(_Owner &__owner, const uint8_t uid, getter_t _getter, setter_t _setter, bool writable = false):
            AbstractAttribute (__owner.uid (), uid, writable), _owner (__owner), getter (_getter), setter (_setter)
         {}

         // =============================================================================
         // API
         // =============================================================================

         virtual void set (value_type value)
         {
            setter (_owner, value);
         }

         value_type get () const
         {
            return value ();
         }

         value_type value () const
         {
            return getter (_owner);
         }

         HF::Interface const *owner () const
         {
            return &_owner;
         }

         size_t size (bool with_uid) const
         {
            return size () + (with_uid ? sizeof(uint8_t) : 0);
         }

         size_t size () const
         {
            return helper.size ();
         }

         size_t pack (Common::ByteArray &array, size_t offset, bool with_uid) const
         {
            size_t start = offset;

            if (with_uid)
            {
               offset += array.write (offset, uid ());
            }

            const_cast <decltype(helper) &>(helper).data = getter (_owner);

            offset                                      += helper.pack (array, offset);

            return offset - start;
         }

         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            const_cast <decltype(helper) &>(helper).data = getter (_owner);

            return helper.pack (array, offset);
         }

         size_t unpack (const Common::ByteArray &array, size_t offset, bool with_uid)
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

            setter (_owner, helper.data);

            _end:
            return offset - start;
         }

         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            size_t result = helper.unpack (array, offset);
            setter (_owner, helper.data);
            return result;
         }

         IAttribute *clone () const
         {
            return new HF::Attributes::Attribute <T, _Owner>(this->_owner, this->uid (),
                                                             this->getter, this->setter,
                                                             this->isWritable ());
         }

         int compare (const IAttribute &other) const
         {
            int res = AbstractAttribute::compare (other);

            if (res == 0)
            {
               const_cast <decltype(helper) &>(helper).data = getter (_owner);

               Attribute <T, _Owner> *temp = (Attribute <T, _Owner> *) & other;
               res = helper.compare (temp->helper);
            }

            return res;
         }

         float changed (const IAttribute &other) const
         {
            int res = AbstractAttribute::compare (other);

            if (res == 0)
            {
               const_cast <decltype(helper) &>(helper).data = getter (_owner);

               Attribute <T, _Owner> *temp = (Attribute <T, _Owner> *) & other;
               return helper.changed (temp->helper);
            }

            return 0.0;
         }

         protected:

         _Owner                                  &_owner;
         getter_t                                getter;
         setter_t                                setter;

         Common::SerializableHelper <value_type> helper;

         private:

         // Don't allow copy.
         Attribute(const Attribute &other) = delete;
      };

      /*!
       * This class has the same behavior has a @c list, however
       * the @c list element access methods where overwritten to
       * allow using the attribute UIDs as indexes.
       *
       * The difference between the @c list indexes and the @c Attribute
       * indexes is that the former start at 0 and the latter at 1.
       */
      struct List:public std::list <IAttribute *>
      {
         IAttribute *operator [](uint8_t uid) const
         {
            for (const_iterator it = begin (); it != end (); ++it)
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
      struct Response:public Protocol::Response
      {
         IAttribute *attribute;

         /*!
          * Constructor.
          *
          * @param [in] _attribute   pointer to the attribute value to send
          *                          in the response.
          */
         Response(IAttribute *_attribute = nullptr):
            attribute (_attribute)
         {}

         virtual ~Response()
         {
            delete attribute;
         }

         size_t size () const
         {
            return Protocol::Response::size () + (attribute != nullptr ? attribute->size () : 0);
         }

         size_t pack (Common::ByteArray &array, size_t offset = 0) const
         {
            size_t start = offset;

            offset += Protocol::Response::pack (array, offset);

            offset += (attribute != nullptr ? attribute->pack (array, offset) : 0);

            return offset - start;
         }

         size_t unpack (const Common::ByteArray &array, size_t offset = 0)
         {
            size_t start = offset;

            offset += Protocol::Response::unpack (array, offset);
            offset += (attribute != nullptr ? attribute->unpack (array, offset) : 0);

            return offset - start;
         }
      };

      /*!
       * @}
       * @addtogroup attr_packs Packs
       * @ingroup attributes
       *
       * This module contains the classes used to implement the attribute pack functionality.
       *
       * An attribute pack is an identifier that can be use to get/set a number of attributes
       * in block.
       * @{
       */

      /*!
       * Attribute Pack Special IDs.
       *
       * These IDs when received on a HF::Message::GET_ATTR_PACK_REQ, allow the return of all
       * mandatory/optional attributes, without the need to indicate all the attribute UIDs.
       */
      typedef enum _Pack
      {
         MANDATORY = 0x00, //!< Return all mandatory attributes for the interface.
         ALL       = 0xFE, //!< Return all mandatory and optional attributes for the interface.
         DYNAMIC   = 0xFF, //!< Return the attributes with the given attributes.
      } Pack;

      /*!
       * Get a list with the attributes for the given interface, pack id or the uids
       * passed in.
       *
       * @param [in] itf      reference to the interface to retrieve the attributes for.
       * @param [in] pack_id  pack id used to get attributes UIDs for. If HF::Attributes::DYNAMIC,
       *                      use uids present in @c uids.
       * @param [in] uids     array containing the attribute UID's if pack id equals HF::Attributes::DYNAMIC.
       *
       * @return  list containing the attributes.
       */
      List get (const HF::Interface &itf, uint8_t pack_id, const UIDS &uids);

      /*! @} */
   }  // namespace Attributes

   namespace Protocol
   {
      /*!
       * @addtogroup attr_packs Packs
       * @{
       */

      /*!
       * This namespace contains the classes that implement the HF::Message::GET_ATTR_PACK_REQ
       * messages.
       */
      namespace GetAttributePack
      {
         /*!
          * This class represents the payload of a HF::Message::GET_ATTR_PACK_REQ request,
          * when the payload is Type::DYNAMIC.
          */
         struct Request
         {
            HF::Attributes::UIDS attributes; //!< Vector containing the attributes UID's to get.

            /*!
             * Unpack attribute count.
             *
             * @warning This value will not be used as the number of attributes, when
             *          packing the request.
             */
            uint8_t count;

            Request():count (0) {}

            Request(HF::Attributes::UIDS &attributes):
               attributes (attributes), count (0)
            {}

            //! @copydoc HF::Common::Serializable::size
            size_t size () const
            {
               return attributes.size ();
            }

            //! @copydoc HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               return attributes.pack (array, offset);
            }

            //! @copydoc HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               return attributes.unpack (array, offset, count);
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
            HF::Attributes::Factory attribute_factory;

            //!< List containing the attributes to send.
            HF::Attributes::List attributes;

            /*!
             * Unpack attribute count.
             *
             * @warning This value will not be used as the number of attributes, when
             *          packing the request.
             */
            uint8_t count;

            /*!
             * Constructor.
             */
            Response():attribute_factory (nullptr)
            {}

            /*!
             * Constructor.
             *
             * @param [in] attributes  list of attributes to send.
             */
            Response(HF::Attributes::List &attributes):
               attribute_factory (nullptr), attributes (attributes)
            {}

            /*!
             * Constructor.
             *
             * @param [in] factory  attribute factory to use when reading the
             *                      incoming attributes.
             */
            Response(HF::Attributes::Factory factory):
               attribute_factory (factory)
            {}

            virtual ~Response()
            {
               /* *INDENT-OFF* */
               std::for_each (attributes.begin (), attributes.end (),
                               [](HF::Attributes::IAttribute *attr)
               {
                  delete attr;
               });
               /* *INDENT-ON* */
            }

            size_t size () const
            {
               size_t result = Protocol::Response::size ();

               result += sizeof(uint8_t);

               /* *INDENT-OFF* */
               std::for_each (attributes.begin(), attributes.end(),
                              [&result](HF::Attributes::IAttribute *attr)
               {
                  result += attr->size(true);
               });
               /* *INDENT-ON* */

               return result;
            }

            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += Protocol::Response::pack (array, offset);

               offset += array.write (offset, (uint8_t) attributes.size ());

               /* *INDENT-OFF* */
               std::for_each (attributes.begin(), attributes.end(),
                              [&array, &offset] (HF::Attributes::IAttribute * attr)
               {
                  offset += attr->pack(array, offset, true);
               });
               /* *INDENT-ON* */

               return offset - start;
            }

            size_t unpack (const Common::ByteArray &array, size_t offset = 0);
         };

      } // namespace GetAttributePack

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
         struct Request
         {
            HF::Attributes::List attributes; //!< List containing the attributes to send.

            /*!
             * Unpack attribute count.
             *
             * @warning This value will not be used as the number of attributes, when
             *          packing the request.
             */
            uint8_t count;

            virtual ~Request()
            {
               /* *INDENT-OFF* */
               std::for_each (attributes.begin (), attributes.end (),
                              [](HF::Attributes::IAttribute *attr)
               {
                  delete attr;
               });
               /* *INDENT-ON* */
            }

            //! @copydoc HF::Common::Serializable::size
            size_t size () const
            {
               size_t result = sizeof(uint8_t);

               /* *INDENT-OFF* */
               std::for_each ( attributes.begin(), attributes.end(),
                               [&result](HF::Attributes::IAttribute * attr)
               {
                  result += attr->size(true);
               });
               /* *INDENT-ON* */

               return result;
            }

            //! @copydoc HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, (uint8_t) attributes.size ());

               /* *INDENT-OFF* */
               std::for_each (attributes.begin (), attributes.end (),
                              [&array,&offset](HF::Attributes::IAttribute * attr)
               {
                  offset += attr->pack (array, offset, true);
               });
               /* *INDENT-ON* */

               return offset - start;
            }

            //! @copydoc HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += array.read (offset, count);

               return offset - start;
            }
         };

         /*!
          * This class represents the payload of a HF::Message::SET_ATTR_PACK_RES message.
          */
         struct Response
         {
            /*!
             * Set attribute operation result.
             */
            struct Result
            {
               uint8_t        uid;  //!< Attribute UID.
               Common::Result code; //!< Command result.

               Result() {}

               /*!
                * Constructor.
                *
                * @param [in] uid   attribute's UID.
                * @param [in] code  operation result code.
                */
               Result(uint8_t uid, Common::Result code):
                  uid (uid), code (code)
               {}

               //! @see HF::Common::Serializable::size
               static constexpr size_t min_size = sizeof(uint8_t) + sizeof(uint8_t);

               //! @copydoc HF::Common::Serializable::size
               size_t size () const
               {
                  return Result::min_size;
               }

               //! @copydoc HF::Common::Serializable::pack
               size_t pack (Common::ByteArray &array, size_t offset = 0) const
               {
                  size_t start = offset;

                  offset += array.write (offset, (uint8_t) uid);
                  offset += array.write (offset, (uint8_t) code);

                  return offset - start;
               }

               //! @copydoc HF::Common::Serializable::unpack
               size_t unpack (const Common::ByteArray &array, size_t offset = 0)
               {
                  size_t  start = offset;

                  uint8_t temp;
                  offset += array.read (offset, temp);
                  uid     = temp;

                  offset += array.read (offset, temp);
                  code    = static_cast <Common::Result>(temp);

                  return offset - start;
               }
            };

            typedef std::vector <Result> results_t;

            results_t results;   //!< Response results.

            /*!
             * Unpack the results count.
             *
             * @warning This value will not be used as the number of results, when
             *          packing the response.
             */
            uint8_t count;

            //! @copydoc HF::Common::Serializable::size
            size_t size () const
            {
               size_t result = sizeof(uint8_t); // Number of attribute results.

               result += results.size () * (sizeof(uint8_t) + sizeof(uint8_t));

               return result;
            }

            //! @copydoc HF::Common::Serializable::pack
            size_t pack (Common::ByteArray &array, size_t offset = 0) const
            {
               size_t start = offset;

               offset += array.write (offset, (uint8_t) results.size ());

               /* *INDENT-OFF* */
               std::for_each (results.begin (), results.end (), [&array,&offset](Result result)
               {
                  offset += result.pack (array, offset);
               });
               /* *INDENT-ON* */

               return offset - start;
            }

            //! @copydoc HF::Common::Serializable::unpack
            size_t unpack (const Common::ByteArray &array, size_t offset = 0)
            {
               size_t start = offset;

               offset += array.read (offset, count);

               for (int i = 0; i < count; i++)
               {
                  if (!array.available (offset, Result::min_size))
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

      } // namespace SetAttributePack
        /*! @} */

   }  // namespace Protocol

}  // namespace HF

#endif /* HF_ATTRIBUTES_H */
