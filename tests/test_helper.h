// =============================================================================
/*!
 * @file       tests/test_helper.h
 *
 * This file contains the definition of helper classes used for testing.
 *
 * @version    1.5.3
 *
 * @copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 */
// =============================================================================
#ifndef HF_TEST_HELPER_H
#define HF_TEST_HELPER_H

#include <string.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "hanfun/common.h"
#include "hanfun/attributes.h"
#include "hanfun/protocol.h"

#include "hanfun/interface.h"
#include "hanfun/profiles.h"
#include "hanfun/devices.h"

#include "hanfun/core/device_information.h"
#include "hanfun/core/device_management.h"
#include "hanfun/core/attribute_reporting.h"
#include "hanfun/core/time.h"
#include "hanfun/core/batch_program_management.h"
#include "hanfun/core/event_scheduling.h"
#include "hanfun/core/weekly_scheduling.h"

#include "hanfun/units.h"

#include "hanfun/interfaces/colour_control.h"

using namespace HF;
using namespace HF::Protocol;

// =============================================================================
// Helper Test Functions
// =============================================================================

#define STRING_FROM(_T)                                                 \
   SimpleString StringFrom(const _T &data)                              \
   {                                                                    \
      Common::SerializableHelper<_T &> wrapper(const_cast<_T &>(data)); \
      return StringFrom(wrapper);                                       \
   }

SimpleString StringFrom(const std::vector<uint8_t> &array);

SimpleString StringFrom(const HF::Common::Serializable &data);

SimpleString StringFrom(const HF::Common::Interface &itf);

SimpleString StringFrom(const HF::Attributes::IAttribute &attr);

SimpleString StringFrom(const HF::Interfaces::ColourControl::HS_Colour &colour);

SimpleString StringFrom(const HF::Interfaces::ColourControl::XY_Colour &colour);

template<typename _type = uint8_t>
void check_index(_type expected, _type actual, uint32_t index, const char *header,
                 const char *fileName,
                 int lineNumber)
{
   if (actual != expected)
   {
      std::ostringstream error;
      error << header << " at index " << index << std::endl
            << "\t\t expected : "
            << expected << std::endl
            << "\t\t but was  : "
            << actual;

      FAIL_TEST_LOCATION(error.str().c_str(), fileName, lineNumber);
   }
}

#define CHECK_INDEX(_header, _index, _expected, _actual) \
   check_index(_expected, _actual, _index, _header, __FILE__, __LINE__)
#define CHECK_ATTRIBUTE_UID(_index, _expected, _actual) \
   check_index<uint8_t>(_expected, _actual, _index, "Attribute UID : ", __FILE__, __LINE__)

#define CHECK_LOCATION_TRUE(condition, file, line) \
   CHECK_TRUE_LOCATION(condition, "CHECK_TRUE", #condition, NULL, file, line)

#define CHECK_LOCATION_FALSE(condition, file, line) \
   CHECK_FALSE_LOCATION(condition, "CHECK_FALSE", #condition, NULL, file, line)

template<typename T> T &to_ref(T &_ref)
{
   return _ref;
}
template<typename T> T &to_ref(T *_ref)
{
   return *_ref;
}

template<typename Attribute, typename Interface, typename Getter, typename Setter>
void check_attribute_common(Interface &itf, bool writable,
                            typename Attribute::value_type first,
                            typename Attribute::value_type second,
                            Getter getter, Setter setter,
                            const char *file, int lineno)
{
   mock("Interface").expectNCalls(2, "notify").ignoreOtherParameters();

   (itf.*setter)(first);

   CHECK_EQUAL_LOCATION(first, (itf.*getter)(), NULL, file, lineno);

   typedef HF::Attributes::Attribute<typename Attribute::value_type, Interface> __Attribute;

   auto attr = static_cast<__Attribute *>(itf.attribute(Attribute::ID));
   CHECK_LOCATION_TRUE(attr != nullptr, file, lineno);

   CHECK_EQUAL_LOCATION(writable, attr->isWritable(), NULL, file, lineno);
   POINTERS_EQUAL_LOCATION(&itf, attr->owner(), NULL, file, lineno);
   LONGS_EQUAL_LOCATION(itf.uid(), attr->interface(), NULL, file, lineno);

   CHECK_EQUAL_LOCATION(first, attr->value(), NULL, file, lineno);

   attr->value(second);
   CHECK_EQUAL_LOCATION(second, attr->value(), NULL, file, lineno);

   mock("Interface").checkExpectations();

   delete attr;
}

template<typename Attribute, typename Interface>
void check_attribute_pack(Interface &itf, const char *file, int lineno)
{
   auto attrs = itf.attributes(HF::Attributes::Pack::MANDATORY);

   CHECK_LOCATION_TRUE(std::any_of(attrs.begin(), attrs.end(),
                                   [](uint8_t uid) {return uid == Attribute::ID;}),
                       file, lineno);

   attrs = itf.attributes(HF::Attributes::Pack::ALL);

   CHECK_LOCATION_TRUE(std::any_of(attrs.begin(), attrs.end(),
                                   [](uint8_t uid) {return uid == Attribute::ID;}),
                       file, lineno);
}


template<typename Attribute, typename Interface, typename Getter, typename Setter,
         typename = void>
void check_attribute(Interface &itf, bool writable,
                     typename Attribute::value_type first,
                     typename Attribute::value_type second,
                     Getter getter, Setter setter,
                     const char *file, int lineno)
{
   check_attribute_pack<Attribute>(itf, file, lineno);

   check_attribute_common<Attribute, Interface, Getter, Setter>(itf, writable, first, second,
                                                                getter, setter, file, lineno);
}

template<typename Attribute, typename Interface, typename Getter, typename Setter>
void check_optional_attribute(Interface &itf, bool writable,
                              typename Attribute::value_type first,
                              typename Attribute::value_type second,
                              Getter getter, Setter setter,
                              const char *file, int lineno)
{
   auto attrs = itf.attributes(HF::Attributes::Pack::MANDATORY);

   CHECK_LOCATION_TRUE(std::none_of(attrs.begin(), attrs.end(),
                                    [](uint8_t uid) {return uid == Attribute::ID;}),
                       file, lineno);

   attrs = itf.attributes(HF::Attributes::Pack::ALL);

   CHECK_LOCATION_TRUE(std::any_of(attrs.begin(), attrs.end(),
                                   [](uint8_t uid) {return uid == Attribute::ID;}),
                       file, lineno);

   check_attribute_common<Attribute, Interface, Getter, Setter>(itf, writable, first, second,
                                                                getter, setter, file, lineno);
}

#define CHECK_ATTRIBUTE(Interface, Type, _writable, _name, _first, _second)                \
   check_attribute<Type>(to_ref(server), _writable, _first, _second,                       \
                         (Type::value_type (Interface::*)(void) const) & Interface::_name, \
                         (void (Interface::*) (Type::value_type)) & Interface::_name,      \
                         __FILE__, __LINE__)

#define CHECK_ATTRIBUTE_PACK(Interface, Type) \
   check_attribute_pack<Type>(to_ref(server), __FILE__, __LINE__)

#define CHECK_OPT_ATTRIBUTE(Interface, Type, _writable, _name, _first, _second)                     \
   check_optional_attribute<Type>(to_ref(server), _writable, _first, _second,                       \
                                  (Type::value_type (Interface::*)(void) const) & Interface::_name, \
                                  (void (Interface::*) (Type::value_type)) & Interface::_name,      \
                                  __FILE__, __LINE__)

#define CHECK_ATTRIBUTE_ALLOC(_name) \
   {                                 \
      auto attr = new _name();       \
      CHECK_TRUE(attr != nullptr);   \
      delete attr;                   \
   }

#define CHECK_ALLOC(_name, ...)           \
   {                                      \
      auto temp = new _name(__VA_ARGS__); \
      CHECK_TRUE(temp != nullptr);        \
      delete temp;                        \
   }

#define CHECK_UNPACK(_expected, _actual) \
   {                                     \
      auto size     = _actual;           \
      auto exp_size = _expected;         \
      LONGS_EQUAL(exp_size, size)        \
   }

// =============================================================================
// Helper Test Classes
// =============================================================================

namespace HF
{
   namespace Testing
   {
      struct Payload
      {
         Common::ByteArray data;

         Payload(uint16_t _size = 0):
            data(_size & Protocol::MAX_PAYLOAD)
         {
            std::random_device rd;
            std::mt19937 mt(rd());
            std::uniform_int_distribution<uint8_t> dist;

            auto gen = std::bind(dist, mt);

            std::generate_n(data.begin(), data.size(), gen);
         }

         virtual ~Payload()
         {}

         uint16_t size() const
         {
            return data.size();
         }

         uint16_t pack(Common::ByteArray &array, uint16_t offset = 0) const
         {
            array.extend(data.size());

            auto start = array.begin() + offset;

            array.insert(start, data.begin(), data.end());

            return data.size();
         }

         uint16_t unpack(const Common::ByteArray &array, uint16_t offset = 0)
         {
            assert(array.available(offset, data.size()));

            auto begin = array.begin() + offset;
            auto end   = begin + data.size();

            std::copy(begin, end, data.begin());

            return data.size();
         }
      };

      //! Test Interface.
      template<class Base>
      struct InterfaceHelper: public Base
      {
         Protocol::Address addr;
         Protocol::Message sendMsg;

         InterfaceHelper()
         {}

         template<typename Arg>
         InterfaceHelper(Arg &&arg): Base(arg)
         {}

         virtual ~InterfaceHelper()
         {}

         void send(const Protocol::Address &addr, Protocol::Message &message)
         {
            mock("Interface").actualCall("send");

            this->addr    = addr;
            this->sendMsg = message;
         }

         void notify(const HF::Attributes::IAttribute &old_value,
                     const HF::Attributes::IAttribute &new_value) const
         {
            UNUSED(old_value);
            UNUSED(new_value);

            mock("Interface").actualCall("notify");
         }
      };

      template<class Base>
      struct InterfaceParentHelper: public InterfaceHelper<Base>
      {
         InterfaceParentHelper(): InterfaceHelper<Base>()
         {}

         template<typename Arg>
         InterfaceParentHelper(Arg &&arg): InterfaceHelper<Base>(arg)
         {}

         Interface::Role role() const
         {
            return Interface::SERVER_ROLE;
         }
      };

      struct TestInterface: public InterfaceHelper<Interfaces::AbstractInterface>
      {
         static const uint16_t UID = 0x5A5A;

         typedef enum
         {
            ATTR1 = 0x01,
            ATTR2 = 0x02,
            ATTR3 = 0x03,
         } Attributes;

         Interface::Role _role;
         uint16_t        _uid;
         uint16_t        attr1;
         uint16_t        attr2;
         uint16_t        attr3;

         TestInterface(): _role(Interface::SERVER_ROLE), _uid(UID) {}

         TestInterface(Role role, uint16_t _uid):
            _role(role), _uid(_uid), attr1(0x5A51), attr2(0x5A52), attr3(0x5A53)
         {}

         uint16_t uid() const
         {
            return _uid;
         }

         Interface::Role role() const
         {
            return _role;
         }

         HF::Attributes::IAttribute *attribute(uint8_t uid)
         {
            return create_attribute(this, uid);
         }

         //! @see AbstractInterface::attributes
         HF::Attributes::UIDS attributes(uint8_t pack_id = HF::Attributes::Pack::MANDATORY) const
         {
            HF::Attributes::UIDS result;

            if (pack_id == HF::Attributes::Pack::ALL)
            {
               result.push_back(ATTR1);
               result.push_back(ATTR2);
            }

            result.push_back(ATTR3);

            return result;
         }

         static HF::Attributes::IAttribute *create_attribute(uint8_t uid)
         {
            return create_attribute(nullptr, uid);
         }

         static HF::Attributes::IAttribute *create_attribute(TestInterface *itf, uint8_t uid)
         {
            uint16_t itf_uid = (itf != nullptr ? itf->uid() : TestInterface::UID);

            switch (uid)
            {
               case ATTR1:
               {
                  if (itf == nullptr)
                  {
                     return new HF::Attributes::Attribute<uint16_t>(itf_uid, uid);
                  }
                  else
                  {
                     return new HF::Attributes::Attribute<uint16_t &>(itf_uid, uid, itf,
                                                                      itf->attr1);
                  }

                  break;
               }

               case ATTR2:
               {
                  if (itf == nullptr)
                  {
                     return new HF::Attributes::Attribute<uint16_t>(itf_uid, uid);
                  }
                  else
                  {
                     return new HF::Attributes::Attribute<uint16_t &>(itf_uid, uid, itf,
                                                                      itf->attr2);
                  }

                  break;
               }
               case ATTR3:
               {
                  if (itf == nullptr)
                  {
                     return new HF::Attributes::Attribute<uint16_t>(itf_uid, uid, itf, 0, true);
                  }
                  else
                  {
                     return new HF::Attributes::Attribute<uint16_t &>(itf_uid,
                                                                      uid,
                                                                      itf,
                                                                      itf->attr3,
                                                                      true);
                  }

                  break;
               }

               default:
                  return nullptr;
            }
         }

         protected:

         Common::Result handle_command(Protocol::Packet &packet,
                                       Common::ByteArray &payload,
                                       uint16_t offset)
         {
            UNUSED(packet);
            UNUSED(payload);
            UNUSED(offset);

            mock("Interface").actualCall("handle_command").onObject(this);

            return Common::Result::OK;
         }

         bool check_uid(uint16_t uid) const
         {
            return this->_uid == uid;
         }
      };

      HF::Attributes::Factory FactoryGetter(HF::Common::Interface itf);

      //! Test Interface.
      template<class Base>
      struct ProfileHelper: public InterfaceHelper<Base>
      {
         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const
         {
            UNUSED(itf);
            UNUSED(pack_id);
            UNUSED(uids);
            return HF::Attributes::List();
         }
      };

      struct Profile: public Profiles::IProfile, public TestInterface
      {
         uint16_t _uid;

         uint16_t uid() const
         {
            return _uid;
         }

         using TestInterface::attributes;

         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const
         {
            UNUSED(itf);
            return HF::Attributes::get(*this, pack_id, uids);
         }
      };

      struct Unit: public HF::Units::Unit<Profile>
      {
         Unit(uint16_t id, IDevice &device):
            HF::Units::Unit<Profile>(id, device)
         {}
      };

      struct Link: public HF::Transport::AbstractLink
      {
         HF::UID::UID         _uid;
         HF::Transport::Layer *tsp;

         Common::ByteArray    data;

         Link(HF::UID::UID_T *uid = new HF::UID::NONE(), HF::Transport::Layer *tsp = nullptr):
            _uid(uid, true), tsp(tsp)
         {}

         virtual ~Link()
         {}

         void send(Common::ByteArray &array)
         {
            this->data = array;
            mock("Link").actualCall("send");
         }

         const HF::UID::UID uid() const
         {
            return HF::UID::UID(_uid);
         }

         HF::Transport::Layer const *transport() const
         {
            return tsp;
         }
      };

      template<class Parent>
      struct AbstractDevice: public Parent
      {
         std::vector<Protocol::Packet *> packets;

         Link                            link;

         AbstractDevice()
         {
            link.address(42);
         }

         virtual ~AbstractDevice()
         {
            /* *INDENT-OFF* */
            std::for_each (packets.begin (), packets.end (), [](Protocol::Packet *packet)
            {
               delete packet;
            });
            /* *INDENT-ON* */

            packets.clear();
         }

         void connected(HF::Transport::Link *link)
         {
            UNUSED(link);
            mock("AbstractDevice").actualCall("connected");
         }

         void disconnected(HF::Transport::Link *link)
         {
            UNUSED(link);
            mock("AbstractDevice").actualCall("disconnected");
         }

         void send(Protocol::Packet &packet)
         {
            mock("AbstractDevice").actualCall("send");

            if (packet.link == nullptr)
            {
               packet.link = &link;
            }

            Parent::send(packet);

            Protocol::Packet *temp = new Protocol::Packet(packet);

            packets.push_back(temp);
         }

         void receive(Protocol::Packet &packet, Common::ByteArray &payload, uint16_t offset)
         {
            mock("AbstractDevice").actualCall("receive");
            Parent::receive(packet, payload, offset);
         }

         uint16_t address() const
         {
            auto &call = mock("AbstractDevice").actualCall("address");

            if (call.hasReturnValue())
            {
               return call.returnUnsignedIntValue();
            }
            else
            {
               return Parent::address();
            }
         }
      };

#define SET_SERVICE(_X, _Y) \
   {                        \
      if (_X != nullptr)    \
      {                     \
         delete _X;         \
      }                     \
                            \
      _X = _Y;              \
   }

      class DeviceUnit0: public HF::Devices::Node::IUnit0
      {
         HF::Core::DeviceInformation::Server *dev_info;
         HF::Core::DeviceManagement::Client *dev_mgt;
         HF::Core::AttributeReporting::Server *attr_reporting;
         HF::Core::GroupTable::IServer *group_table_server;
         HF::Core::Time::Server *time_srv;
         HF::Core::BatchProgramManagement::IServer *batch_program_server;
         HF::Core::Scheduling::Event::IServer *event_scheduling_server;
         HF::Core::Scheduling::Weekly::IServer *weekly_scheduling_server;

         public:

         DeviceUnit0(HF::IDevice &device):
            HF::Devices::Node::IUnit0(device), dev_info(nullptr), dev_mgt(nullptr),
            attr_reporting(nullptr), group_table_server(nullptr), time_srv(nullptr),
            batch_program_server(nullptr), event_scheduling_server(nullptr),
            weekly_scheduling_server(nullptr)
         {}

         virtual ~DeviceUnit0()
         {
            delete dev_info;
            delete dev_mgt;
            delete attr_reporting;
            delete group_table_server;
            delete time_srv;
            delete batch_program_server;
            delete event_scheduling_server;
            delete weekly_scheduling_server;
         }

         void device_info(HF::Core::DeviceInformation::Server *_dev_info)
         {
            SET_SERVICE(dev_info, _dev_info);
         }

         HF::Core::DeviceInformation::Server *device_info()
         {
            if (dev_info == nullptr)
            {
               device_info(new HF::Core::DeviceInformation::Server(*this));
            }

            return dev_info;
         }

         HF::Core::DeviceInformation::Server *device_info() const
         {
            return dev_info;
         }

         void device_management(HF::Core::DeviceManagement::Client *_dev_mgt)
         {
            SET_SERVICE(dev_mgt, _dev_mgt);
         }

         HF::Core::DeviceManagement::Client *device_management()
         {
            if (dev_mgt == nullptr)
            {
               device_management(new HF::Core::DeviceManagement::Client(*this));
            }

            return dev_mgt;
         }

         HF::Core::DeviceManagement::Client *device_management() const
         {
            return dev_mgt;
         }

         void attribute_reporting(HF::Core::AttributeReporting::Server *_attr_reporting)
         {
            SET_SERVICE(attr_reporting, _attr_reporting);
         }

         HF::Core::AttributeReporting::Server *attribute_reporting()
         {
            if (attr_reporting == nullptr)
            {
               attribute_reporting(new HF::Core::AttributeReporting::Server(*this));
            }

            return attr_reporting;
         }

         HF::Core::AttributeReporting::Server *attribute_reporting() const
         {
            return attr_reporting;
         }

         void group_table(HF::Core::GroupTable::IServer *_group_table)
         {
            SET_SERVICE(group_table_server, _group_table);
         }

         HF::Core::GroupTable::IServer *group_table()
         {
            return group_table_server;
         }

         HF::Core::GroupTable::IServer *group_table() const
         {
            return group_table_server;
         }

         HF::Core::Time::Server *time()
         {
            return time_srv;
         }

         HF::Core::Time::Server *time() const
         {
            return time_srv;
         }

         void time(HF::Core::Time::Server *server)
         {
            time_srv = server;
         }

         HF::Core::BatchProgramManagement::IServer *batch_program()
         {
            return batch_program_server;
         }

         HF::Core::BatchProgramManagement::IServer *batch_program() const
         {
            return batch_program_server;
         }

         void batch_program(HF::Core::BatchProgramManagement::IServer *server)
         {
            batch_program_server = server;
         }

         HF::Core::Scheduling::Event::IServer *event_scheduling()
         {
            return event_scheduling_server;
         }

         HF::Core::Scheduling::Event::IServer *event_scheduling() const
         {
            return event_scheduling_server;
         }

         HF::Core::Scheduling::Weekly::IServer *weekly_scheduling()
         {
            return weekly_scheduling_server;
         }

         HF::Core::Scheduling::Weekly::IServer *weekly_scheduling() const
         {
            return weekly_scheduling_server;
         }

         Common::Result handle(HF::Protocol::Packet &packet,
                               Common::ByteArray &payload,
                               uint16_t offset)
         {
            UNUSED(packet);
            UNUSED(payload);
            UNUSED(offset);
            return Common::Result::FAIL_UNKNOWN;
         }

         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const
         {
            UNUSED(itf);
            UNUSED(pack_id);
            UNUSED(uids);
            return HF::Attributes::List();
         }
      };

      class ConcentratorUnit0: public HF::Devices::Concentrator::IUnit0
      {
         HF::Core::DeviceInformation::Server *dev_info;
         HF::Core::DeviceManagement::IServer *dev_mgt;
         HF::Core::AttributeReporting::Server *attr_reporting;
         HF::Core::GroupTable::IServer *group_tbl;
         HF::Core::GroupManagement::IServer *group_mgt;
         HF::Core::BindManagement::IServer *bind_mgt;
         HF::Core::Time::Server *time_srv;
         HF::Core::BatchProgramManagement::IServer *batch_program_server;
         HF::Core::Scheduling::Event::IServer *event_scheduling_server;
         HF::Core::Scheduling::Weekly::IServer *weekly_scheduling_server;

         public:

         ConcentratorUnit0(HF::IDevice &device):
            HF::Devices::Concentrator::IUnit0(device), dev_info(nullptr), dev_mgt(nullptr),
            attr_reporting(nullptr), group_tbl(nullptr), group_mgt(nullptr), bind_mgt(nullptr),
            time_srv(nullptr), batch_program_server(nullptr), event_scheduling_server(nullptr),
            weekly_scheduling_server(nullptr)
         {}

         virtual ~ConcentratorUnit0()
         {
            delete dev_info;
            delete dev_mgt;
            delete attr_reporting;
            delete group_tbl;
            delete group_mgt;
            delete bind_mgt;
            delete time_srv;
            delete batch_program_server;
            delete event_scheduling_server;
            delete weekly_scheduling_server;
         }

         // =============================================================================
         // API
         // =============================================================================

         void device_info(HF::Core::DeviceInformation::Server *_dev_info)
         {
            SET_SERVICE(dev_info, _dev_info);
         }

         HF::Core::DeviceInformation::Server *device_info() const override
         {
            return dev_info;
         }

         HF::Core::DeviceInformation::Server *device_info() override
         {
            if (dev_info == nullptr)
            {
               device_info(new HF::Core::DeviceInformation::Server(*this));
            }

            return dev_info;
         }

         void device_management(HF::Core::DeviceManagement::IServer *_dev_mgt)
         {
            SET_SERVICE(dev_mgt, _dev_mgt);
         }

         HF::Core::DeviceManagement::IServer *device_management() override
         {
            if (dev_mgt == nullptr)
            {
               device_management(new HF::Core::DeviceManagement::DefaultServer(*this));
            }

            return dev_mgt;
         }

         HF::Core::DeviceManagement::IServer *device_management() const override
         {
            return dev_mgt;
         }

         void attribute_reporting(HF::Core::AttributeReporting::Server *_attr_reporting)
         {
            SET_SERVICE(attr_reporting, _attr_reporting);
         }

         HF::Core::AttributeReporting::Server *attribute_reporting() const override
         {
            return attr_reporting;
         }

         HF::Core::AttributeReporting::Server *attribute_reporting() override
         {
            if (attr_reporting == nullptr)
            {
               attribute_reporting(new HF::Core::AttributeReporting::Server(*this));
            }

            return attr_reporting;
         }

         void bind_management(HF::Core::BindManagement::IServer *_bind_mgt)
         {
            SET_SERVICE(bind_mgt, _bind_mgt);
         }

         HF::Core::BindManagement::IServer *bind_management() override
         {
            if (bind_mgt == nullptr)
            {
               bind_management(new HF::Core::BindManagement::DefaultServer(*this));
            }

            return bind_mgt;
         }

         HF::Core::BindManagement::IServer *bind_management() const override
         {
            return bind_mgt;
         }

         void group_management(HF::Core::GroupManagement::IServer *_group_mgt)
         {
            SET_SERVICE(group_mgt, _group_mgt);
         }

         HF::Core::GroupManagement::IServer *group_management() override
         {
            if (group_mgt == nullptr)
            {
               group_management(new HF::Core::GroupManagement::DefaultServer(*this));
            }

            return group_mgt;
         }

         HF::Core::GroupManagement::IServer *group_management() const override
         {
            return group_mgt;
         }

         void group_table(HF::Core::GroupTable::IServer *_group_tbl)
         {
            SET_SERVICE(group_tbl, _group_tbl);
         }

         HF::Core::GroupTable::IServer *group_table() override
         {
            if (group_tbl == nullptr)
            {
               group_table(new HF::Core::GroupTable::DefaultServer(*this));
            }

            return group_tbl;
         }

         HF::Core::GroupTable::IServer *group_table() const override
         {
            return group_tbl;
         }

         HF::Core::Time::Server *time() override
         {
            return time_srv;
         }

         HF::Core::Time::Server *time() const override
         {
            return time_srv;
         }

         HF::Core::BatchProgramManagement::IServer *batch_program() override
         {
            return batch_program_server;
         }

         HF::Core::BatchProgramManagement::IServer *batch_program() const override
         {
            return batch_program_server;
         }

         HF::Core::Scheduling::Event::IServer *event_scheduling() override
         {
            return event_scheduling_server;
         }

         HF::Core::Scheduling::Event::IServer *event_scheduling() const override
         {
            return event_scheduling_server;
         }

         HF::Core::Scheduling::Weekly::IServer *weekly_scheduling() override
         {
            return weekly_scheduling_server;
         }

         HF::Core::Scheduling::Weekly::IServer *weekly_scheduling() const override
         {
            return weekly_scheduling_server;
         }

         Common::Result handle(HF::Protocol::Packet &packet,
                               Common::ByteArray &payload,
                               uint16_t offset) override
         {
            switch (packet.message.itf.id)
            {
               case HF::Interface::DEVICE_INFORMATION:
               {
                  return device_info()->handle(packet, payload, offset);
               }
               case HF::Interface::ATTRIBUTE_REPORTING:
               {
                  return attribute_reporting()->handle(packet, payload, offset);
               }
               case HF::Interface::DEVICE_MANAGEMENT:
               {
                  return device_management()->handle(packet, payload, offset);
               }
               case HF::Interface::BIND_MANAGEMENT:
               {
                  return bind_management()->handle(packet, payload, offset);
               }
               case HF::Interface::GROUP_TABLE:
               {
                  return group_table()->handle(packet, payload, offset);
               }
               case HF::Interface::TIME:
               {
                  return time()->handle(packet, payload, offset);
               }
               case HF::Interface::BATCH_PROGRAM_MANAGEMENT:
               {
                  return batch_program()->handle(packet, payload, offset);
               }
               case HF::Interface::EVENT_SCHEDULING:
               {
                  return event_scheduling()->handle(packet, payload, offset);
               }
               case HF::Interface::WEEKLY_SCHEDULING:
               {
                  return weekly_scheduling()->handle(packet, payload, offset);
               }
               default:
                  return Common::Result::FAIL_UNKNOWN;
            }
         }

         HF::Attributes::List attributes(Common::Interface itf, uint8_t pack_id,
                                         const HF::Attributes::UIDS &uids) const override
         {
            UNUSED(itf);
            UNUSED(pack_id);
            UNUSED(uids);
            return HF::Attributes::List();
         }
      };

      typedef AbstractDevice<HF::Devices::Node::Abstract<DeviceUnit0>> Device;

      typedef AbstractDevice<HF::Devices::Concentrator::Abstract<ConcentratorUnit0>> Concentrator;
      template<typename T>
      T generate_random(T start = std::numeric_limits<T>::min,
                        T end = std::numeric_limits<T>::max)
      {
         std::random_device rd;
         std::mt19937 generator(rd());
         std::uniform_int_distribution<T> distribution(start, end);

         return static_cast<T>(distribution(generator));
      }

      namespace DevMgt
      {
         using DevicePtr = HF::Core::DeviceManagement::DevicePtr;
         using UnitPtr   = HF::Core::DeviceManagement::UnitPtr;

         DevicePtr create_device(Concentrator &base, uint16_t _dev_addr);

         UnitPtr add_unit(DevicePtr &device, uint8_t unit_id, uint16_t profile);

         UnitPtr add_unit0(DevicePtr &device);

         void fill_unit(UnitPtr unit, Interface::UID uid, Interface::Role role);

      }  // namespace DevMgt

   } // namespace Testing

} // namespace HF

#endif /* HF_TEST_HELPER_H */
