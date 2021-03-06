﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2015, 2017-2018 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#include <lofty/collections/static_list.hxx>
#include <lofty/logging.hxx>
#include <lofty/testing/test_case.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace test {

namespace {

// Forward declaration.
class static_list_node_test;

//! Singleton static_list test subclass.
class static_list_test : public collections::static_list<static_list_test, static_list_node_test> {
public:
   /*! Returns the one and only instance of this class.

   @return
      *this.
   */
   static static_list_test & instance() {
      return *static_cast<static_list_test *>(&data_members_);
   }

private:
   //! Only instance of this class’ data.
   static data_members data_members_;
};

static_list_test::data_members static_list_test::data_members_ = LOFTY_COLLECTIONS_STATIC_LIST_INITIALIZER;


//! Element of static_list_test.
class static_list_node_test :
   public collections::static_list<static_list_test, static_list_node_test>::node {
public:
   /*! Constructor.

   @param i_
      Value of the internal integer.
   */
   static_list_node_test(int i_) :
      i(i_) {
   }

   /*! Returns the internal integer.

   @return
      Internal integer.
   */
   int get() const {
      return i;
   }

private:
   //! Internal integer.
   int i;
};

} //namespace

LOFTY_TESTING_TEST_CASE_FUNC(
   collections_static_list_basic,
   "lofty::collections::static_list – basic operations"
) {
   LOFTY_TRACE_FUNC();

   auto & sl = static_list_test::instance();

   /* Since by design static_list elements are added automatically on instantiation and removed on
   destruction, additions and removals are governed by nested scopes. */

   ASSERT(sl.empty());
   ASSERT(sl.size() == 0u);
   ASSERT((sl.begin() == sl.end()));
   ASSERT((sl.rbegin() == sl.rend()));

   {
      static_list_node_test n10(10);
      ASSERT(!sl.empty());
      ASSERT(sl.size() == 1u);
      {
         // Simple forward iteration.
         auto itr(sl.begin());
         ASSERT(itr->get() == 10);
         ++itr;
         ASSERT((itr == sl.end()));
      }

      {
         static_list_node_test n20(20);
         ASSERT(!sl.empty());
         ASSERT(sl.size() == 2u);
         {
            // Backwards iteration.
            auto itr(sl.rbegin());
            ASSERT(itr->get() == 20);
            ++itr;
            ASSERT(itr->get() == 10);
            ++itr;
            ASSERT((itr == sl.rend()));
         }
      }

      ASSERT(!sl.empty());
      ASSERT(sl.size() == 1u);
   }

   ASSERT(sl.empty());
   ASSERT(sl.size() == 0u);
   ASSERT((sl.begin() == sl.end()));
   ASSERT((sl.rbegin() == sl.rend()));

   {
      static_list_node_test n30(30);
      ASSERT(!sl.empty());
      ASSERT(sl.size() == 1u);
      ASSERT((sl.begin() != sl.end()));
      ASSERT((sl.rbegin() != sl.rend()));
   }

   ASSERT(sl.empty());
   ASSERT(sl.size() == 0u);
   ASSERT((sl.begin() == sl.end()));
}

}} //namespace lofty::test
