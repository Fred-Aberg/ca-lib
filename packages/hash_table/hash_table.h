#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "common.h"
#include "linked_list.h"

/**
 * @file hash_table.h
 * @author Marta Björknäs, Fred Åberg
 * @date 14 Sep 2022
 * @brief Simple hash table that maps integer keys to string values.
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
**/

typedef struct hash_table ioopm_hash_table_t;

typedef int (*ioopm_hash_function) (void *key);

/// @brief Create a new hash table
/// @param order_keys an order function for keys
/// @param order_values an order function for values
/// @param hash_func a function to convert keys to integers > 0
/// @return a new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(size_t no_buckets ,ioopm_order_function order_keys, ioopm_order_function order_values, ioopm_hash_function hash_func, ioopm_alloc_function key_alloc, ioopm_alloc_function data_alloc, ioopm_free_function key_free, ioopm_free_function data_free);

/// @brief Delete a hash table and completely free its memory
/// @param ht an address to a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t **ht);

/// @brief Add key, value and their sizes to an entry in a hash table,
/// when key is run through the hash function it must result in a key > 0
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
/// @param key_size the byte-size of key
/// @param value_size the byte-size of value
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, void *key, void *value, size_t key_size, size_t value_size);

/// @brief Lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param value a pointer to an alloced copy of value at 'key', iff lookup was successful
/// @return true if lookup was successful, else false
bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, void *key, void **value);

/// @brief Remove any element in a hash table
/// @param ht hash table operated upon
/// @param key key to remove
/// @param value a pointer to the alloced value of the removed element, iff removal was successful
/// @return true if removal was successful, else false
bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, void *key, void **value);

/// @brief Return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param ht hash table operated upon
/// @return a linked list with all keys of ht
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

/// @brief Return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param ht hash table operated upon
/// @return a linked list with all values of ht
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht);

/// @brief Gets the size of hash table
/// @param ht hash table operated upon
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief Checks if a hash table is empty
/// @param ht hash table operated upon
bool ioopm_hash_table_empty(ioopm_hash_table_t *ht);

/// @brief Removes all entries in a hash table
/// @param ht hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief Returns true if 'key' exists in given hash table
/// @param ht hash table operated upon
/// @param key key to be searched
/// @return true if 'ht' has 'key', else false
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, void *key);

/// @brief Returns true if 'value' exists in given hash table
/// @param ht hash table operated upon
/// @param value value to be searched
/// @return true if 'ht' has 'value', else false
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, void *value);

/// @brief Check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief Check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

/// @brief Apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_func the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_func, void *arg);


//-----------------------------------------------------------------------------------//

// Sample functions that a user can chose to use when creating a hash table

/// @brief Checks the order of two integers
/// @param x first integer to compare
/// @param y second integer to compare
/// @return 1 iff x>y, 0 iff x==y, and -1 iff x<y
int ioopm_int_order(void *x, void *y);

/// @brief Checks the order of two strings
/// @param x first string to compare
/// @param y second string to compare
/// @return 1 iff x>y, 0 iff x==y, and -1 iff x<y
int ioopm_string_order(void *x, void *y);

/// @brief Hash function to convert an integer to an integer hash key
/// must return a hash key > 0
/// @param key the key to convert
/// @return the converted key
int ioopm_hash_function_int(void *key);

/// @brief Hash function to convert a string to an integer hash key
/// must return a hash key > 0
/// @param key the key to convert
/// @return the converted key
int ioopm_hash_function_string(void *key);

/// @brief Hash function to sort a key into buckets corresponding to A-Z
/// @param key key to convert
/// @return the converted key
int ioopm_hash_function_alphabetical(void *key);

/// @brief Allocate a type that consists of a single pointer, eg. int *, char * etc.
/// @param data_loc The address to the pointer to be alloced
/// @param data The piece of data that is to be copied to 'data_loc'
/// @param data_size The size of 'data'
void ioopm_alloc_simple_ptr(void **data_loc, void *data, size_t data_size);

/// @brief Free an allocated pointer
/// @param data_loc The address to the pointer to be freed
void ioopm_free_simple_ptr(void **data_loc);
