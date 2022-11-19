#pragma once
#include <stddef.h>
#include "linked_list.h"

typedef struct element element_t;

typedef struct list ioopm_list_t;

struct element
{
    void *data;
    size_t data_size;
    element_t *next;
};

struct list
{
    element_t *head;
    element_t *last;
    size_t size;
};

/// @brief Collects element before index
/// @param list the list to search through
/// @param index the index after element
/// @return element just before index
element_t *get_element_before_index(ioopm_list_t *list, int index);

/// @brief Creates a new element.
/// @param data the data to insert into element.
/// @param data_size the size of 'data'. 
/// @return new element with 'data' and 'data_size' 
element_t *create_element(void *data, size_t data_size);

/// @brief Frees an element completely
/// @param element to be freed
void free_element(element_t *element);