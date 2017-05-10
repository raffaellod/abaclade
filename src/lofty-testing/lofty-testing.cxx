﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2013-2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License along with Lofty. If not, see
<http://www.gnu.org/licenses/>.
------------------------------------------------------------------------------------------------------------*/

#include <lofty.hxx>
#include <lofty/testing/app.hxx>
#include <lofty/testing/runner.hxx>
#include <lofty/testing/test_case.hxx>
#include <lofty/testing/utility.hxx>

#include <lofty/io/text.hxx>
#include <lofty/text.hxx>
#include <lofty/text/char_ptr_to_str_adapter.hxx>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace testing {

/*virtual*/ int app::main(collections::vector<str> & args) /*override*/ {
   LOFTY_TRACE_FUNC(this, args);

   LOFTY_UNUSED_ARG(args);

   runner r(io::text::stderr);
   r.load_registered_test_cases();
   r.run();
   bool all_passed = r.log_summary();

   return all_passed ? 0 : 1;
}

}} //namespace lofty::testing

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace testing {

assertion_error::assertion_error() {
}

}} //namespace lofty::testing

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace testing {

runner::runner(_std::shared_ptr<io::text::ostream> ostream_) :
   ostream(_std::move(ostream_)),
   failed_assertions(0) {
}

runner::~runner() {
}

void runner::load_registered_test_cases() {
   LOFTY_TRACE_FUNC(this);

   LOFTY_FOR_EACH(auto const & factory_list_elt, test_case_factory_list::instance()) {
      // Instantiate the test case.
      test_cases.push_back(factory_list_elt.factory(this));
   }
}

void runner::log_assertion(
   text::file_address const & file_addr, bool pass, str const & expr, str const & operand,
   str const & expected, str const & actual /*= str::empty*/
) {
   LOFTY_TRACE_FUNC(this, file_addr, expr, operand, expected, actual);

   if (pass) {
      ostream->print(
         LOFTY_SL("COMK-TEST-ASSERT-PASS {}: pass: {} {}{}\n"), file_addr, expr, operand, expected
      );
   } else {
      ++failed_assertions;
      ostream->print(
         LOFTY_SL("COMK-TEST-ASSERT-FAIL {}: fail: {}\n")
            LOFTY_SL("  expected: {}{}\n")
            LOFTY_SL("  actual:   {}\n"),
         file_addr, expr, operand, expected, actual
      );
   }
}

bool runner::log_summary() {
   LOFTY_TRACE_FUNC(this);

   return failed_assertions == 0;
}

void runner::run() {
   LOFTY_TRACE_FUNC(this);

   for (auto itr(test_cases.begin()); itr != test_cases.end(); ++itr) {
      run_test_case(**itr);
   }
}

void runner::run_test_case(class test_case & test_case) {
   LOFTY_TRACE_FUNC(this/*, test_case*/);

   ostream->print(LOFTY_SL("COMK-TEST-CASE-START {}\n"), test_case.title());

   try {
      test_case.run();
   } catch (assertion_error const &) {
      // This exception type is only used to interrupt lofty::testing::test_case::run().
      ostream->write(LOFTY_SL("test case execution interrupted\n"));
   } catch (_std::exception const & x) {
      exception::write_with_scope_trace(ostream.get(), &x);
      ostream->write(LOFTY_SL("COMK-TEST-ASSERT-FAIL unhandled exception, see stack trace above\n"));
   } catch (...) {
      exception::write_with_scope_trace(ostream.get());
      ostream->write(LOFTY_SL("COMK-TEST-ASSERT-FAIL unhandled exception, see stack trace above\n"));
   }

   ostream->write(LOFTY_SL("COMK-TEST-CASE-END\n"));
}

}} //namespace lofty::testing

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace testing {

test_case::test_case() {
}

/*virtual*/ test_case::~test_case() {
}

void test_case::init(class runner * runner_) {
   LOFTY_TRACE_FUNC(this, runner_);

   runner = runner_;
}

void test_case::assert_does_not_throw(
   text::file_address const & file_addr, _std::function<void ()> expr_fn, str const & expr
) {
   LOFTY_TRACE_FUNC(this, file_addr, /*expr_fn, */expr);

   text::sstr<64> caught;
   try {
      expr_fn();
   } catch (_std::exception const & x) {
      caught.format(LOFTY_SL("throws {}: {}"), typeid(x), text::char_ptr_to_str_adapter(x.what()));
   } catch (...) {
      caught = LOFTY_SL("unknown type");
   }
   runner->log_assertion(file_addr, !caught, expr, str::empty, LOFTY_SL("does not throw"), caught.str());
}

void test_case::assert_false(text::file_address const & file_addr, bool actual, str const & expr) {
   LOFTY_TRACE_FUNC(this, file_addr, actual, expr);

   runner->log_assertion(
      file_addr, !actual, expr, str::empty, !actual ? str::empty : LOFTY_SL("false"), LOFTY_SL("true")
   );
}

void test_case::assert_true(text::file_address const & file_addr, bool actual, str const & expr) {
   LOFTY_TRACE_FUNC(this, file_addr, actual, expr);

   runner->log_assertion(
      file_addr, actual, expr, str::empty, actual ? str::empty : LOFTY_SL("true"), LOFTY_SL("false")
   );
}

void test_case::assert_throws(
   text::file_address const & file_addr, _std::function<void ()> expr_fn, str const & expr,
   _std::function<bool (_std::exception const &)> instanceof_fn, _std::type_info const & expected_type
) {
   LOFTY_TRACE_FUNC(this, file_addr, /*expr_fn, */expr, /*instanceof_fn, */expected_type);

   bool pass = false;
   text::sstr<64> caught, expected;
   expected.format(LOFTY_SL("throws {}"), expected_type);
   try {
      expr_fn();
      caught = LOFTY_SL("does not throw");
   } catch (_std::exception const & x) {
      if (instanceof_fn(x)) {
         pass = true;
      }
      caught.format(LOFTY_SL("throws {}: {}"), typeid(x), text::char_ptr_to_str_adapter(x.what()));
   } catch (...) {
      caught = LOFTY_SL("unknown type");
   }
   runner->log_assertion(file_addr, pass, expr, str::empty, expected.str(), caught.str());
}


test_case_factory_list::data_members test_case_factory_list::data_members_ =
   LOFTY_COLLECTIONS_STATIC_LIST_INITIALIZER;

}} //namespace lofty::testing

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace testing { namespace utility {

std::size_t instances_counter::copies_ = 0;
std::size_t instances_counter::moves_ = 0;
std::size_t instances_counter::new_ = 0;
int instances_counter::next_unique = 0;

}}} //namespace lofty::testing::utility
