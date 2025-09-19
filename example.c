// SPDX-License-Identifier: MIT

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
