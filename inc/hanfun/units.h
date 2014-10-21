// =============================================================================
/*!
 * \file       inc/hanfun/units.h
 *
 * This file contains the definitions for the units implementation.
 *
 * \version    1.0.1
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================
#ifndef HF_UNITS_H
#define HF_UNITS_H

#include "hanfun/common.h"
#include "hanfun/profiles.h"

#include "hanfun/device.h"

namespace HF
{
   /*!
    * This is the top level namespace for the HAN-FUN units.
    */
   namespace Units
   {
      /*!
       * This class represents the interface implemented by all Units.
       */
      struct IUnit:public Profiles::IProfile
      {
         //! Id number of this unit on the device.
         virtual uint8_t id () const = 0;

         //! The device this unit is associated with.
         virtual IDevice &device () const = 0;

         //! \see Interface::handle
         virtual Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset) = 0;

         /*!
          * Create and send a new packet with the given message to the given address.
          *
          * @param [in] addr     address to send the message to.
          * @param [in] message  message to send.
          * @param [in] link     preferred link to send the message on.
          */
         virtual void send (const Protocol::Address &addr, Protocol::Message &message,
                            Transport::Link *link = nullptr) = 0;

         /*!
          * Return a vector containing a list of extra interfaces, other than the interfaces
          * specified by the profile the unit implements)
          *
          * @return  vector containing the list of extra interfaces.
          */
         virtual std::vector <Common::Interface> interfaces () const = 0;
      };

      /*!
       * This is the parent class for all Units.
       */
      class AbstractUnit:public IUnit
      {
         IDevice &_device;

         public:

         //! Get the device associated with this unit.
         IDevice &device () const
         {
            return _device;
         }

         void send (const Protocol::Address &addr, Protocol::Message &message,
                    Transport::Link *link);

         std::vector <Common::Interface> interfaces () const
         {
            return std::vector <Common::Interface>();
         }

         protected:

         AbstractUnit(IDevice &device):_device (device)
         {
            device.add (this);
         }

         void notify (const HF::Attributes::IAttribute &old_value,
                      const HF::Attributes::IAttribute &new_value) const;
      };

      /*!
       * This the base template for all the Units.
       */
      template<class Profile, typename... ITF>
      class Unit:public HF::Units::AbstractUnit, public virtual Profile
      {
         uint8_t _id;

         typedef std::tuple <HF::Interfaces::Proxy <ITF, Unit> ...> interfaces_t;

         interfaces_t _interfaces;

         public:

         Unit(uint8_t index, IDevice &device):
            Profile (), HF::Units::AbstractUnit (device), _id (index),
            _interfaces (HF::Interfaces::Proxy <ITF, Unit>(*this) ...)
         {}

         //! \see IUnit::id
         uint8_t id () const
         {
            return _id;
         }

         uint16_t uid () const
         {
            return Profile::uid ();
         }

         Common::Result handle (Protocol::Packet &packet, Common::ByteArray &payload, size_t offset)
         {
            Common::Result result = Profile::handle (packet, payload, offset);

            // Message not handled by base profile, then try extra interfaces.
            if (result == Common::Result::FAIL_ID)
            {
               Interface *itf = find <0, ITF...>(packet.message.itf.id);

               if (itf != nullptr)
               {
                  return itf->handle (packet, payload, offset);
               }
               else
               {
                  return Common::Result::FAIL_SUPPORT;
               }
            }

            return result;
         }

         HF::Attributes::List attributes (Common::Interface itf, uint8_t pack_id,
                                          const HF::Attributes::UIDS &uids) const
         {
            auto result = Profile::attributes (itf, pack_id, uids);

            attributes_itf <0, ITF...>(result, itf, pack_id, uids);

            return result;
         }

         using HF::Units::AbstractUnit::send;

         void send (const Protocol::Address &addr, Protocol::Message &message)
         {
            AbstractUnit::send (addr, message, nullptr);
         }

         void notify (const HF::Attributes::IAttribute &old_value,
                      const HF::Attributes::IAttribute &new_value) const
         {
            AbstractUnit::notify (old_value, new_value);
         }

         std::vector <Common::Interface> interfaces () const
         {
            std::vector <Common::Interface> result;
            result.reserve (sizeof ... (ITF));

            Common::Interface temp;
            for_each ([&result, &temp](HF::Interface &itf)
                      {
                         temp.id = itf.uid ();
                         temp.role = itf.role ();
                         result.push_back (temp);
                      }
                     );

            return result;
         }

         void periodic (uint32_t time)
         {
            Profile::periodic (time);

            for_each ([time](HF::Interface &itf)
                      {
                         itf.periodic (time);
                      }
                     );
         }

         protected:

         template<uint8_t N>
         const typename std::tuple_element <N, interfaces_t>::type::base * get () const
         {
            return &std::get <N>(_interfaces);
         }

         void for_each (std::function <void(HF::Interface &)> func) const
         {
            for_each <0, ITF...>(func);
         }

         void for_each (std::function <void(HF::Interface &)> func)
         {
            for_each <0, ITF...>(func);
         }

         private:

         template<uint8_t N, typename Head, typename... Tail>
         HF::Interface *find (uint16_t itf_uid) const
         {
            static_assert (std::is_base_of <HF::Interface, Head>::value,
                           "Head must be of type HF::Interface");

            const Head &head = std::get <N>(_interfaces);

            if (head.uid () == itf_uid)
            {
               return const_cast <Head *>(&head);
            }
            else
            {
               return find <N + 1, Tail...>(itf_uid);
            }
         }

         template<uint8_t N>
         HF::Interface *find (uint16_t itf_uid) const
         {
            UNUSED (itf_uid);
            return nullptr;
         }

         template<uint8_t N, typename Head, typename... Tail>
         void for_each (std::function <void(HF::Interface &)> func) const
         {
            const auto &head = std::get <N>(this->_interfaces);

            func (*(const_cast <HF::Interface *>(static_cast <const HF::Interface *>(&head))));

            for_each <N + 1, Tail...>(func);
         }

         template<uint8_t N>
         void for_each (std::function <void(HF::Interface &)> func) const
         {
            UNUSED (func);
         }

         template<uint8_t N, typename Head, typename... Tail>
         void attributes_itf (HF::Attributes::List &attrs, Common::Interface itf, uint8_t pack_id,
                              const HF::Attributes::UIDS &uids) const
         {
            const auto &head = std::get <N>(this->_interfaces);

            if (head.uid () == itf.id)
            {
               auto result = HF::Attributes::get (head, pack_id, uids);
               attrs.merge (result);
            }
            else
            {
               attributes_itf <N + 1, Tail...>(attrs, itf, pack_id, uids);
            }
         }

         template<uint8_t N>
         void attributes_itf (HF::Attributes::List &attrs, Common::Interface itf, uint8_t pack_id,
                              const HF::Attributes::UIDS &uids) const
         {
            UNUSED (attrs);
            UNUSED (itf);
            UNUSED (pack_id);
            UNUSED (uids);
         }
      };

   }  // namespace Units

}  // namespace HF

#endif /* HF_UNITS_H */
