#include "iterator.h"
#include "linked_list.h"
#include "linked_list_struct_def.h"

struct iterator 
{
    int current_index;
    element_t *current;
    ioopm_list_t *list;
};

ioopm_list_iterator_t *ioopm_iterator_initialize(ioopm_list_t *source_list)
{
    ioopm_list_iterator_t *new_iter = calloc(1, sizeof(ioopm_list_iterator_t));
    new_iter->list = source_list;
    new_iter->current = source_list->head;
    new_iter->current_index = 0;
    return new_iter;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return (iter->current->next != NULL);
}

bool ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
    if (iter->current) // if current is not null
    {
        iter->current = iter->current->next; // increment the pointer
        iter->current_index++; // increment the index
        return true;
    }
    return false; // if current is null, then return false. 
}

void *ioopm_iterator_remove(ioopm_list_iterator_t *iter)
{
    void *value;
    ioopm_linked_list_remove(iter->list, iter->current_index, &value);
    return value;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, void *element, size_t element_size)
{
    if (iter->list->size == 0)
    {
        ioopm_linked_list_insert(iter->list, iter->current_index, element, element_size);
        iter->current = iter->list->head;
        return;
    }
    ioopm_linked_list_insert(iter->list, iter->current_index, element, element_size);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    iter->current = iter->list->head;
}

int ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    return iter->current_index;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t **iter)
{
    free(*iter);
    *iter = NULL; 
}