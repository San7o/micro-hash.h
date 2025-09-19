//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// Tests
// -----
//
// This program calculates the number of collisions and the hash
// uniformity of the hash functions
//

// Number of iterations
//
// !!!Warning: memory space and execution time scales linearly with
// the number of iterations!!!
#define ITERATIONS 10000000 // 40 MB

// Precision of the uniformity estimate (higher is better)
//
// !!!Warning: memory space scales exponentially with PRECISION!!!
// memory required = (2^PRECISION)*8 bytes.
#define PRECISION 12

//
// Program
//

#define MICRO_TESTS_MULTITHREADED
#define MICRO_TESTS_IMPLEMENTATION
#include "micro-tests.h"
#include "hashset.h"
#define MICRO_HASH_IMPLEMENTATION
#include "../micro-hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// LCG pseudo random number generator
#define MAGIC1_32 1664525    // a
#define MAGIC2_32 1013904223 // c
uint32_t lcg32(uint32_t seed)
{
  return MAGIC1_32 * seed + MAGIC2_32;
}

// Magic from Newlib
#define MAGIC1_64 6364136223846793005 // a
#define MAGIC2_64 1442695040888963407 // c
uint64_t lcg64(uint64_t seed)
{
  return MAGIC1_64 * seed + MAGIC2_64;
}


// Get the absolute value of a double
inline double absd(double x)
{
  return (x > 0.0) ? x : -x;
}

#define COUNT_COLLISIONS(__hash_func, __hash_unit, __hashset_prefix, __rng_func, __count, __collisions) \
  do {                                                                  \
    __collisions = 0;                                                   \
    __hash_unit random = __rng_func(6969);                              \
    __hash_unit hash;                                                   \
    __hashset_prefix##_set s;                                           \
    __hashset_prefix##_set_init(&s);                                    \
    for (unsigned int i = 0; i < ITERATIONS; ++i)                       \
    {                                                                   \
      hash = __hash_func(random);                                       \
      random = __rng_func(random);                                      \
      if (!__hashset_prefix##_set_insert(&s, hash))                     \
      {                                                                 \
        __collisions++;                                                 \
        continue;                                                       \
      }                                                                 \
      __count[hash % (1 << PRECISION)]++;                               \
    }                                                                   \
  } while (0)

#define UNIFORMITY_DEVIATION(__deviation, __count)              \
  do {                                                          \
    double total_deviation = 0.0;                               \
    double expected_count = ITERATIONS / (1ULL << PRECISION);   \
    for (unsigned int i = 0; i < (1 << PRECISION); ++i)         \
    {                                                           \
      total_deviation += absd(__count[i] - expected_count);     \
    }                                                           \
    __deviation = total_deviation / (1 << PRECISION);           \
  } while (0)

#define PRINT_RESULT(__hash_name, __collisions, __mean_deviation) \
    printf("| %-23.23s | %-12d | %-12.12f |\n", #__hash_name, __collisions, __mean_deviation);

static inline bool eq_u32(uint32_t a, uint32_t b) { return a == b; }
HASHSET_DECLARE(uint32_t, u32, micro_hash_int32_wang, eq_u32)

TEST(hash_tests, micro_hash_int32_wang)
{
  unsigned int *count = calloc(sizeof(unsigned int), (1 << PRECISION));

  u32_set s;
  u32_set_init(&s);

  unsigned int collisions;
  COUNT_COLLISIONS(micro_hash_int32_wang, uint32_t, u32, lcg32, count, collisions);
  
  double mean_deviation;
  UNIFORMITY_DEVIATION(mean_deviation, count);

  PRINT_RESULT(micro_hash_int32_wang, collisions, mean_deviation);
  
  free(count);
  u32_set_free(&s);
  
  TEST_SUCCESS;
}

TEST(hash_tests, micro_hash_int32_wang2)
{
  unsigned int *count = calloc(sizeof(unsigned int), (1 << PRECISION));

  u32_set s;
  u32_set_init(&s);

  unsigned int collisions;
  COUNT_COLLISIONS(micro_hash_int32_wang2, uint32_t, u32, lcg32, count, collisions);
  
  double mean_deviation;
  UNIFORMITY_DEVIATION(mean_deviation, count);

  PRINT_RESULT(micro_hash_int32_wang2, collisions, mean_deviation);
  
  free(count);
  u32_set_free(&s);
  
  TEST_SUCCESS;
}

TEST(hash_tests, micro_hash_int32_rob)
{
  unsigned int *count = calloc(sizeof(unsigned int), (1 << PRECISION));

  u32_set s;
  u32_set_init(&s);

  unsigned int collisions;
  COUNT_COLLISIONS(micro_hash_int32_rob, uint32_t, u32, lcg32, count, collisions);
  
  double mean_deviation;
  UNIFORMITY_DEVIATION(mean_deviation, count);

  PRINT_RESULT(micro_hash_int32_rob, collisions, mean_deviation);
  
  free(count);
  u32_set_free(&s);
  
  TEST_SUCCESS;
}

static inline bool eq_u64(uint64_t a, uint64_t b) { return a == b; }
HASHSET_DECLARE(uint64_t, u64, micro_hash_int64_wang, eq_u64)
  
TEST(hash_tests, micro_hash_int64_wang)
{
  unsigned int *count = calloc(sizeof(unsigned int), (1 << PRECISION));

  u64_set s;
  u64_set_init(&s);

  unsigned int collisions;
  COUNT_COLLISIONS(micro_hash_int64_wang, uint64_t, u64, lcg64, count, collisions);
  
  double mean_deviation;
  UNIFORMITY_DEVIATION(mean_deviation, count);

  PRINT_RESULT(micro_hash_int64_wang, collisions, mean_deviation);
  
  free(count);
  u64_set_free(&s);
  
  TEST_SUCCESS;
}

TEST(hash_tests, micro_hash_int6432_wang)
{
  unsigned int *count = calloc(sizeof(unsigned int), (1 << PRECISION));

  u32_set s;
  u32_set_init(&s);

  unsigned int collisions;
  COUNT_COLLISIONS(micro_hash_int6432_wang, uint32_t, u64, lcg64, count, collisions);
  
  double mean_deviation;
  UNIFORMITY_DEVIATION(mean_deviation, count);

  PRINT_RESULT(micro_hash_int6432_wang, collisions, mean_deviation);
  
  free(count);
  u32_set_free(&s);
  
  TEST_SUCCESS;
}
 
int main(int argc, char **argv)
{
  printf("Iterating over %d random values...\n", ITERATIONS);
  printf("Precision set to %d\n", PRECISION);
  printf("/----------------------------------------------------------\\\n");
  printf("|      hash function      |  collisions  |  non-uniformity |\n");
  printf("| ----------------------- | ------------ | --------------- |\n");

  int out = micro_tests_run(argc, argv);

  printf("\\----------------------------------------------------------/\n");
  
  return out;
}
