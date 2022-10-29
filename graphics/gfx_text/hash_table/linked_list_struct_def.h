#pragma once

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
    ioopm_alloc_function data_alloc_func;
    ioopm_free_function data_free_func;
};