﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2014, 2015
Raffaello D. Di Napoli

This file is part of Abaclade.

Abaclade is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Abaclade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with Abaclade. If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------*/

#ifndef _ABACLADE_TESTING_UTILITY_HXX
#define _ABACLADE_TESTING_UTILITY_HXX

#ifndef _ABACLADE_HXX
   #error "Please #include <abaclade.hxx> before this file"
#endif
#ifdef ABC_CXX_PRAGMA_ONCE
   #pragma once
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace testing {

//! Utility classes useful for testing.
namespace utility {}

}} //namespace abc::testing

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace testing { namespace utility {

// Forward declaration.
template <class T>
class container_data_ptr_tracker;

/*! Allows to declare a container_data_ptr_tracker instance using the auto keyword.

@param pt
   Pointer to the object to track.
@return
   Tracker instance.
*/
template <class T>
container_data_ptr_tracker<T> make_container_data_ptr_tracker(T const * pt);

}}} //namespace abc::testing::utility

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace testing { namespace utility {

//! Allows to verify that its move constructor was invoked instead of the raw bytes being copied.
class class_with_internal_pointer {
public:
   //! Default constructor.
   class_with_internal_pointer() :
      m_pi(&m_i),
      m_i(0xcafe) {
   }

   /*! Move constructor.

   @param cwip
      Source object.
   */
   class_with_internal_pointer(class_with_internal_pointer && cwip) :
      m_pi(&m_i),
      m_i(cwip.m_i) {
   }

   /*! Copy constructor.

   @param cwip
      Source object.
   */
   class_with_internal_pointer(class_with_internal_pointer const & cwip) :
      m_pi(&m_i),
      m_i(cwip.m_i) {
   }

   /*! Validates that the object’s internal pointer has the expected value.

   @return
      true if the internal pointer is valid, or false otherwise.
   */
   bool validate() {
      ABC_TRACE_FUNC(this);

      return m_i == 0xcafe && m_pi == &m_i;
   }

private:
   //! Pointer to m_i.
   std::uint16_t * m_pi;
   //! Data referenced by m_pi.
   std::uint16_t m_i;
};

}}} //namespace abc::testing::utility

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace testing { namespace utility {

//! Tracks changes in the data() member of a container.
template <class T>
class container_data_ptr_tracker {
public:
   /*! Constructor. Starts tracking changes in the specified object.

   @param pt
      Object to track.
   */
   container_data_ptr_tracker(T const * pt) :
      m_pt(pt),
      m_ptpData(m_pt->data()) {
   }

   /*! Checks if the monitored object’s data pointer has changed.

   @return
      true if the data pointer has changed, or false otherwise.
   */
   bool changed() {
      ABC_TRACE_FUNC(this);

      auto ptpDataNew = m_pt->data();
      // Check if the data pointer has changed.
      if (ptpDataNew != m_ptpData) {
         // Update the data pointer for the next call.
         m_ptpData = ptpDataNew;
         return true;
      } else {
         return false;
      }
   }

private:
   //! Pointer to the T instance to be monitored.
   T const * m_pt;
   //! Pointer to m_t’s data.
   typename T::const_pointer m_ptpData;
};


// Now this can be defined.

template <class T>
inline container_data_ptr_tracker<T> make_container_data_ptr_tracker(T const * pt) {
   return container_data_ptr_tracker<T>(pt);
}

}}} //namespace abc::testing::utility

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace testing { namespace utility {

/*! This class is meant for use in containers to track when items are copied, when they’re moved,
and to check if individual instances have been copied instead of being moved. */
class ABACLADE_TESTING_SYM instances_counter {
public:
   //! Default constructor.
   instances_counter() :
      m_iUnique(++m_iNextUnique) {
      ++m_cNew;
   }

   /*! Move constructor.

   @param ic
      Source object.
   */
   instances_counter(instances_counter && ic) :
      m_iUnique(ic.m_iUnique) {
      ++m_cMoves;
   }

   /*! Copy constructor. Doesn’t use its argument since the only non-static member (m_iUnique) is
   always generated. */
   instances_counter(instances_counter const &) :
      m_iUnique(++m_iNextUnique) {
      ++m_cCopies;
   }

   /*! Move-assigment operator.

   @param ic
      Source object.
   */
   instances_counter & operator=(instances_counter && ic) {
      m_iUnique = ic.m_iUnique;
      ++m_cMoves;
      return *this;
   }

   /*! Copy-assigment operator. Doesn’t use its argument since the only non-static member
   (m_iUnique) is always generated.

   @return
      *this.
   */
   instances_counter & operator=(instances_counter const &) {
      m_iUnique = ++m_iNextUnique;
      ++m_cCopies;
      return *this;
   }

   /*! Equality relational operator. Should always return false, since no two simultaneously-living
   instances should have the same unique value.

   @param oc
      Object to compare to *this.
   @return
      true if *this has the same unique value as oc, or false otherwise.
   */
   bool operator==(instances_counter const & oc) const {
      return m_iUnique == oc.m_iUnique;
   }

   /*! Inequality relational operator. Should always return true, since no two simultaneously-living
   instances should have the same unique value.

   @param oc
      Object to compare to *this.
   @return
      true if *this has a different unique value than oc, or false otherwise.
   */
   bool operator!=(instances_counter const & oc) const {
      return !operator==(oc);
   }

   /*! Returns the count of instances created, excluding moved ones.

   @return
      Count of instances.
   */
   static std::size_t copies() {
      return m_cCopies;
   }

   /*! Returns the count of moved instances.

   @return
      Count of instances.
   */
   static std::size_t moves() {
      return m_cMoves;
   }

   /*! Returns the count of new (not copied, not moved) instances. Useful to track how many
   instances have not been created from a source instance, perhaps only to be copy- or move-assigned
   later, which would be less efficient than just copy- or move-constructing, 

   @return
      Count of instances.
   */
   static std::size_t new_insts() {
      return m_cNew;
   }

   //! Resets the copies/moves/new instance counts.
   static void reset_counts() {
      m_cCopies = 0;
      m_cMoves = 0;
      m_cNew = 0;
   }

   /*! Returns the unique value associated to this object.

   @return
      Unique value.
   */
   int unique() const {
      return m_iUnique;
   }

private:
   //! Unique value associated to this object.
   int m_iUnique;
   //! Count of instances created, excluding moved ones.
   static std::size_t m_cCopies;
   //! Count of moved instances.
   static std::size_t m_cMoves;
   //! Count of new (not copied, not moved) instances.
   static std::size_t m_cNew;
   //! Value of m_iUnique for the next instance.
   static int m_iNextUnique;
};

}}} //namespace abc::testing::utility

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ifndef _ABACLADE_TESTING_UTILITY_HXX
