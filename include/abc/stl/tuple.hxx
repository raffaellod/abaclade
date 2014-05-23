﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2011, 2012, 2013, 2014
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

#ifndef _ABC_STL_TUPLE_HXX
#define _ABC_STL_TUPLE_HXX

#ifndef _ABC_HXX
   #error Please #include <abc.hxx> before this file
#endif
#ifdef ABC_CXX_PRAGMA_ONCE
   #pragma once
#endif

#include <abc/stl/type_traits.hxx>
#include <abc/stl/utility.hxx>



////////////////////////////////////////////////////////////////////////////////////////////////////
// std::_tuple_head


namespace std {

/** Base for a tuple item. For empty T, it derives from T; otherwise, it has a T member. This allows
for empty base optimization (EBO), if the compiler is smart enough.
*/
template <size_t t_i, typename T, bool t_bEmpty = is_empty<T>::value>
class _tuple_head;

// Specialization for empty types: enable EBO.
template <size_t t_i, typename T>
class _tuple_head<t_i, T, true> :
   private T {
public:

   /** Constructor.

   TODO: comment signature.
   */
   _tuple_head() noexcept_false :
      T() {
   }
   _tuple_head(_tuple_head const & th) noexcept_false :
      T(static_cast<T const &>(th)) {
   }
   _tuple_head(_tuple_head && th) noexcept_true :
      T(static_cast<T &&>(th)) {
   }
   template <typename Tr>
   _tuple_head(Tr && t) noexcept_true :
      T(forward<Tr>(t)) {
   }


   /** Assignment operator.

   TODO: comment signature.
   */
   _tuple_head & operator=(_tuple_head const & th) noexcept_false {
      get() = th.get();
      return *this;
   }
   _tuple_head & operator=(_tuple_head && th) noexcept_true {
      get() = move(th.get());
      return *this;
   }


   /** Accessor to the wrapped object.

   TODO: comment signature.
   */
   T & get() noexcept_true {
      return *this;
   }
   T const & get() const noexcept_true {
      return *this;
   }
};

// Specialization non non-empty types.
template <size_t t_i, typename T>
class _tuple_head<t_i, T, false> {
public:

   /** Constructor.

   TODO: comment signature.
   */
   _tuple_head() noexcept_false :
      m_t() {
   }
   _tuple_head(_tuple_head const & th) noexcept_false :
      m_t(th.m_t) {
   }
   _tuple_head(_tuple_head && th) noexcept_true :
      m_t(move(th.m_t)) {
   }
   template <typename Tr>
   _tuple_head(Tr && t) noexcept_true :
      m_t(forward<Tr>(t)) {
   }


   /** Assignment operator.

   TODO: comment signature.
   */
   _tuple_head & operator=(_tuple_head const & th) noexcept_false {
      get() = th.get();
      return *this;
   }
   _tuple_head & operator=(_tuple_head && th) noexcept_true {
      get() = move(th.get());
      return *this;
   }


   /** Accessor to the wrapped object.

   TODO: comment signature.
   */
   T & get() noexcept_true {
      return m_t;
   }
   T const & get() const noexcept_true {
      return m_t;
   }


private:

   /** Internal T instance. */
   T m_t;
};

} //namespace std


////////////////////////////////////////////////////////////////////////////////////////////////////
// std::_tuple_tail


namespace std {

/** Internal implementation of tuple.
*/
#ifdef ABC_CXX_VARIADIC_TEMPLATES

// Base case for the template recursion.
template <size_t t_i>
class _tuple_tail<t_i> {
};

// Template recursion step.
template <size_t t_i, typename T0, typename ... Ts>
class _tuple_tail<t_i, T0, Ts ...> :
   public _tuple_head<t_i, T0>,
   public _tuple_tail<t_i + 1, Ts ...> {

   typedef _tuple_head<t_i, T0> _thead;
   typedef _tuple_tail<t_i + 1, Ts ...> _ttail;

public:

   /** Constructor.

   thead
      Source tuple head.
   ts
      Source elements.
   tt
      Source tuple tail.
   */
   _tuple_tail() noexcept_false :
      _thead(), _ttail() {
   }
   _tuple_tail(T0 thead, Ts ... ts) noexcept_false :
      _thead(move(thead)), _ttail(move(ts) ...) {
   }
   _tuple_tail(_tuple_tail const & tt) noexcept_false :
      _thead(tt.get_thead()), _ttail(tt.get_ttail()) {
   }
   _tuple_tail(_tuple_tail && tt) noexcept_true :
      _thead(move(tt.get_thead())), _ttail(move(tt.get_ttail())) {
   }


   /** Assignment operator.

   tt
      Source tuple tail.
   return
      *this.
   */
   _tuple_tail & operator=(_tuple_tail const & tt) noexcept_false {
      get_thead() = tt.get_thead();
      get_ttail() = tt.get_ttail();
      return *this;
   }
   _tuple_tail & operator=(_tuple_tail && tt) noexcept_true {
      get_thead() = move(tt.get_thead());
      get_ttail() = move(tt.get_ttail());
      return *this;
   }


   /** Returns the embedded _tuple_head.

   return
      Reference to the embedded tuple head.
   */
   _thead & get_thead() noexcept_true {
      return *static_cast<_thead *>(this);
   }
   _thead const & get_thead() const noexcept_true {
      return *static_cast<_thead const *>(this);
   }


   /** Returns the embedded _tuple_tail.

   return
      Reference to the embedded tuple tail.
   */
   _ttail & get_ttail() noexcept_true {
      return *static_cast<_ttail *>(this);
   }
   _ttail const & get_ttail() const noexcept_true {
      return *static_cast<_ttail const *>(this);
   }
};

#else //ifdef ABC_CXX_VARIADIC_TEMPLATES

template <
   size_t t_i, typename T0 = _tuple_void, typename T1 = _tuple_void, typename T2 = _tuple_void,
   typename T3 = _tuple_void, typename T4 = _tuple_void, typename T5 = _tuple_void,
   typename T6 = _tuple_void, typename T7 = _tuple_void, typename T8 = _tuple_void,
   typename T9 = _tuple_void
>
class _tuple_tail :
   public _tuple_head<t_i, T0>,
   public _tuple_tail<t_i + 1, T1, T2, T3, T4, T5, T6, T7, T8, T9, _tuple_void> {

   typedef _tuple_head<t_i, T0> _thead;
   typedef _tuple_tail<t_i + 1, T1, T2, T3, T4, T5, T6, T7, T8, T9, _tuple_void> _ttail;

public:

   /** Constructor.

   tt
      Source tuple tail.
   t0...t9
      Source elements.
   */
   _tuple_tail() noexcept_false :
      _thead(), _ttail() {
   }
   _tuple_tail(_tuple_tail const & tt) noexcept_false :
      _thead(tt.get_thead()),
      _ttail(tt.get_ttail()) {
   }
   _tuple_tail(_tuple_tail && tt) noexcept_true :
      _thead(move(tt.get_thead())),
      _ttail(move(tt.get_ttail())) {
   }
   _tuple_tail(
      T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9
   ) noexcept_false :
      _thead(move(t0)),
      _ttail(
         move(t1), move(t2), move(t3), move(t4), move(t5), move(t6), move(t7), move(t8), move(t9),
         _tuple_void()
      ) {
   }


   /** Assignment operator.

   tt
      Source tuple tail.
   return
      *this.
   */
   _tuple_tail & operator=(_tuple_tail const & tt) noexcept_false {
      get_thead() = tt.get_thead();
      get_ttail() = tt.get_ttail();
      return *this;
   }
   _tuple_tail & operator=(_tuple_tail && tt) noexcept_true {
      get_thead() = move(tt.get_thead());
      get_ttail() = move(tt.get_ttail());
      return *this;
   }


   /** Returns the embedded _tuple_head.

   return
      Reference to the embedded tuple head.
   */
   _thead & get_thead() noexcept_true {
      return *static_cast<_thead *>(this);
   }
   _thead const & get_thead() const noexcept_true {
      return *static_cast<_thead const *>(this);
   }


   /** Returns the embedded _tuple_tail.

   return
      Reference to the embedded tuple tail.
   */
   _ttail & get_ttail() noexcept_true {
      return *static_cast<_ttail *>(this);
   }
   _ttail const & get_ttail() const noexcept_true {
      return *static_cast<_ttail const *>(this);
   }
};

// Base case for the template recursion.
template <size_t t_i>
class _tuple_tail<
   t_i, _tuple_void, _tuple_void, _tuple_void, _tuple_void, _tuple_void, _tuple_void, _tuple_void,
   _tuple_void, _tuple_void, _tuple_void
> {
public:

   /** Constructor.

   tt
      Source tuple tail.
   */
   _tuple_tail() noexcept_true {
   }
   _tuple_tail(_tuple_tail const & tt) noexcept_true {
   }
   _tuple_tail(
      _tuple_void const &, _tuple_void const &, _tuple_void const &, _tuple_void const &,
      _tuple_void const &, _tuple_void const &, _tuple_void const &, _tuple_void const &,
      _tuple_void const &, _tuple_void const &
   ) noexcept_true {
   }


   /** Assignment operator.

   return
      *this.
   */
   _tuple_tail & operator=(_tuple_tail const &) noexcept_true {
      return *this;
   }
};

#endif //ifdef ABC_CXX_VARIADIC_TEMPLATES … else

} //namespace std


////////////////////////////////////////////////////////////////////////////////////////////////////
// std::tuple


namespace std {

/** Fixed-size ordered collection of heterogeneous objects (C++11 § 20.4.2 “Class template tuple”).
*/
#ifdef ABC_CXX_VARIADIC_TEMPLATES

template <typename ... Ts>
class tuple :
   public _tuple_tail<0, Ts ...> {

   typedef _tuple_tail<0, Ts ...> _timpl;

public:

   /** Constructor.

   ts
      Source elements.
   tpl
      Source tuple.
   */
   tuple() noexcept_false :
      _timpl() {
   }
   explicit tuple(Ts ... ts) noexcept_false :
      _timpl(move(ts) ...) {
   }
   tuple(tuple const & tpl) noexcept_false :
      _timpl(static_cast<_timpl const &>(tpl)) {
   }
   tuple(tuple && tpl) noexcept_true :
      _timpl(static_cast<_timpl &&>(tpl)) {
   }


   /** Assignment operator.

   tpl
      Source tuple.
   return
      *this.
   */
   tuple & operator=(tuple const & tpl) noexcept_false {
      _timpl::operator=(static_cast<_timpl const &>(tpl));
      return *this;
   }
   tuple & operator=(tuple && tpl) noexcept_true {
      _timpl::operator=(static_cast<_timpl &&>(tpl));
      return *this;
   }
};

#else //ifdef ABC_CXX_VARIADIC_TEMPLATES

template <
   typename T0 = _tuple_void, typename T1 = _tuple_void, typename T2 = _tuple_void,
   typename T3 = _tuple_void, typename T4 = _tuple_void, typename T5 = _tuple_void,
   typename T6 = _tuple_void, typename T7 = _tuple_void, typename T8 = _tuple_void,
   typename T9 = _tuple_void
>
class tuple :
   public _tuple_tail<0, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> {

   typedef _tuple_tail<0, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> _timpl;

public:

   /** Constructor.

   t0...t9
      Source elements.
   tpl
      Source tuple.
   */
   tuple() noexcept_true :
      _timpl(
         _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 1.
   explicit tuple(T0 t0) noexcept_false :
      _timpl(
         move(t0), _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 2.
   tuple(T0 t0, T1 t1) noexcept_false :
      _timpl(
         move(t0), move(t1), _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 3.
   tuple(T0 t0, T1 t1, T2 t2) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), _tuple_void(), _tuple_void(), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 4.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), _tuple_void(), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 5.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), move(t4), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 6.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), move(t4), move(t5), _tuple_void(), _tuple_void(),
         _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 7.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), move(t4), move(t5), move(t6), _tuple_void(),
         _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 8.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), move(t4), move(t5), move(t6), move(t7),
         _tuple_void(), _tuple_void()
      ) {
   }
   // Overload for tuple of 9.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), move(t4), move(t5), move(t6), move(t7), move(t8),
         _tuple_void()
      ) {
   }
   // Overload for tuple of 10.
   tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9) noexcept_false :
      _timpl(
         move(t0), move(t1), move(t2), move(t3), move(t4), move(t5), move(t6), move(t7), move(t8),
         move(t9)
      ) {
   }
   tuple(tuple const & tpl) noexcept_false :
      _timpl(static_cast<_timpl const &>(tpl)) {
   }
   tuple(tuple && tpl) noexcept_true :
      _timpl(static_cast<_timpl &&>(tpl)) {
   }


   /** Assignment operator.

   tpl
      Source tuple.
   return
      *this.
   */
   tuple & operator=(tuple const & tpl) noexcept_false {
      _timpl::operator=(static_cast<_timpl const &>(tpl));
      return *this;
   }
   tuple & operator=(tuple && tpl) noexcept_true {
      _timpl::operator=(static_cast<_timpl &&>(tpl));
      return *this;
   }
};

#endif //ifdef ABC_CXX_VARIADIC_TEMPLATES … else

} //namespace std


////////////////////////////////////////////////////////////////////////////////////////////////////
// std::tuple_element


namespace std {

/** Defines as its member type the type of the Nth element in the tuple (C++11 § 20.4.2.5 “Tuple
helper classes”).
*/
template <size_t t_i, typename T>
struct tuple_element;

#ifdef ABC_CXX_VARIADIC_TEMPLATES

// Recursion: remove 1 from the index, and 1 item from the tuple.
template <size_t t_i, typename T0, typename ... Ts>
struct tuple_element<t_i, tuple<T0, Ts ...>> :
   public tuple_element<t_i - 1, tuple<Ts ...>> {
};

// Base recursion step.
template <typename T0, typename ... Ts>
struct tuple_element<0, tuple<T0, Ts ...>> {
   typedef T0 type;
};

#else //ifdef ABC_CXX_VARIADIC_TEMPLATES

template <size_t t_i, typename T>
struct tuple_element;

#define ABC_SPECIALIZE_tuple_element_FOR_INDEX(i) \
   template < \
      typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, \
      typename T7, typename T8, typename T9 \
   > \
   struct tuple_element<i, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>> { \
      typedef T ## i type; \
   };
ABC_SPECIALIZE_tuple_element_FOR_INDEX(0)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(1)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(2)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(3)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(4)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(5)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(6)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(7)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(8)
ABC_SPECIALIZE_tuple_element_FOR_INDEX(9)
#undef ABC_SPECIALIZE_tuple_element_FOR_INDEX

#endif //ifdef ABC_CXX_VARIADIC_TEMPLATES … else

} //namespace std


////////////////////////////////////////////////////////////////////////////////////////////////////
// std::get (tuple)


namespace std {

#ifndef ABC_CXX_VARIADIC_TEMPLATES

/** Null type, used to reduce the number of tuple items from the preset maximum.
*/
struct _tuple_void {
};


/** Helper for get<>(tuple). Being a class, it can be partially specialized, which is necessary to
make it work.
*/
template <
   size_t t_i, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
   typename T6, typename T7, typename T8, typename T9
>
struct _tuple_get_helper;

#define ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(i) \
   template < \
      typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, \
      typename T7, typename T8, typename T9 \
   > \
   struct _tuple_get_helper<i, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> { \
   \
      inline static T ## i & get( \
         tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> & tpl \
      ) noexcept_true { \
         return static_cast<_tuple_head<i, T ## i> &>(tpl).get(); \
      } \
      inline static T ## i const & get( \
         tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> const & tpl \
      ) noexcept_true { \
         return static_cast<_tuple_head<i, T ## i> const &>(tpl).get(); \
      } \
   };
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(0)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(1)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(2)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(3)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(4)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(5)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(6)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(7)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(8)
ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX(9)
#undef ABC_SPECIALIZE_tuple_get_helper_FOR_INDEX

#endif //ifndef ABC_CXX_VARIADIC_TEMPLATES


/** Retrieves an element from a tuple (C++11 § 20.4.2.6 “Element access”).

tpl
   Tuple from which to extract an element.
return
   Reference to the tuple element.
*/
#ifdef ABC_CXX_VARIADIC_TEMPLATES

template <size_t t_i, typename ... Ts>
inline typename tuple_element<t_i, tuple<Ts ...>>::type & get(tuple<Ts ...> & tpl) noexcept_true {
   return static_cast<_tuple_head<
      t_i, typename tuple_element<t_i, tuple<Ts ...>>::type
   > &>(tpl).get();
}
template <size_t t_i, typename ... Ts>
inline typename tuple_element<t_i, tuple<Ts ...>>::type const & get(
   tuple<Ts ...> const & tpl
) noexcept_true {
   return static_cast<_tuple_head<
      t_i, typename tuple_element<t_i, tuple<Ts ...>>::type
   > const &>(tpl).get();
}

#else //ifdef ABC_CXX_VARIADIC_TEMPLATES

template <
   size_t t_i, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
   typename T6, typename T7, typename T8, typename T9
>
inline typename tuple_element<t_i, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>>::type & get(
   tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> & tpl
) noexcept_true {
   return _tuple_get_helper<t_i, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::get(tpl);
}
template <
   size_t t_i, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
   typename T6, typename T7, typename T8, typename T9
>
inline typename tuple_element<t_i, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>>::type const & get(
   tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> const & tpl
) noexcept_true {
   return _tuple_get_helper<t_i, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::get(tpl);
}

#endif //ifdef ABC_CXX_VARIADIC_TEMPLATES … else

} //namespace std


////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //ifndef _ABC_STL_TUPLE_HXX

