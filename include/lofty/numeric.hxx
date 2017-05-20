﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010-2015, 2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#ifndef _LOFTY_NUMERIC_HXX
#define _LOFTY_NUMERIC_HXX

#ifndef _LOFTY_HXX
   #error "Please #include <lofty.hxx> before this file"
#endif
#ifdef LOFTY_CXX_PRAGMA_ONCE
   #pragma once
#endif

#include <climits> // *_MAX *_MIN


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty {

//! Type traits and functions for numeric types.
namespace numeric {}

} //namespace lofty

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace numeric {

/*! Returns true if the argument is negative. It avoids annoying compiler warnings if the argument will never
be negative (i.e. T is unsigned).

@param t
   Value to check for negativity.
@return
   true if t is negative, or false otherwise.
*/
template <typename T>
inline /*constexpr*/ bool is_negative(typename _std::enable_if<_std::is_signed<T>::value, T>::type t) {
   return t < T(0);
}
//! @cond
template <typename T>
inline /*constexpr*/ bool is_negative(typename _std::enable_if<!_std::is_signed<T>::value, T>::type t) {
   LOFTY_UNUSED_ARG(t);
   return false;
}
//! @endcond

}} //namespace lofty::numeric

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace numeric {

//! Defines the minimum value for a numeric type.
template <typename T>
struct min;

//! Defines the maximum value for a numeric type.
template <typename T>
struct max;

//! @cond
// Specialization of min and max for UTF character types (note: wchar_t is not among these).
template <>
struct min<    char> : public std::integral_constant<    char, CHAR_MIN> {};
template <>
struct max<    char> : public std::integral_constant<    char, CHAR_MAX> {};
#if LOFTY_CXX_CHAR16 > 0
template <>
struct min<char16_t> : public std::integral_constant<char16_t,        0> {};
template <>
struct max<char16_t> : public std::integral_constant<char16_t,
   #ifdef UINT_LEAST16_MAX
   UINT_LEAST16_MAX
   #else
   0xffff
   #endif
> {};
#endif //if LOFTY_CXX_CHAR16 > 0
#if LOFTY_CXX_CHAR32 > 0
template <>
struct min<char32_t> : public std::integral_constant<char32_t,        0> {};
template <>
struct max<char32_t> : public std::integral_constant<char32_t,
   #ifdef UINT_LEAST32_MAX
   UINT_LEAST32_MAX
   #else
   0xffffffff
   #endif
> {};
#endif //if LOFTY_CXX_CHAR32 > 0

// Specialization of min and max for integer types.
template <>
struct min<  signed      char> : public std::integral_constant<  signed      char,  SCHAR_MIN> {};
template <>
struct max<  signed      char> : public std::integral_constant<  signed      char,  SCHAR_MAX> {};
template <>
struct min<unsigned      char> : public std::integral_constant<unsigned      char,          0> {};
template <>
struct max<unsigned      char> : public std::integral_constant<unsigned      char,  UCHAR_MAX> {};
template <>
struct min<             short> : public std::integral_constant<             short,   SHRT_MIN> {};
template <>
struct max<             short> : public std::integral_constant<             short,   SHRT_MAX> {};
template <>
struct min<unsigned     short> : public std::integral_constant<unsigned     short,          0> {};
template <>
struct max<unsigned     short> : public std::integral_constant<unsigned     short,  USHRT_MAX> {};
template <>
struct min<               int> : public std::integral_constant<               int,    INT_MIN> {};
template <>
struct max<               int> : public std::integral_constant<               int,    INT_MAX> {};
template <>
struct min<unsigned          > : public std::integral_constant<unsigned          ,          0> {};
template <>
struct max<unsigned          > : public std::integral_constant<unsigned          ,   UINT_MAX> {};
template <>
struct min<              long> : public std::integral_constant<              long,   LONG_MIN> {};
template <>
struct max<              long> : public std::integral_constant<              long,   LONG_MAX> {};
template <>
struct min<unsigned      long> : public std::integral_constant<unsigned      long,          0> {};
template <>
struct max<unsigned      long> : public std::integral_constant<unsigned      long,  ULONG_MAX> {};
template <>
struct min<         long long> : public std::integral_constant<         long long,  LLONG_MIN> {};
template <>
struct max<         long long> : public std::integral_constant<         long long,  LLONG_MAX> {};
template <>
struct min<unsigned long long> : public std::integral_constant<unsigned long long,          0> {};
template <>
struct max<unsigned long long> : public std::integral_constant<unsigned long long, ULLONG_MAX> {};
//! @endcond

}} //namespace lofty::numeric

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ifndef _LOFTY_NUMERIC_HXX
