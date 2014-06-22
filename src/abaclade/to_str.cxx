﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010, 2011, 2012, 2013, 2014
Raffaello D. Di Napoli

This file is part of Abaclade.

Abaclade is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Abaclade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with Abaclade. If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------*/

#include <abaclade.hxx>
#include <abaclade/math.hxx>



////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::to_str_backend – specialization for bool


namespace abc {

void to_str_backend<bool>::set_format(istr const & sFormat) {
   ABC_TRACE_FUNC(this, sFormat);

   auto it(sFormat.cbegin());

   // Add parsing of the format string here.

   // If we still have any characters, they are garbage.
   if (it != sFormat.cend()) {
      ABC_THROW(syntax_error, (
         SL("unexpected character"), sFormat, unsigned(it - sFormat.cbegin())
      ));
   }
}


void to_str_backend<bool>::write(bool b, io::text::writer * ptwOut) {
   ABC_TRACE_FUNC(this, b, ptwOut);

   if (b) {
      ptwOut->write(SL("true"));
   } else {
      ptwOut->write(SL("false"));
   }
}

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_int_to_str_backend_base


namespace abc {

char_t const _int_to_str_backend_base::smc_achIntToStrU[16] = {
   CL('0'), CL('1'), CL('2'), CL('3'), CL('4'), CL('5'), CL('6'), CL('7'), CL('8'), CL('9'),
   CL('A'), CL('B'), CL('C'), CL('D'), CL('E'), CL('F')
};
char_t const _int_to_str_backend_base::smc_achIntToStrL[16] = {
   CL('0'), CL('1'), CL('2'), CL('3'), CL('4'), CL('5'), CL('6'), CL('7'), CL('8'), CL('9'),
   CL('a'), CL('b'), CL('c'), CL('d'), CL('e'), CL('f')
};


_int_to_str_backend_base::_int_to_str_backend_base(unsigned cbInt) :
   m_pchIntToStr(smc_achIntToStrL),
   // Default to generating at least a single zero.
   m_cchWidth(1),
   m_cchBuf(1 /*possible sign*/ + 3 /*max base10 characters per byte*/ * cbInt),
   mc_cbInt(uint8_t(cbInt)),
   // Default to decimal notation.
   m_iBaseOrShift(10),
   // Default padding is with spaces (and won’t be applied by default).
   m_chPad(CL(' ')),
   // A sign will only be displayed if the number is negative and no prefix is applied.
   m_chSign(CL('\0')),
   m_chPrefix0(CL('\0')),
   m_chPrefix1(CL('\0')) {
}


void _int_to_str_backend_base::set_format(istr const & sFormat) {
   ABC_TRACE_FUNC(this, sFormat);

   bool bPrefix(false);
   auto it(sFormat.cbegin());
   char32_t ch;
   if (it == sFormat.cend()) {
      goto default_notation;
   }
   ch = *it++;
   // Display a plus or a space in front of non-negative numbers.
   if (ch == U32CL('+') || ch == U32CL(' ')) {
      // Force this character to be displayed for non-negative numbers.
      m_chSign = char_t(ch);
      if (it == sFormat.cend()) {
         goto default_notation;
      }
      ch = *it++;
   }
   // Prefix with 0b, 0B, 0, 0x or 0X.
   if (ch == U32CL('#')) {
      bPrefix = true;
      if (it == sFormat.cend()) {
         goto default_notation;
      }
      ch = *it++;
   }
   // Pad with zeroes instead of spaces.
   if (ch == U32CL('0')) {
      m_chPad = CL('0');
      if (it == sFormat.cend()) {
         goto default_notation;
      }
      ch = *it++;
   }
   // “Width” - minimum number of digits.
   if (ch >= U32CL('1') && ch <= U32CL('9')) {
      // Undo the default; the following loop will yield at least 1 anyway (because we don’t get
      // here for a 0 – see if above).
      m_cchWidth = 0;
      do {
         m_cchWidth = m_cchWidth * 10 + unsigned(ch) - U32CL('0');
         if (it == sFormat.cend()) {
            goto default_notation;
         }
         ch = *it++;
      } while (ch >= U32CL('0') && ch <= U32CL('9'));
   }

   // We jump in this impossible if to set the default notation when we run out of characters in any
   // of the above blocks. If we do get here without jumping, the last character retrieved and
   // stored in is the requested notation.
   if (false) {
default_notation:
      ch = U32CL('d');
   }

   // Determine which notation to use, which will also yield the approximate number of characters
   // per byte.
   unsigned cchByte;
   switch (ch) {
      case U32CL('b'):
      case U32CL('B'):
      case U32CL('o'):
      case U32CL('x'):
      case U32CL('X'):
         if (bPrefix) {
            m_chPrefix0 = CL('0');
         }
         // Fall through.
      case U32CL('d'):
         switch (ch) {
            case U32CL('b'): // Binary notation, lowercase prefix.
            case U32CL('B'): // Binary notation, uppercase prefix.
               m_chPrefix1 = char_t(ch);
               m_iBaseOrShift = 1;
               cchByte = 8;
               break;
            case U32CL('o'): // Octal notation.
               m_iBaseOrShift = 3;
               cchByte = 3;
               break;
            case U32CL('X'): // Hexadecimal notation, uppercase prefix and letters.
               m_pchIntToStr = smc_achIntToStrU;
               // Fall through.
            case U32CL('x'): // Hexadecimal notation, lowercase prefix and letters.
               m_chPrefix1 = char_t(ch);
               m_iBaseOrShift = 4;
               cchByte = 2;
               break;
            case U32CL('d'): // Decimal notation.
               m_iBaseOrShift = 10;
               cchByte = 3;
               break;
         }
         if (it == sFormat.cend()) {
            break;
         }
         // If we still have any characters, they are garbage (fall through).
      default:
         ABC_THROW(syntax_error, (
            SL("unexpected character"), sFormat, unsigned(it - sFormat.cbegin())
         ));
   }

   // Now we know enough to calculate the required buffer size.
   m_cchBuf = 2 /*prefix or sign*/ + std::max(m_cchWidth, cchByte * mc_cbInt);
}


void _int_to_str_backend_base::add_prefixes_and_write(
   bool bNegative, io::text::writer * ptwOut, mstr * psBuf, mstr::iterator itBufFirstUsed
) const {
   ABC_TRACE_FUNC(this, bNegative, ptwOut, psBuf, itBufFirstUsed);

   auto itEnd(psBuf->cend());
   auto it(itBufFirstUsed);
   // Ensure that at least one digit is generated.
   if (it == itEnd) {
      *--it = U32CL('0');
   }
   // Determine the sign character: only if in decimal notation, and make it a minus sign if the
   // number is negative.
   char_t chSign(m_iBaseOrShift == 10 ? bNegative ? CL('-') : m_chSign : CL('\0'));
   // Decide whether we’ll put a sign last, after the padding.
   bool bSignLast(chSign && m_chPad == CL('0'));
   // Add the sign character if there’s no prefix and the padding is not zeroes.
   if (chSign && m_chPad != CL('0')) {
      *--it = text::codepoint(chSign);
   }
   // Ensure that at least m_cchWidth characters are generated (but reserve a space for the sign).
   auto itFirstDigit(itEnd - ptrdiff_t(m_cchWidth - (bSignLast ? 1 : 0)));
   while (it > itFirstDigit) {
      *--it = text::codepoint(m_chPad);
   }
   // Add prefix or sign (if padding with zeroes), if any.
   if (m_chPrefix0) {
      if (m_chPrefix1) {
         *--it = text::codepoint(m_chPrefix1);
      }
      *--it = text::codepoint(m_chPrefix0);
   } else if (bSignLast) {
      // Add the sign character.
      *--it = text::codepoint(chSign);
   }
   // Write the constructed string.
   ptwOut->write_binary(it.base(), sizeof(char_t) * size_t(itEnd - it), text::encoding::host);
}


template <typename I>
inline void _int_to_str_backend_base::write_impl(I i, io::text::writer * ptwOut) const {
   ABC_TRACE_FUNC(this, i, ptwOut);

   // Create a buffer of sufficient size for binary notation (the largest).
   smstr<2 /* prefix or sign */ + sizeof(I) * CHAR_BIT> sBuf;
   // Use bClear = true since we need to iterate backwards on sBuf, which requires reading its
   // otherwise uninitialized charactes.
   sBuf.set_size_in_chars(m_cchBuf, true);
   auto it(sBuf.end());

   // Generate the digits.
   I iRest(i);
   if (m_iBaseOrShift == 10) {
      // Base 10: must use % and /.
      I iDivider((I(m_iBaseOrShift)));
      while (iRest) {
         I iMod(iRest % iDivider);
         iRest /= iDivider;
         *--it = text::codepoint(m_pchIntToStr[math::abs(iMod)]);
      }
   } else {
      // Base 2 ^ n: can use & and >>.
      I iMask((I(1) << m_iBaseOrShift) - 1);
      while (iRest) {
         *--it = text::codepoint(m_pchIntToStr[iRest & iMask]);
         iRest >>= m_iBaseOrShift;
      }
   }

   // Add prefix or sign, and output to the writer.
   add_prefixes_and_write(numeric::is_negative<I>(i), ptwOut, &sBuf, it);
}


void _int_to_str_backend_base::write_s64(int64_t i, io::text::writer * ptwOut) const {
   write_impl(i, ptwOut);
}


void _int_to_str_backend_base::write_u64(uint64_t i, io::text::writer * ptwOut) const {
   write_impl(i, ptwOut);
}


#if ABC_HOST_WORD_SIZE < 64

void _int_to_str_backend_base::write_s32(int32_t i, io::text::writer * ptwOut) const {
   write_impl(i, ptwOut);
}


void _int_to_str_backend_base::write_u32(uint32_t i, io::text::writer * ptwOut) const {
   write_impl(i, ptwOut);
}


#if ABC_HOST_WORD_SIZE < 32

void _int_to_str_backend_base::write_s16(int16_t i, io::text::writer * ptwOut) const {
   write_impl(i, ptwOut);
}


void _int_to_str_backend_base::write_u16(uint16_t i, io::text::writer * ptwOut) const {
   write_impl(i, ptwOut);
}

#endif //if ABC_HOST_WORD_SIZE < 32
#endif //if ABC_HOST_WORD_SIZE < 64


} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::to_str_backend – specialization for char_ptr_to_str_adapter


namespace abc {

void to_str_backend<char_ptr_to_str_adapter>::write(
   char_ptr_to_str_adapter const & cs, io::text::writer * ptwOut
) {
   size_t cch(text::size_in_chars(cs.m_psz));
   text::encoding enc(text::guess_encoding(cs.m_psz, cs.m_psz + cch));
   _str_to_str_backend::write(cs.m_psz, sizeof(char) * cch, enc, ptwOut);
}

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_ptr_to_str_backend


namespace abc {

char_t const _ptr_to_str_backend::smc_achFormat[] = SL("#x");


_ptr_to_str_backend::_ptr_to_str_backend() {
   ABC_TRACE_FUNC(this);

   m_tsbInt.set_format(smc_achFormat);
}


void _ptr_to_str_backend::set_format(istr const & sFormat) {
   ABC_TRACE_FUNC(this, sFormat);

   auto it(sFormat.cbegin());

   // Add parsing of the format string here.

   // If we still have any characters, they are garbage.
   if (it != sFormat.cend()) {
      ABC_THROW(syntax_error, (
         SL("unexpected character"), sFormat, unsigned(it - sFormat.cbegin())
      ));
   }
}


void _ptr_to_str_backend::_write_impl(uintptr_t iPtr, io::text::writer * ptwOut) {
   ABC_TRACE_FUNC(this, iPtr, ptwOut);

   if (iPtr) {
      m_tsbInt.write(iPtr, ptwOut);
   } else {
      m_tsbStr.write(istr(SL("nullptr")), ptwOut);
   }
}

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::_sequence_to_str_backend


namespace abc {

_sequence_to_str_backend::_sequence_to_str_backend(istr const & sStart, istr const & sEnd) :
   m_sSeparator(SL(", ")),
   m_sStart(sStart),
   m_sEnd(sEnd) {
}


void _sequence_to_str_backend::set_format(istr const & sFormat) {
   ABC_TRACE_FUNC(this, sFormat);

   auto it(sFormat.cbegin());

   // Add parsing of the format string here.

   // If we still have any characters, they are garbage.
   if (it != sFormat.cend()) {
      ABC_THROW(syntax_error, (
         SL("unexpected character"), sFormat, unsigned(it - sFormat.cbegin())
      ));
   }
}


_sequence_to_str_backend::~_sequence_to_str_backend() {
}

} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////

