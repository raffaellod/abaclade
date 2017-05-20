﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2014-2015, 2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#include <lofty.hxx>
#include <lofty/testing/test_case.hxx>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace test {

//! Generates a lofty::text::char_t literal followed by a comma.
#define _LOFTY_CHAR_COMMA(ch) ::lofty::text::char_t(ch),

#if LOFTY_HOST_UTF == 8
   #define LOFTY_TESTING_ASSERT_text_str_traits_validate(t_or_f, ...) \
      do { \
         /* Append to the strings 6 nasty 0xff character, which will make validate() fail if they’re accessed.
         We don’t include them in the count of characters to validate, but an off-by-one (or more) error will
         incorrectly access them, causing validate() to fail.
         Why 6? Because that’s the longest, albeit invalid, encoding possible in UTF-8, so even the longest
         (wrong) jump will still land on one of these characters. */ \
         \
         static char8_t const chars[] = { \
            LOFTY_CPP_LIST_WALK(_LOFTY_CHAR_COMMA, __VA_ARGS__) \
            char8_t(0xff), char8_t(0xff), char8_t(0xff), char8_t(0xff), char8_t(0xff), char8_t(0xff) \
         }; \
         this->LOFTY_CPP_CAT(assert_, t_or_f)( \
            LOFTY_THIS_FILE_ADDRESS(), \
            text::str_traits::validate(chars, chars + LOFTY_COUNTOF(chars) - 6), \
            LOFTY_SL("text::str_traits::validate(") LOFTY_SL(# __VA_ARGS__) LOFTY_SL(")") \
         ); \
      } while (false)
#elif LOFTY_HOST_UTF == 16 //if LOFTY_HOST_UTF == 8
   #define LOFTY_TESTING_ASSERT_text_str_traits_validate(t_or_f, ...) \
      do { \
         /* Append to the string a second NUL terminator preceded by 2 invalid lead surrogates, which will
         make validate() fail if they’re accessed, which would mean that validate() erroneously skipped past
         the first NUL terminator. */ \
         \
         static char16_t const chars[] = { \
            LOFTY_CPP_LIST_WALK(_LOFTY_CHAR_COMMA, __VA_ARGS__) char16_t(0xd834), char16_t(0xd834) \
         }; \
         this->LOFTY_CPP_CAT(assert_, t_or_f)( \
            LOFTY_THIS_FILE_ADDRESS(), \
            text::str_traits::validate(chars, chars + LOFTY_COUNTOF(chars) - 2), \
            LOFTY_SL("text::str_traits::validate(") LOFTY_SL(# __VA_ARGS__) LOFTY_SL(")") \
         ); \
      } while (false)
#endif //if LOFTY_HOST_UTF == 8 … elif LOFTY_HOST_UTF == 16

#define LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(...) \
   LOFTY_TESTING_ASSERT_text_str_traits_validate(true, __VA_ARGS__)

#define LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(...) \
   LOFTY_TESTING_ASSERT_text_str_traits_validate(false, __VA_ARGS__)

LOFTY_TESTING_TEST_CASE_FUNC(
   text_str_traits_validation,
   "lofty::text::str_traits – validity of counted strings"
) {
   LOFTY_TRACE_FUNC(this);

#if LOFTY_HOST_UTF == 8

   // Valid single character.
   LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(0x01);
   // Increasing run lengths.
   LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(
      0x01, 0xc2, 0xa2, 0xe2, 0x82, 0xac, 0xf0, 0xa4, 0xad, 0xa2
   );
   // Decreasing run lengths.
   LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(
      0xf0, 0xa4, 0xad, 0xa2, 0xe2, 0x82, 0xac, 0xc2, 0xa2, 0x01
   );

   // Invalid single character.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0x81);
   // Invalid single character in the beginning of a valid string.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(
      0x81, 0x01, 0xc2, 0xa2, 0xe2, 0x82, 0xac, 0xf0, 0xa4, 0xad, 0xa2
   );
   // Invalid single character at the end of a valid string.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(
      0x01, 0xc2, 0xa2, 0xe2, 0x82, 0xac, 0xf0, 0xa4, 0xad, 0xa2, 0x81
   );

   // Invalid single overlong.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xc0, 0x81);
   // Invalid single overlong in the beginning of a valid string.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(
      0xc0, 0x81, 0x01, 0xc2, 0xa2, 0xe2, 0x82, 0xac, 0xf0, 0xa4, 0xad, 0xa2
   );
   // Invalid single overlong at the end of a valid string.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(
      0x01, 0xc2, 0xa2, 0xe2, 0x82, 0xac, 0xf0, 0xa4, 0xad, 0xa2, 0xc0, 0x81
   );

   // Technically possible, but not valid UTF-8.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xf9, 0x81, 0x81, 0x81, 0x81);
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xfd, 0x81, 0x81, 0x81, 0x81, 0x81);

   /* Technically possible, but not valid UTF-8. Here the string continues with a few more valid
   characters, so we can detect if the invalid byte was interpreted as the lead byte of some
   UTF-8 sequence. */
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xfe, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01);
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01);

#elif LOFTY_HOST_UTF == 16 //if LOFTY_HOST_UTF == 8

   // Valid single character.
   LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(0x007a);
   // Valid single character and surrogate pair.
   LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(0x007a, 0xd834, 0xdd1e);
   // Valid surrogate pair and single character.
   LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate(0xd834, 0xdd1e, 0x007a);

   // Invalid lead surrogate.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xd834);
   // Invalid lead surrogate in the beginning of a valid string.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0xd834, 0x0079, 0x007a);
   // Invalid lead surrogate at the end of a valid string.
   LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate(0x0079, 0x007a, 0xd834);

#endif //if LOFTY_HOST_UTF == 8 … elif LOFTY_HOST_UTF == 16
}

#undef LOFTY_TESTING_ASSERT_text_str_traits_validate
#undef LOFTY_TESTING_ASSERT_TRUE_text_str_traits_validate
#undef LOFTY_TESTING_ASSERT_FALSE_text_str_traits_validate

}} //namespace lofty::test
