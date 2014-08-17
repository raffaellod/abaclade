﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2014
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
   #error Please #include <abaclade.hxx> before this file
#endif
#ifdef ABC_CXX_PRAGMA_ONCE
   #pragma once
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::testing::utility globals

namespace abc {
namespace testing {
namespace utility {

// Forward declaration.
template <class T>
class container_data_ptr_tracker;

/*! Allows to declare a container_data_ptr_tracker instance using the auto keyword.

t
   Object to track.
return
   Tracker instance.
*/
template <class T>
container_data_ptr_tracker<T> make_container_data_ptr_tracker(T const & t);

} //namespace utility
} //namespace testing
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::testing::utility::class_with_internal_pointer

namespace abc {
namespace testing {
namespace utility {

/*! Allows to verify that its move constructor was invoked instead of the raw bytes being copied.
*/
class class_with_internal_pointer {
public:

   /*! Constructor.

   cwip
      Source object.
   */
   class_with_internal_pointer() :
      m_pi(&m_i),
      m_i(0xcafe) {
   }
   class_with_internal_pointer(class_with_internal_pointer const & cwip) :
      m_pi(&m_i),
      m_i(cwip.m_i) {
   }
   class_with_internal_pointer(class_with_internal_pointer && cwip) :
      m_pi(&m_i),
      m_i(cwip.m_i) {
   }


   /*! Validates that the object’s internal pointer has the expected value.

   return
      true if the internal pointer is valid, or false otherwise.
   */
   bool validate() {
      ABC_TRACE_FUNC(this);

      return m_i == 0xcafe && m_pi == &m_i;
   }


private:

   /*! Pointer to m_i. */
   uint16_t * m_pi;
   /*! Data referenced by m_pi. */
   uint16_t m_i;
};

} //namespace utility
} //namespace testing
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::testing::utility::container_data_ptr_tracker

namespace abc {
namespace testing {
namespace utility {

/*! Tracks changes in the internal data pointer of a container.
*/
template <class T>
class container_data_ptr_tracker {
public:

   /*! Constructor. Starts tracking changes in the specified object.

   t
      Object to track.
   */
   container_data_ptr_tracker(T const & t) :
      m_t(t),
      m_pti(t.cbegin().base()) {
   }


   /*! Checks if the monitored object’s data pointer has changed.

   return
      true if the data pointer has changed, or false otherwise.
   */
   bool changed() {
      ABC_TRACE_FUNC(this);

      typename T::const_pointer ptiNew(m_t.cbegin().base());
      // Check if the data pointer has changed.
      if (ptiNew != m_pti) {
         // Update the data pointer for the next call.
         m_pti = ptiNew;
         return true;
      } else {
         return false;
      }
   }


private:

   /*! Reference to the T instance to be monitored. */
   T const & m_t;
   /*! Pointer to m_t’s data. */
   typename T::const_pointer m_pti;
};


// Now this can be implemented.
template <class T>
inline container_data_ptr_tracker<T> make_container_data_ptr_tracker(T const & t) {
   return container_data_ptr_tracker<T>(t);
}

} //namespace utility
} //namespace testing
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::testing::utility::instances_counter

namespace abc {
namespace testing {
namespace utility {

/*! This class is meant for use in containers to track when items are copied, when they’re moved,
and to check if individual instances have been copied instead of being moved.
*/
class ABACLADE_TESTING_SYM instances_counter {
public:

   /*! Constructor. The copying overload doesn’t really use their argument, because the only non-
   static member (m_iUnique) is always generated.

   ic
      Source object.
   */
   instances_counter() :
      m_iUnique(++m_iNextUnique) {
      ++m_cNew;
   }
   instances_counter(instances_counter const & ic) :
      m_iUnique(++m_iNextUnique) {
      ABC_UNUSED_ARG(ic);
      ++m_cCopies;
   }
   instances_counter(instances_counter && ic) :
      m_iUnique(ic.m_iUnique) {
      ++m_cMoves;
   }


   /*! Assigment operator. The copying overload doesn’t really use its argument, because the only
   non-static member (m_iUnique) is always generated.

   ic
      Source object.
   */
   instances_counter & operator=(instances_counter const & ic) {
      ABC_UNUSED_ARG(ic);
      m_iUnique = ++m_iNextUnique;
      ++m_cCopies;
      return *this;
   }
   instances_counter & operator=(instances_counter && ic) {
      m_iUnique = ic.m_iUnique;
      ++m_cMoves;
      return *this;
   }


   /*! Equality comparison operator. Should always return false, since no two simultaneously-living
   instances should have the same unique value.

   oc
      Object to compare to *this.
   return
      true if *this has the same unique value as oc, or false otherwise.
   */
   bool operator==(instances_counter const & oc) const {
      return m_iUnique == oc.m_iUnique;
   }


   /*! Inequality comparison operator. Should always return true, since no two simultaneously-living
   instances should have the same unique value.

   oc
      Object to compare to *this.
   return
      true if *this has a different unique value than oc, or false otherwise.
   */
   bool operator!=(instances_counter const & oc) const {
      return !operator==(oc);
   }


   /*! Returns the count of instances created, excluding moved ones.

   return
      Count of instances.
   */
   static size_t copies() {
      return m_cCopies;
   }


   /*! Returns the count of moved instances.

   return
      Count of instances.
   */
   static size_t moves() {
      return m_cMoves;
   }


   /*! Returns the count of new (not copied, not moved) instances. Useful to track how many
   instances have not been created from a source instance, perhaps only to be copy- or move-assigned
   later, which would be less efficient than just copy- or move-constructing, 

   return
      Count of instances.
   */
   static size_t new_insts() {
      return m_cNew;
   }


   /*! Resets the copies/moves/new instance counts.
   */
   static void reset_counts() {
      m_cCopies = 0;
      m_cMoves = 0;
      m_cNew = 0;
   }


   /*! Returns the unique value associated to this object.

   return
      Unique value.
   */
   int unique() const {
      return m_iUnique;
   }


private:

   /*! Unique value associated to this object. */
   int m_iUnique;
   /*! Count of instances created, excluding moved ones. */
   static size_t m_cCopies;
   /*! Count of moved instances. */
   static size_t m_cMoves;
   /*! Count of new (not copied, not moved) instances. */
   static size_t m_cNew;
   /*! Value of m_iUnique for the next instance. */
   static int m_iNextUnique;
};

} //namespace utility
} //namespace testing
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //ifndef _ABACLADE_TESTING_UTILITY_HXX

