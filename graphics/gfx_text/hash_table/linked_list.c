#include "linked_list.h"
#include "linked_list_struct_def.h"


//-------------------------------------------------//


/// @brief Collects element before index
/// @param list the list to search through
/// @param index the index after element
/// @return element just before index
element_t *get_element_before_index(ioopm_list_t *list, int index);


/// @brief Creates a new element.
/// @param data the data to insert into element.
/// @param data_size the size of 'data'. 
/// @return new element with 'data' and 'data_size' 
element_t *create_element(ioopm_list_t *list, void *data, size_t data_size);


/// @brief Completely frees an element
/// @param element element to be freed
void free_element(ioopm_list_t *list, element_t *element);


//-------------------------------------------------//


ioopm_list_t *ioopm_linked_list_create(ioopm_alloc_function alloc_func, ioopm_free_function free_func)
{
    ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
    list->data_alloc_func = alloc_func;
    list->data_free_func = free_func;
    list->head = NULL;
    list->last = NULL;
    list->size = 0;
    return list;
}


void ioopm_linked_list_destroy(ioopm_list_t **list)
{    
    ioopm_linked_list_clear(*list);
    free(*list);
    *list = NULL;
}


void ioopm_linked_list_clear(ioopm_list_t *list)
{
    element_t *current = list->head;

    if (current == NULL)
    {
        return;
    }
    
    while (current != list->last)
    {
        element_t *tmp = current; // set to match current
        current = current->next; //iterate current          --- ERROR EXAMPLE
        free_element(list, tmp);
    }
    
    free_element(list, list->last); // make sure last is freed as well

    list->head = NULL;
    list->last = NULL;
    list->size = 0;
}


bool ioopm_linked_list_insert(ioopm_list_t *list, int index, void *data, size_t data_size)
{
    size_t list_size = ioopm_linked_list_size(list);

    if (index < 0 || index > list_size) // if index not in list, do not insert anything
    {
        return false;
    }

    element_t *new_element = create_element(list, data, data_size);

    if (list_size == 0) // edge case, insert to empty list
    {
        list->head = new_element;
        list->last = new_element;
    }
    else if (index == 0) // edge case, insert first, need to reset head
    {
        new_element->next = list->head;
        list->head = new_element;
    }
    else if (index == list_size) // edge case, insert list, need to reset last
    {
        list->last->next = new_element;
        list->last = new_element;
    }
    else
    {
        element_t *previous_element = get_element_before_index(list, index);
        new_element->next = previous_element->next;
        previous_element->next = new_element;
    }
    
    list->size++;
    return true;
}


void ioopm_linked_list_append(ioopm_list_t *list, void *data, size_t data_size)
{
    ioopm_linked_list_insert(list, ioopm_linked_list_size(list), data, data_size);
}


void ioopm_linked_list_prepend(ioopm_list_t *list, void *data, size_t data_size)
{
    ioopm_linked_list_insert(list, 0, data, data_size);
}


bool ioopm_linked_list_remove(ioopm_list_t *list, int index, void **data)
{
    size_t list_size = ioopm_linked_list_size(list);

    if (index < 0 || index + 1 > list_size) // if index not in list, do not remove anything, also catches empty list
    {
        return false;
    }

    element_t *removed_element;

    if(list_size == 1) // edge case, only one element in list, need to reset head and last
    {
        removed_element = list->head;
        list->head = NULL; // head, last set to null.
        list->last = NULL;
    }
    else if(index == 0) // edge case, remove first element, need to set head
    {
        removed_element = list->head;
        list->head = list->head->next; // skip head, original head is now unlinked from the list
    }
    else if(index == list_size - 1) // edge case, remove last element, need to set last
    {
        removed_element = list->last;
        list->last = get_element_before_index(list, index);
        list->last->next = NULL;
    }
    else
    {
        element_t *previous_element = get_element_before_index(list, index);
        removed_element = previous_element->next;
        previous_element->next = removed_element->next; // skip element to be removed
    }

    // Data inherits the values of removed_element
    *data = removed_element->data;

    free(removed_element);
    list->size--;
    return true;
}


size_t ioopm_linked_list_size(ioopm_list_t *list)
{
    return list->size;
}


bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
    return ioopm_linked_list_size(list) == 0;
}


bool ioopm_linked_list_get(ioopm_list_t *list, int index, void **data)
{
    if (index < 0 || index + 1 > list->size) // check that index is valid
    {
        return false;
    }

    element_t *gotten_element;

    if (index == 0) // edge case, doesn't have a previous value
    {
        gotten_element = list->head;
    }
    else
    {
        gotten_element = get_element_before_index(list, index)->next;
    }
    
    *data = calloc(1, gotten_element->data_size);
    *data = memcpy(*data, gotten_element->data, gotten_element->data_size);
    return true;
}


bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    if (ioopm_linked_list_is_empty(list))
    {
        return false;
    }

    element_t *current_element = list->head;

    int index = 0;

    while (current_element != list->last)
    {
        if (prop(&index, current_element->data, extra))
        {
            return true;
        }

        current_element = current_element->next;
        ++index;
    }    

    return false || prop(&index, current_element->data, extra); // we need to check the last element as well
}


bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    if (ioopm_linked_list_is_empty(list))
    {
        return true;
    }
    
    element_t *current_element = list->head;

    int index = 0;

    while (current_element != list->last)
    {
        if (!prop(&index, current_element->data, extra))
        {
            return false;
        }

        current_element = current_element->next;
        ++index;
    }
    return true && prop(&index, current_element->data, extra); // we need to check the last element as well
}


bool ioopm_linked_list_contains(ioopm_list_t *list, ioopm_order_function order, void *data)
{
    element_t *current_element = list->head;

    while (current_element != NULL)
    {
        if (order(data, current_element->data) == 0)
        {
            return true;
        }

        current_element = current_element->next;
    }
    return false;
}


void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_function apply_func, void *extra)
{
    if (list->size == 0)
    {
        return;
    }

    element_t *current_element = list->head;
    int index = 0;

    while (current_element != list->last)
    {
        apply_func(&index, &(current_element->data), extra);
        current_element = current_element->next;
        index++;
    }
}

//-------------------------------------------------//

element_t *create_element(ioopm_list_t *list, void *data, size_t data_size)
{
    element_t *element = calloc(1, sizeof(element_t));
    list->data_alloc_func(&element->data, data, data_size);
    element->data_size = data_size;
    element->next = NULL;
    return element;
}

element_t *get_element_before_index(ioopm_list_t *list, int index)
{
    element_t *current = list->head; // set current to first element in list

    for (int i = 0; i < index - 1; ++i)
    {
        current = current->next;
    }
    
    return current;
}

void free_element(ioopm_list_t *list, element_t *element)
{
    list->data_free_func(&element->data);
    free(element);
}
