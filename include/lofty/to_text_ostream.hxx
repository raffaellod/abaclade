﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010-2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#ifndef _LOFTY_HXX_INTERNAL
   #error "Please #include <lofty.hxx> instead of this file"
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace _pvt {

/*! Defines a member named value that is true if “void T::to_text_ostream(io::text::ostream * dst) const” is
declared, or false otherwise. */
template <typename T>
struct has_to_text_ostream_member {
   template <typename U, void (U::*)(io::text::ostream *) const>
   struct member_test {};
   template <typename U>
   static long test(member_test<U, &U::to_text_ostream> *);
   template <typename>
   static short test(...);

   static bool const value = (sizeof(test<T>(nullptr)) == sizeof(long));
};

}} //namespace lofty::_pvt

namespace lofty {

/*! Generates and writes a string representation of an object of type T, according to an optional format
string. Once constructed with the desired format specification, an instance must be able to convert to string
any number of T instances.

The default implementation assumes that a public T member with signature
“void T::to_text_ostream(io::text::ostream * dst) const” is declared, and offers no support for a format
string.

This class template and its specializations are at the core of lofty::to_str() and
lofty::io::text::ostream::print(). */
template <typename T>
class to_text_ostream {
public:
   static_assert(
      _pvt::has_to_text_ostream_member<T>::value,
      "specialization lofty::to_text_ostream<T> must be provided, or public " \
      "“void T::to_text_ostream(lofty::io::text::ostream * dst) const” must be declared"
   );

   /*! Changes the output format.

   @param format
      Formatting options.
   */
   void set_format(str const & format) {
      // No format expected/allowed.
      throw_on_unused_streaming_format_chars(format.cbegin(), format);
   }

   /*! Converts a T instance to its string representation.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(T const & src, io::text::ostream * dst) {
      src.to_text_ostream(dst);
   }
};

//! @cond
// Partial specializations for cv-qualified T and T reference.
template <typename T>
class to_text_ostream<T const> : public to_text_ostream<T> {};
template <typename T>
class to_text_ostream<T volatile> : public to_text_ostream<T> {};
template <typename T>
class to_text_ostream<T const volatile> : public to_text_ostream<T> {};
template <typename T>
class to_text_ostream<T &> : public to_text_ostream<T> {};
//! @endcond

} //namespace lofty

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! @cond
namespace lofty {

template <>
class LOFTY_SYM to_text_ostream<bool> {
public:
   /*! Changes the output format.

   @param format
      Formatting options.
   */
   void set_format(str const & format);

   /*! Converts a boolean value to its string representation.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(bool src, io::text::ostream * dst);
};

} //namespace lofty
//! @endcond

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace _pvt {

//! Base class for the specializations of to_text_ostream for integer types.
class LOFTY_SYM int_to_text_ostream_base {
public:
   /*! Constructor.

   @param bytes_per_int
      Size of the integer type.
   */
   int_to_text_ostream_base(unsigned bytes_per_int);

   /*! Changes the output format.

   @param format
      Formatting options.
   */
   void set_format(str const & format);

protected:
   /*! Writes the provided buffer to an output stream, prefixed as necessary.

   @param negative
      true if the number is negative, or false otherwise.
   @param dst
      Pointer to the stream to output to.
   @param buf
      Pointer to the string containing the characters to write.
   @param buf_first_used_itr
      Iterator to the first used character in *buf; the last used character is always the last character in
      *buf.
   */
   void add_prefixes_and_write(
      bool negative, io::text::ostream * dst, str * buf, str::iterator buf_first_used_itr
   ) const;

   /*! Converts an integer to its string representation.

   @param i
      Integer to write.
   @param dst
      Pointer to the stream to output to.
   */
   template <typename I>
   void write_impl(I i, io::text::ostream * dst) const;

   //! Converts a 64-bit signed integer to its string representation. See write_impl().
   void write_s64(std::int64_t i, io::text::ostream * dst) const;

   //! Converts a 64-bit unsigned integer to its string representation. See write_impl().
   void write_u64(std::uint64_t i, io::text::ostream * dst) const;

   //! Converts a 32-bit signed integer to its string representation. See write_impl().
   void write_s32(std::int32_t i, io::text::ostream * dst) const;

   //! Converts a 32-bit unsigned integer to its string representation. See write_impl().
   void write_u32(std::uint32_t i, io::text::ostream * dst) const;

   //! Converts a 16-bit signed integer to its string representation. See write_impl().
   void write_s16(std::int16_t i, io::text::ostream * dst) const;

   //! Converts a 16-bit unsigned integer to its string representation. See write_impl().
   void write_u16(std::uint16_t i, io::text::ostream * dst) const;

   //! Converts an 8-bit signed integer to its string representation. See write_impl().
   void write_s8(std::int8_t i, io::text::ostream * dst) const {
      if (base_or_shift == 10) {
         write_s16(i, dst);
      } else {
         /* Avoid extending the sign, as it would generate too many digits in any notation except
         decimal. */
         write_s16(static_cast<std::uint8_t>(i), dst);
      }
   }

   //! Converts an 8-bit unsigned integer to its string representation. See write_impl().
   void write_u8(std::uint8_t i, io::text::ostream * dst) const {
      write_u16(i, dst);
   }

protected:
   //! Pointer to either int_to_lower_str_map or int_to_upper_str_map.
   char const * int_to_str_map;
   /*! Minimum number of digits to be generated. Always >= 1, to ensure the generation of at least a
   single zero. */
   unsigned width;
   //! Required buffer size.
   unsigned buf_size;
   //! Integer size, in bytes.
   std::uint8_t const bytes_per_int;
   //! 10 (for decimal notation) or log2(notation) (for power-of-two notations).
   std::uint8_t base_or_shift;
   //! Character to be used to pad the digits to width length.
   char padding_char;
   //! Character to be used as sign in case the number is not negative; NUL if none.
   char positive_sign_char;
   //! First character of the prefix; NUL if none (which means that prefix_char_1 is ignored).
   char prefix_char_0;
   //! Second character of the prefix; NUL if none.
   char prefix_char_1;
   //! Map from int [0-15] to its uppercase hexadecimal representation.
   static char const int_to_upper_str_map[16];
   //! Map from int [0-15] to its lowercase hexadecimal representation.
   static char const int_to_lower_str_map[16];
};

#if LOFTY_HOST_WORD_SIZE >= 32
#if LOFTY_HOST_WORD_SIZE >= 64

// On a machine with 64-bit word size, write_64*() will be faster.

inline void int_to_text_ostream_base::write_s32(std::int32_t i, io::text::ostream * dst) const {
   if (base_or_shift == 10) {
      write_s64(i, dst);
   } else {
      // Avoid extending the sign in any notation except decimal, as it would generate too many digits.
      write_s64(static_cast<std::uint32_t>(i), dst);
   }
}

inline void int_to_text_ostream_base::write_u32(std::uint32_t i, io::text::ostream * dst) const {
   write_u64(i, dst);
}

#endif //if LOFTY_HOST_WORD_SIZE >= 64

/* On a machine with 32-bit word size, write_32*() will be faster. Note that the latter might in turn defer to
write_64*() (see above). */

inline void int_to_text_ostream_base::write_s16(std::int16_t i, io::text::ostream * dst) const {
   if (base_or_shift == 10) {
      write_s32(i, dst);
   } else {
      // Avoid extending the sign in any notation except decimal, as it would generate too many digits.
      write_s32(static_cast<std::uint16_t>(i), dst);
   }
}

inline void int_to_text_ostream_base::write_u16(std::uint16_t i, io::text::ostream * dst) const {
   write_u32(i, dst);
}

#endif //if LOFTY_HOST_WORD_SIZE >= 32

}} //namespace lofty::_pvt

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace _pvt {

//! Implementation of the specializations of to_text_ostream for integer types.
template <typename I>
class int_to_text_ostream : public int_to_text_ostream_base {
public:
   //! Default constructor.
   int_to_text_ostream() :
      int_to_text_ostream_base(sizeof(I)) {
   }

   /*! Converts an integer to its string representation.

   This design is rather tricky in the way one implementation calls another:

   1. int_to_text_ostream<I>::write()
      Always inlined, dispatches to step 2. based on number of bits;
   2. int_to_text_ostream_base::write_{s,u}{8,16,32,64}()
      Inlined to a bit-bigger variant or implemented in to_text_ostream.cxx, depending on the host
      architecture’s word size;
   3. int_to_text_ostream_base::write_impl()
      Always inlined, but only used in functions defined in to_text_ostream.cxx, so it only generates as many
      copies as strictly necessary to have fastest performance for any integer size.

   The net result is that after all the inlining occurs, this will become a direct call to the fastest
   implementation for I of any given size.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(I src, io::text::ostream * dst) {
      if (sizeof src <= sizeof(std::int8_t)) {
         if (_std::is_signed<I>::value) {
            write_s8(static_cast<std::int8_t>(src), dst);
         } else {
            write_u8(static_cast<std::uint8_t>(src), dst);
         }
      } else if (sizeof src <= sizeof(std::int16_t)) {
         if (_std::is_signed<I>::value) {
            write_s16(static_cast<std::int16_t>(src), dst);
         } else {
            write_u16(static_cast<std::uint16_t>(src), dst);
         }
      } else if (sizeof src <= sizeof(std::int32_t)) {
         if (_std::is_signed<I>::value) {
            write_s32(static_cast<std::int32_t>(src), dst);
         } else {
            write_u32(static_cast<std::uint32_t>(src), dst);
         }
      } else {
         static_assert(sizeof src <= sizeof(std::int64_t), "unsupported integer size");
         if (_std::is_signed<I>::value) {
            write_s64(static_cast<std::int64_t>(src), dst);
         } else {
            write_u64(static_cast<std::uint64_t>(src), dst);
         }
      }
   }
};

}} //namespace lofty::_pvt

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty {

#define LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(I) \
   template <> \
   class to_text_ostream<I> : public _pvt::int_to_text_ostream<I> {};
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(  signed char)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(unsigned char)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(         short)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(unsigned short)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(     int)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(unsigned)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(         long)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(unsigned long)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(         long long)
LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE(unsigned long long)
#undef LOFTY_SPECIALIZE_to_text_ostream_FOR_TYPE

} //namespace lofty

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace _pvt {

//! Base class for the specializations of to_text_ostream for integer types.
class LOFTY_SYM ptr_to_text_ostream : public to_text_ostream<std::uintptr_t> {
public:
   //! Default constructor.
   ptr_to_text_ostream();

   /*! Changes the output format.

   @param format
      Formatting options.
   */
   void set_format(str const & format);

protected:
   /*! Converts a pointer to a string representation.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void _write_impl(std::uintptr_t src, io::text::ostream * dst);
};

}} //namespace lofty::_pvt

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! @cond
namespace lofty {

// Specialization for raw pointer types.
template <typename T>
class to_text_ostream<T *> : public _pvt::ptr_to_text_ostream {
public:
   /*! Converts a pointer to a string representation.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(T * src, io::text::ostream * dst) {
      _write_impl(reinterpret_cast<std::uintptr_t>(src), dst);
   }
};

// Specialization for _std::unique_ptr.
template <typename T, typename TDel>
class to_text_ostream<_std::unique_ptr<T, TDel>> : public _pvt::ptr_to_text_ostream {
public:
   //! See _pvt::ptr_to_text_ostream::write().
   void write(_std::unique_ptr<T, TDel> const & src, io::text::ostream * dst) {
      _write_impl(reinterpret_cast<std::uintptr_t>(src.get()), dst);
   }
};

// Specialization for _std::shared_ptr.
// TODO: show reference count and other info.
template <typename T>
class to_text_ostream<_std::shared_ptr<T>> : public _pvt::ptr_to_text_ostream {
public:
   //! See _pvt::ptr_to_text_ostream::write().
   void write(_std::shared_ptr<T> const & src, io::text::ostream * dst) {
      _write_impl(reinterpret_cast<std::uintptr_t>(src.get()), dst);
   }
};

// Specialization for _std::weak_ptr.
// TODO: show reference count and other info.
template <typename T>
class to_text_ostream<_std::weak_ptr<T>> : public _pvt::ptr_to_text_ostream {
public:
   //! See _pvt::ptr_to_text_ostream::write().
   void write(_std::weak_ptr<T> const & src, io::text::ostream * dst) {
      _write_impl(reinterpret_cast<std::uintptr_t>(src.lock().get()), dst);
   }
};

} //namespace lofty
//! @endcond

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! @cond
namespace lofty {

template <>
class LOFTY_SYM to_text_ostream<_std::type_info> {
public:
   //! Default constructor.
   to_text_ostream();

   //! Constructor.
   ~to_text_ostream();

   /*! Changes the output format.

   @param format
      Formatting options.
   */
   void set_format(str const & format);

   /*! Writes the name of a type, applying the formatting options.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(_std::type_info const & src, io::text::ostream * dst);
};

} //namespace lofty
//! @endcond

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace _pvt {

/*! Base class for the specializations of to_text_ostream for sequence types. Not using templates, so the
implementation can be in a .cxx file. */
class LOFTY_SYM sequence_to_text_ostream {
public:
   /*! Constructor.

   @param start_delim
      Sequence start delimiter.
   @param end_delim
      Sequence end delimiter.
   */
   sequence_to_text_ostream(str const & start_delim, str const & end_delim);

   //! Destructor.
   ~sequence_to_text_ostream();

   /*! Changes the output format.

   @param format
      Formatting options.
   */
   void set_format(str const & format);

   /*! Writes the sequence end delimiter.

   @param dst
      Pointer to the stream to output to.
   */
   void _write_end(io::text::ostream * dst);

   /*! Writes an element separator.

   @param dst
      Pointer to the stream to output to.
   */
   void _write_separator(io::text::ostream * dst);

   /*! Writes the sequence start delimiter.

   @param dst
      Pointer to the stream to output to.
   */
   void _write_start(io::text::ostream * dst);

protected:
   //! Separator to be output between elements.
   str separator;
   //! Sequence start delimiter.
   str start_delim;
   //! Sequence end delimiter.
   str end_delim;
};

}} //namespace lofty::_pvt

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! @cond
#ifdef LOFTY_CXX_VARIADIC_TEMPLATES

namespace lofty { namespace _pvt {

//! Helper to write a single element out of a tuple, recursing to print any remaining ones.
template <class TTuple, typename... Ts>
class tuple_to_text_ostream_element_writer;

// Base case for the template recursion.
template <class TTuple>
class tuple_to_text_ostream_element_writer<TTuple> {
public:
   /*! Writes the current element to the specified text stream, then recurses to write the rest.

   @param src
      Tuple from which to extract the element to write.
   @param dst
      Pointer to the stream to output to.
   */
   void _write_elements(TTuple const & src, io::text::ostream * dst) {
      LOFTY_UNUSED_ARG(src);
      LOFTY_UNUSED_ARG(dst);
   }
};

// Template recursion step.
template <class TTuple, typename T0, typename... Ts>
class tuple_to_text_ostream_element_writer<TTuple, T0, Ts ...> :
   public tuple_to_text_ostream_element_writer<TTuple, Ts ...> {
public:
   //! See tuple_to_text_ostream_element_writer<TTuple>::_write_elements().
   void _write_elements(TTuple const & src, io::text::ostream * dst);

protected:
   //! Backend for the current element type.
   to_text_ostream<T0> ttos0;
};

}} //namespace lofty::_pvt

namespace lofty {

template <typename... Ts>
class to_text_ostream<_std::tuple<Ts ...>> :
   public _pvt::sequence_to_text_ostream,
   public _pvt::tuple_to_text_ostream_element_writer<_std::tuple<Ts ...>, Ts ...> {
public:
   //! Default constructor.
   to_text_ostream() :
      _pvt::sequence_to_text_ostream(LOFTY_SL("("), LOFTY_SL(")")) {
   }

   /*! Converts a tuple into its string representation.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(_std::tuple<Ts ...> const & src, io::text::ostream * dst) {
      _write_start(dst);
      this->_write_elements(src, dst);
      _write_end(dst);
   }
};

} //namespace lofty

namespace lofty { namespace _pvt {

// Now this can be defined.

template <class TTuple, typename T0, typename... Ts>
inline void tuple_to_text_ostream_element_writer<TTuple, T0, Ts ...>::_write_elements(
   TTuple const & src, io::text::ostream * dst
) {
   ttos0.write(_std::get<_std::tuple_size<TTuple>::value - (1 /*Ts*/ + sizeof ...(Ts))>(src), dst);
   // If there are any remaining elements, write a separator and recurse to write the rest.
   if (sizeof ...(Ts)) {
      static_cast<to_text_ostream<TTuple> *>(this)->_write_separator(dst);
      tuple_to_text_ostream_element_writer<TTuple, Ts ...>::_write_elements(src, dst);
   }
}

}} //namespace lofty::_pvt

#else //ifdef LOFTY_CXX_VARIADIC_TEMPLATES

namespace lofty { namespace _pvt {

//! Helper to write the elements of a tuple.
// Template recursion step.
template <
   class TTuple, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
   typename T7, typename T8, typename T9
>
class tuple_to_text_ostream_element_writer :
   public tuple_to_text_ostream_element_writer<
      TTuple, T1, T2, T3, T4, T5, T6, T7, T8, T9, _std::_pvt::tuple_void
   > {
public:
   //! See tuple_to_text_ostream_element_writer<TTuple>::_write_elements().
   void _write_elements(TTuple const & src, io::text::ostream * dst);

protected:
   //! Backend for the current element type.
   to_text_ostream<T0> ttos0;
};

// Base case for the template recursion.
template <class TTuple>
class tuple_to_text_ostream_element_writer<
   TTuple,
   _std::_pvt::tuple_void, _std::_pvt::tuple_void, _std::_pvt::tuple_void, _std::_pvt::tuple_void,
   _std::_pvt::tuple_void, _std::_pvt::tuple_void, _std::_pvt::tuple_void, _std::_pvt::tuple_void,
   _std::_pvt::tuple_void, _std::_pvt::tuple_void
> {
public:
   /*! Writes the current element to the specified text stream, then recurses to write the rest.

   @param src
      Tuple from which to extract the element to write.
   @param dst
      Pointer to the stream to output to.
   */
   void _write_elements(TTuple const & src, io::text::ostream * dst) {
      LOFTY_UNUSED_ARG(src);
      LOFTY_UNUSED_ARG(dst);
   }
};

}} //namespace lofty::_pvt

namespace lofty {

template <
   typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7,
   typename T8, typename T9
>
class to_text_ostream<_std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>> :
   public _pvt::sequence_to_text_ostream,
   public _pvt::tuple_to_text_ostream_element_writer<
      _std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9
   > {
public:
   //! Constructor.
   to_text_ostream() :
      _pvt::sequence_to_text_ostream(LOFTY_SL("("), LOFTY_SL(")")) {
   }

   /*! Converts a tuple into its string representation.

   @param src
      Object to write.
   @param dst
      Pointer to the stream to output to.
   */
   void write(_std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> const & src, io::text::ostream * dst) {
      _write_start(dst);
      this->_write_elements(src, dst);
      _write_end(dst);
   }
};

} //namespace lofty

// Now this can be defined.

namespace lofty { namespace _pvt {

template <
   class TTuple, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
   typename T7, typename T8, typename T9
>
inline void tuple_to_text_ostream_element_writer<
   TTuple, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9
>::_write_elements(TTuple const & src, io::text::ostream * dst) {
   static std::size_t const tuple_size = _std::tuple_size<
      _std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>
   >::value;
   ttos0.write(_std::get<_std::tuple_size<TTuple>::value - tuple_size>(src), dst);
   // If there are any remaining elements, write a separator and recurse to write the rest.
   if (tuple_size > 1) {
      static_cast<to_text_ostream<TTuple> *>(this)->_write_separator(dst);
      tuple_to_text_ostream_element_writer<
         TTuple, T1, T2, T3, T4, T5, T6, T7, T8, T9, _std::_pvt::tuple_void
      >::_write_elements(src, dst);
   }
}

}} //namespace lofty::_pvt

#endif //ifdef LOFTY_CXX_VARIADIC_TEMPLATES … else
//! @endcond
