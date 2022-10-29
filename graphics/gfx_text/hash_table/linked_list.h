#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "common.h"

typedef struct list ioopm_list_t;

/// @brief Creates a new empty list
/// @return an empty linked list
ioopm_list_t *ioopm_linked_list_create(ioopm_alloc_function alloc_func, ioopm_free_function free_func);

/// @brief Tear down the linked list and return all its memory
/// @param list the list to be destroyed
void ioopm_linked_list_destroy(ioopm_list_t **list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended to
/// @param data the data to be appended
/// @param data_size the size of data
void ioopm_linked_list_append(ioopm_list_t *list, void *data, size_t data_size);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param data the data to be prepended
/// @param data_size the size of data
void ioopm_linked_list_prepend(ioopm_list_t *list, void *data, size_t data_size);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param data the data to be inserted 
/// @param data_size the size of data
/// @return true if insert was successful, else false
bool ioopm_linked_list_insert(ioopm_list_t *list, int index, void *data, size_t data_size);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list
/// @param index the position in the list
/// @param data a pointer to the alloced data of the removed element, iff removal was successful
/// @return true if removal was successful, else false
bool ioopm_linked_list_remove(ioopm_list_t *list, int index, void **data);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param data a pointer to an alloced copy of data at 'index', iff get was successful
/// @return true if get was successful, else false
bool ioopm_linked_list_get(ioopm_list_t *list, int index, void **data);

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param order the equality test (function pointer)
/// @param element the element sought
/// @return true if element is in the list, else false
bool ioopm_linked_list_contains(ioopm_list_t *list, ioopm_order_function order, void *data);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_linked_list_size(ioopm_list_t *list);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_linked_list_is_empty(ioopm_list_t *list);

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ioopm_linked_list_clear(ioopm_list_t *list);

/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, void *extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param apply_func the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of 'apply_func'
void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_function apply_func, void *extra);