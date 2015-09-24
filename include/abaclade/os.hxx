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

#ifndef _ABACLADE_OS_HXX
#define _ABACLADE_OS_HXX

#ifndef _ABACLADE_HXX
   #error "Please #include <abaclade.hxx> before this file"
#endif
#ifdef ABC_CXX_PRAGMA_ONCE
   #pragma once
#endif

#include <abaclade/os/path.hxx>


////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc {

//! Provides facilities to interact with the underlying OS.
namespace os {}

} //namespace abc

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace os {

//! A path failed validation. Path validation is typically file system- or OS-dependent.
class ABACLADE_SYM invalid_path : public generic_error {
public:
   /*! Constructor.

   @param opInvalid
      Path that failed validation.
   @param err
      OS-defined error number associated to the exception.
   */
   explicit invalid_path(abc::os::path const & opInvalid, errint_t err = 0);

   /*! Copy onstructor.

   @param x
      Source object.
   */
   invalid_path(invalid_path const & x);

   //! Destructor.
   virtual ~invalid_path();

   /*! Copy-assignment operator.

   @param x
      Source object.
   @return
      *this.
   */
   invalid_path & operator=(invalid_path const & x);

   /*! Returns the path that couldn’t be found.

   @return
      Path that couldn’t be found at the moment it was accessed.
   */
   os::path const & path() const {
      return m_opInvalid;
   }

private:
   //! Path that caused the error.
   os::path m_opInvalid;
};

}} //namespace abc::os

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc { namespace os {

//! A path could not be found on the file system.
class ABACLADE_SYM path_not_found : public generic_error {
public:
   /*! Constructor.

   @param opNotFound
      Path that couldn’t be found.
   @param err
      OS-defined error number associated to the exception.
   */
   explicit path_not_found(abc::os::path const & opNotFound, errint_t err = 0);

   /*! Copy onstructor.

   @param x
      Source object.
   */
   path_not_found(path_not_found const & x);

   //! Destructor.
   virtual ~path_not_found();

   /*! Copy-assignment operator.

   @param x
      Source object.
   @return
      *this.
   */
   path_not_found & operator=(path_not_found const & x);

   /*! Returns the path that couldn’t be found.

   @return
      Path that couldn’t be found at the moment it was accessed.
   */
   os::path const & path() const {
      return m_opNotFound;
   }

private:
   //! Path that caused the error.
   os::path m_opNotFound;
};

}} //namespace abc::os

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ifndef _ABACLADE_OS_HXX
