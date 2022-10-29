#pragma once
#include <stdlib.h>
#include "linked_list.h"

typedef struct iterator ioopm_list_iterator_t;

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is at least one more element 
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter the iterator
/// @return the next element
bool ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element
void *ioopm_iterator_remove(ioopm_list_iterator_t *iter);

/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, void *element, size_t element_size);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element index from the underlying list
/// @param iter the iterator
/// @return the current element index
int ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Create an iterator from existing list, set current_element to head.
/// @param source_list the list to be iterated upon
/// @return a pointer to an iterator
ioopm_list_iterator_t *ioopm_iterator_initialize(ioopm_list_t *source_list);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t **iter);