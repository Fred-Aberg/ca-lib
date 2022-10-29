#pragma once
#include <stddef.h>
#include "common.h"
#include "linked_list.h"

typedef struct element element_t;

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
    ioopm_alloc_function data_alloc_func;
    ioopm_free_function data_free_func;
};

/// @brief Creates a new element.
/// @param data the data to insert into element.
/// @param data_size the size of 'data'. 
/// @return new element with 'data' and 'data_size' 
element_t *create_element(void *data, size_t data_size);

// /// @brief Collects element before index
// /// @param list the list to search through
// /// @param index the index after element
// /// @return element just before index
// element_t *get_element_before_index(ioopm_list_t *list, int index);

/// @brief Frees an element completely
/// @param element to be freed
void free_element(element_t *element);

// /// @brief Edits the value of an element in list.
// /// @param element which will be changed
// /// @param value the value that element will be changed into
// void edit_value_of_element(element_t *element, void *value, int value_count);

// /// @brief Copies the value of 'element' into 'copy'
// /// @param element element 
// /// @param copy the copy destination
// void copy_element_value(element_t *element, val_t *copy);