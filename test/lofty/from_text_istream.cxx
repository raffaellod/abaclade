﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2016-2018 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#include <lofty/from_str.hxx>
#include <lofty/logging.hxx>
#include <lofty/testing/test_case.hxx>
#include <lofty/text.hxx>
#include <lofty/text/parsers/dynamic.hxx>
#include <lofty/text/parsers/regex.hxx>
#include <lofty/text/str.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace test { namespace {

class type_with_ftis {
public:
   static text::str const twf;

public:
   text::str & get() {
      return s;
   }

private:
   text::str s;
};

text::str const type_with_ftis::twf(LOFTY_SL("TWF"));

}}} //namespace lofty::test::

namespace lofty {

template <>
class from_text_istream<test::type_with_ftis> {
public:
   void convert_capture(text::parsers::dynamic_match_capture const & capture0, test::type_with_ftis * dst) {
      dst->get() = capture0.str_copy();
   }

   text::parsers::dynamic_state const * format_to_parser_states(
      text::parsers::regex_capture_format const & format, text::parsers::dynamic * parser
   ) {
      LOFTY_UNUSED_ARG(format);

      return parser->create_string_state(&test::type_with_ftis::twf);
   }
};

} //namespace lofty

namespace lofty { namespace test {

LOFTY_TESTING_TEST_CASE_FUNC(
   from_text_istream_basic,
   "lofty::from_text_istream – basic"
) {
   LOFTY_TRACE_FUNC();

   /* This assertion is more important at compile time than at run time; if the from_str() call compiles, it
   will return the correct value. */
   ASSERT(from_str<type_with_ftis>(type_with_ftis::twf).get() == type_with_ftis::twf);
}

}} //namespace lofty::test

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace test {

LOFTY_TESTING_TEST_CASE_FUNC(
   from_text_istream_bool,
   "lofty::from_text_istream – bool"
) {
   LOFTY_TRACE_FUNC();

   ASSERT(from_str<bool>(LOFTY_SL("false")) == false);
   ASSERT(from_str<bool>(LOFTY_SL("true")) == true);
   ASSERT_THROWS(text::syntax_error, from_str<bool>(LOFTY_SL("")));
   ASSERT_THROWS(text::syntax_error, from_str<bool>(LOFTY_SL("a")));
   ASSERT_THROWS(text::syntax_error, from_str<bool>(LOFTY_SL("atrue")));
   ASSERT_THROWS(text::syntax_error, from_str<bool>(LOFTY_SL("falseb")));
}

}} //namespace lofty::test

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace test {

LOFTY_TESTING_TEST_CASE_FUNC(
   from_text_istream_int,
   "lofty::from_text_istream – int"
) {
   LOFTY_TRACE_FUNC();

   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("q")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("-")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("-w")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("-1-")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("0x1")));

   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL(""), LOFTY_SL("#")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("q"), LOFTY_SL("#")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("-"), LOFTY_SL("#")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("-w"), LOFTY_SL("#")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("-1-"), LOFTY_SL("#")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("0b"), LOFTY_SL("#")));
   ASSERT_THROWS(text::syntax_error, from_str<int>(LOFTY_SL("0p1"), LOFTY_SL("#")));

   ASSERT(from_str<int>(LOFTY_SL("0")) == 0);
   ASSERT(from_str<int>(LOFTY_SL("0"), LOFTY_SL("d")) == 0);
   ASSERT(from_str<int>(LOFTY_SL("0"), LOFTY_SL("#")) == 0);
   ASSERT(from_str<int>(LOFTY_SL("0b0"), LOFTY_SL("#")) == 0);
   ASSERT(from_str<int>(LOFTY_SL("0"), LOFTY_SL("#d")) == 0);

   ASSERT(from_str<int>(LOFTY_SL("1")) == 1);
   ASSERT(from_str<int>(LOFTY_SL("8"), LOFTY_SL("d")) == 8);
   ASSERT(from_str<int>(LOFTY_SL("012"), LOFTY_SL("d")) == 12);
   ASSERT(from_str<int>(LOFTY_SL("15"), LOFTY_SL("#")) == 15);
   ASSERT(from_str<int>(LOFTY_SL("013"), LOFTY_SL("#")) == 11);
   ASSERT(from_str<int>(LOFTY_SL("16"), LOFTY_SL("#d")) == 16);

   ASSERT(from_str<int>(LOFTY_SL("-1")) == -1);
   ASSERT(from_str<int>(LOFTY_SL("-5"), LOFTY_SL("d")) == -5);
   ASSERT(from_str<int>(LOFTY_SL("-021"), LOFTY_SL("#")) == -17);
   ASSERT(from_str<int>(LOFTY_SL("-0xa"), LOFTY_SL("#")) == -10);
   ASSERT(from_str<int>(LOFTY_SL("-32"), LOFTY_SL("#d")) == -32);
}

}} //namespace lofty::test

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace test {

LOFTY_TESTING_TEST_CASE_FUNC(
   from_text_istream_std_int8_t,
   "lofty::from_text_istream – std::int8_t"
) {
   LOFTY_TRACE_FUNC();

   ASSERT(from_str<std::int8_t>(LOFTY_SL("0"), LOFTY_SL("x")) == 0);

   ASSERT(from_str<std::int8_t>(LOFTY_SL("1"), LOFTY_SL("x")) == 1);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("f"), LOFTY_SL("x")) == 15);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("0Xf"), LOFTY_SL("#x")) == 15);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("7f"), LOFTY_SL("x")) == 127);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("0x7f"), LOFTY_SL("#x")) == 127);

   ASSERT(from_str<std::int8_t>(LOFTY_SL("ff"), LOFTY_SL("x")) == -1);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("0Xff"), LOFTY_SL("#x")) == -1);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("ff"), LOFTY_SL("x")) == -1);
   ASSERT(from_str<std::int8_t>(LOFTY_SL("0xff"), LOFTY_SL("#x")) == -1);
}

}} //namespace lofty::test
