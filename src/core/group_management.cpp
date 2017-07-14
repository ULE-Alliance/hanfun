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


uint16_t GroupAddress::size() const
{
   return min_size;
}

uint16_t GroupAddress::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   HF_ASSERT(START_ADDR <= address && address <= END_ADDR,return 0;);

   array.write(offset, address);

   return min_size;
}

uint16_t GroupAddress::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   array.read(offset,address);

   return min_size;
}




uint16_t Group::size() const
{
   uint16_t result = min_size;
   result += name.length();
   return result;
}

uint16_t Group::pack (Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());
   HF_ASSERT(GroupAddress::START_ADDR <= address && address <= GroupAddress::END_ADDR, return 0;);

   uint16_t start = offset;
   uint16_t size;

   size = GroupAddress::pack(array, offset);
   HF_ASSERT(size > 0, return 0;);
   offset += size;

   size = HF::Common::SerializableHelper<std::string>::pack(name, array, offset);
   HF_ASSERT(size > 0, return 0;);
   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Member>, uint8_t> helper(const_cast<std::vector<Member> &>(members));
   offset += helper.pack(array, offset);

   return offset - start;
}

uint16_t Group::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;
   uint16_t size;

   offset  += GroupAddress::unpack(array, offset);

   size = HF::Common::SerializableHelper<std::string>::unpack(name, array, offset);

   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Member>, uint8_t> helper(members);
   size += helper.unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return offset - start;
}


/*====================================================================================
 * Messages
 *====================================================================================*/

/*==== Create Group ====*/

uint16_t CreateMessage::size() const
{
   return (name.length() + min_size);
}

uint16_t CreateMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += HF::Common::SerializableHelper<std::string>::pack(name, array, offset);

   return (offset - start);
}

uint16_t CreateMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = HF::Common::SerializableHelper<std::string>::unpack(name, array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}


/*==== Create Group Response ====*/

uint16_t CreateResponse::size() const
{
   if(code != Common::Result::OK)
   {
      return min_size;
   }

   return (Response::size() + GroupAddress::size());
}

uint16_t CreateResponse::pack (Common::ByteArray &array, uint16_t offset) const
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
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return offset - start;
}

uint16_t CreateResponse::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);

   uint16_t start = offset;

   offset += Response::unpack(array, offset);

   if (this->code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, sizeof(uint16_t));

   offset += GroupAddress::unpack(array, offset);

   _end:
   return offset - start;
}


/*==== Add to Group ====*/

uint16_t AddMessage::size() const
{
   return (GroupAddress::size()+Protocol::Address::size());
}

uint16_t AddMessage::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   HF_ASSERT(device != 0, {return 0;});

   uint16_t start = offset;

   offset += GroupAddress::pack(array, offset);

   offset += Protocol::Address::pack(array, offset);

   return (offset - start);
}

uint16_t AddMessage::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;
   uint16_t size;

   size = GroupAddress::unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   size = Protocol::Address::unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   return (offset - start);
}



/*==== Group Info Response ====*/

uint16_t InfoResponse::size() const
{
   if (code != Common::Result::OK)
   {
      return min_size;
   }

   HF::Common::SerializableHelperVector<std::vector<Member>, uint16_t> helper(const_cast<std::vector<Member> &>(members));

   return (min_size + HF::Common::SerializableHelper<std::string>::size(name) + helper.size());
}

uint16_t InfoResponse::pack(Common::ByteArray &array, uint16_t offset) const
{
   HF_SERIALIZABLE_CHECK(array, offset, size());

   uint16_t start = offset;

   offset += Response::pack(array, offset);

   if (this->code != Common::Result::OK)
   {
      return min_size;
   }

   offset += HF::Common::SerializableHelper<std::string>::pack(name,array,offset);

   HF::Common::SerializableHelperVector<std::vector<Member>, uint16_t> helper(const_cast<std::vector<Member> &>(members));

   offset += helper.pack(array, offset);

   return offset - start;
}

uint16_t InfoResponse::unpack(const Common::ByteArray &array, uint16_t offset)
{
   HF_SERIALIZABLE_CHECK(array, offset, min_size);


   uint16_t start = offset;
   uint16_t size;

   size = Response::unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   HF::Common::SerializableHelperVector<std::vector<Member>, uint16_t> helper(members);

   if (this->code != Common::Result::OK)
   {
      goto _end;
   }

   HF_SERIALIZABLE_CHECK(array, offset, sizeof(uint16_t));

   size = HF::Common::SerializableHelper<std::string>::unpack(name,array,offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;


   size = helper.unpack(array, offset);
   HF_ASSERT(size != 0, {return 0;});
   offset += size;

   _end:
   return offset - start;
}
