//////////////////////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
//
// hashset.h
// ---------
//
// Simple implementation of an hashset in C99 for any type, uses
// macros.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//

#ifndef _HASHSET_H_
#define _HASHSET_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//
// Configuration
//

#define HASHSET_INITIAL_CAPACITY 16
#define HASHSET_MAX_LOAD_FACTOR 0.7

//
// Macros
//

#define HASHSET_DECLARE(type, prefix, hash_fn, eq_fn)                          \
typedef struct {                                                               \
    type *data;                                                                \
    uint8_t *state; /* 0=empty,1=used,2=deleted */                             \
    size_t size;                                                               \
    size_t capacity;                                                           \
} prefix##_set;                                                                \
                                                                               \
static inline void prefix##_set_init(prefix##_set *set) {                      \
    set->size = 0;                                                             \
    set->capacity = HASHSET_INITIAL_CAPACITY;                                  \
    set->data = malloc(set->capacity * sizeof(type));                          \
    set->state = calloc(set->capacity, sizeof(uint8_t));                       \
}                                                                              \
                                                                               \
static inline void prefix##_set_free(prefix##_set *set) {                      \
    free(set->data);                                                           \
    free(set->state);                                                          \
    set->data = NULL; set->state = NULL;                                       \
    set->size = set->capacity = 0;                                             \
}                                                                              \
                                                                               \
static inline size_t prefix##_set_find_slot(prefix##_set *set, type key) {     \
    size_t mask = set->capacity - 1;                                           \
    size_t idx = hash_fn(key) & mask;                                          \
    size_t start = idx;                                                        \
    while (set->state[idx] == 1 && !eq_fn(set->data[idx], key)) {              \
        idx = (idx + 1) & mask;                                                \
        if (idx == start) return set->capacity; /* full */                     \
    }                                                                          \
    return idx;                                                                \
}                                                                              \
                                                                               \
static inline void prefix##_set_resize(prefix##_set *set, size_t newcap) {     \
    type *old_data = set->data;                                                \
    uint8_t *old_state = set->state;                                           \
    size_t old_cap = set->capacity;                                            \
                                                                               \
    set->capacity = newcap;                                                    \
    set->data = malloc(newcap * sizeof(type));                                 \
    set->state = calloc(newcap, sizeof(uint8_t));                              \
    set->size = 0;                                                             \
                                                                               \
    for (size_t i = 0; i < old_cap; i++) {                                     \
        if (old_state[i] == 1) {                                               \
            type val = old_data[i];                                            \
            size_t slot = prefix##_set_find_slot(set, val);                    \
            set->data[slot] = val;                                             \
            set->state[slot] = 1;                                              \
            set->size++;                                                       \
        }                                                                      \
    }                                                                          \
    free(old_data); free(old_state);                                           \
}                                                                              \
                                                                               \
static inline bool prefix##_set_insert(prefix##_set *set, type key) {          \
    if ((double)set->size / set->capacity > HASHSET_MAX_LOAD_FACTOR)           \
        prefix##_set_resize(set, set->capacity * 2);                           \
                                                                               \
    size_t idx = prefix##_set_find_slot(set, key);                             \
    if (set->state[idx] == 1) return false; /* already exists */               \
    set->data[idx] = key;                                                      \
    set->state[idx] = 1;                                                       \
    set->size++;                                                               \
    return true;                                                               \
}                                                                              \
                                                                               \
static inline bool prefix##_set_contains(prefix##_set *set, type key) {        \
    size_t idx = prefix##_set_find_slot(set, key);                             \
    return set->state[idx] == 1;                                               \
}                                                                              \
                                                                               \
static inline bool prefix##_set_remove(prefix##_set *set, type key) {          \
    size_t idx = prefix##_set_find_slot(set, key);                             \
    if (set->state[idx] != 1) return false;                                    \
    set->state[idx] = 2; /* mark deleted */                                    \
    set->size--;                                                               \
    return true;                                                               \
}

//
// Examples
//

#if 0

#include <stdio.h>
#include "hashset.h"

// Declare a uint64_t hashset
HASHSET_DECLARE(uint64_t, u64, hash_u64, eq_u64)

int main(void) {
    u64_set s;
    u64_set_init(&s);

    u64_set_insert(&s, 42);
    u64_set_insert(&s, 1337);

    if (u64_set_contains(&s, 42))
        printf("Found 42!\n");

    u64_set_remove(&s, 42);

    if (!u64_set_contains(&s, 42))
        printf("42 removed!\n");

    u64_set_free(&s);
    return 0;
}

#endif // 0

#endif // _HASHSET_H_
