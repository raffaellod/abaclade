﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010, 2011, 2012, 2013
Raffaello D. Di Napoli

This file is part of Application-Building Components (henceforth referred to as ABC).

ABC is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

ABC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License along with ABC. If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------*/

#ifndef ABC__VEXTR_HXX
#define ABC__VEXTR_HXX

#include <abc/core.hxx>
#ifdef ABC_CXX_PRAGMA_ONCE
   #pragma once
#endif

#include <abc/numeric.hxx>
#include <abc/memory.hxx>
#include <abc/type_void_adapter.hxx>



/** DOC:4019 abc::*str_ and abc::*vector design

*str_ and *vector are implemented using the same base set of classes:

•  _raw_vextr_impl_base, core functionality for a vector of items: a little code and all member
   variables; this is then extended by three implementation classes:

   •  _raw_complex_vextr_impl, implementation of a vector of objects of non-trivial class: this is
      fully transactional and therefore exception-proof, but it's of course slower and uses more
      memory even during simpler operations;

   •  _raw_trivial_vextr_impl, implementation of a vector of plain values (instances of trivial
      class or native type): this is a near-optimal solution, still exception-proof but also taking
      advantage of the knowledge that no copy constructors need to be called; this class also
      supports the presence of a last element of value 0, opening up for the implementation of a
      string-like vector:

      •  _raw_str, implementation of a string: mostly based on _raw_trivial_vector_impl, it also
         provides means for clients of str_ to avoid having to be templates themselves, by giving
         access to type-deleted (void *) methods.

A vector/string using a static item array is nearly as fast as the C-style direct manipulation of an
array, only wasting a very small amount of space, and providing the ability to switch to a
dynamically-allocated item array on-the-fly in case the client needs to store in it more items than
are available.

Note: vextr is a silly portmanteau of vector and str(ing), because most of the above classes are
used by both.


Underlying data storage:

1. istr() or dmstr()
   ┌───┬───┬───────┐
   │ p │ 1 │ 0|f|f │
   └───┴───┴───────┘
     │                 ┌────┐
     ╰────────────────▶│ \0 │               Read-only memory (NUL)
                       └────┘

2. smstr<5>()
   ┌───┬───┬───────╥───┬─────────┐
   │ p │ 1 │ 0|f|t ║ 4 │ - - - - │          Static (can be stack-allocated) fixed-size buffer
   └───┴───┴───────╨───┴─────────┘
     │                 ┌────┐
     └────────────────▶│ \0 │               Read-only memory (NUL)
                       └────┘

3. istr("abc"):
   ┌───┬───┬───────┐
   │ p │ 4 │ 0|f|f │
   └───┴───┴───────┘
     │                 ┌──────────┐
     └────────────────▶│ a b c \0 │         Read-only memory
                       └──────────┘

4. dmstr("abc")
   ┌───┬───┬───────┐
   │ p │ 4 │ 8|t|f │
   └───┴───┴───────┘
     │                 ┌──────────────────┐
     └────────────────▶│ a b c \0 - - - - │ Dynamically-allocated variable-size buffer
                       └──────────────────┘

5. smstr<3>("abc")
   ┌───┬───┬───────╥───┬──────────┐
   │ p │ 4 │ 4|f|t ║ 4 │ a b c \0 │         Static (can be stack-allocated) fixed-size buffer
   └───┴───┴───────╨───┴──────────┘
     │                 ▲
     └─────────────────┘

6. smstr<2>("abc"):
   ┌───┬───┬───────╥───┬───────┐
   │ p │ 4 │ 8|t|t ║ 3 │ - - - │            Static (can be stack-allocated) fixed-size buffer
   └───┴───┴───────╨───┴───────┘
     │                 ┌──────────────────┐
     └────────────────▶│ a b c \0 - - - - │ Dynamically-allocated variable-size buffer
                       └──────────────────┘


String types:

   istr (immutable string)
      Item array can be read-only (and shared) or dynamic.
   smstr (statically- or dynamically-allocated mutable string)
      Item array cannot be read-only nor shared, but it can be static or dynamic.
   dmstr (dynamically-allocated mutable string)
      Item array cannot be read-only, nor shared, nor static - always dynamic and writable.


Argument usage scenarios:

   istr           g_is;
   istr const     gc_is;
   dmstr          g_dms;
   dmstr const    gc_dms;
   smstr<n>       g_sms;
   smstr<n> const gc_sms;
   mstr           g_ms;
   mstr const     gc_ms;


•  No need to modify:

   void f1(istr const & isArg) {
      // N/A - const.
      isArg += "abc";

      // Share a read-only item array, or copy it: istr::operator=(istr const &)
      // Use assign_share_ro_or_copy().
      g_is = isArg;

      // TODO: validate these!
      // 1. Copy-construct: istr::istr(istr const &)
      //    Use assign_share_ro_or_copy(): will share a read-only item array, but will copy anything
      //    else. It's a copy - it's expected to have a separate life.
      // 2. Move-assign from the copy: istr::operator=(istr &&) (“nothrow”)
      //    Use assign_move().
      g_is = std::move(isArg);
      // 3. Destruct the now-empty copy: istr::~istr()

      // Copy the item array: mstr::operator=(istr const &)
      // Use assign_copy().
      g_ms = isArg;

      // TODO: validate these!
      // 1. Same as 1. above.
      // 2. Move-assign from the copy: dmstr::operator=(istr &&) (can throw)
      //    Use assign_move_dynamic_or_move_items(): will move a dynamic item array or move its
      //    items.
      g_ms = std::move(isArg);
      // 3. Same as 3. above.

      // Copy the item array: dmstr::operator=(istr const &)
      // Use assign_copy().
      g_dms = isArg;

      // TODO: validate these!
      // 1. Same as 1. above.
      // 2. Move-assign from the copy: dmstr::operator=(istr &&) (can throw)
      //    Use assign_move_dynamic_or_move_items(): will move a dynamic item array or move its
      //    items.
      g_dms = std::move(isArg);
      // 3. Same as 3. above.

      // Copy the item array: smstr<n>::operator=(istr const &)
      // Use assign_copy().
      g_sms = isArg;

      // TODO: validate these!
      // 1. Same as 1. above.
      // 2. Move-assign from the copy: smstr<n>::operator=(istr &&) (can throw)
      //    See considerations for 2. above.
      g_sms = std::move(isArg);
      // 3. Same as 3. above.
   }

   // 1. Construct a temporary object: istr::istr(char (& ach)[t_cch])
   f1("abc");
   // 2. Destruct the temporary object: istr::~istr()

   // Pass by const &.
   f1(g_is);
   f1(gc_is);

   // Invoke mstr::operator istr const &() const. Given that it's a REFERENCE, it's fine if the
   // source goes away and you get a crash: it's like freeing a pointer after passing it around.
   f1(g_ms);
   f1(gc_ms);

   // Invoke dmstr::operator istr const &() const. See considerations above.
   f1(g_dms);
   f1(gc_dms);

   // Invoke smstr<n>::operator istr const &() const. See considerations above.
   f1(g_sms);
   f1(gc_sms);


•  Writable dynamic string:

   void f2(dmstr * pdmsArg) {
      // Modify the buffer, maybe changing it for size reasons.
      *pdmsArg += "abc";

      // Copy the item array: istr::operator=(dmstr const &)
      // Use assign_copy(). Can never share, because dmstr never uses a read-only buffer.
      g_is = *pdmsArg;

      // Move the item array: istr::operator=(dmstr &&) (“nothrow”)
      // Use assign_move(). “nothrow” because dmstr cannot be a smstr<n> under covers.
      g_is = std::move(*pdmsArg);

      // Copy the item array: mstr::operator=(dmstr const &)
      // Use assign_copy().
      g_ms = *pdmsArg;

      // Move the item array: mstr::operator=(dmstr &&) (“nothrow”)
      // Use assign_move(). “nothrow” because dmstr cannot be a smstr<n> under covers.
      g_ms = std::move(*pdmsArg);

      // Copy the item array: dmstr::operator=(dmstr const &)
      // Use assign_copy().
      g_dms = *pdmsArg;

      // Move the item array: dmstr::operator=(dmstr &&) (“nothrow”)
      // Use assign_move(). “nothrow” because mdstr cannot be a smstr<n> under covers.
      g_dms = std::move(*pdmsArg);

      // Copy the item array: smstr<n>::operator=(dmstr const &)
      // Use assign_copy().
      g_sms = *pdmsArg;

      // Move the item array: smstr<n>::operator=(dmstr &&) (“nothrow”)
      // Use assign_move(). “nothrow” because dmstr cannot be a smstr<n> under covers.
      g_sms = std::move(*pdmsArg);
   }

   // N/A - no such conversion.
   f2("abc");
   f2(&g_is);
   f2(&gc_is);

   // N/A - no such conversion. This must be the case, otherwise the “nothrow” conditions described
   // above cannot be guaranteed.
   f2(&g_ms);
   f2(&gc_ms);

   // Pass by &.
   f2(&g_dms);

   // N/A - const.
   f2(&gc_dms);

   // N/A - no such conversion. This must be the case, otherwise the “nothrow” conditions described
   // above cannot be guaranteed.
   f2(&g_sms);
   f2(&gc_sms);


•  Writable (static or dynamic) string:

   void f3(mstr * pmsArg) {
      // Modify the buffer, maybe changing it for size reasons.
      *pmsArg += "abc";

      // Copy the item array: istr::operator=(mstr const &)
      // Use assign_copy(): can never share, because mstr never uses a read-only buffer.
      g_is = *pmsArg;

      // Move the item array: istr::operator=(mstr &&) (can throw)
      // Use assign_move_dynamic_or_move_items(). can throw because mstr can be a smstr<n> under the
      // covers!
      g_is = std::move(*pmsArg);

      // Copy the item array: mstr::operator=(mstr const &)
      // Use assign_copy().
      g_ms = *pmsArg;

      // Move the item array: mstr::operator=(mstr &&) (can throw)
      // Use assign_move_dynamic_or_move_items(). See considerations above.
      // WARNING - this class has a throwing move constructor/assignment operator!
      g_ms = std::move(*pmsArg);

      // Copy the item array: dmstr::operator=(mstr const &)
      // Use assign_copy().
      g_dms = *pmsArg;

      // Move the item array: dmstr::operator=(mstr &&) (“nothrow”)
      // Use assign_move(). Can throw because mstr can be a smstr<n> under covers!
      g_dms = std::move(*pmsArg);

      // Copy the item array: smstr<n>::operator=(mstr const &)
      // Use assign_copy().
      g_sms = *pmsArg;

      // Move the item array: smstr<n>::operator=(mstr &&) (can throw)
      // Use assign_move_dynamic_or_move_items(): will move a dynamic item array or move its items.
      // else (like assign_copy()).
      g_sms = std::move(*pmsArg);
   }

   // N/A - no such conversion.
   f3("abc");
   f3(&g_is);
   f3(&gc_is);

   // Pass by &.
   f3(&g_ms);

   // N/A - const.
   f3(&gc_ms);

   // Down-cast to mstr &.
   f3(&g_dms);

   // N/A - const.
   f3(&gc_dms);

   // Down-cast to mstr &.
   f3(&g_sms);

   // N/A - const.
   f3(&gc_sms);


From the above, it emerges that:

•  mstr and smstr<n> cannot publicly derive from istr or dmstr, because that would enable automatic
   down-cast to i/dmstr &, which would then expose to the i/dmstr move constructor/assignment
   operator being invoked to move a static item array, which is wrong, or (if attempting to work
   around the move) would result in the static item array being copied, which would violate the
   “nothrow” requirement for the move constructor/assignment operator.

•  dmstr can publicly derive from mstr, with mstr being a base class for both dmstr and smstr<n>.

•  The only differences between istr and istr const & are:
   1. istr const & can be using a static item array (because it can be a smstr<n>), while any other
      istr will always use a const/read-only item array or a dynamic one;
   2. other string types can only be automatically converted to istr const &.

•  The difference between istr and mstr (and therefore dmstr/smstr<n>) is that the former can be
   constructed from a static string without copying it, but only offers read-only methods and
   operators; the latter offers the whole range of features one would expect, but will create a new
   item array upon construction or assignment (or use the embedded static one, in case of smstr<n>).

•  mstr cannot have a “nothrow” move constructor or assignment operator from itself, because the
   underlying objects might have static item arrays of different sizes. This isn't a huge deal-
   breaker because of the intended limited usage for mstr and smstr<n>.

The resulting class hierarchy is therefore:

   str_base (near-complete implementation of istr)
      istr
      mstr (near-complete implementation of dmstr/smstr<n>)
         dmstr
         smstr<n>

             ┌─────────────────────────────────────────────────────────┐
             │                     Functional need                     │
┌────────────┼──────────────┬─────────────────┬──────────┬─────────────┤
│            │ Local/member │ Method/function │ Writable │  Constant   │
│ Class      │ variable     │ argument        │          │ (read-only) │
├────────────┼──────────────┼─────────────────┼──────────┼─────────────┤
│ istr const │       x      │    x (const &)  │          │      x      │
│ mstr       │              │      x (*)      │     x    │             │
│ dmstr      │       x      │                 │     x    │             │
│ smstr      │       x      │                 │     x    │             │
└────────────┴──────────────┴─────────────────┴──────────┴─────────────┘
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_raw_vextr_packed_data

namespace abc {

// Note: getters and setters in this class don’t follow the regular naming convention used
// everywhere else, to underline the fact this is just a group of member variables rather than a
// regular class.
class _raw_vextr_packed_data {
public:

   /** Constructor.

   ciMax
      Count of slots in the item array.
   bDynamic
      true if the item array is allocated dynamically, or false otherwise (static or read-only).
   bHasStatic
      true if the parent object is followed by a static item array, or false otherwise.
   */
   _raw_vextr_packed_data(size_t ciMax, bool bDynamic, bool bHasStatic) :
      m_iPackedData(
         ciMax | (bDynamic ? smc_bDynamicMask : 0) | (bHasStatic ? smc_bHasStaticMask : 0)
      ) {
   }


   /** Assignment operator. Updates all components except bHasStatic.

   rvpd
      Source data.
   return
      *this.
   */
   _raw_vextr_packed_data & operator=(_raw_vextr_packed_data const & rvpd) {
      m_iPackedData = (rvpd.m_iPackedData & ~smc_bHasStaticMask)
                    | (m_iPackedData & smc_bHasStaticMask);
      return *this;
   }


   /** Assigns new values to all components except bHasStatic.

   ciMax
      Count of slots in the item array.
   bDynamic
      true if the item array is allocated dynamically, or false otherwise (static or read-only).
   return
      *this.
   */
   _raw_vextr_packed_data & set(size_t ciMax, bool bDynamic) {
      m_iPackedData = ciMax
                    | (bDynamic ? smc_bDynamicMask : 0)
                    | (m_iPackedData & smc_bHasStaticMask);
      return *this;
   }


   /** Returns ciMax.

   return
      Count of slots in the item array.
   */
   size_t get_ciMax() const {
      return m_iPackedData & smc_ciMaxMask;
   }


   /** Returns true if the parent object’s m_p points to a dynamically-allocated item array.

   return
      true if the item array is allocated dynamically, or false otherwise (static or read-only).
   */
// bool is_item_array_dynamic() const {
   bool get_bDynamic() const {
      return (m_iPackedData & smc_bDynamicMask) != 0;
   }


   /** Returns true if the parent object is followed by a static item array.

   return
      true if the object also has a static item array, or false otherwise.
   */
// bool has_static_item_array() const {
   bool get_bHasStatic() const {
      return (m_iPackedData & smc_bHasStaticMask) != 0;
   }


   /** Assigns a new value to ciMax.

   ciMax
      Count of slots in the item array.
   */
   void set_ciMax(size_t ciMax) {
      m_iPackedData = (m_iPackedData & ~smc_ciMaxMask) | ciMax;
   }


private:

   /** Bit-field composed by the following components:

   bool const bHasStatic
      true if the parent object is followed by a static item array.
   bool bDynamic
      true if the item array is allocated dynamically, or false otherwise (static or read-only).
   size_t ciMax;
      Size of the item array.
   */
   size_t m_iPackedData;

   /** Mask to access bHasStatic from m_iPackedData. */
   static size_t const smc_bHasStaticMask = 0x01;
   /** Mask to access bDynamic from m_iPackedData. */
   static size_t const smc_bDynamicMask = 0x02;


public:

   /** Mask to access ciMax from m_iPackedData. */
   static size_t const smc_ciMaxMask = ~(smc_bDynamicMask | smc_bHasStaticMask);
};

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_raw_vextr_impl_base


namespace abc {

/** Template-independent members of _raw_*_vextr_impl that are identical for trivial and non-trivial
types.
*/
class ABCAPI _raw_vextr_impl_base {
protected:

   /** Allows to get a temporary item array from a pool of options, then work with it, and upon
   destruction it ensures that the array is either adopted by the associated _raw_vextr_impl_base,
   or properly discarded.

   A transaction will not take care of copying the item array, if switching to a different item
   array.

   For size increases, the reallocation (if any) is performed in the constructor; for decreases,
   it’s performed in commit().
   */
   class ABCAPI transaction :
      public noncopyable {
   public:

      /** Constructor.

      cbItem
         Size of a single array item, in bytes.
      prvib
         Subject of the transaction.
      ciNew
         New item count, or -1 to use ciDelta instead.
      ciDelta
         Item count change; can be positive or negative.
      bNulT
         true if the item array is NUL-terminated, or false otherwise.
      */
      transaction(
         size_t cbItem,
         _raw_vextr_impl_base * prvib, ptrdiff_t ciNew, ptrdiff_t ciDelta = 0, bool bNulT = false
      );


      /** Destructor.
      */
      ~transaction() {
         if (m_bFree) {
            memory::free(m_p);
         }
      }


      /** Commits the transaction; if the item array is to be replaced, the current one will be
      released if necessary; it’s up to the client to destruct any items in it. If this method is
      not called before the transaction is destructed, it’s up to the client to also ensure that any
      and all objects constructed in the work array have been properly destructed.

      cbItem
         Size of a single array item, in bytes.
      bNulT
         true if the item array is NUL-terminated, or false otherwise.
      */
      void commit(size_t cbItem = 0, bool bNulT = false);


      /** Returns the work item array.

      return
         Pointer to the working item array.
      */
      template <typename T>
      T * work_array() const {
         return static_cast<T *>(m_p);
      }


      /** Returns true if the contents of the item array need to migrated due to the transaction
      switching item arrays. If the array was/will be only resized, the return value is false,
      because the reallocation did/will take care of moving the item array.

      return
         true if the pointer to the item array will be changed upon destruction, or false otherwise.
      */
      bool will_replace_item_array() const {
         return m_p != m_prvib->m_p;
      }


   private:

      /** Subject of the transaction. */
      _raw_vextr_impl_base * m_prvib;
      /** Pointer to the item array to which clients must write. This may or may not be the same as
      m_prvib->m_p, depending on whether we needed a new item array. This pointer will replace
      m_prvib->m_p upon commit(). */
      void * m_p;
      /** Number of currently used items in m_p. */
      size_t m_ci;
      /** See _raw_vextr_impl_base::m_rvpd. */
      _raw_vextr_packed_data m_rvpd;
      /** true if m_p has been dynamically allocated for the transaction and needs to be freed in
      the destructor, either because the transaction didn’t get committed, or because it did and the
      item array is now owned by m_prvib. */
      bool m_bFree;
   };

   // Allow transactions to access this class’s protected members.
   friend class transaction;


public:

   /** Destructor.
   */
   ~_raw_vextr_impl_base() {
      if (m_rvpd.get_bDynamic()) {
         memory::free(m_p);
      }
   }


   /** Returns the count of item slots in the current item array.

   bNulT
      If true, this vextr is intended to be NUL-terminated, and the returned count will not include
      the trailing NUL terminator; in other words, the capacity will be underreported to reserve
      space for the NUL terminator.
   return
      Count of slots in the item array.
   */
   size_t capacity(bool bNulT = false) const {
      size_t ciMax(m_rvpd.get_ciMax());
      return ciMax - (ciMax > 0 && bNulT ? 1 /*NUL*/ : 0);
   }


   /** Returns a pointer to the item array.

   return
      Pointer to the item array.
   */
   template <typename T>
   T * data() {
      return static_cast<T *>(m_p);
   }
   template <typename T>
   T const * data() const {
      return static_cast<T const *>(m_p);
   }


   /** See buffered_vector::size() and _raw_str::size().

   bNulT
      true if the item array is NUL-terminated, or false otherwise.
   return
      Count of items in the item array.
   */
   size_t size(bool bNulT = false) const {
      return m_ci - (bNulT ? 1 /*NUL*/ : 0);
   }


protected:

   /** Constructor. The overload with ciStaticMax constructs the object as empty, setting m_p to
   nullptr or an empty string; the overload with pConstSrc constructs the object assigning an item
   array.

   ciStaticMax
      Count of slots in the static item array, or 0 if no static item array is present.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   pConstSrc
      Pointer to an array that will be adopted by the vextr as read-only.
   ciSrc
      Count of items in the array pointed to by pConstSrc.
   */
   _raw_vextr_impl_base(size_t ciStaticMax, bool bNulT = false);
   _raw_vextr_impl_base(void const * pConstSrc, size_t ciSrc) :
      m_p(const_cast<void *>(pConstSrc)),
      m_ci(ciSrc),
      // ciMax = 0 means that the item array is read-only.
      m_rvpd(0, false, false) {
      ABC_ASSERT(pConstSrc);
   }


   /** Adjusts a 0-based index in the array. If negative, it’s interpreted as a 1-based index from
   the end.

   TODO: comment signature.
   */
   size_t adjust_index(ptrdiff_t i, bool bNulT = false) const;


   /** Adjusts a 0-based index and count in the array. iFirst is treated like adjust_index() does;
   if the count of items is negative, it’s the count of elements to skip from the end of the item
   array.

   TODO: comment signature.
   */
   void adjust_range(ptrdiff_t * piFirst, ptrdiff_t * pci, bool bNulT = false) const;


   /** Resets the contents of the object to an empty item array: a single NUL character in case of
   a string, or nullptr for everything else.

   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void assign_empty(bool bNulT = false) {
      m_p = bNulT ? const_cast<char32_t *>(&smc_chNUL) : nullptr;
      m_ci = bNulT ? 1u /*NUL*/ : 0;
      m_rvpd.set(0, false);
   }


   /** Returns true if m_p points to a read-only item array.

   return
      true if m_p points to a read-only item array, or false otherwise.
   */
   bool is_item_array_readonly() const {
      // No capacity means read-only item array.
      return m_rvpd.get_ciMax() == 0;
   }


   /** Returns a pointer to the static item array that follows this object, if present, or nullptr
   otherwise.

   return
      Pointer to the static item array, or nullptr if no static item array is present.
   */
   template <typename T>
   T * static_array_ptr();


   /** Returns the size of the array returned by static_array_ptr(), or 0 if no such array is
   present.

   return
      Capacity of the static item array, or 0 if no static item array is present.
   */
   size_t static_capacity() const;


   /** Puts a NUL terminator at the provided address.

   cbItem
      Size of a single array item, in bytes.
   p
      Pointer to the item to be overwritten with a NUL.
   */
   static void terminate(size_t cbItem, void * p) {
      switch (cbItem) {
         case sizeof(int8_t):
            *static_cast<int8_t *>(p) = 0;
            break;
         case sizeof(int16_t):
            *static_cast<int16_t *>(p) = 0;
            break;
         case sizeof(int32_t):
            *static_cast<int32_t *>(p) = 0;
            break;
      }
   }


   /** Throws an exception if the specified index is out of bounds for the item array.

   i
      Index to validate.
   */
   void validate_index(intptr_t i) const;


protected:

   /** Pointer to the item array. */
   void * m_p;
   /** Number of currently used items in m_p. */
   size_t m_ci;
   /** Size of the item array pointed to by m_p, and other bits. */
   _raw_vextr_packed_data m_rvpd;

   /** NUL terminator of the largest character type. */
   static char32_t const smc_chNUL;
   /** No less than this many items. Must be greater than, and not overlap any bits with,
   _raw_vextr_impl_base::smc_ciMaxMask. */
   static size_t const smc_cMinSlots = 8;
   /** Size multiplier. This should take into account that we want to reallocate as rarely as
   possible, so every time we do it it should be for a rather conspicuous growth. */
   static unsigned const smc_iGrowthRate = 2;
};


/** Used to find out what the offset are for an embedded static item array.
*/
class _raw_vextr_impl_base_with_static_item_array :
   public _raw_vextr_impl_base {
public:

   /** Static size. */
   size_t m_ciStaticMax;
   /** First item of the static array. This can’t be a T[], because we don’t want its items to be
   constructed/destructed automatically, and because this class doesn’t know its size. */
   std::max_align_t m_tFirst;
};


/** Rounds up an array size to avoid interfering with the bits outside of
_raw_vextr_packed_data::smc_ciMaxMask. Should be a constexpr function, but for now it’s just a
macro.

ci
   Count of items.
return
   Rounded-up count of items.
*/
#define _ABC__RAW_VEXTR_IMPL_BASE__ADJUST_ITEM_COUNT(ci) \
   ((size_t(ci) + ~_raw_vextr_packed_data::smc_ciMaxMask) & _raw_vextr_packed_data::smc_ciMaxMask)


// Now these can be implemented.

template <typename T>
inline T * _raw_vextr_impl_base::static_array_ptr() {
   if (!m_rvpd.get_bHasStatic()) {
      return nullptr;
   }
   _raw_vextr_impl_base_with_static_item_array * prvibwsia(
      static_cast<_raw_vextr_impl_base_with_static_item_array *>(this)
   );
   return reinterpret_cast<T *>(&prvibwsia->m_tFirst);
}


inline size_t _raw_vextr_impl_base::static_capacity() const {
   if (!m_rvpd.get_bHasStatic()) {
      return 0;
   }
   _raw_vextr_impl_base_with_static_item_array const * prvibwsia(
      static_cast<_raw_vextr_impl_base_with_static_item_array const *>(this)
   );
   return prvibwsia->m_ciStaticMax;
}

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_raw_complex_vextr_impl


namespace abc {

/** Template-independent implementation of a vector for non-trivial contained types.
*/
class ABCAPI _raw_complex_vextr_impl :
   public _raw_vextr_impl_base {
public:

   /** Appends one or more items.

   type
      Adapter for the items’ type.
   pAdd
      Pointer to the first item to add.
   ciAdd
      Count of items to add.
   bMove
      true to move the items from pAdd to the vextr’s item array, or false to copy them instead.
   */
   void append(type_void_adapter const & type, void const * pAdd, size_t ciAdd, bool bMove) {
      if (ciAdd) {
         _insert(type, size(), pAdd, ciAdd, bMove);
      }
   }


   /** Copies or moves the contents of the two sources to *this, according to the source type. If
   bMove{1,2} == true, the source items will be moved by having their const-ness cast away - be
   careful.

   type
      Adapter for the items’ type.
   p1
      Pointer to the first source array.
   ci1
      Count of items in the first source array.
   bMove1
      true to move the items from p1 to the vextr’s item array, or false to copy them instead.
   p2
      Pointer to the second source array.
   ci2
      Count of items in the second source array.
   bMove2
      true to move the items from p1 to the vextr’s item array, or false to copy them instead.
   */
   void assign_concat(
      type_void_adapter const & type,
      void const * p1, size_t ci1, bool bMove1, void const * p2, size_t ci2, bool bMove2
   );


   /** Copies the contents of the source to *this.

   type
      Adapter for the items’ type.
   p
      Pointer to the source array.
   ci
      Count of items in the source array.
   */
   void assign_copy(type_void_adapter const & type, void const * p, size_t ci);


   /** Moves the contents of the source to *this, taking ownership of the whole item array (items
   are not moved nor copied).

   type
      Adapter for the items’ type.
   rcvi
      Source vextr.
   */
   void assign_move(type_void_adapter const & type, _raw_complex_vextr_impl && rcvi);


   /** Moves the source’s item array if dynamically-allocated, else copies it to *this, moving the
   items instead.

   type
      Adapter for the items’ type.
   rcvi
      Source vextr.
   */
   void assign_move_dynamic_or_move_items(
      type_void_adapter const & type, _raw_complex_vextr_impl && rcvi
   );


   /** Destructs a range of items, or the whole item array. It does not deallocate the item array.

   type
      Adapter for the items’ type.
   ci
      Count of items to destruct.
   */
   void destruct_items(type_void_adapter const & type) {
      type.destruct(m_p, m_ci);
   }
   void destruct_items(type_void_adapter const & type, size_t ci) {
      type.destruct(m_p, ci);
   }


   /** Inserts elements at a specific position in the vextr.

   type
      Adapter for the items’ type.
   iOffset
      Index at which the items should be inserted. If negative, it will be interpreted as an offset
      from the end of the vextr.
   pAdd
      Pointer to the first item to add.
   ciAdd
      Count of items to add.
   bMove
      true to move the items from pAdd to the vextr’s item array, or false to copy them instead.
   */
   void insert(
      type_void_adapter const & type, ptrdiff_t iOffset, void const * pAdd, size_t ciAdd, bool bMove
   ) {
      if (ciAdd) {
         _insert(type, adjust_index(iOffset), pAdd, ciAdd, bMove);
      }
   }


   /** Removes elements from the vextr.

   type
      Adapter for the items’ type.
   iOffset
      Index at which the items should be removed. If negative, it will be interpreted as an offset
      from the end of the vextr.
   ciRemove
      Count of items to remove.
   */
   void remove_at(type_void_adapter const & type, ptrdiff_t iOffset, ptrdiff_t ciRemove);


   /** Ensures that the item array has at least ciMin of actual item space. If this causes *this to
   switch to using a different item array, any data in the current one will be lost unless bPreserve
   == true.

   type
      Adapter for the items’ type.
   ciMin
      Minimum count of items requested.
   bPreserve
      If true, the previous contents of the item array will be preserved even if the reallocation
      causes the vextr to switch to a different item array.
   */
   void set_capacity(type_void_adapter const & type, size_t ciMin, bool bPreserve);


protected:

   /** Constructor. See _raw_vextr_impl_base::_raw_vextr_impl_base().
   */
   _raw_complex_vextr_impl(size_t ciStaticMax, bool bNulT = false) :
      _raw_vextr_impl_base(ciStaticMax, bNulT) {
   }
   _raw_complex_vextr_impl(void const * pConstSrc, size_t ciSrc) :
      _raw_vextr_impl_base(pConstSrc, ciSrc) {
   }


private:

   /** Actual implementation of append() and insert(). Does not validate iOffset or ciAdd.

   type
      Adapter for the items’ type.
   iOffset
      Index at which the items should be inserted.
   pAdd
      Pointer to the first item to add.
   ciAdd
      Count of items to add.
   bMove
      true to move the items from pAdd to the vextr’s item array, or false to copy them instead.
   */
   void _insert(
      type_void_adapter const & type, size_t iOffset, void const * pAdd, size_t ciAdd, bool bMove
   );
};

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_raw_trivial_vextr_impl


namespace abc {

/** Template-independent implementation of a vector for trivial contained types. The entire class is
NUL-termination-aware; this is the most derived common base class of both vector and str_.
*/
class ABCAPI _raw_trivial_vextr_impl :
   public _raw_vextr_impl_base {
public:

   /** Appends one or more items.

   cbItem
      Size of a single array item, in bytes.
   pAdd
      Pointer to the first item to add.
   ciAdd
      Count of items to add.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void append(size_t cbItem, void const * pAdd, size_t ciAdd, bool bNulT = false) {
      if (ciAdd) {
         _insert_or_remove(cbItem, size(bNulT), pAdd, ciAdd, 0, bNulT);
      }
   }


   /** Copies the contents of the two sources to *this. This method must never be called with p1 or
   p2 == m_p.

   cbItem
      Size of a single array item, in bytes.
   p1
      Pointer to the first source array.
   ci1
      Count of items in the first source array.
   p2
      Pointer to the second source array.
   ci2
      Count of items in the second source array.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void assign_concat(
      size_t cbItem, void const * p1, size_t ci1, void const * p2, size_t ci2, bool bNulT = false
   );


   /** Copies the contents of the source array to *this.

   cbItem
      Size of a single array item, in bytes.
   p
      Pointer to the source array.
   ci
      Count of items in the source array.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void assign_copy(size_t cbItem, void const * p, size_t ci, bool bNulT = false) {
      if (p == m_p) {
         return;
      }
      // assign_concat() is fast enough. Pass the source as the second argument pair, because its
      // code path is faster.
      assign_concat(cbItem, nullptr, 0, p, ci, bNulT);
   }


   /** Moves the source’s item array to *this. This must be called with rtvi being in control of a
   read-only or dynamic item array; see [DOC:4019 abc::*str_ and abc::*vector design] to see how
   str_ and vector ensure this.

   rtvi
      Source vextr.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void assign_move(_raw_trivial_vextr_impl && rtvi, bool bNulT = false) {
      if (rtvi.m_p == m_p) {
         return;
      }
      // Share the item array.
      _assign_share(rtvi);
      // And now empty the source.
      rtvi.assign_empty(bNulT);
   }


   /** Moves the source’s item array if dynamically-allocated, else copies its items (not move –
   items are trivial) to *this.

   cbItem
      Size of a single array item, in bytes.
   rtvi
      Source vextr.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void assign_move_dynamic_or_move_items(
      size_t cbItem, _raw_trivial_vextr_impl && rtvi, bool bNulT = false
   );


   /** Shares the source’s item array if read-only, else copies it to *this.

   cbItem
      Size of a single array item, in bytes.
   rtvi
      Source vextr.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void assign_share_ro_or_copy(
      size_t cbItem, _raw_trivial_vextr_impl const & rtvi, bool bNulT = false
   ) {
      if (rtvi.m_p == m_p) {
         return;
      }
      if (rtvi.is_item_array_readonly()) {
         _assign_share(rtvi);
      } else {
         // Non-read-only, cannot share.
         assign_copy(cbItem, rtvi.m_p, rtvi.size(bNulT), bNulT);
      }
   }


   /** Inserts elements at a specific position in the vextr.

   cbItem
      Size of a single array item, in bytes.
   iOffset
      Index at which the items should be inserted. If negative, it’s going to be interpreted as an
      index from the end of the vextr.
   pAdd
      Pointer to the first item to add.
   ciAdd
      Count of items to add.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void insert(
      size_t cbItem, ptrdiff_t iOffset, void const * pAdd, size_t ciAdd, bool bNulT = false
   ) {
      if (ciAdd) {
         size_t iRealOffset(adjust_index(iOffset, bNulT));
         _insert_or_remove(cbItem, iRealOffset, pAdd, ciAdd, 0, bNulT);
      }
   }


   /** Removes elements from the vextr.

   cbItem
      Size of a single array item, in bytes.
   iOffset
      Index at which the items should be removed. If negative, it’s going to be interpreted as an
      index from the end of the vextr.
   ciRemove
      Count of items to remove.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void remove_at(size_t cbItem, ptrdiff_t iOffset, ptrdiff_t ciRemove, bool bNulT = false) {
      adjust_range(&iOffset, &ciRemove, bNulT);
      if (ciRemove) {
         _insert_or_remove(cbItem, size_t(iOffset), nullptr, 0, size_t(ciRemove), bNulT);
      }
   }


   /** Ensures that the item array has at least ciMin of actual item space (excluding the trailing
   NUL character, in case of NUL-terminated item array). If this causes *this to switch to using a
   different item array, any data in the current one will be lost unless bPreserve == true.

   cbItem
      Size of a single array item, in bytes.
   ciMin
      Minimum count of items requested.
   bPreserve
      If true, the previous contents of the item array will be preserved even if the reallocation
      causes the vextr to switch to a different item array.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void set_capacity(size_t cbItem, size_t ciMin, bool bPreserve, bool bNulT = false);


protected:

   /** Constructor. See _raw_vextr_impl_base::_raw_vextr_impl_base().
   */
   _raw_trivial_vextr_impl(size_t ciStaticMax, bool bNulT = false) :
      _raw_vextr_impl_base(ciStaticMax, bNulT) {
   }
   _raw_trivial_vextr_impl(void const * pConstSrc, size_t ciSrc) :
      _raw_vextr_impl_base(pConstSrc, ciSrc) {
   }


private:

   /** Shares the source’s item array. It only allows sharing read-only or dynamically-allocated
   item arrays (the latter only as part of moving them).

   rtvi
      Source vextr.
   */
   void _assign_share(_raw_trivial_vextr_impl const & rtvi);


   /** Actual implementation append(), insert() and remove_at().

   cbItem
      Size of a single array item, in bytes.
   iOffset
      Index at which the items should be inserted.
   pAdd
      Pointer to the first item to add.
   ciAdd
      Count of items to add.
   ciAdd
      Count of items to remove.
   bNulT
      true if the item array is a NUL-terminated string, or false otherwise.
   */
   void _insert_or_remove(
      size_t cbItem,
      size_t iOffset, void const * pAdd, size_t ciAdd, size_t ciRemove, bool bNulT = false
   );
};

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_iterable_vector


namespace abc {

/** Provides standard methods to create iterators of type pointer_iterator from a
_raw_vextr_impl_base-derived class.
*/
template <class TCont, typename TVal>
struct _iterable_vector {
public:

   typedef TVal value_type;
   typedef TVal * pointer;
   typedef TVal const * const_pointer;
   typedef TVal & reference;
   typedef TVal const & const_reference;
   typedef size_t size_type;
   typedef ptrdiff_t difference_type;
   typedef pointer_iterator<TCont, TVal> iterator;
   typedef pointer_iterator<TCont, TVal const> const_iterator;
   typedef std::reverse_iterator<iterator> reverse_iterator;
   typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


public:

   /** Returns a forward iterator set to the first element.

   TODO: comment signature.
   */
   iterator begin() {
      // const_cast is required because base_str_::data() returns const only.
      return iterator(const_cast<TVal *>(static_cast<TCont *>(this)->data()));
   }
   const_iterator begin() const {
      return cbegin();
   }


   /** Returns a const forward iterator set to the first element.

   TODO: comment signature.
   */
   const_iterator cbegin() const {
      return const_iterator(static_cast<TCont const *>(this)->data());
   }


   /** Returns a const reverse iterator set to the first element.

   TODO: comment signature.
   */
   const_reverse_iterator crbegin() const {
      return const_reverse_iterator(cbegin());
   }


   /** Returns a const forward iterator set beyond the last element.

   TODO: comment signature.
   */
   const_iterator cend() const {
      return const_iterator(cbegin() + ptrdiff_t(static_cast<TCont const *>(this)->size()));
   }


   /** Returns a const reverse iterator set beyond the last element.

   TODO: comment signature.
   */
   const_reverse_iterator crend() const {
      return const_reverse_iterator(cend());
   }


   /** Returns a forward iterator set beyond the last element.

   TODO: comment signature.
   */
   iterator end() {
      return iterator(begin() + ptrdiff_t(static_cast<TCont *>(this)->size()));
   }
   const_iterator end() const {
      return cend();
   }


   /** Returns a reverse iterator set to the first element of the vector.

   TODO: comment signature.
   */
   reverse_iterator rbegin() {
      return reverse_iterator(begin());
   }
   const_reverse_iterator rbegin() const {
      return crbegin();
   }


   /** Returns a reverse iterator set beyond the last element.

   TODO: comment signature.
   */
   reverse_iterator rend() {
      return reverse_iterator(end());
   }
   const_reverse_iterator rend() const {
      return crend();
   }
};

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //ifndef ABC__VEXTR_HXX

