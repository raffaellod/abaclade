﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010-2015 Raffaello D. Di Napoli

This file is part of Abaclade.

Abaclade is free software: you can redistribute it and/or modify it under the terms of the GNU
Lesser General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Abaclade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with Abaclade. If
not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------*/

/*! @file
Defines C++ preprocessor macros to perform useful code generation tasks. */

#ifndef _ABACLADE_HXX_INTERNAL
   #error "Please #include <abaclade.hxx> instead of this file"
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Expands into its argument, working around a MSC16 bug concerning __VA_ARGS__.

Due to a confirmed bug in MSC16 (see <http://connect.microsoft.com/VisualStudio/feedback/details/
380090/variadic-macro-replacement>), all variadic macros need to wrap their implementation in an
expansion of ABC_UWE() to have the result of the implementation expanded once more, which will
result in __VA_ARGS__ being expanded correctly, working around the bug. 

The name of this macro stands for Ugly Workaround Expansion.

@param x
   Argument to expand into.
@return
   x.
*/
#define ABC_UWE(x) x

/*! Expands into 1 or 0 depending on whether its arguments list expands into nothing or not.

@param ...
   Arguments to check.
@return
   1 if called with no arguments (after their expansion), or 0 otherwise.
*/
#define ABC_CPP_IS_EMPTY(...) \
   ABC_CPP_NOT(ABC_CPP_CAT2( \
      ABC_CPP_CAT2(_ABC_CPP_IS_EMPTY_RET_, _ABC_CPP_1_IF_NOT_CALLABLE(__VA_ARGS__)), \
      ABC_CPP_CAT2(_,                      _ABC_CPP_2_IF_EMPTY(__VA_ARGS__)) \
   ))

//! @cond
/* Only combination of the outputs of _ABC_CPP_1_IF_NOT_CALLABLE() and _ABC_CPP_2_IF_EMPTY() that
will expand into “0”. */
#define _ABC_CPP_IS_EMPTY_RET_1_2 \
   0

/* Expands into “1” if the arguments don’t expand into something that can be called. Why “1”?
Because that’s what we get from reusing _ABC_CPP_LIST_COUNT_0() instead of creating a nearly-
identical macro. */
#define _ABC_CPP_1_IF_NOT_CALLABLE(...) \
   _ABC_CPP_LIST_COUNT_0(_ABC_CPP_COMMA_IF_CALL __VA_ARGS__)

/* Expands into “2” if the arguments expand into nothing. Why “2”? Because that’s what we get from
reusing _ABC_CPP_LIST_COUNT_0() instead of creating a nearly-identical macro. */
#define _ABC_CPP_2_IF_EMPTY(...) \
   _ABC_CPP_LIST_COUNT_0(_ABC_CPP_COMMA_IF_CALL __VA_ARGS__ ())

// Expands into “,” if called.
#define _ABC_CPP_COMMA_IF_CALL(...) ,
//! @endcond

/*! Expands into the count of its arguments.

@param ...
   Arguments to count.
@return
   Count of the arguments.
*/
#define ABC_CPP_LIST_COUNT(...) \
   ABC_CPP_CAT2(_ABC_CPP_LIST_COUNT_, ABC_CPP_IS_EMPTY(__VA_ARGS__))(__VA_ARGS__)

//! @cond
// Implementation of ABC_CPP_LIST_COUNT() for when ABC_CPP_IS_EMPTY() returns 1 (no arguments).
#define _ABC_CPP_LIST_COUNT_1(...) \
   0

// Implementation of ABC_CPP_LIST_COUNT() for when ABC_CPP_IS_EMPTY() returns 0 (not empty).
#define _ABC_CPP_LIST_COUNT_0(...) \
   ABC_UWE(_ABC_CPP_LIST_COUNT_1_IMPL(__VA_ARGS__, \
      99, 98, 97, 96, 95, 94, 93, 92, 91, 90, \
      89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
      79, 78, 77, 76, 75, 74, 73, 72, 71, 70, \
      69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
      59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
      49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
      39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
      29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
      19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
       9,  8,  7,  6,  5,  4,  3,  2,  1,     \
      _ \
   ))

// Returns a number provided by _ABC_CPP_LIST_COUNT_0() after shifting it by that macro’s arguments.
#define _ABC_CPP_LIST_COUNT_1_IMPL( \
      _99, _98, _97, _96, _95, _94, _93, _92, _91, _90, \
      _89, _88, _87, _86, _85, _84, _83, _82, _81, _80, \
      _79, _78, _77, _76, _75, _74, _73, _72, _71, _70, \
      _69, _68, _67, _66, _65, _64, _63, _62, _61, _60, \
      _59, _58, _57, _56, _55, _54, _53, _52, _51, _50, \
      _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, \
      _39, _38, _37, _36, _35, _34, _33, _32, _31, _30, \
      _29, _28, _27, _26, _25, _24, _23, _22, _21, _20, \
      _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, \
       _9,  _8,  _7,  _6,  _5,  _4,  _3,  _2,  _1,      \
      count, ... \
   ) \
   count
//! @endcond

/*! Expands into a joined version of the two provided tokens. Necessary to implement the more
generic ABC_CPP_CAT().

@param token1
   Left token to join.
@param token2
   Right token to join.
@return
   Concatenation of token1 and token2, in this order.
*/
#define ABC_CPP_CAT2(token1, token2) \
   _ABC_CPP_CAT2_IMPL(token1, token2)

//! @cond
#define _ABC_CPP_CAT2_IMPL(token1, token2) \
   token1 ## token2
//! @endcond

/*! Expands into a joined version of the provided tokens.

@param ...
   Tokens to join.
@return
   Tokens joined in the order in which they were provided.
*/
#define ABC_CPP_CAT(...) \
   ABC_UWE(ABC_CPP_CAT2(_ABC_CPP_CAT_, ABC_CPP_LIST_COUNT(__VA_ARGS__))(__VA_ARGS__))

//! @cond
#define _ABC_CPP_CAT_1(token1) \
   _ABC_CPP_CAT_1_IMPL(token1)
#define _ABC_CPP_CAT_1_IMPL(token1) \
   token1
#define _ABC_CPP_CAT_2(token1, token2) \
   _ABC_CPP_CAT_2_IMPL(token1, token2)
#define _ABC_CPP_CAT_2_IMPL(token1, token2) \
   token1 ## token2
#define _ABC_CPP_CAT_3(t1, t2, t3) \
   _ABC_CPP_CAT_3_IMPL(t1, t2, t3)
#define _ABC_CPP_CAT_3_IMPL(t1, t2, t3) \
   t1 ## t2 ## t3
#define _ABC_CPP_CAT_4(t1, t2, t3, t4) \
   _ABC_CPP_CAT_4_IMPL(t1, t2, t3, t4)
#define _ABC_CPP_CAT_4_IMPL(t1, t2, t3, t4) \
   t1 ## t2 ## t3 ## t4
#define _ABC_CPP_CAT_5(t1, t2, t3, t4, t5) \
   _ABC_CPP_CAT_5_IMPL(t1, t2, t3, t4, t5)
#define _ABC_CPP_CAT_5_IMPL(t1, t2, t3, t4, t5) \
   t1 ## t2 ## t3 ## t4 ## t5
#define _ABC_CPP_CAT_6(t1, t2, t3, t4, t5, t6) \
   _ABC_CPP_CAT_6_IMPL(t1, t2, t3, t4, t5, t6)
#define _ABC_CPP_CAT_6_IMPL(t1, t2, t3, t4, t5, t6) \
   t1 ## t2 ## t3 ## t4 ## t5 ## t6
#define _ABC_CPP_CAT_7(t1, t2, t3, t4, t5, t6, t7) \
   _ABC_CPP_CAT_7_IMPL(t1, t2, t3, t4, t5, t6, t7)
#define _ABC_CPP_CAT_7_IMPL(t1, t2, t3, t4, t5, t6, t7) \
   t1 ## t2 ## t3 ## t4 ## t5 ## t6 ## t7
#define _ABC_CPP_CAT_8(t1, t2, t3, t4, t5, t6, t7, t8) \
   _ABC_CPP_CAT_8_IMPL(t1, t2, t3, t4, t5, t6, t7, t8)
#define _ABC_CPP_CAT_8_IMPL(t1, t2, t3, t4, t5, t6, t7, t8) \
   t1 ## t2 ## t3 ## t4 ## t5 ## t6 ## t7 ## t8
#define _ABC_CPP_CAT_9(t1, t2, t3, t4, t5, t6, t7, t8, t9) \
   _ABC_CPP_CAT_9_IMPL(t1, t2, t3, t4, t5, t6, t7, t8, t9)
#define _ABC_CPP_CAT_9_IMPL(t1, t2, t3, t4, t5, t6, t7, t8, t9) \
   t1 ## t2 ## t3 ## t4 ## t5 ## t6 ## t7 ## t8 ## t9
#define _ABC_CPP_CAT_10(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) \
   _ABC_CPP_CAT_10_IMPL(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10)
#define _ABC_CPP_CAT_10_IMPL(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) \
//! @endcond

/*! Expands into a string version of the specified token.

@param x
   Expression to convert to a string literal.
@return
   String literal representing the specified expression.
*/
#define ABC_CPP_TOSTRING(x) \
   _ABC_CPP_TOSTRING_IMPL(x)

//! @cond
#define _ABC_CPP_TOSTRING_IMPL(x) \
   #x
//! @endcond

/*! Expands into a mostly unique number prefixed by the specified token. Uniqueness is not
guaranteed on all platforms.

@param s
   Prefix for the unique identifier.
@return
   Unique identifier.
*/
#if ABC_HOST_CXX_CLANG || ABC_HOST_CXX_GCC || ABC_HOST_CXX_MSC
   #define ABC_CPP_APPEND_UID(s) \
      ABC_CPP_CAT2(s, __COUNTER__)
#else
   #define ABC_CPP_APPEND_UID(s) \
      ABC_CPP_CAT2(s, __LINE__)
#endif

/*! Expands into either the first argument or the second, depending on whether a condition evaluates
to non-zero or 0 (see ABC_CPP_NOT() ), respectively.

@param condition
   Condition to evaluate.
@param true
   Expression to expand into if condition evaluates to non-zero.
@param false
   Expression to expand into if condition evaluates to 0.
@return
   Expansion of true or false.
*/
#define ABC_CPP_IF(condition, true, false) \
   ABC_CPP_CAT2(_ABC_CPP_IF_, ABC_CPP_NOT(condition))(true, false)

//! @cond
#define _ABC_CPP_IF_0(true, false) \
   true
#define _ABC_CPP_IF_1(true, false) \
   false
//! @endcond

/*! Used with _ABC_CPP_MAKE_CHECK_RET_ONE(); it expands into 1 or 0 depending on whether the latter
is expanded or not.

@param ...
   Unused.
*/
// Comma after 0 necessary just to provide something for _ABC_CPP_CHECK_EXPAND’s “...”.
#define ABC_CPP_CHECK(...) \
   ABC_UWE(_ABC_CPP_CHECK_EXPAND(__VA_ARGS__, 0, ))

//! @cond
#define _ABC_CPP_CHECK_EXPAND(ignore, ret, ...) \
   ret

/*! Expands into a placeholder and 1, which will replace the 0 if passed as argument to
ABC_CPP_CHECK(). */
// TODO: is a comma after 1 necessary?
#define _ABC_CPP_MAKE_CHECK_RET_ONE(...) \
   placeholder, 1
//! @endcond

/*! Expands into either 1 or 0 depending on whether the argument is a tuple or not.

@param x
   Expression.
@return
   1 if the argument is a tuple, or 0 otherwise.
*/
#define ABC_CPP_IS_TUPLE(x) \
   ABC_CPP_CHECK(_ABC_CPP_MAKE_CHECK_RET_ONE x)

/*! Expands into either 1 or 0 depending on whether the argument expands into 0 or anything else,
respectively.

@param x
   Expression to negate.
@return
   0 or 1.
*/
#define ABC_CPP_NOT(x) \
   ABC_CPP_CHECK(ABC_CPP_CAT2(_ABC_CPP_NOT_, x))

//! @cond
#define _ABC_CPP_NOT_0 \
   _ABC_CPP_MAKE_CHECK_RET_ONE()
//! @endcond

/*! Expands into the invocation of the specified macro once for each of the remaining scalar
arguments.

@param macro
   Macro to invoke on each item in the remaining arguments.
@return
   Result of the expansion of macro.
*/
#define ABC_CPP_LIST_WALK(macro, ...) \
   ABC_UWE(ABC_CPP_CAT2(_ABC_CPP_LIST_W_, ABC_CPP_LIST_COUNT(__VA_ARGS__))(macro, __VA_ARGS__))

//! @cond
#define _ABC_CPP_LIST_W_0(macro)
#define _ABC_CPP_LIST_W_1(macro, head) macro(head)
#define _ABC_CPP_LIST_W_2(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_1(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_3(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_2(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_4(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_3(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_5(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_4(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_6(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_5(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_7(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_6(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_8(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_7(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_9(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_8(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_10(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_9(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_11(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_10(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_12(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_11(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_13(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_12(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_14(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_13(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_15(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_14(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_16(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_15(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_17(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_16(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_18(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_17(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_19(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_18(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_20(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_19(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_21(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_20(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_22(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_21(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_23(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_22(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_24(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_23(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_25(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_24(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_26(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_25(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_27(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_26(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_28(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_27(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_29(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_28(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_30(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_29(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_31(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_30(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_32(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_31(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_33(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_32(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_34(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_33(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_35(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_34(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_36(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_35(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_37(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_36(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_38(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_37(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_39(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_38(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_40(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_39(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_41(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_40(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_42(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_41(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_43(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_42(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_44(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_43(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_45(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_44(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_46(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_45(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_47(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_46(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_48(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_47(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_49(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_48(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_50(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_49(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_51(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_50(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_52(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_51(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_53(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_52(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_54(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_53(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_55(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_54(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_56(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_55(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_57(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_56(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_58(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_57(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_59(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_58(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_60(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_59(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_61(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_60(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_62(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_61(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_63(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_62(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_64(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_63(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_65(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_64(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_66(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_65(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_67(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_66(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_68(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_67(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_69(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_68(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_70(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_69(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_71(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_70(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_72(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_71(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_73(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_72(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_74(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_73(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_75(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_74(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_76(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_75(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_77(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_76(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_78(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_77(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_79(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_78(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_80(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_79(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_81(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_80(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_82(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_81(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_83(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_82(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_84(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_83(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_85(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_84(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_86(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_85(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_87(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_86(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_88(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_87(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_89(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_88(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_90(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_89(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_91(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_90(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_92(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_91(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_93(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_92(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_94(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_93(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_95(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_94(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_96(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_95(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_97(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_96(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_98(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_97(m, __VA_ARGS__))
#define _ABC_CPP_LIST_W_99(m, h, ...) m(h) ABC_UWE(_ABC_CPP_LIST_W_98(m, __VA_ARGS__))
//! @endcond

/*! Expands into the invocation of the specified macro once for each of the remaining tuples passed
as arguments.

@param macro
   Macro to invoke on each tuple in the remaining arguments.
@return
   Result of the expansion of macro.
*/
#define ABC_CPP_TUPLELIST_WALK(macro, ...) \
   ABC_UWE(ABC_CPP_CAT2(_ABC_CPP_TUPLELIST_W_, ABC_CPP_LIST_COUNT(__VA_ARGS__))(macro, __VA_ARGS__))

//! @cond
#define _ABC_CPP_TUPLELIST_W_0(macro)
#define _ABC_CPP_TUPLELIST_W_1(macro, head) macro head
#define _ABC_CPP_TUPLELIST_W_2(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_1(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_3(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_2(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_4(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_3(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_5(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_4(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_6(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_5(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_7(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_6(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_8(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_7(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_9(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_8(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_10(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_9(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_11(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_10(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_12(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_11(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_13(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_12(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_14(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_13(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_15(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_14(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_16(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_15(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_17(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_16(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_18(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_17(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_19(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_18(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_20(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_19(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_21(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_20(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_22(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_21(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_23(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_22(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_24(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_23(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_25(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_24(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_26(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_25(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_27(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_26(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_28(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_27(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_29(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_28(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_30(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_29(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_31(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_30(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_32(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_31(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_33(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_32(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_34(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_33(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_35(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_34(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_36(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_35(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_37(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_36(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_38(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_37(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_39(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_38(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_40(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_39(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_41(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_40(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_42(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_41(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_43(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_42(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_44(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_43(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_45(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_44(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_46(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_45(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_47(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_46(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_48(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_47(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_49(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_48(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_50(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_49(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_51(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_50(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_52(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_51(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_53(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_52(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_54(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_53(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_55(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_54(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_56(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_55(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_57(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_56(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_58(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_57(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_59(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_58(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_60(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_59(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_61(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_60(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_62(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_61(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_63(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_62(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_64(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_63(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_65(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_64(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_66(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_65(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_67(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_66(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_68(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_67(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_69(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_68(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_70(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_69(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_71(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_70(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_72(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_71(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_73(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_72(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_74(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_73(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_75(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_74(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_76(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_75(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_77(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_76(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_78(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_77(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_79(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_78(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_80(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_79(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_81(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_80(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_82(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_81(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_83(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_82(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_84(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_83(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_85(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_84(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_86(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_85(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_87(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_86(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_88(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_87(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_89(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_88(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_90(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_89(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_91(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_90(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_92(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_91(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_93(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_92(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_94(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_93(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_95(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_94(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_96(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_95(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_97(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_96(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_98(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_97(m, __VA_ARGS__))
#define _ABC_CPP_TUPLELIST_W_99(m, h, ...) m h ABC_UWE(_ABC_CPP_TUPLELIST_W_98(m, __VA_ARGS__))
//! @endcond