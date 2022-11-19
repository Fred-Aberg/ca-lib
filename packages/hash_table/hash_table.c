#include "hash_table.h"
#include "linked_list.h"
#include <ctype.h>

// __________________________________________ //


#define No_Buckets 17 // Separate from the definition in unit_tests.c, make sure they are the same!

typedef struct entry entry_t;

struct entry
{
    void *key; 
    void *value; 
    size_t key_size; // byte-size of key
    size_t value_size; // byte-size of value
    entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
    ioopm_order_function order_keys;
    ioopm_order_function order_values;
    ioopm_hash_function hash_func;
    ioopm_alloc_function key_alloc_func;
    ioopm_free_function key_free_func;
    ioopm_alloc_function data_alloc_func;
    ioopm_free_function data_free_func;
    size_t size;
    size_t no_buckets;
    entry_t buckets[];
};

/// @brief collects entry just before key
/// @param ht hash table to search thourgh
/// @param entry bucket to go through 
/// @param key key for possible entry just after the entry returned 
/// @return entry just before the (hypothetical) entry with key 'key'
static entry_t *find_previous_entry_for_key(ioopm_hash_table_t *ht, void *key);

/// @brief creates a entry with entered with arguments inserted
/// @param key key to give entry
/// @param value value to give entry
/// @param key_size byte-size of 'key'
/// @param value_size byte-size of 'value'
/// @param next the next entry in list
/// @return new entry with 'key', 'value' calloced and 'value_size', 'key_size', and 'next' all linked to entry
static entry_t *entry_create(ioopm_hash_table_t *ht,void *key, void* value, size_t key_size, size_t value_size, entry_t *next);

/// @brief completely frees an entry and all its fields
/// @param entry the entry to free
static void free_entry(ioopm_hash_table_t *ht, entry_t *entry);

/// @brief removes all entries in a bucket
/// @param entry the bucket to clear
static void clear_bucket(ioopm_hash_table_t *ht, entry_t *entry);

/// @brief changes one alloced value to another value
/// @param old_value the address to a pointer to the old value to replace
/// @param new_value a pointer to the new value 
/// @param new_value_size the byte-size of 'new_value'
static void edit_value(ioopm_hash_table_t *ht, void **old_value, void *new_value, size_t new_value_size);

/// @brief gets the bucket a key should be in
/// @param ht the hash table to get buckets from
/// @param key the key to get a bucket for
/// @return the bucket for 'key'
static entry_t *get_bucket_for_key(ioopm_hash_table_t *ht, void *key);

/// @brief gets the bucket at index from a hash table
/// index is in [0, n-1] where n is the number of buckets in the hash table 
/// @param ht the hash table to get the bucket from
/// @param index the index of bucket to get
/// @return the bucket at position 'index'
static entry_t *get_bucket_for_index(ioopm_hash_table_t *ht, int index);

/// @brief checks if entered key is valid
/// @param ht the hash table key is used in
/// @param key the key to check validity for
/// @return true if key is valid, else false
static bool is_key_valid(ioopm_hash_table_t *ht, void *key);


//-----------------------------------------------------------------------------------//


ioopm_hash_table_t *ioopm_hash_table_create(size_t no_buckets ,ioopm_order_function order_keys, ioopm_order_function order_values, ioopm_hash_function hash_func, ioopm_alloc_function key_alloc, ioopm_alloc_function data_alloc, ioopm_free_function key_free, ioopm_free_function data_free)
{
    
    size_t buckets_size = no_buckets * sizeof(entry_t);
    ioopm_hash_table_t *table = calloc(1, sizeof(ioopm_hash_table_t) + buckets_size);

    table->no_buckets = no_buckets;

    table->order_keys = order_keys;
    table->order_values = order_values;
    table->hash_func = hash_func;

    table->data_alloc_func = data_alloc;
    table->data_free_func = data_free;
    table->key_alloc_func = key_alloc;
    table->key_free_func = key_free;

    table->size = 0; // initialize hash table size to 0
    return table;
}


void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    entry_t *bucket;

    for (int i = 0; i < ht->no_buckets; ++i) // iterate through each bucket and clear them one and one
    {
        bucket = get_bucket_for_index(ht, i);

        if(bucket->next != NULL) // check that bucket is not already empty
        {
            clear_bucket(ht, bucket->next);
            bucket->next = NULL; // set buckets pointer to null
        }
    }
    ht->size = 0; // reset ht size
}


void ioopm_hash_table_destroy(ioopm_hash_table_t **ht) 
{
    ioopm_hash_table_clear(*ht); // clears all entries except dummies
    free(*ht); // frees ht and its dummies
    *ht = NULL;
}


void ioopm_hash_table_insert(ioopm_hash_table_t *ht, void *key, void *value, size_t key_size, size_t value_size)
{
    if (!is_key_valid(ht, key)) // edge case, key invalid
    {
        return;
    }

    entry_t *previous_entry = find_previous_entry_for_key(ht, key);
    entry_t *current_entry = previous_entry->next;

    // check if next entry should be updated or not
    if (current_entry != NULL && ht->order_keys(current_entry->key, key) == 0)
    {
        edit_value(ht, &(current_entry->value), value, value_size);
        current_entry->value_size = value_size;
    }
    else // insert new entry at key
    {
        ht->size++;
        previous_entry->next = entry_create(ht ,key, value, key_size, value_size, previous_entry->next);
    }   
}


bool ioopm_hash_table_lookup(ioopm_hash_table_t *ht, void *key, void **value)
{
    if (!is_key_valid(ht, key)) // edge case, key invalid
    {
        return false;
    }

    // find the previous entry for key
    entry_t *previous_element = find_previous_entry_for_key(ht, key);
    entry_t *current_element = previous_element->next;

    if (current_element && ht->order_keys(current_element->key, key) == 0) // checks if we found a matching key
    {
        *value = calloc(1, current_element->value_size);
        *value = memcpy(*value, current_element->value, current_element->value_size);
        return true;
    }
    return false;
}


size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
    return ht->size;
}


bool ioopm_hash_table_empty(ioopm_hash_table_t *ht)
{
    return ioopm_hash_table_size(ht) == 0;
}


bool ioopm_hash_table_remove(ioopm_hash_table_t *ht, void *key, void **value)
{
    if (!is_key_valid(ht, key)) // edge case, key invalid
    {
        return false;
    }

    // find the previous entry for key
    entry_t *previous_element = find_previous_entry_for_key(ht, key);
    entry_t *current_element = previous_element->next;

    if (current_element && ht->order_keys(current_element->key, key) == 0) // runs if key is found and current valid
    {
        previous_element->next = current_element->next;

        *value = current_element->value; // value inherits the element's value
        ht->key_free_func(&current_element->key);
        free(current_element);
        ht->size--;
        return true;
    }
    else
    {
        return false;
    }
}


ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
    // The key-list will inherit the hash table's key-memory functions
    ioopm_list_t *keys = ioopm_linked_list_create(ht->key_alloc_func, ht->key_free_func);

    for (int i = 0; i < ht->no_buckets; ++i)
    {
        entry_t *entry = get_bucket_for_index(ht, i); // sets entry to the current bucket being iterated over
        
        while (entry->next != NULL)
        {
            entry = entry->next;
            ioopm_linked_list_append(keys, entry->key, entry->key_size);
        }
    }

    return keys;
}    


ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
    // The values-list will inherit the hash table's value-memory functions
    ioopm_list_t *values = ioopm_linked_list_create(ht->data_alloc_func, ht->data_free_func);

    for (int i = 0; i < ht->no_buckets; i++)
    {
        entry_t *entry = get_bucket_for_index(ht, i); // sets entry to the current bucket being iterated over

        while (entry->next != NULL)
        {
            entry = entry->next;
            ioopm_linked_list_append(values, entry->value, entry->value_size);

        }       
    }

    return values;
}


bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
   for (int i = 0; i < ht->no_buckets; i++)
   {
        entry_t *entry = get_bucket_for_index(ht, i);

        while (entry->next != NULL)
        {
            entry = entry->next;

            if (pred(entry->key, entry->value, arg))
            {
                return true;
            }
        }
   }

   return false;
}


bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    for (int i = 0; i < ht->no_buckets; i++)
   {
        entry_t *entry = get_bucket_for_index(ht, i);

        while (entry->next != NULL)
        {
            entry = entry->next;

            if (!pred(entry->key, entry->value, arg))
            {
                return false;
            }
        }
   }
    return true;
}


void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_func, void *arg)
{
    for (int i = 0; i < ht->no_buckets; ++i) // iterates over every bucket
    {
        entry_t *entry = get_bucket_for_index(ht, i); // gets said bucket
        
        while (entry->next != NULL)
        {
            entry = entry->next;
            apply_func(entry->key, &(entry->value), arg); // applies apply_func onto said entry
        } 
    }
}


bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, void *key)
{
    for (int i = 0; i < ht->no_buckets; i++)
   {
        entry_t *entry = get_bucket_for_index(ht, i);

        while (entry->next != NULL)
        {
            entry = entry->next;
            if (ht->order_keys(entry->key, key) == 0)
            {
                return true;
            }
        }
   }
   return false;
}


bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, void *value)
{
    for (int i = 0; i < ht->no_buckets; i++)
   {
        entry_t *entry = get_bucket_for_index(ht, i);

        while (entry->next != NULL)
        {
            entry = entry->next;
            if (ht->order_values(entry->value, value) == 0)
            {
                return true;
            }
        }
   }
   return false;
}


//-----------------------------------------------------------------------------------//


int ioopm_int_order(void *first_key, void *second_key)
{
    if (*(int *)first_key < *(int *)second_key)
    {
        return -1;
    }
    else if (*(int *)first_key == *(int *)second_key)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int ioopm_string_order(void *x, void *y)
{
  return strcmp((char *)x, (char *)y);
}

int ioopm_hash_function_int(void *key)
{
    return abs(*(int *)key); // + 1
}

int ioopm_hash_function_string(void *key)
{
    return strlen((char *)key) + 1;
}

// Presupposes a HT with 26 buckets.
// a-z --> 97-122
int ioopm_hash_function_alphabetical(void *key)
{
    char hash_char = tolower(*(char *)key);
    if (!isalpha(hash_char))
    {
        return 26; // all non-letters or extra letters go last.
    }
    return (int)hash_char - 97 + 26; // a = 1, z = 27
}

void ioopm_alloc_simple_ptr(void **data_loc, void *data, size_t data_size)
{
    *data_loc = calloc(1, data_size);
    *data_loc = memcpy(*data_loc, data, data_size);
}

void ioopm_free_simple_ptr(void **data_loc)
{
    free(*data_loc);
    *data_loc = NULL;
}

//-----------------------------------------------------------------------------------//


static entry_t *find_previous_entry_for_key(ioopm_hash_table_t *ht, void *key)
{
    entry_t *entry = get_bucket_for_key(ht, key); // begin sarching from bucket

    while (entry->next != NULL) // check so we don't go past the end of the buckets elements
    {
        if (ht->order_keys(entry->next->key, key) >= 0)
        {
            break;
        }
        entry = entry->next; // step forward one entry
    }

    return entry;
}


static entry_t *entry_create(ioopm_hash_table_t *ht, void *key, void* value, size_t key_size, size_t value_size, entry_t *next)
{
    entry_t *entry = calloc(1, sizeof(entry_t));

    // Allocate key & data
    ht->key_alloc_func(&entry->key, key, key_size);
    ht->data_alloc_func(&entry->value, value, value_size);

    entry->key_size = key_size;
    entry->value_size = value_size;

    entry->next = next;
    return entry;
}


static void free_entry(ioopm_hash_table_t *ht, entry_t *entry)
{
    ht->data_free_func(&entry->value);
    ht->key_free_func(&entry->key);
    free(entry);
}


static void clear_bucket(ioopm_hash_table_t *ht, entry_t *entry)
{
    if(entry->next == NULL)
    {
        free_entry(ht, entry);
    }
    else
    {
        clear_bucket(ht, entry->next);
        free_entry(ht, entry);
    }
}


static entry_t *get_bucket_for_key(ioopm_hash_table_t *ht, void *key)
{
    int internal_key = ht->hash_func(key);
    int bucket = internal_key % ht->no_buckets;
    return get_bucket_for_index(ht, bucket);
}


static entry_t *get_bucket_for_index(ioopm_hash_table_t *ht, int index)
{
    return &(ht->buckets[index]);
}


static void edit_value(ioopm_hash_table_t *ht, void **old_value, void *new_value, size_t new_value_size)
{
    ht->data_free_func(old_value);
    ht->data_alloc_func(old_value, new_value, new_value_size);
}


static bool is_key_valid(ioopm_hash_table_t *ht, void *key)
{
    return ht->hash_func(key) > 0;
}