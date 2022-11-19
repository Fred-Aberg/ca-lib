#pragma once

typedef bool (*ioopm_predicate) (void *key, void *data, void *extra);

typedef int(*ioopm_order_function)(void *a, void *b);

typedef void (*ioopm_apply_function) (void *key, void **data, void *extra);

typedef void (*ioopm_alloc_function) (void **data_loc, void *data, size_t data_size);

typedef void (*ioopm_free_function) (void **data);