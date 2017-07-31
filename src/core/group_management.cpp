// =============================================================================
/*!
 * @file       src/core/group_management.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Group Management service.
 *
 * @version    x.x.x
 *
 * @copyright  Copyright &copy; &nbsp; 2017 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/group_management.h"

// =============================================================================
// API
// =============================================================================

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::GroupManagement;

// =============================================================================
// Core::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *Core::create_attribute(IServer *server, uint8_t uid)
{
   if (server != nullptr)
   {
      return server->attribute(uid);
   }
   else
   {
      return GroupManagement::create_attribute(uid);
   }
}

// =============================================================================
// GroupManagement::create_attribute
// =============================================================================
/*!
 *
 */
// =============================================================================
HF::Attributes::IAttribute *GroupManagement::create_attribute(uint8_t uid)
{
   using namespace HF::Core::GroupManagement;

   GroupManagement::Attributes attr = static_cast<GroupManagement::Attributes>(uid);

   switch (attr)
   {
      case NUMBER_OF_GROUPS_ATTR:
      {
         return new HF::Attributes::Attribute<uint8_t>(HF::Interface::GROUP_MANAGEMENT, attr,
                                                       NumberOfGroups::WRITABLE);
      }

      default:
         return nullptr;
   }
}

// =============================================================================
// GroupManagement::GroupAddress
// =============================================================================

// =============================================================================
// GroupAddress::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t GroupAddress::size() const
{
   return min_size;
}

// =============================================================================
// GroupAddress::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t GroupAddress::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   /* *INDENT-OFF* */
   HF_ASSERT(START_ADDR <= address && address <= END_ADDR, {return 0;});
   /* *INDENT-ON* */

   array.write(offset, address);

   return min_size;
}

// =============================================================================
// GroupAddress::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t GroupAddress::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   array.read(offset, address);

   return min_size;
}

// =============================================================================
// GroupManagement::Group
// =============================================================================

// =============================================================================
// Group::find_member
// =============================================================================
/*!
 *
 */
// =============================================================================
Group::Container::iterator Group::find_member(const Member &member)
{
   /* *INDENT-OFF* */
   auto it = std::find_if(_members.begin(), _members.end(), [&member](const Member &m)
   {
      return member == m;
   });
   /* *INDENT-ON* */

   return it;
}

// =============================================================================
// Group::add
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Group::add(const Member &member)
{
   HF_ASSERT(members().size() <= MAX_MEMBERS, {return false;});

   auto it = find_member(member);

   if (it == _members.end())
   {
      _members.push_back(member);
      return true;
   }

   return false;
}

// =============================================================================
// Group::update
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Group::update(const Member &member)
{
   auto it = find_member(Protocol::BROADCAST_ADDR, Protocol::BROADCAST_UNIT);

   if(it == _members.end())
   {
      return false;
   }
   else
   {
      *it = member;
      return true;
   }
}

// =============================================================================
// Group::remove
// =============================================================================
/*!
 *
 */
// =============================================================================
bool Group::remove(const Member &member)
{
   auto it = find_member(member);

   if (it != _members.end())
   {
      _members.erase(it);
      return true;
   }

   return false;
}

// =============================================================================
// Group::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Group::size() const
{
   uint16_t result = min_size;

   result += name.length();
   return result;
}

// =============================================================================
// Group::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Group::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   /* *INDENT-OFF* */
   HF_ASSERT(GroupAddress::START_ADDR <= address && address <= GroupAddress::END_ADDR,
             {return 0;});
   /* *INDENT-ON* */

   uint16_t start = offset;
   uint16_t size;

   size = GroupAddress::pack(array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size > 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   size    = HF::Common::SerializableHelper<std::string>::pack(name, array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size > 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Member>, uint8_t> helper(
      const_cast<Container &>(_members));
   offset += helper.pack(array, offset);

   return offset - start;
}

// =============================================================================
// Group::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Group::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;
   uint16_t size;

   offset += GroupAddress::unpack(array, offset);

   size    = HF::Common::SerializableHelper<std::string>::unpack(name, array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   HF::Common::SerializableHelperVector<Container, uint8_t> helper(_members);
   size += helper.unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   return offset - start;
}

// =============================================================================
// Messages
// =============================================================================

// =============================================================================
// Create Group
// =============================================================================

// =============================================================================
// GroupManagement::CreateMessage
// =============================================================================

// =============================================================================
// CreateMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t CreateMessage::size() const
{
   return (name.length() + min_size);
}

// =============================================================================
// CreateMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t CreateMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += HF::Common::SerializableHelper<std::string>::pack(name, array, offset);

   return (offset - start);
}

// =============================================================================
// CreateMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t CreateMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = HF::Common::SerializableHelper<std::string>::unpack(name, array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   return (offset - start);
}

// =============================================================================
// GroupManagement::CreateResponse
// =============================================================================

// =============================================================================
// CreateResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t CreateResponse::size() const
{
   if (code != Common::Result::OK)
   {
      return min_size;
   }

   return (Response::size() + GroupAddress::size());
}

// =============================================================================
// CreateResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t CreateResponse::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint8_t size;

   offset += Response::pack(array, offset);

   if (this->code != Common::Result::OK)
   {
      return min_size;
   }

   size = GroupAddress::pack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   return offset - start;
}

// =============================================================================
// CreateResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t CreateResponse::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;

   offset       += Response::unpack(array, offset);

   this->address = GroupAddress::NO_ADDR;

   if (this->code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, sizeof(uint16_t));

   offset += GroupAddress::unpack(array, offset);

   _end:
   return offset - start;
}

// =============================================================================
// Add to Group
// =============================================================================

// =============================================================================
// GroupManagement::AddMessage
// =============================================================================

// =============================================================================
// AddMessage::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Message::size() const
{
   return (GroupAddress::size() + Protocol::Address::size());
}

// =============================================================================
// AddMessage::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Message::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   /* *INDENT-OFF* */
   HF_ASSERT(device != 0, {return 0;});
   /* *INDENT-ON* */

   uint16_t start = offset;

   offset        += GroupAddress::pack(array, offset);

   offset        += Protocol::Address::pack(array, offset);

   return (offset - start);
}

// =============================================================================
// AddMessage::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Message::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = GroupAddress::unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   size    = Protocol::Address::unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   return (offset - start);
}

// =============================================================================
// Group Information
// =============================================================================

// =============================================================================
// GroupManagement::InfoResponse
// =============================================================================

// =============================================================================
// InfoResponse::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t InfoResponse::size() const
{
   if (code != Common::Result::OK)
   {
      return min_size;
   }

   HF::Common::SerializableHelperVector<std::vector<Member>, uint16_t> helper(
      const_cast<std::vector<Member> &>(members));

   return (min_size + HF::Common::SerializableHelper<std::string>::size(name) + helper.size());
}

// =============================================================================
// InfoResponse::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t InfoResponse::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += Response::pack(array, offset);

   if (this->code != Common::Result::OK)
   {
      return min_size;
   }

   offset += HF::Common::SerializableHelper<std::string>::pack(name, array, offset);

   HF::Common::SerializableHelperVector<std::vector<Member>, uint16_t> helper(
      const_cast<std::vector<Member> &>(members));

   offset += helper.pack(array, offset);

   return offset - start;
}

// =============================================================================
// InfoResponse::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t InfoResponse::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;
   uint16_t size;

   size = Response::unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Member>, uint16_t> helper(members);

   if (this->code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, sizeof(uint16_t));

   size = HF::Common::SerializableHelper<std::string>::unpack(name, array, offset);
   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   size    = helper.unpack(array, offset);

   /* *INDENT-OFF* */
   HF_ASSERT(size != 0, {return 0;});
   /* *INDENT-ON* */

   offset += size;

   _end:
   return offset - start;
}

// =============================================================================
// GroupManagement::Entries
// =============================================================================

// =============================================================================
// Entries::size
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entries::size() const
{
   return db.size();
}

// =============================================================================
// Entries::save
// =============================================================================
/*!
 */
// =============================================================================
Common::Result Entries::save(const Group &entry)
{
   db.insert(db.end(), std::pair<uint16_t, Group>(entry.address, entry));

   return Common::Result::OK;
}

// =============================================================================
// Entries::save
// =============================================================================
/*!
 */
// =============================================================================
Common::Result Entries::save(uint16_t address, const std::string &name)
{
   db.insert(db.end(), std::pair<uint16_t, Group>(address, Group(address,name)));

   return Common::Result::OK;
}

// =============================================================================
// Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::destroy(const uint16_t &address)
{
   auto count = db.erase(address);

   if (count != 1)
   {
      return Common::Result::FAIL_ARG;
   }
   else
   {
      return Common::Result::OK;
   }
}

// =============================================================================
// Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::destroy(const Group &group)
{
   return destroy(group.address);
}

// =============================================================================
// Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
GroupPtr Entries::find(uint16_t address) const
{
   auto it = db.find(address);

   if (it == db.end())
   {
      return GroupPtr();
   }
   else
   {
      return GroupPtr(const_cast<Group *>(&(*it).second));
   }
}

// =============================================================================
// Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
GroupPtr Entries::find(const std::string &name) const
{
   /* *INDENT-OFF* */
   auto it = std::find_if(db.begin(), db.end(), [&name](const std::pair< const uint16_t, Group> &device)
   {
      return device.second.name == name;
   });
   /* *INDENT-ON* */

   if (it == db.end())
   {
      return GroupPtr();
   }
   else
   {
      return GroupPtr(const_cast<Group *>(&((it->second))));
   }

   return GroupPtr();
}

// =============================================================================
// Entries::next_address
// =============================================================================
/*!
 *
 */
// =============================================================================
uint16_t Entries::next_address() const
{
   uint16_t address = 0;

   for (address = GroupAddress::START_ADDR; address <= GroupAddress::END_ADDR; ++address)
   {
      if (db.find(address) == db.end())
      {
         return address;
      }
   }

   return GroupAddress::END_ADDR;
}
