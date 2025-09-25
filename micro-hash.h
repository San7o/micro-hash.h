//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// micro-hash.h
// ------------
//
// Quick and dirty hash functions in C99, with some benchmarks. Lots
// of functions for int32, int64 and bytes keys!
//
// THESE FUNCTIONS ARE NOT CRYPTOGRAPHICALLY SECURE. IF YOU NEED A
// CRYPTOGRAPHICALLY SECURE HASH FUNCTION THEN GO USE A KNOWN
// IMPLEMENTATION OF AN APPROVED HASH FUNCTION FROM NIST
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//
//
// Documentation
// -------------
//
// This header-only library is a collection of fast hash functions
// gathered from all around the interweb. If you are looking for a
// quick hash function for your needs, be my guest.
// 
// Here you will find:
//   - micro_hash_int32_wang
//   - micro_hash_int32_wang2
//   - micro_hash_int32_rob
//   - micro_hash_int64_wang
//   - micro_hash_int6432_wang
//   - micro_hash_bytes_curl
//   - micro_hash_bytes_jenkins
//   - micro_hash_str_stb
//   - micro_hash_str_djb2
//   - micro_hash_str_sdbm
//
// Check out the signatures to see the type that they accept and
// generate.
//
//
// Benchmarks
// ----------
//
// There are some benchmarks under the `tests/` directory, for now
// only the functions that work with integers are tested.
//
// If you run `make check`, you should get similar results:
//
// /----------------------------------------------------------
// |      hash function      |  collisions  |  non-uniformity |
// | ----------------------- | ------------ | --------------- |
// | micro_hash_int32_rob    | 0            | 39.740234375000 |
// | micro_hash_int32_wang2  | 0            | 39.412597656250 |
// | micro_hash_int6432_wang | 11580        | 39.166503906250 |
// | micro_hash_int64_wang   | 0            | 38.696289062500 |
// | micro_hash_int32_wang   | 0            | 39.614257812500 |
// \----------------------------------------------------------/
//
// `collisions` is the number of collisions found by generating
// ITERATIONS number of random values. `non-uniformity` is a measure
// of how much the hash values are not distributed uniformly, more
// specifically it is the mean of the difference between the the expected
// distribution (uniform) and the actual distribution of the hashes,
// with the hashes being mapped over a smaller space (2^PRECISION)
// compared to total hash space (for practical reasons). Lower is
// better.
//
//
// Usage
// -----
//
// Do this:
//
//   #define MICRO_HASH_IMPLEMENTATION
//
// before you include this file in *one* C or C++ file to create the
// implementation.
//
// i.e. it should look like this:
//
//   #include ...
//   #include ...
//   #include ...
//   #define MICRO_HASH_IMPLEMENTATION
//   #include "micro-hash.h"
//
// Then use whatever hash function you fancy most.
//
// Some more hash functions:
// - https://en.wikipedia.org/wiki/List_of_hash_functions
//
//
// Code
// ----
//
// The official git repository of micro-hash.h is hosted at:
//
//     https://github.com/San7o/micro-hash.h
//
// This is part of a bigger collection of header-only C99 libraries
// called "micro-headers", contributions are welcome:
//
//     https://github.com/San7o/micro-headers
//


#ifndef _MICRO_HASH_H_
#define _MICRO_HASH_H_

#define MICRO_HASH_MAJOR 0
#define MICRO_HASH_MINOR 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// Function declarations
//

// Integer
// -------
//
// An integer hash function accepts an integer hash key, and returns
// an integer hash result with uniform distribution.

// This function uses a combination of bit shifts and integer
// multiplication to hash the input key.
// Credits: Thomas Wang
uint32_t micro_hash_int32_wang(uint32_t key);

// Credits: Thomas Wang
uint32_t micro_hash_int32_wang2(uint32_t key);

// Credits: Robert Jenkins
uint32_t micro_hash_int32_rob(uint32_t key);

// Credits: Thomas Wang
uint64_t micro_hash_int64_wang(uint64_t key);

// Credits: Thomas Wang
uint32_t micro_hash_int6432_wang(uint64_t key);

// Bytes
// -----
//
// Hash a sequence of bytes

// curl/lib/hash.c
size_t micro_hash_bytes_curl(void *key, size_t key_length);

// https://en.wikipedia.org/wiki/Jenkins_hash_function 
uint32_t micro_hash_bytes_jenkins(uint8_t* key, size_t key_length);

// String
// ------
//
// Hash a string

// stb/stb_ds.h
size_t micro_hash_str_stb(char *str, size_t seed);

// djb2 algorithm
//
// This algorithm (k=33) was first reported by dan bernstein many
// years ago in comp.lang.c. another version of this algorithm (now
// favored by bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^
// str[i]; the magic of number 33 (why it works better than many other
// constants, prime or not) has never been adequately explained.
unsigned long micro_hash_str_djb2(unsigned char *str);

// sdbm
//
// This algorithm was created for sdbm (a public-domain
// reimplementation of ndbm) database library. it was found to do well
// in scrambling bits, causing better distribution of the keys and
// fewer splits. it also happens to be a good general hashing function
// with good distribution. the actual function is hash(i) = hash(i -
// 1) * 65599 + str[i]; what is included below is the faster version
// used in gawk.
unsigned long micro_hash_str_sdbm(unsigned char *str);

//
// Implementation
//

#ifdef MICRO_HASH_IMPLEMENTATION

// Integer

uint32_t micro_hash_int32_wang(uint32_t a)
{
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

uint32_t micro_hash_int32_wang2(uint32_t key)
{
  key = ~key + (key << 15); // key = (key << 15) - key - 1;
  key = key ^ (key >> 12);
  key = key + (key << 2);
  key = key ^ (key >> 4);
  key = key * 2057; // key = (key + (key << 3)) + (key << 11);
  key = key ^ (key >> 16);
  return key;
}

uint32_t micro_hash_int32_rob(uint32_t a)
{
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}

uint64_t micro_hash_int64_wang(uint64_t key)
{
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}

uint32_t micro_hash_int6432_wang(uint64_t key)
{
  key = (~key) + (key << 18); // key = (key << 18) - key - 1;
  key = key ^ (key >> 31);
  key = key * 21; // key = (key + (key << 2)) + (key << 4);
  key = key ^ (key >> 11);
  key = key + (key << 6);
  key = key ^ (key >> 22);
  return (int) key;
}

// Bytes

size_t micro_hash_bytes_curl(void *key, size_t key_length)
{
  char *key_str = (char *) key;
  char *end = key_str + key_length;
  size_t h = 5381;

  while(key_str < end) {
    size_t j = (size_t)*key_str++;
    h += h << 5;
    h ^= j;
  }

  return h;
}

uint32_t micro_hash_bytes_jenkins(uint8_t* key, size_t length)
{
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

// Strings
  
size_t micro_hash_str_stb(char *str, size_t seed)
{
#define SIZE_T_BITS           ((sizeof (size_t)) * 8)
#define ROTATE_LEFT(val, n)   (((val) << (n)) | ((val) >> (SIZE_T_BITS - (n))))
#define ROTATE_RIGHT(val, n)  (((val) >> (n)) | ((val) << (SIZE_T_BITS - (n))))

  size_t hash = seed;
  while (*str)
     hash = ROTATE_LEFT(hash, 9) + (unsigned char) *str++;

  // Thomas Wang 64-to-32 bit mix function, hopefully also works in 32 bits
  hash ^= seed;
  hash = (~hash) + (hash << 18);
  hash ^= hash ^ ROTATE_RIGHT(hash,31);
  hash = hash * 21;
  hash ^= hash ^ ROTATE_RIGHT(hash,11);
  hash += (hash << 6);
  hash ^= ROTATE_RIGHT(hash,22);
  return hash+seed;
  
#undef ROTATE_LEFT
#undef ROTATE_RIGHT
#undef SIZE_T_BITS
}

unsigned long micro_hash_str_djb2(unsigned char *str)
{
  unsigned long hash = 5381;
  int c;
  
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  
  return hash;
}

unsigned long micro_hash_str_sdbm(unsigned char *str)
{
  unsigned long hash = 0;
  int c;

  while ((c = *str++))
    hash = c + (hash << 6) + (hash << 16) - hash;

  return hash;
}

#endif // MICRO_HASH_IMPLEMENTATION

//
// Examples
//

#if 0

#define MICRO_HASH_IMPLEMENTATION
#include "micro-hash.h"

#include <stdio.h>

int main(void)
{
  uint32_t key = 69;
  uint32_t hash = micro_hash_int32_wang(key);

  printf("Key:  %d\n", key);
  printf("Hash: %d\n", hash);
  
  return 0;
}

#endif // 0

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _MICRO_HASH_H_
