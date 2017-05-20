﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2015-2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

/*! @file
Utilities for the execution of code at the end of a scope. */

#ifndef _LOFTY_DEFER_TO_SCOPE_END_HXX
#define _LOFTY_DEFER_TO_SCOPE_END_HXX

#ifndef _LOFTY_HXX
   #error "Please #include <lofty.hxx> before this file"
#endif
#ifdef LOFTY_CXX_PRAGMA_ONCE
   #pragma once
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace _pvt {

/*! @cond
Implementation of LOFTY_DEFER_TO_SCOPE_END(). This cannot define a move constructor (and delete the copy
constructor), because if F is a lambda it will lack means for the destructor to check if it’s been moved out
(no operator bool() support); instead, just don’t declare any move/copy constructors or operators, and rely on
the compiler to implement RVO so not to generate more than one instance for each call to
LOFTY_DEFER_TO_SCOPE_END(). */
template <typename F>
class deferred_to_scope_end {
public:
   /*! Constructor.

   @param fn
      Code to execute when the object goes out of scope.
   */
   deferred_to_scope_end(F fn) :
      m_fn(_std::move(fn)) {
   }

   //! Destructor. Most important part of the class.
   ~deferred_to_scope_end() {
      m_fn();
   }

private:
   //! Code to execute in the destructor.
   F m_fn;
};

/*! Implementation of LOFTY_DEFER_TO_SCOPE_END().

Note that this relies on the compiler to implement return value optimization, which means that one call to
defer_to_scope_end() must result in the creation of a single lofty::_pvt::deferred_to_scope_end instance (see
that class for more information).

@param fn
   Code to execute when the returned object goes out of scope.
@return
   Object that will track the enclosing scope.
*/
template <typename F>
inline deferred_to_scope_end<F> defer_to_scope_end(F fn) {
   return deferred_to_scope_end<F>(_std::move(fn));
}
//! @endcond

}} //namespace lofty::_pvt

namespace lofty {

/*! Allows to execute a statement at the ed of the enclosing scope, regardless of in-flight exceptions. This
is functionally equivalent to a try … finally statement in Python, Java and other languages, or Golang’s defer
keyword.

@code
int i = 1;
{
   ++i;
   LOFTY_DEFER_TO_SCOPE_END(--i);
   risky_operation_that_may_throw();
}
// At this point i is guaranteed to be 1, even if an exception was thrown.
@endcode

This works by encapsulating the statement in a lambda with automatic by-reference capturing, which is then
stored in an object whose lifetime will track that of the enclosing scope; when the object is destructed, the
lambda is executed. In order to avoid violating Lofty’s (and common sense) requirement that destructors never
throw exceptions, the statement contained in the lambda should only perform simple, fail-proof tasks, such as
changing the value or a local or global variable.

@param stmt
   Statement(s) to execute when the containing scope ends.
*/
#define LOFTY_DEFER_TO_SCOPE_END(stmt) \
   auto LOFTY_CPP_APPEND_UID(__defer_to_scope_end_)(::lofty::_pvt::defer_to_scope_end([&] { \
      stmt; \
   }))

} //namespace lofty

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ifndef _LOFTY_DEFER_TO_SCOPE_END_HXX
