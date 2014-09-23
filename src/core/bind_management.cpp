// =============================================================================
/*!
 * \file       src/core/bind_management.cpp
 *
 * This file contains the implementation of the common functionality for the
 * Bind Management core interface.
 *
 * \version    1.0.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 ULE Alliance
 *
 * For licensing information, please see the file 'LICENSE' in the root folder.
 *
 * Initial development by Bithium S.A. [http://www.bithium.com]
 */
// =============================================================================

#include "hanfun/core/bind_management.h"

#include "hanfun/interface.h"

#include <set>

using namespace HF;
using namespace HF::Core;
using namespace HF::Core::BindManagement;

// =============================================================================
// BindManagement::Entry
// =============================================================================

// =============================================================================
// BindManagement::Entry::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::size () const
{
   return source.size () + destination.size () + sizeof(uint16_t);
}

// =============================================================================
// BindManagement::Entry::pack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::pack (Common::ByteArray &array, size_t offset) const
{
   size_t start = offset;

   offset += this->source.pack (array, offset);

   offset += this->itf.pack (array, offset);

   offset += this->destination.pack (array, offset);

   return offset - start;
}

// =============================================================================
// BindManagement::Entry::unpack
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t Entry::unpack (const Common::ByteArray &array, size_t offset)
{
   size_t start = offset;

   offset += this->source.unpack (array, offset);

   offset += this->itf.unpack (array, offset);

   offset += this->destination.unpack (array, offset);

   return offset - start;
}

// =============================================================================
// BindManagement::Entries
// =============================================================================

// =============================================================================
// BindManagement::Entries::create
// =============================================================================
/*!
 *
 */
// =============================================================================
std::pair <Common::Result, const Entry *> Entries::create (Protocol::Address const &source,
                                                           Common::Interface const &itf,
                                                           Protocol::Address const &destination)
{
   auto res              = this->db.insert (Entry (source, itf, destination));

   Common::Result result = Common::Result::FAIL_ARG;

   if (res.second)
   {
      result = Common::Result::OK;
   }

   return std::make_pair (result, res.second ? &(*(res.first)) : nullptr);
}

// =============================================================================
// BindManagement::Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::destroy (Entry const &entry)
{
   if (this->db.erase (entry) == 1)
   {
      return Common::Result::OK;
   }

   return Common::Result::FAIL_ARG;
}

// =============================================================================
// BindManagement::Entries::destroy
// =============================================================================
/*!
 *
 */
// =============================================================================
Common::Result Entries::destroy (uint16_t address, Protocol::Address::Type type)
{
   return destroy ([address, type](BindManagement::Entry const &entry) {
                      if ((entry.source.device == address && entry.source.mod == type) ||
                          (entry.destination.device == address && entry.destination.mod == type))
                      {
                         return true;
                      }
                      else
                      {
                         return false;
                      }
                   }
                  );
}

// =============================================================================
// BindManagement::Entries::size
// =============================================================================
/*!
 *
 */
// =============================================================================
size_t BindManagement::Entries::size () const
{
   return this->db.size ();
}

// =============================================================================
// BindManagement::Entries - Query API
// =============================================================================

Entries::Iterator Entries::begin () const
{
   return this->db.begin ();
}

Entries::Iterator Entries::end () const
{
   return this->db.end ();
}

std::pair <Entries::Iterator, Entries::Iterator> Entries::find (Protocol::Address const &source,
                                                                Common::Interface const &itf) const
{
   Entry _begin (source, itf);
   Entry _end (source, itf);

   memset (&(_begin.destination), 0x00, sizeof(Protocol::Address));
   memset (&(_end.destination), 0xFF, sizeof(Protocol::Address));

   return std::make_pair (this->db.lower_bound (_begin), this->db.upper_bound (_end));
}

// =============================================================================
// BindManagement::Entries::find
// =============================================================================
/*!
 *
 */
// =============================================================================
const Entry *Entries::find (const Protocol::Address &source, const Common::Interface &itf,
                            const Protocol::Address &destination)
{
   Entry *temp = new Entry (source, itf, destination);

   auto  it    = this->db.find (*temp);

   delete temp;

   if (it == this->db.end ())
   {
      return nullptr;
   }
   else
   {
      return &(*it);
   }
}
