﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010-2016 Raffaello D. Di Napoli

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

#include <abaclade.hxx>
#include <abaclade/text.hxx>
#include <abaclade/text/char_ptr_to_str_adapter.hxx>


////////////////////////////////////////////////////////////////////////////////////////////////////

namespace abc {

void to_text_ostream<text::char_ptr_to_str_adapter>::write(
   text::char_ptr_to_str_adapter const & cs, io::text::ostream * ptos
) {
   void const * p;
   std::size_t cb;
   text::encoding enc;
   if (cs.m_psz) {
      p = cs.m_psz;
      std::size_t cch = text::size_in_chars(cs.m_psz);
      enc = text::guess_encoding(cs.m_psz, cs.m_psz + cch);
      cb = sizeof cs.m_psz[0] * cch;
   } else {
      static char_t const sc_szNull[] = ABC_SL("<nullptr>");
      p = sc_szNull;
      enc = text::encoding::host;
      cb = sizeof sc_szNull - sizeof sc_szNull[0] /*NUL*/;
   }
   text::_pvt::str_to_text_ostream::write(p, cb, enc, ptos);
}

} //namespace abc
