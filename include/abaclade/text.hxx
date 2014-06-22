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

#ifndef _ABACLADE_HXX
   #error Please #include <abaclade.hxx> instead of this file
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::text globals

namespace abc {
namespace text {

/** Recognized text encodings. Little endians should be listed before big endians; some code relies
on this.
*/
ABC_ENUM(encoding, \
   /** Unknown/undetermined encoding. */ \
   (unknown,      0), \
   /** UTF-8 encoding. */ \
   (utf8,         1), \
   /** UTF-16 Little Endian encoding. */ \
   (utf16le,      2), \
   /** UTF-16 Big Endian encoding. */ \
   (utf16be,      3), \
   /** UTF-32 Little Endian encoding. */ \
   (utf32le,      4), \
   /** UTF-32 Big Endian encoding. */ \
   (utf32be,      5), \
   /** ISO-8859-1 encoding. Only supported in detection and handling, but not as internal string
    * representation. */ \
   (iso_8859_1,   6), \
   /** Windows-1252 encoding. Only supported in detection and handling, but not as internal string
    * representation. */ \
   (windows_1252, 7), \
   /** EBCDIC encoding. Only supported in detection and handling, but not as internal string
    * representation. */ \
   (ebcdic,       8), \
   /** UTF-16 encoding (host endianness). */ \
   (utf16_host,   (ABC_HOST_LITTLE_ENDIAN ? utf16le : utf16be)), \
   /** UTF-32 encoding (host endianness). */ \
   (utf32_host,   (ABC_HOST_LITTLE_ENDIAN ? utf32le : utf32be)), \
   /** Default host encoding. */ \
   (host,         (ABC_HOST_UTF == 8 ? utf8 : (ABC_HOST_UTF == 16 ? utf16_host : utf32_host))) \
);


/** Recognized line terminators.
*/
ABC_ENUM(line_terminator, \
   /** Unknown/undetermined line terminator. */ \
   (unknown, 0), \
   /** Old Mac style: Carriage Return, '\r'. */ \
   (cr,      1), \
   /** Unix/POSIX style: Line Feed, '\n'. */ \
   (lf,      2), \
   /** DOS/Windows style: Carriage Return + Line Feed, '\r', '\n'. */ \
   (cr_lf,   3), \
   /** EBCDIC style: Next Line, '\x15'. */ \
   (nel,     4), \
   /** Default host line terminator. */ \
   (host,    (ABC_HOST_API_WIN32 ? cr_lf : lf)) \
);


/** This must be used to replace any invalid char32_t value. */
char32_t const replacement_char(0x00fffd);

/** Maximum run length for the encoding of a code point, in any encoding.

Technically, 6 is an illegal UTF-8 run, but it’s possible due to the way bits are encoded, so it’s
here. */
size_t const max_codepoint_length(6);


/** Casts a single character into a code point.

ch
   Character.
return
   Equivalent code point.
*/
inline char32_t codepoint(char_t ch) {
#if ABC_HOST_UTF == 8
   return char32_t(uint8_t(ch));
#elif ABC_HOST_UTF == 16
   return char32_t(ch);
#endif
}


/** Provides an estimate of the space, in bytes, necessary to store a string, transcoded in a
different encoding. For example, transcoding from UTF-32 to UTF-16 will yield half the source size,
although special cases such as surrogates might make the estimate too low.

encSrc
   Source encoding.
pSrc
   Pointer to the source string.
cbSrc
   Length of the source string, in bytes.
encDst
   Target encoding.
return
   Estimated size necessary for the destination string, in bytes.
*/
ABACLADE_SYM size_t estimate_transcoded_size(
   encoding encSrc, void const * pSrc, size_t cbSrc, encoding encDst
);


/** Returns the character size, in bytes, for the specified charset encoding, or 0 for non-charset
encodings (e.g. identity_encoding).

enc
   Desired encoding.
return
   Size of a character (not a code point, which can require more than one character) for the
   specified encoding, in bytes.
*/
ABACLADE_SYM size_t get_encoding_size(encoding enc);


/** Returns a line terminator string corresponding to the specified line_terminator value.

lterm
   Desired line terminator.
return
   String with the requested line terminator sequence.
*/
ABACLADE_SYM istr get_line_terminator_str(line_terminator lterm);


/** Tries to guess the encoding of a sequence of bytes, optionally also taking into account the
total number of bytes in the source of which the buffer is the beginning.

While this function can check for validity of some encodings, it does not guarantee that, for
example, for a return value of utf8_encoding utf8_str_traits::is_valid() will return true for the
same buffer.
TODO: why not guarantee validity? It would help weed out more encodings with fewer bytes.

pchBegin
   Pointer to the beginning of the buffer to scan for encoding clues.
pchEnd
   Pointer to beyond the end of the buffer.
cbSrcTotal
   Total size, in bytes, of a larger string of which *pBuf is the beginning.
pcbBom
   Pointer to a variable that will receive the size of the Byte Order Mark if found at the beginning
   of the string, in bytes, or 0 otherwise.
return
   Detected encoding of the string pointed to by pBuf.
*/
ABACLADE_SYM encoding guess_encoding(
   void const * pBufBegin, void const * pBufEnd, size_t cbSrcTotal = 0, size_t * pcbBom = nullptr
);


/** Tries to guess the line terminators employed in a string.

pchBegin
   Pointer to the first character of the string to scan for a line terminator sequence.
pchEnd
   Pointer to beyond the last character of the string.
return
   Detected line terminator sequence.
*/
ABACLADE_SYM line_terminator guess_line_terminator(char_t const * pchBegin, char_t const * pchEnd);


/** Checks if a UTF-32 character is a valid Unicode code point, which means that its ordinal value
must be included in the interval [0, U+10FFFF] (see Unicode Standard 6.2 § 2.4 “Code Points and
Characters”).

ch
   UTF-32 character to validate.
return
   true if the character is a valid code point, or false otherwise.
*/
inline /*constexpr*/ bool is_codepoint_valid(char32_t ch) {
   return ch <= 0x10ffff;
}


/** Calculates the length of a NUL-terminated string, in characters.

psz
   Pointer to the NUL-terminated string of which to calculate the length.
return
   Length of the string pointed to by psz, in characters.
*/
template <typename C>
inline size_t size_in_chars(C const * psz) {
//   ABC_TRACE_FUNC(psz);

   C const * pch(psz);
   while (*pch) {
      ++pch;
   }
   return size_t(pch - psz);
}


/** Converts from one character encoding to another. All pointed-by variables are updated to discard
the bytes used in the conversion; the number of bytes written is returned.

UTF validity: not necessary; invalid sequences are replaced with text::replacement_char.

encSrc
   Encoding of the string pointed to by *ppSrc.
ppSrc
   Pointer to a pointer to the source string; the pointed-to pointer will be incremented as
   characters are transcoded.
pcbSrc
   Pointer to a variable that holds the size of the string pointed to by *ppSrc, and that will be
   decremented by the number of source characters transcoded.
encDst
   Encoding of the string pointed to by *ppDst.
ppDst
   Pointer to a pointer to the destination buffer; the pointed-to pointer will be incremented as
   characters are stored in the buffer. If nullptr is passed no writes will be attempted, but all
   the calculations of source/destination used bytes will still take place.
pcbDstMax
   Pointer to a variable that holds the size of the buffer pointed to by *ppDst, and that will be
   decremented by the number of characters stored in the buffer.
return
   Count of bytes that were written to **ppDst.
*/
ABACLADE_SYM size_t transcode(
   std::nothrow_t const &,
   encoding encSrc, void const ** ppSrc, size_t * pcbSrc,
   encoding encDst, void       ** ppDst, size_t * pcbDstMax
);

} //namespace text
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::text::error


namespace abc {
namespace text {

/** A text encoding or decoding error occurred.
*/
class ABACLADE_SYM error :
   public virtual generic_error {
public:

   /** Constructor.
   */
   error();


   /** See abc::generic_error::init().
   */
   void init(errint_t err = 0);
};

} //namespace text
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::text::decode_error


namespace abc {
namespace text {

/** A text decoding error occurred.
*/
class ABACLADE_SYM decode_error :
   public virtual error {
public:

   /** Constructor.
   */
   decode_error();


   /** See abc::text::error::init().
   */
   void init(errint_t err = 0);
};

} //namespace text
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::text::encode_error


namespace abc {
namespace text {

/** A text encoding error occurred.
*/
class ABACLADE_SYM encode_error :
   public virtual error {
public:

   /** Constructor.
   */
   encode_error();


   /** See abc::text::error::init().
   */
   void init(errint_t err = 0);
};

} //namespace text
} //namespace abc


////////////////////////////////////////////////////////////////////////////////////////////////////

