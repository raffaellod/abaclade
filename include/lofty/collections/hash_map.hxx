﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010-2018 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#ifndef _LOFTY_COLLECTIONS_HASH_MAP_HXX

#ifndef _LOFTY_NOPUB
   #define _LOFTY_NOPUB
   #define _LOFTY_COLLECTIONS_HASH_MAP_HXX
#endif

#ifndef _LOFTY_COLLECTIONS_HASH_MAP_HXX_NOPUB
#define _LOFTY_COLLECTIONS_HASH_MAP_HXX_NOPUB

#include <lofty/collections.hxx>
#include <lofty/collections/_pvt/hash_map_impl.hxx>
#include <lofty/_std/functional.hxx>
#include <lofty/_std/utility.hxx>
#include <lofty/type_void_adapter.hxx>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace lofty { namespace collections {
_LOFTY_PUBNS_BEGIN

/*! Key/value map using a derivative of the hopscotch hashing collision resolution algorithm.

This implementation uses a variable hash table size (number of buckets) to deal with varying item counts, as
well as a variable neighborhood size (number of buckets sharing the same logical index) in order to tolerate
high-collision hash functions. */
template <
   typename TKey,
   typename TValue,
   typename THasher = _std::_LOFTY_PUBNS hash<TKey>,
   typename TKeyEqual = _std::_LOFTY_PUBNS equal_to<TKey>
>
class hash_map : public _pvt::hash_map_impl, private THasher, private TKeyEqual {
public:
   //! Key type.
   typedef TKey key_type;
   //! Mapped value type.
   typedef TValue mapped_type;
   //! Hash generator for TKey.
   typedef THasher hasher;
   //! Functor that can compare two TKey instances for equality.
   typedef TKeyEqual key_equal;

   /*! Type used to contain both a key and a value. Note that this is not used in the internal data model, so
   it’s not the same as value_type. */
   struct pair_type {
      //! Key.
      TKey key;
      //! Value.
      TValue value;

      //! Constructor.
      pair_type(TKey && key_, TValue && value_) :
         key(_std::_pub::move(key_)),
         value(_std::_pub::move(value_)) {
      }
   };

   /*! Pointer type returned by iterator::operator->() that behaves like a pointer, but in fact includes the
   object it points to.

   Needed because iterator::operator->() must return a pointer-like type to a key/value pair, but keys and
   values are not stored in the map as a pair. */
   template <typename TRefPair>
   class pair_ptr {
   public:
      /*! Constructor.

      @param key
         Pointer to the key in the map.
      @param value
         Pointer to the value in the map.
      */
      pair_ptr(TKey * key, TValue * value) :
         ref_pair(key, value) {
      }

      /*! Dereferencing operator.

      @return
         Reference to the key/value reference pair.
      */
      TRefPair const & operator*() const {
         return ref_pair;
      }

      /*! Dereferencing member access operator.

      @return
         Pointer to the key/value reference pair.
      */
      TRefPair const * operator->() const {
         return &ref_pair;
      }

   private:
      //! Pair of references returned by operator->().
      TRefPair const ref_pair;
   };

   //! Const iterator type.
   class const_iterator : public hash_map_impl::iterator_base {
   private:
      friend class hash_map;

   public:
      //! Const key/value type. It should be called ref_pair, but iterators need to have value_type.
      struct value_type {
         //! Reference to the key.
         TKey const & key;
         //! Reference to the value.
         TValue const & value;

         /*! Constructor.

         @param key_
            Pointer to the key to refer to.
         @param value_
            Pointer to the value to refer to.
         */
         value_type(TKey const * key_, TValue const * value_) :
            key(*key_),
            value(*value_) {
         }
      };

      typedef value_type * pointer;
      typedef value_type & reference;

   public:
      //! Default constructor.
      const_iterator() {
      }

      /*! Dereferencing operator.

      @return
         Reference to the current key/value pair.
      */
      value_type operator*() const {
         validate();
         hash_map const * map = static_cast<hash_map const *>(owner_map);
         return value_type(map->key_ptr(bucket), map->value_ptr(bucket));
      }

      /*! Dereferencing member access operator.

      @return
         Pointer to the current key/value pair.
      */
      pair_ptr<value_type> operator->() const {
         validate();
         hash_map const * map = static_cast<hash_map const *>(owner_map);
         return pair_ptr<value_type>(map->key_ptr(bucket), map->value_ptr(bucket));
      }

      /*! Preincrement operator.

      @return
         *this.
      */
      const_iterator & operator++() {
         validate();
         increment();
         return *this;
      }

      /*! Postincrement operator.

      @return
         Iterator pointing to the previous key/value pair.
      */
      const_iterator operator++(int) {
         validate();
         std::size_t old_bucket = bucket;
         increment();
         return const_iterator(owner_map, old_bucket);
      }

   protected:
      //! See hash_map_impl::iterator_base::iterator_base.
      const_iterator(hash_map_impl const * owner_map_, std::size_t bucket_) :
         hash_map_impl::iterator_base(owner_map_, bucket_) {
      }
   };

   //! Iterator type.
   class iterator : public const_iterator {
   private:
      friend class hash_map;

   public:
      //! Key/value type. It should be called ref_pair, but iterators need to have value_type.
      struct value_type {
         //! Reference to the key.
         TKey const & key;
         //! Reference to the value.
         TValue & value;

         /*! Constructor.

         @param key_
            Pointer to the key to refer to.
         @param value_
            Pointer to the value to refer to.
         */
         value_type(TKey const * key_, TValue * value_) :
            key(*key_),
            value(*value_) {
         }
      };

      typedef value_type * pointer;
      typedef value_type & reference;

   public:
      //! Default constructor.
      iterator() {
      }

      //! See const_iterator::operator*().
      value_type operator*() const {
         this->validate();
         auto map = static_cast<hash_map const *>(this->owner_map);
         return value_type(map->key_ptr(this->bucket), map->value_ptr(this->bucket));
      }

      /*! Dereferencing member access operator.

      @return
         Pointer to the current key/value pair.
      */
      pair_ptr<value_type> operator->() const {
         this->validate();
         auto map = static_cast<hash_map const *>(this->owner_map);
         return pair_ptr<value_type>(map->key_ptr(this->bucket), map->value_ptr(this->bucket));
      }

      //! See const_iterator.operator++().
      iterator & operator++() {
         return static_cast<iterator &>(const_iterator::operator++());
      }

      //! See const_iterator::operator++(int).
      iterator operator++(int) {
         return iterator(const_iterator::operator++());
      }

   protected:
      //! See const_iterator::const_iterator.
      iterator(hash_map_impl const * owner_map_, std::size_t bucket_) :
         const_iterator(owner_map_, bucket_) {
      }

   private:
      /*! Constructor used for cv-removing promotions from const_iterator to iterator.

      @param it
         Source object.
      */
      iterator(const_iterator const & it) :
         const_iterator(it) {
      }
   };

   typedef typename iterator::value_type value_type;
   typedef typename const_iterator::value_type const_value_type;

   //! Type returned by add_or_assign().
   struct add_or_assign_ret {
      //! Iterator to the (possibly newly-added) key/value.
      iterator itr;
      /*! true if the key/value pair was just added, or false if the key already existed in the map and the
      corresponding value was overwritten. */
      bool added;

      /*! Constructor.

      @param owner_map
         Pointer to the map owning the iterator.
      @param impl
         add_or_assign_impl_ret instance to convert into an add_or_assign_ret instance.
      */
      add_or_assign_ret(hash_map const * owner_map, add_or_assign_impl_ret impl) :
         itr(iterator(owner_map, impl.bucket)),
         added(impl.added) {
      }
   };

public:
   //! Default constructor.
   hash_map() {
   }

   /*! Move constructor.

   @param src
      Source object.
   */
   hash_map(hash_map && src) :
      _pvt::hash_map_impl(_std::_pub::move(src)) {
   }

   //! Destructor.
   ~hash_map() {
      clear();
   }

   /*! Move-assignment operator.

   @param src
      Source object.
   @return
      *this.
   */
   hash_map & operator=(hash_map && src) {
      _pvt::hash_map_impl::operator=(_std::_pub::move(src));
      return *this;
   }

   /*! Element lookup operator.

   @param key
      Key to lookup.
   @return
      Value corresponding to key. If key is not in the map, an exception will be thrown.
   */
   TValue & operator[](TKey const & key) const {
      std::size_t bucket = lookup_key(key);
      if (bucket == null_index) {
         // TODO: provide more information in the exception.
         LOFTY_THROW(bad_key, ());
      }
      return *value_ptr(bucket);
   }

   /*! Adds a key/value pair to the map, overwriting the value if key is already associated to one.

   TODO: make four copies of this method, taking const &/const&, &&/&&, const &/&&, &&/const &; this requires
   more work to avoid the commented-out set_copy_construct() below for non-copiable types.

   @param key
      Key to add.
   @param value
      Value to add.
   @return
      Object containing an iterator to the (possibly newly-added) key/value, and a bool value that is true if
      the key/value pair was just added, or false if the key already existed in the map and the corresponding
      value was overwritten.
   */
   add_or_assign_ret add_or_assign(TKey key, TValue value) {
      lofty::_pub::type_void_adapter key_type, value_type;
//      key_type.set_copy_construct<TKey>();
      key_type.set_destruct<TKey>();
      key_type.set_move_construct<TKey>();
      key_type.set_size<TKey>();
//      value_type.set_copy_construct<TValue>();
      value_type.set_destruct<TValue>();
      value_type.set_move_construct<TValue>();
      value_type.set_size<TValue>();
      std::size_t key_hash = calculate_and_adjust_hash(key);
      return add_or_assign_ret(this, hash_map_impl::add_or_assign(
         key_type, value_type, &keys_equal, &key, key_hash, &value, 1 | 2
      ));
   }

   /*! Returns an iterator set to the first key/value pair in the map.

   @return
      Iterator to the first key/value pair.
   */
   iterator begin() {
      return iterator(this, find_first_used_bucket());
   }

   /*! Returns a const iterator set to the first key/value pair in the map.

   @return
      Const iterator to the first key/value pair.
   */
   const_iterator begin() const {
      return const_cast<hash_map *>(this)->begin();
   }

   /*! Returns a const iterator set to the first key/value pair in the map.

   @return
      Const iterator to the first key/value pair.
   */
   const_iterator cbegin() const {
      return const_cast<hash_map *>(this)->begin();
   }

   /*! Returns a const iterator set beyond the last key/value pair in the map.

   @return
      Const iterator set to beyond the last key/value pair.
   */
   const_iterator cend() const {
      return const_cast<hash_map *>(this)->end();
   }

   //! Removes all elements from the map.
   void clear() {
      lofty::_pub::type_void_adapter key_type, value_type;
      key_type.set_destruct<TKey>();
      key_type.set_size<TKey>();
      value_type.set_destruct<TValue>();
      value_type.set_size<TValue>();
      hash_map_impl::clear(key_type, value_type);
   }

   /*! Returns an iterator set beyond the last key/value pair in the map.

   @return
      Iterator set to beyond the last key/value pair.
   */
   iterator end() {
      return iterator(this, null_index);
   }

   /*! Returns a const iterator set beyond the last key/value pair in the map.

   @return
      Const iterator set to beyond the last key/value pair.
   */
   const_iterator end() const {
      return const_cast<hash_map *>(this)->begin();
   }

   /*! Searches the map for a specific key, returning an iterator to the corresponding key/value pair if
   found.

   @param key
      Key to search for.
   @return
      Iterator to the matching key/value, or cend() if the key could not be found.
   */
   iterator find(TKey const & key) {
      std::size_t bucket = lookup_key(key);
      return iterator(this, bucket);
   }

   /*! Searches the map for a specific key, returning an iterator to the corresponding key/value pair if
   found.

   @param key
      Key to search for.
   @return
      Iterator to the matching key/value, or cend() if the key could not be found.
   */
   const_iterator find(TKey const & key) const {
      std::size_t bucket = lookup_key(key);
      return const_iterator(this, bucket);
   }

   /*! Removes and returns a value given an iterator to it.

   @param itr
      Iterator to the key/value to extract.
   @return
      Value removed from the map.
   */
   TValue pop(const_iterator itr) {
      itr.validate();
      TValue value(_std::_pub::move(*value_ptr(itr.bucket)));
      lofty::_pub::type_void_adapter key_type, value_type;
      key_type.set_destruct<TKey>();
      key_type.set_size<TKey>();
      value_type.set_destruct<TValue>();
      value_type.set_size<TValue>();
      empty_bucket(key_type, value_type, itr.bucket);
      return _std::_pub::move(value);
   }

   /*! Removes and returns a value given a key, which must be in the map.

   @param key
      Key associated to the value to extract.
   @return
      Value removed from the map.
   */
   TValue pop(TKey const & key) {
      std::size_t bucket = lookup_key(key);
      if (bucket == null_index) {
         // TODO: provide more information in the exception.
         LOFTY_THROW(bad_key, ());
      }
      TValue value(_std::_pub::move(*value_ptr(bucket)));
      lofty::_pub::type_void_adapter key_type, value_type;
      key_type.set_destruct<TKey>();
      key_type.set_size<TKey>();
      value_type.set_destruct<TValue>();
      value_type.set_size<TValue>();
      empty_bucket(key_type, value_type, bucket);
      return _std::_pub::move(value);
   }

   /*! Removes and returns a non-random key/value pair from the map.

   @return
      Pair containing the removed key/value.
   */
   pair_type pop() {
      std::size_t bucket = find_first_used_bucket();
      if (bucket == null_index) {
         LOFTY_THROW(bad_access, ());
      }
      pair_type ret(_std::_pub::move(*key_ptr(bucket)), _std::_pub::move(*value_ptr(bucket)));
      lofty::_pub::type_void_adapter key_type, value_type;
      key_type.set_destruct<TKey>();
      key_type.set_size<TKey>();
      value_type.set_destruct<TValue>();
      value_type.set_size<TValue>();
      empty_bucket(key_type, value_type, bucket);
      return _std::_pub::move(ret);
   }

   /*! Removes a value given an iterator to it.

   @param it
      Iterator to the key/value to remove.
   */
   void remove(const_iterator it) {
      lofty::_pub::type_void_adapter key_type, value_type;
      key_type.set_destruct<TKey>();
      key_type.set_size<TKey>();
      value_type.set_destruct<TValue>();
      value_type.set_size<TValue>();
      empty_bucket(key_type, value_type, it);
   }

   /*! Removes a value given a key, which must be in the map.

   @param key
      Key associated to the value to remove.
   */
   void remove(TKey const & key) {
      if (!remove_if_found(key)) {
         // TODO: provide more information in the exception.
         LOFTY_THROW(bad_key, ());
      }
   }

   /*! Removes a value given a key, if found in the map. If the key is not in the map, no removal occurs.

   @param key
      Key associated to the value to remove.
   @return
      true if a value matching the key was found (and removed), or false otherwise.
   */
   bool remove_if_found(TKey const & key) {
      std::size_t bucket = lookup_key(key);
      if (bucket != null_index) {
         lofty::_pub::type_void_adapter key_type, value_type;
         key_type.set_destruct<TKey>();
         key_type.set_size<TKey>();
         value_type.set_destruct<TValue>();
         value_type.set_size<TValue>();
         empty_bucket(key_type, value_type, bucket);
         return true;
      } else {
         return false;
      }
   }

private:
   /*! Calculates, adjusts and returns the hash value for the specified key.

   @param key
      Key to calculate a hash value for.
   @return
      Hash value of key.
   */
   std::size_t calculate_and_adjust_hash(TKey const & key) const {
      std::size_t key_hash = hasher::operator()(key);
      return key_hash == empty_bucket_hash ? zero_hash : key_hash;
   }

   /*! Returns a pointer to the key in the specified bucket index.

   @param bucket
      Bucket index.
   @return
      Pointer to the key.
   */
   TKey * key_ptr(std::size_t bucket) const {
      return static_cast<TKey *>(keys.get()) + bucket;
   }

   /*! Compares two keys for equality. Static helper used by _pvt::hash_map_impl.

   @param this_ptr
      Pointer to *this.
   @param key1
      Pointer to the first key to compare.
   @param key2
      Pointer to the second key to compare.
   @return
      true if the two keys compare as equal, or false otherwise.
   */
   static bool keys_equal(hash_map_impl const * this_ptr, void const * key1, void const * key2) {
      auto map = static_cast<hash_map const *>(this_ptr);
      return map->key_equal::operator()(*static_cast<TKey const *>(key1), *static_cast<TKey const *>(key2));
   }

   /*! Looks for a specific key in the map.

   @param key
      Key to lookup.
   @return
      Index of the bucket at which the key could be found, or null_index if the key could not be found.
   */
   std::size_t lookup_key(TKey const & key) const {
      std::size_t key_hash = calculate_and_adjust_hash(key);
      if (total_buckets == 0) {
         // The key cannot possibly be in the map.
         return null_index;
      }
      auto nh_range(hash_neighborhood_range(key_hash));

      auto hash_ptr      = hashes.get() + *nh_range.begin();
      auto hashes_nh_end = hashes.get() + *nh_range.end();
      auto hashes_end    = hashes.get() + total_buckets;
      /* nh_range may be a wrapping range, so we can only test for inequality and rely on the wrap-
      around logic at the end of the loop body. Also, we need to iterate at least once, otherwise we won’t
      enter the loop at all if the start condition is the same as the end condition, which is the case for
      neighborhood_size_ == total_buckets. */
      do {
         /* Multiple calculations of the second condition should be rare enough (exact key match or hash
         collision) to make recalculating the offset from keys cheaper than keeping a cursor over keys running
         in parallel to hash_ptr. */
         if (*hash_ptr == key_hash) {
            std::size_t bucket = static_cast<std::size_t>(hash_ptr - hashes.get());
            if (key_equal::operator()(*key_ptr(bucket), key)) {
               return bucket;
            }
         }

         // Move on to the next bucket, wrapping around to the first one if needed.
         if (++hash_ptr == hashes_end) {
            hash_ptr = hashes.get();
         }
      } while (hash_ptr != hashes_nh_end);
      return null_index;
   }

   /*! Returns a pointer to the value in the specified bucket index.

   @param bucket
      Bucket index.
   @return
      Pointer to the value.
   */
   TValue * value_ptr(std::size_t bucket) const {
      return static_cast<TValue *>(values.get()) + bucket;
   }
};

_LOFTY_PUBNS_END
}} //namespace lofty::collections

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ifndef _LOFTY_COLLECTIONS_HASH_MAP_HXX_NOPUB

#ifdef _LOFTY_COLLECTIONS_HASH_MAP_HXX
   #undef _LOFTY_NOPUB

   namespace lofty { namespace collections {

   using _pub::hash_map;

   }}

   #ifdef LOFTY_CXX_PRAGMA_ONCE
      #pragma once
   #endif
#endif

#endif //ifndef _LOFTY_COLLECTIONS_HASH_MAP_HXX
