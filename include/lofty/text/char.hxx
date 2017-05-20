﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010-2015, 2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Macros to generate proper Unicode characters. */

#ifndef _LOFTY_HXX_INTERNAL
   #error "Please #include <lofty.hxx> instead of this file"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Indicates the level of UTF-8 string literals support:
•  2 - The UTF-8 string literal prefix (u8) is supported;
•  1 - The u8 prefix is not supported, but the compiler will generate valid UTF-8 string literals if the
       source file is UTF-8+BOM-encoded;
•  0 - UTF-8 string literals are not supported in any way.
*/
#define LOFTY_CXX_UTF8LIT 0

/*! Indicates how char16_t is defined:
•  2 - char16_t is a native type, distinct from std::uint16_t and wchar_t;
•  1 - lofty::char16_t is a typedef for native 16-bit wchar_t, distinct from std::uint16_t;
•  0 - lofty::char16_t is a typedef for std::uint16_t.
*/
#define LOFTY_CXX_CHAR16 0

/*! Indicates how char32_t is defined:
•  2 - char32_t is a native type, distinct from std::uint32_t and wchar_t;
•  1 - lofty::char32_t is a typedef for native 32-bit wchar_t, distinct from std::uint32_t;
•  0 - lofty::char32_t is a typedef for std::uint32_t.
*/
#define LOFTY_CXX_CHAR32 0

//! @cond

// Only support Unicode Windows programs.
// TODO: support non-Unicode Windows programs (Win9x and Win16). In a very, very distant future!
#ifndef UNICODE
   #define UNICODE
#endif

// Make sure UNICODE and _UNICODE are coherent; UNICODE wins.
#if defined(UNICODE) && !defined(_UNICODE)
   #define _UNICODE
#elif !defined(UNICODE) && defined(_UNICODE)
   #undef _UNICODE
#endif

//! @endcond

#if LOFTY_HOST_CXX_CLANG || LOFTY_HOST_CXX_GCC 
   // char16_t is a native type, different than std::uint16_t.
   #undef LOFTY_CXX_CHAR16
   #define LOFTY_CXX_CHAR16 2
   // char32_t is a native type, different than std::uint32_t.
   #undef LOFTY_CXX_CHAR32
   #define LOFTY_CXX_CHAR32 2

   #if __has_feature(cxx_unicode_literals) || LOFTY_HOST_CXX_GCC
      // UTF-8 string literals are supported.
      #undef LOFTY_CXX_UTF8LIT
      #define LOFTY_CXX_UTF8LIT 2
   #endif
#else
   #if LOFTY_HOST_CXX_MSC
      #if !defined(_WCHAR_T_DEFINED) || !defined(_NATIVE_WCHAR_T_DEFINED)
         #error "Please compile with /Zc:wchar_t"
      #endif

      // char16_t is not a native type, but we can typedef it as wchar_t.
      #undef LOFTY_CXX_CHAR16
      #define LOFTY_CXX_CHAR16 1
   #else
      /* MSC16 will transcode non-wchar_t string literals into whatever single-byte encoding is selected for
      the user running cl.exe; a solution has been provided in form of a hotfix
      (<http://support.microsoft.com/kb/2284668/en-us>), but it no longer seems available, and it was not
      ported to MSC17/VS2012, thought it seems it was finally built into MSC18/VS2013
      (<http://connect.microsoft.com/VisualStudio/feedback/details/773186/pragma-execution-character-set-utf-
      8-didnt-support-in-vc-2012>).

      Here we assume that no other compiler exhibits such a random behavior, and they will all emit valid
      UTF-8 string literals if the source file is UTF-8+BOM-encoded. */
      #undef LOFTY_CXX_UTF8LIT
      #define LOFTY_CXX_UTF8LIT 1

      // char32_t is not a native type, but we can typedef it as wchar_t.
      #undef LOFTY_CXX_CHAR32
      #define LOFTY_CXX_CHAR32 1
   #endif
#endif
#if LOFTY_CXX_CHAR16 == 0 && LOFTY_CXX_CHAR32 == 0
   #error "LOFTY_CXX_CHAR16 and/or LOFTY_CXX_CHAR32 must be > 0; please fix detection logic"
#endif


//! UTF-8 character type.
typedef char char8_t;

//! UTF-16 character type.
#if LOFTY_CXX_CHAR16 == 1
   typedef wchar_t char16_t;
#elif LOFTY_CXX_CHAR16 == 0
   typedef std::uint16_t char16_t;
#endif

//! UTF-32 character type.
#if LOFTY_CXX_CHAR32 == 1
   typedef wchar_t char32_t;
#elif LOFTY_CXX_CHAR32 == 0
   typedef std::uint32_t char32_t;
#endif

//! UTF-* encoding supported by the host.
#if LOFTY_HOST_API_WIN32 && defined(UNICODE)
   #define LOFTY_HOST_UTF 16
#else
   #define LOFTY_HOST_UTF 8
#endif

namespace lofty { namespace text {

/*! Default UTF character type for the host. Note that only UTF-8 and UTF-16 are supported as native
characters types. */
/* When introducing a new possible value for this constant, please make sure to update the value selection
logic for lofty::text::encoding::host to provide the corresponding UTF encoding. */
#if LOFTY_HOST_UTF == 8
   typedef char8_t char_t;
#elif LOFTY_HOST_UTF == 16
   typedef char16_t char_t;
#endif

}} //namespace lofty::text


/*! Use this to specify a non-ASCII character literal. When compiled, this will expand into a character
literal of the widest type supported by the compiler, which is char32_t in the best case and wchar_t
otherwise, which on Windows is limited to 16 bits (UCS-2).

@param ch
   Character literal.
@return
   Unicode character literal.
*/
#if LOFTY_CXX_CHAR32 == 2
   /* Use U so that the resulting literal is of type char32_t, which cuts down the number of overloads we
   need. */
   #define LOFTY_CHAR(ch) U ## ch
#else
   // Everybody else can only use wchar_t as the largest character literal type, so here it goes.
   #define LOFTY_CHAR(ch) L ## ch
#endif

/*! @cond
Implementation of LOFTY_SL(); allows for expansion of the argument prior to pasting it to the appropriate
string literal prefix, as is necessary for e.g. __FILE__.

@param s
   String literal.
@return
   UTF string literal.
*/
#if LOFTY_HOST_UTF == 8
   #if LOFTY_CXX_UTF8LIT == 2
      #define _LOFTY_SL(s) u8 ## s
   #else
      #define _LOFTY_SL(s) s
   #endif
#elif LOFTY_HOST_UTF == 16
   #define _LOFTY_SL(s) L ## s
#endif
//! @endcond

/*! Defines a string literal of the default host string literal type (UTF-8 or UTF-16).

@param s
   String literal.
@return
   UTF string literal.
*/
#define LOFTY_SL(s) _LOFTY_SL(s)

/*! Returns the size of a string literal (character array), excluding the trailing NUL character, if present.

@param s
   String literal.
@return
   Size of s, in characters, minus 1 if its last character is NUL.
*/
#define LOFTY_SL_SIZE(s) \
   (LOFTY_COUNTOF(s) - (s[LOFTY_COUNTOF(s) - 1 /*NUL*/] == '\0'))
