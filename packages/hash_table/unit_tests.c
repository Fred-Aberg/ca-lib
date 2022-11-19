#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"

// __________________________________________

int init_suite(void) {
  return 0;
}

int clean_suite(void) {
  return 0;
}

// __________________________________________

#pragma region // hash_table tests

#define No_Buckets 17 // Separate from the definition in hash_table.c, make sure they are the same!

/// @brief Creates a hash table with int keys and string values
/// @return Hash table with entries (3, "three"), (20, "twenty"), (4, "four"), (5, "five")
static ioopm_hash_table_t *sample_normal_hash_table()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_string_order, ioopm_hash_function_int, ioopm_alloc_simple_ptr, ioopm_alloc_simple_ptr, ioopm_free_simple_ptr, ioopm_free_simple_ptr);


  int key_a = 3;
  int key_b = 20;
  int key_c = 4;
  int key_d = 5;

  char *value_a = "three";
  char *value_b = "twenty";
  char *value_c = "four";
  char *value_d = "five";

  ioopm_hash_table_insert(ht, &key_a, value_a, sizeof(key_a), strlen(value_a) + 1);
  ioopm_hash_table_insert(ht, &key_b, value_b, sizeof(key_b), strlen(value_b) + 1);
  ioopm_hash_table_insert(ht, &key_c, value_c, sizeof(key_c), strlen(value_c) + 1);
  ioopm_hash_table_insert(ht, &key_d, value_d, sizeof(key_d), strlen(value_d) + 1);

  return ht;
}

/// @brief Creates a hash table with int keys and string values
/// @return Hash table with entries (23, 1), (40, 2), (57, 3), (74, 4)
static ioopm_hash_table_t *sample_same_bucket_hash_table()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);

  int key_a = 1 * No_Buckets + 6;
  int key_b = 2 * No_Buckets + 6;
  int key_c = 3 * No_Buckets + 6;
  int key_d = 4 * No_Buckets + 6;

  int value_a = 1;
  int value_b = 2;
  int value_c = 3;
  int value_d = 4;
  
  ioopm_hash_table_insert(ht, &key_a, &value_a, 4, 4);
  ioopm_hash_table_insert(ht, &key_c, &value_c, 4, 4);
  ioopm_hash_table_insert(ht, &key_d, &value_d, 4, 4);
  ioopm_hash_table_insert(ht, &key_b, &value_b, 4, 4);

  return ht;
}

// Tests create and destroy empty hash table
void test_create_destroy_empty()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, *ioopm_int_order, *ioopm_string_order, *ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(&ht);
  CU_ASSERT_PTR_NULL(ht);
}
 
// Tests destory filled hash table
void test_create_destroy_full()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();
  ioopm_hash_table_destroy(&ht);
  CU_ASSERT_PTR_NULL(ht);
}

// Testing insert with a fresh key that is not already in use
void test_insert_once()
{
  int key = 3;
  char *value = "three";

  void *return_value;

  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_string_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  bool success = ioopm_hash_table_lookup(ht, &key, &return_value);
  CU_ASSERT_FALSE(success);

  ioopm_hash_table_insert(ht, &key, value, sizeof(key), strlen(value) + 1);

  success = ioopm_hash_table_lookup(ht, &key, &return_value);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(0, strcmp(value, (char *)return_value));

  free(return_value);
  ioopm_hash_table_destroy(&ht);
}

// Testing lookup of inserted value
void test_lookup_inserted_value()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();
  
  void *return_value;

  int key = 4;

  ioopm_hash_table_lookup(ht, &key, &return_value);

  CU_ASSERT_EQUAL(0, strcmp("four", (char *)return_value));

  free(return_value);
  ioopm_hash_table_destroy(&ht);
}

// Testing insert with a key that is already in use
void test_insert_key_in_use()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();

  int key = 3;
  char *value = "Wobba!";

  // old value should be overwritten with "Wobba!"
  ioopm_hash_table_insert(ht, &key, value, sizeof(key), strlen(value) + 1);

  void *return_value;

  ioopm_hash_table_lookup(ht, &key, &return_value);

  CU_ASSERT_EQUAL(0, strcmp(value, (char *)return_value));

  free(return_value);
  ioopm_hash_table_destroy(&ht);
}

/// @brief A hash function that may create an invaldi key
/// @param a an integer
/// @return a as an integer
int only_invalid_keys_hash_func(void *a)
{
  return *(int *)a;
}

// Testing invalid key, a key <= 0
void test_lookup_insert_invalid_key()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, only_invalid_keys_hash_func, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);

  int key_a = 1 * No_Buckets + 6;
  int key_b = 2 * No_Buckets + 6;
  int key_c = 3 * No_Buckets + 6;
  int key_d = 4 * No_Buckets + 6;

  int value_a = 1;
  int value_b = 2;
  int value_c = 3;
  int value_d = 4;
  
  ioopm_hash_table_insert(ht, &key_a, &value_a, 4, 4);
  ioopm_hash_table_insert(ht, &key_c, &value_c, 4, 4);
  ioopm_hash_table_insert(ht, &key_d, &value_d, 4, 4);
  ioopm_hash_table_insert(ht, &key_b, &value_b, 4, 4);

  int invalid_key_a = -10;
  int invalid_key_b = 0;
  char *temp_value = "Will not be inserted";

  ioopm_hash_table_insert(ht, &invalid_key_a, temp_value, sizeof(invalid_key_a), strlen(temp_value) + 1);
  ioopm_hash_table_insert(ht, &invalid_key_b, temp_value, sizeof(invalid_key_b), strlen(temp_value) + 1);
  
  void *return_value;
  bool success = ioopm_hash_table_lookup(ht, &invalid_key_a, &return_value); // Should return false
  bool success_edge = ioopm_hash_table_lookup(ht, &invalid_key_b, &return_value); // Should return false

  CU_ASSERT_FALSE(success);
  CU_ASSERT_FALSE(success_edge);

  ioopm_hash_table_destroy(&ht);
}

// Testing lookup with key not in use
void test_lookup_key_not_in_use()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();
  
  int invalid_key = 45;
  void *return_value;

  bool success = ioopm_hash_table_lookup(ht, &invalid_key, &return_value); // Lookup key = 3, load into value.

  CU_ASSERT_FALSE(success);
  ioopm_hash_table_destroy(&ht);
}

// Testing chain adding, mulitple entries one bucket
void test_chain_inserts()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);

  int key_a = 1 * No_Buckets + 6;
  int key_b = 2 * No_Buckets + 6;
  int key_c = 3 * No_Buckets + 6;
  int key_d = 4 * No_Buckets + 6;

  int value_a = 1;
  int value_b = 2;
  int value_c = 3;
  int value_d = 4;
  
  ioopm_hash_table_insert(ht, &key_a, &value_a, 4, 4);
  ioopm_hash_table_insert(ht, &key_c, &value_c, 4, 4);
  ioopm_hash_table_insert(ht, &key_d, &value_d, 4, 4);
  ioopm_hash_table_insert(ht, &key_b, &value_b, 4, 4);

  void *result_a;
  void *result_b;
  void *result_c;
  void *result_d;

  bool a = ioopm_hash_table_lookup(ht, &key_a, &result_a) && *(int *)result_a == value_a;
  bool d = ioopm_hash_table_lookup(ht, &key_d, &result_d) && *(int *)result_d == value_d;
  bool b = ioopm_hash_table_lookup(ht, &key_b, &result_b) && *(int *)result_b == value_b;
  bool c = ioopm_hash_table_lookup(ht, &key_c, &result_c) && *(int *)result_c == value_c;

  CU_ASSERT_TRUE(a && b && c && d);

  free(result_a);
  free(result_b);
  free(result_c);
  free(result_d);

  ioopm_hash_table_destroy(&ht); 
}

//Test remove middle element
void test_remove_middle_element()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int key = 40;
  int value = 2;
  void *result_value;
  bool success = ioopm_hash_table_remove(ht, &key, &result_value);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(value, *(int *)result_value);

  free(result_value);
  ioopm_hash_table_destroy(&ht);
}

//Test remove first element
void test_remove_first_element()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int key = 23;
  int value = 1;
  void *result_value;
  bool success = ioopm_hash_table_remove(ht, &key, &result_value);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(value, *(int *)result_value);

  free(result_value);
  ioopm_hash_table_destroy(&ht);
}

//Test remove last element
void test_remove_last_element()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int key = 74;
  int value = 4;
  void *result_value;
  bool success = ioopm_hash_table_remove(ht, &key, &result_value);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(value, *(int *)result_value);

  free(result_value);
  ioopm_hash_table_destroy(&ht);
}

//Test remove one element
void test_remove_one_element()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();

  int key = 4;
  char *value = "four";

  void *result_value;
  
  bool success = ioopm_hash_table_remove(ht, &key, &result_value);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(0, (strcmp(value, (char *)result_value)));

  free(result_value);
  ioopm_hash_table_destroy(&ht);
}

//Test remove non-existing element
void test_remove_non_existing_element()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int key = 2;
  void *result_value;

  bool success = ioopm_hash_table_remove(ht, &key, &result_value);

  CU_ASSERT_FALSE(success);

  ioopm_hash_table_destroy(&ht);
}

//Test remove invalid key
void test_remove_invalid_key()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int key = -10;
  void *result_value;

  bool success = ioopm_hash_table_remove(ht, &key, &result_value);

  CU_ASSERT_FALSE(success);

  ioopm_hash_table_destroy(&ht);
}

//Test remove twice element
void test_remove_same_element_twice()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int key = 74;
  int value = 4;
  void *result_value_first;
  void *result_value_second;

  bool success_first_remove = ioopm_hash_table_remove(ht, &key, &result_value_first);
  bool success_second_remove = ioopm_hash_table_remove(ht, &key, &result_value_second);

  CU_ASSERT_TRUE(success_first_remove);
  CU_ASSERT_FALSE(success_second_remove);
  CU_ASSERT_EQUAL(value, *(int *)result_value_first);

  free(result_value_first);
  ioopm_hash_table_destroy(&ht);
}

// ////////// REMOVE //////////

void test_hash_table_empty()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  CU_ASSERT_TRUE(ioopm_hash_table_empty(ht)); // ht should be empty upon creation
  int key = 6;
  int value = 6;
  ioopm_hash_table_insert(ht, &key, &value, 4, 4);
  CU_ASSERT_FALSE(ioopm_hash_table_empty(ht)); // shouldn't be empty after insertion
  ioopm_hash_table_destroy(&ht);
}

void test_hash_table_size ()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0); // ht should be empty upon creation

  for (int i = 0; i < 3; ++i)
  {
    int j = i * 2;

    ioopm_hash_table_insert(ht, &i, &j, 4, 4);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), i + 1); 
  }
  
  ioopm_hash_table_destroy(&ht);
}

void test_hash_table_clear()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();
  CU_ASSERT_FALSE(ioopm_hash_table_empty(ht));
  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_empty(ht)); // should be empty after clearing
  ioopm_hash_table_destroy(&ht);
}


// will only take int data atm tyty
int index_of_data(ioopm_list_t *list, void *data_to_find)
{
  for (int i = 0; i < ioopm_linked_list_size(list); ++i)
  {
    void *data_at_current_index;
    ioopm_linked_list_get(list, i, &data_at_current_index);

    if (*(int *)data_at_current_index == *(int *)data_to_find)
    {
      free(data_at_current_index);
      return i;
    }

    free(data_at_current_index);
  }

  return -1;
}

// Tests that all keys inserted are returned
void test_hash_table_keys()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  int original_keys[5] = {4, 8, 9, 15, 393};
  bool found[5] = {false};
  
  int value = 3;

  for (int i = 0; i < 5; ++i)
  {
    ioopm_hash_table_insert(ht, &original_keys[i], &value, 4, 4);
  }

  ioopm_list_t *inserted_keys;
  inserted_keys = ioopm_hash_table_keys(ht);

  for (int i = 0; i < 5; ++i)
  {
    int data_to_find = original_keys[i];
    int index = index_of_data(inserted_keys, &data_to_find);

    if (index == -1)
    {
      CU_FAIL("Found a key that was never inserted!");
    }
    else
    {
      found[i] = true;
    }
  }

  for (int i = 0; i < 5; ++i)
  {
    CU_ASSERT_TRUE(found[i]);
  }
  
  ioopm_linked_list_destroy(&inserted_keys);
  ioopm_hash_table_destroy(&ht);
}


// Tests that all values inserted are returned
void test_hash_table_values()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  int original_values[5] = {4, 8, 9, 15, 393};
  bool found[5] = {false};

  for (int i = 0; i < 5; ++i)
  {
    ioopm_hash_table_insert(ht, &i, &original_values[i], 4, 4);
  }

  ioopm_list_t *inserted_values;
  inserted_values = ioopm_hash_table_values(ht);

  for (int i = 0; i < 5; ++i)
  {
    int data_to_find = original_values[i];
    int index = index_of_data(inserted_values, &data_to_find);

    if (index == -1)
    {
      CU_FAIL("Found a value that was never inserted!");
    }
    else
    {
      found[i] = true;
    }
  }

  for (int i = 0; i < 5; ++i)
  {
    CU_ASSERT_TRUE(found[i]);
  }
  
  ioopm_linked_list_destroy(&inserted_values);
  ioopm_hash_table_destroy(&ht);
}


// Tests that keys and values are returned in corresponding order
void test_hash_table_keys_and_values()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  int original_keys[5] = {3, 10, 42, 1, 99};
  int original_values[5] = {3, 10, 42, 1, 99};

  for (int i = 0; i < 5; ++i)
  {
    int key = original_keys[i];
    int value = original_values[i];
    ioopm_hash_table_insert(ht, &key, &value, sizeof(key), sizeof(value));
  }
  
  ioopm_list_t *inserted_keys = ioopm_hash_table_keys(ht);
  ioopm_list_t *inserted_values = ioopm_hash_table_values(ht);

  for (int i = 0; i < 5; ++i)
  {
    int key_to_find = original_keys[i];
    int value_to_find = original_values[i];

    int index_key = index_of_data(inserted_keys, &key_to_find);
    int index_value = index_of_data(inserted_values, &value_to_find);

    if (index_key == -1)
    {
      CU_FAIL("Found a key that was never inserted!")
    }
    if (index_value == -1)
    {
      CU_FAIL("Found a value that was never inserted!")
    }
    
    CU_ASSERT_TRUE(index_key == index_value);
  }
  
  ioopm_linked_list_destroy(&inserted_keys);
  ioopm_linked_list_destroy(&inserted_values);
  ioopm_hash_table_destroy(&ht);
}

void test_find_existing_value_and_key()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();
  int key = 20;
  char *value = "five";
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, &key)); // should return true
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, value)); // should return true
  ioopm_hash_table_destroy(&ht);
}

void test_find_non_existing_value_and_key()
{
  ioopm_hash_table_t *ht = sample_normal_hash_table();
  int key = 24;
  char *value = "twentyfour";
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, &key)); // should return false
  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, value)); // should return false
  ioopm_hash_table_destroy(&ht);
}

bool predicate_test_int_key_and_int_value_larger_than_extra(void *key, void *value, void *extra)
{
  return *(int *)key > *(int *)extra && *(int *)value > *(int *)extra;
}

void test_has_any_empty_ht()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  CU_ASSERT_FALSE(ioopm_hash_table_any(ht, &predicate_test_int_key_and_int_value_larger_than_extra, NULL));
  ioopm_hash_table_destroy(&ht);
}


void test_has_any_successful()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int larger_than = 2;

  CU_ASSERT_TRUE(ioopm_hash_table_any(ht, &predicate_test_int_key_and_int_value_larger_than_extra, &larger_than));
  ioopm_hash_table_destroy(&ht);
}

void test_has_any_unsuccessful()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int larger_than = 5;

  CU_ASSERT_FALSE(ioopm_hash_table_any(ht, &predicate_test_int_key_and_int_value_larger_than_extra, &larger_than));
  ioopm_hash_table_destroy(&ht);
}


void test_has_all_empty_ht()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, &predicate_test_int_key_and_int_value_larger_than_extra, NULL)); // should be true ? (philosooophyyyy hakuna matata)
  
  ioopm_hash_table_destroy(&ht);
}

void test_has_all_successful()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int larger_than = -1;

  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, &predicate_test_int_key_and_int_value_larger_than_extra, &larger_than));
  ioopm_hash_table_destroy(&ht);
}

void test_has_all_unsuccessful()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  int larger_than = 1;

  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, &predicate_test_int_key_and_int_value_larger_than_extra, &larger_than));
  ioopm_hash_table_destroy(&ht);
}

// typedef void (*ioopm_apply_function) (void *key, void **data, void *extra);
//

void set_value_to_key(void *key, void **value, void *size)
{
  *value = realloc(*value, sizeof(*(int *)key));
  *value = memcpy(*value, key, sizeof(*(int *)key));
}


void test_apply_to_empty() //TODO BÖRJA HÄR
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(No_Buckets, ioopm_int_order, ioopm_int_order, ioopm_hash_function_int, *ioopm_alloc_simple_ptr, *ioopm_alloc_simple_ptr, *ioopm_free_simple_ptr, *ioopm_free_simple_ptr);
  ioopm_hash_table_apply_to_all(ht, *set_value_to_key, NULL);
  ioopm_hash_table_destroy(&ht);
}


void test_apply_to_all()
{
  ioopm_hash_table_t *ht = sample_same_bucket_hash_table();

  size_t size = sizeof(int);
  ioopm_hash_table_apply_to_all(ht, *set_value_to_key, &size);

  int key_a = 23;
  int key_b = 40;
  int key_c = 57;
  int key_d = 74;

  void *value_a;
  void *value_b;
  void *value_c;
  void *value_d;

  ioopm_hash_table_lookup(ht, &key_a, &value_a);
  ioopm_hash_table_lookup(ht, &key_b, &value_b);
  ioopm_hash_table_lookup(ht, &key_c, &value_c);
  ioopm_hash_table_lookup(ht, &key_d, &value_d);

  CU_ASSERT_EQUAL(key_a, *(int *)value_a);
  CU_ASSERT_EQUAL(key_b, *(int *)value_b);
  CU_ASSERT_EQUAL(key_c, *(int *)value_c);
  CU_ASSERT_EQUAL(key_d, *(int *)value_d);

  free(value_a);
  free(value_b);
  free(value_c);
  free(value_d);
  ioopm_hash_table_destroy(&ht);
}


// // This function is written in main() before the tests.
// void visualize_sample_hash_table()
// {
//   ioopm_hash_table_t *ht = ioopm_hash_table_create();

//   // INSERTS
//   ioopm_hash_table_insert(ht, 1 * No_Buckets, "one");
//   ioopm_hash_table_insert(ht, 2 * No_Buckets, "two");
//   ioopm_hash_table_insert(ht, 4 * No_Buckets, "four");
//   ioopm_hash_table_insert(ht, 3 * No_Buckets, "three");

//   ioopm_hash_table_insert(ht, 1 * No_Buckets + 1, "hello");
//   ioopm_hash_table_insert(ht, 2 * No_Buckets + 1, "I");
//   ioopm_hash_table_insert(ht, 3 * No_Buckets + 1, "am");
//   ioopm_hash_table_insert(ht, 4 * No_Buckets + 1, "an");
//   ioopm_hash_table_insert(ht, 5 * No_Buckets + 1, "entry!");

//   ioopm_hash_table_insert(ht, 5 * No_Buckets + 2, "This");
//   ioopm_hash_table_insert(ht, 4 * No_Buckets + 2, "is");
//   ioopm_hash_table_insert(ht, 1 * No_Buckets + 2, "a");
//   ioopm_hash_table_insert(ht, 2 * No_Buckets + 2, "weird");
//   ioopm_hash_table_insert(ht, 3 * No_Buckets + 2, "order!");

//   ioopm_hash_table_insert(ht, 1 * No_Buckets + 5, "Wow!");
//   ioopm_hash_table_insert(ht, 2 * No_Buckets + 5, "vilket");
//   ioopm_hash_table_insert(ht, 3 * No_Buckets + 5, "fint");
//   ioopm_hash_table_insert(ht, 5 * No_Buckets + 5, "hash table!");

//   ioopm_hash_table_insert(ht, 1 * No_Buckets + 16, "Ensam... ):");
//   // -------

//   ioopm_hash_table_print(ht);
//   ioopm_hash_table_clear(ht);
//   ioopm_hash_table_print(ht);

//   ioopm_hash_table_destroy(&ht);
// }

#pragma endregion

#pragma region // linked_list tests

/// @brief Creates an int list
/// @return A list with data 20, 21, 22
ioopm_list_t *create_int_list()
{
  int data_a = 20;
  int data_b = 21;
  int data_c = 22;

  size_t data_size = sizeof(data_a);

  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  ioopm_linked_list_insert(list, 0, &data_a, data_size);
  ioopm_linked_list_insert(list, 1, &data_b, data_size);
  ioopm_linked_list_insert(list, 2, &data_c, data_size);

  return list;
}

/// @brief Creates a string list
/// @return A list with data: "I", "Am", "A", "String", "List"
ioopm_list_t *create_string_list()
{
  char *data_a = "I";
  char *data_b = "Am";
  char *data_c = "A";
  char *data_d = "String";
  char *data_e = "List";

  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  ioopm_linked_list_insert(list, 0, data_a, strlen(data_a) + 1);
  ioopm_linked_list_insert(list, 1, data_b, strlen(data_b) + 1);
  ioopm_linked_list_insert(list, 2, data_c, strlen(data_c) + 1);
  ioopm_linked_list_insert(list, 3, data_d, strlen(data_d) + 1);
  ioopm_linked_list_insert(list, 4, data_e, strlen(data_e) + 1);

  return list;
}

// Tests create and remove on empty list
void test_linked_list_create_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  CU_ASSERT_PTR_NOT_NULL(list);
  ioopm_linked_list_destroy(&list);
  CU_ASSERT_PTR_NULL(list);
}

// Tests create and remove on full list with basic data
void test_linked_list_create_full_basic_data()
{
  ioopm_list_t *list = create_int_list();
  ioopm_linked_list_destroy(&list);
  CU_ASSERT_PTR_NULL(list);
}

// Tests create and remove on full list with non-basic data
void test_linked_list_create_full_non_basic_data()
{
  ioopm_list_t *list = create_string_list();
  ioopm_linked_list_destroy(&list);
  CU_ASSERT_PTR_NULL(list);
}

//Tests clear list with one element
void test_linked_list_clear_one()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  int data = 1;
  ioopm_linked_list_insert(list, 0, &data, sizeof(data));
  ioopm_linked_list_clear(list);

  bool success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests clear list with mulitple elements
void test_linked_list_clear_mulitple()
{
  ioopm_list_t *list = create_int_list();
  
  ioopm_linked_list_clear(list);

  int data_a = 20;
  int data_b = 21;
  int data_c = 22;

  bool a_bool = ioopm_linked_list_contains(list, *ioopm_int_order, &data_a);
  bool b_bool = ioopm_linked_list_contains(list, *ioopm_int_order, &data_b);
  bool c_bool = ioopm_linked_list_contains(list, *ioopm_int_order, &data_c);

  CU_ASSERT_FALSE(a_bool);
  CU_ASSERT_FALSE(b_bool);
  CU_ASSERT_FALSE(c_bool);

  ioopm_linked_list_destroy(&list);
}

// Tests clear on empty list
void test_linked_list_clear_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  ioopm_linked_list_clear(list);
  CU_ASSERT_PTR_NOT_NULL(list);
  ioopm_linked_list_destroy(&list);
}

// Tests double clear
void test_linked_list_clear_double()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int data = 1;

  ioopm_linked_list_append(list, &data, sizeof(int));
  ioopm_linked_list_clear(list); // first clear, removes element with value 1
  ioopm_linked_list_clear(list); // second clear, clears the already empty list

  CU_ASSERT_PTR_NOT_NULL(list);

  ioopm_linked_list_destroy(&list);
}

// Remove element from list of size 1
void test_linked_list_remove_single()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int data = 20;

  ioopm_linked_list_append(list, &data, sizeof(data));

  void *result;

  bool success = ioopm_linked_list_remove(list, 0, &result);

  CU_ASSERT_TRUE(success);          // make sure it succeeds
  CU_ASSERT_EQUAL(data, * (int *) result);    // make sure return value matches entered value
  free(result);
  ioopm_linked_list_destroy(&list);
}

// Remove first element
void test_linked_list_remove_first()
{
  ioopm_list_t *list = create_int_list();
  
  void *result;

  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 0, &result)); // make sure it succeeds  
  CU_ASSERT_EQUAL(20, * (int *) result); // make sure return value matches entered value

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Remove middle element
void test_linked_list_remove_middle()
{
  ioopm_list_t *list = create_int_list();

  void *result;

  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 1, &result)); // make sure it succeeds  
  CU_ASSERT_EQUAL(21, * (int *) result); // make sure return value matches entered value

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Remove last element
void test_linked_list_remove_last()
{
  ioopm_list_t *list = create_int_list();

  void *result;

  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 2, &result)); // make sure it succeeds  
  CU_ASSERT_EQUAL(22, * (int *) result); // make sure return value matches entered value

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Remove on empty list
void test_linked_list_remove_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  void *result;

  CU_ASSERT_FALSE(ioopm_linked_list_remove(list, 0, &result)); // make sure it doesn't succeed  

  ioopm_linked_list_destroy(&list);
}

// Remove a non basic element
void test_linked_list_remove_middle_non_basic()
{
  ioopm_list_t *list = create_string_list();

  void *result;

  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 3, &result)); // make sure it succeeds  
  CU_ASSERT_EQUAL(0, strcmp("String", (char *) result)); // make sure return value matches entered value
  //CU_ASSERT_FALSE(ioopm_linked_list_contains(list, value)); // make sure it does not exist

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Insert into empty list
void test_linked_list_insert_into_empty_list()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int data = 20;

  bool insert_success = ioopm_linked_list_insert(list, 0, &data, sizeof(int));
  bool contains_success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_TRUE(insert_success);
  CU_ASSERT_TRUE(contains_success);

  ioopm_linked_list_destroy(&list);
}

// Tests if we can change head element
void test_linked_list_insert_into_head()
{
  ioopm_list_t *list = create_int_list();

  int data = 10;

  bool insert_success = ioopm_linked_list_insert(list, 0, &data, sizeof(int));
  bool contains_success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_TRUE(insert_success);
  CU_ASSERT_TRUE(contains_success);

  ioopm_linked_list_destroy(&list);
}

// Tests if we can insert last
void test_linked_list_insert_into_last()
{
  ioopm_list_t *list = create_int_list();

  int data = 10;

  bool insert_success = ioopm_linked_list_insert(list, 2, &data, sizeof(int));
  bool contains_success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_TRUE(insert_success);
  CU_ASSERT_TRUE(contains_success);

  ioopm_linked_list_destroy(&list);
}

// Tests if we can change a middle element
void test_linked_list_insert_into_middle()
{
  ioopm_list_t *list = create_int_list();

  int data = 10;

  bool insert_success = ioopm_linked_list_insert(list, 1, &data, sizeof(int));
  bool contains_success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_TRUE(insert_success);
  CU_ASSERT_TRUE(contains_success);

  ioopm_linked_list_destroy(&list);
}

// Tests if insertions with non valid indexes are caught
void test_linked_list_insert_non_valid_index()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int data = 20;

  bool success_positive_index = ioopm_linked_list_insert(list, 10, &data, sizeof(int));
  bool success_negative_index = ioopm_linked_list_insert(list, -10, &data, sizeof(int));
  
  bool contains_success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_FALSE(success_positive_index);
  CU_ASSERT_FALSE(success_negative_index);
  CU_ASSERT_FALSE(contains_success);

  ioopm_linked_list_destroy(&list);
}

// Test contains of non-existing value
void test_linked_list_contains_nonexisting_value()
{
  ioopm_list_t *list = create_int_list();

  int data = 10;

  bool success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests if contains all elements
void test_linked_list_insert_contains_all()
{
  ioopm_list_t *list = create_int_list();

  int data_a = 22;
  int data_b = 21;
  int data_c = 20;

  bool a = ioopm_linked_list_contains(list, *ioopm_int_order, &data_a);
  bool b = ioopm_linked_list_contains(list, *ioopm_int_order, &data_b);
  bool c = ioopm_linked_list_contains(list, *ioopm_int_order, &data_c);

  CU_ASSERT_TRUE(a);
  CU_ASSERT_TRUE(b);
  CU_ASSERT_TRUE(c);

  ioopm_linked_list_destroy(&list);
}

// Tests get on empty list
void test_linked_list_get_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  void *result;
  bool success = ioopm_linked_list_get(list, 0, &result);

  CU_ASSERT_FALSE(success);
  ioopm_linked_list_destroy(&list);
}

// Tests get first index
void test_linked_list_get_first()
{
  ioopm_list_t *list = create_int_list();

  void *result;
  bool success = ioopm_linked_list_get(list, 0, &result);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_TRUE(20 == * (int *) result);

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Test get last index
void test_linked_list_get_last()
{
  ioopm_list_t *list = create_int_list();

  void *result;
  bool success = ioopm_linked_list_get(list, 2, &result);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_TRUE(22 == * (int *) result);

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Test get middle index
void test_linked_list_get_middle()
{
  ioopm_list_t *list = create_int_list();

  void *result;
  bool success = ioopm_linked_list_get(list, 1, &result);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_TRUE(21 == * (int *) result);

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Test get index non-basic
void test_linked_list_get_middle_non_basic()
{
  ioopm_list_t *list = create_string_list();

  void *result;
  bool success = ioopm_linked_list_get(list, 3, &result);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(0, strcmp("String", (char *) result));

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Tests get with non-valid index
void test_linked_list_get_invalid_index()
{
  ioopm_list_t *list = create_int_list();

  void *result;
  bool success = ioopm_linked_list_get(list, 5, &result);

  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests append on empty list
void test_linked_list_append_empty()
{
  ioopm_list_t *list = create_int_list();
  
  void *a_result;
  void *b_result;
  void *c_result;
  bool a_success = ioopm_linked_list_get(list, 0, &a_result);
  bool b_success = ioopm_linked_list_get(list, 1, &b_result);
  bool c_success = ioopm_linked_list_get(list, 2, &c_result);

  CU_ASSERT_TRUE(a_success);
  CU_ASSERT_TRUE(b_success);
  CU_ASSERT_TRUE(c_success);
  CU_ASSERT_EQUAL(20, * (int *) a_result);
  CU_ASSERT_EQUAL(21, * (int *) b_result);
  CU_ASSERT_EQUAL(22, * (int *) c_result);

  free(a_result);
  free(b_result);
  free(c_result);
  ioopm_linked_list_destroy(&list); 
}

// Tests append on list
void test_linked_list_append()
{
  ioopm_list_t *list = create_int_list();

  int data = 24;

  ioopm_linked_list_append(list, &data, sizeof(int));
  
  void *result;
  bool success = ioopm_linked_list_get(list, 3, &result);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(24, * (int *) result);

  free(result);
  ioopm_linked_list_destroy(&list);
}

// Test prepend on empty list
void test_linked_list_prepend_empty()
{
  ioopm_list_t *list = create_int_list();
  
  void *a_result;
  void *b_result;
  void *c_result;
  bool a_success = ioopm_linked_list_get(list, 2, &a_result);
  bool b_success = ioopm_linked_list_get(list, 1, &b_result);
  bool c_success = ioopm_linked_list_get(list, 0, &c_result);

  CU_ASSERT_TRUE(a_success);
  CU_ASSERT_TRUE(b_success);
  CU_ASSERT_TRUE(c_success);
  CU_ASSERT_EQUAL(22, * (int *) a_result);
  CU_ASSERT_EQUAL(21, * (int *) b_result);
  CU_ASSERT_EQUAL(20, * (int *) c_result);

  free(a_result);
  free(b_result);
  free(c_result);
  ioopm_linked_list_destroy(&list); 
}

// Tests prepend on list
void test_linked_list_prepend()
{
  ioopm_list_t *list = create_int_list();

  int data = 19;

  ioopm_linked_list_prepend(list, &data, sizeof(int));
  
  void *result;
  bool success = ioopm_linked_list_get(list, 0, &result);

  CU_ASSERT_TRUE(success);
  CU_ASSERT_EQUAL(19, * (int *) result);

  free(result);
  ioopm_linked_list_destroy(&list);
}

/// @brief Checks if one value is larger than the other
/// @param key integer index of element
/// @param data integer data of element
/// @param extra integer to compare against
/// @return true iff data is larger than extra, else false
bool is_value_larger_than_extra(void *key, void *data, void *extra)
{
  return *(int *)data > *(int *)extra;
}

// Tests all good predicate
void test_linked_list_all_good()
{
  ioopm_list_t *list = create_int_list();

  int larger_than = 19;
  bool success = ioopm_linked_list_all(list, *is_value_larger_than_extra, &larger_than);
  
  CU_ASSERT_TRUE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests not all good predicate
void test_linked_list_not_all_good()
{
  ioopm_list_t *list = create_int_list();

  int larger_than = 21;
  bool success = ioopm_linked_list_all(list, *is_value_larger_than_extra, &larger_than);
  
  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests all on empty list
void test_linked_list_all_on_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int larger_than = 20;
  bool success = ioopm_linked_list_all(list, *is_value_larger_than_extra, &larger_than);
  
  CU_ASSERT_TRUE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests any good predicate
void test_linked_list_any_good()
{
  ioopm_list_t *list = create_int_list();

  int larger_than = 21;
  bool success = ioopm_linked_list_any(list, *is_value_larger_than_extra, &larger_than);
  
  CU_ASSERT_TRUE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests not any good predicate
void test_linked_list_not_any_good()
{
  ioopm_list_t *list = create_int_list();

  int larger_than = 23;
  bool success = ioopm_linked_list_any(list, *is_value_larger_than_extra, &larger_than);
  
  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

// Tests any on empty list
void test_linked_list_any_on_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int larger_than = 20;
  bool success = ioopm_linked_list_any(list, *is_value_larger_than_extra, &larger_than);
  
  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

/// @brief Changes every even data to another
/// @param key integer index of element
/// @param data the data of element
/// @param extra the data to change even elements into
void every_other_int_to_extra(void *key, void **data, void *extra)
{
  if (*(int *)key % 2 == 0)
  {
    *data = realloc(*data, sizeof(int));
    *data = memcpy(*data, extra, sizeof(int));
  } 
}

// Tests apply to all
void test_linked_list_apply_to_all()
{
  ioopm_list_t *list = create_int_list();
  
  int extra = 69;
  ioopm_linked_list_apply_to_all(list, *every_other_int_to_extra, &extra);

  int data = 69;
  bool success = ioopm_linked_list_contains(list, *ioopm_int_order, &data);

  CU_ASSERT_TRUE(success);

  ioopm_linked_list_destroy(&list);
}

// Apply to all on empty list
void test_linked_list_apply_to_all_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  
  int extra = 69;
  ioopm_linked_list_apply_to_all(list, *every_other_int_to_extra, &extra);

  CU_ASSERT_TRUE(true);

  ioopm_linked_list_destroy(&list);
}

// Tests size with different insert methods and removals
void test_linked_list_size()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  CU_ASSERT_EQUAL(0, ioopm_linked_list_size(list)); // List should be of length: 0
  
  int data_a = 1;
  int data_b = 2;
  ioopm_linked_list_append(list, &data_a, sizeof(int));
  CU_ASSERT_EQUAL(1, ioopm_linked_list_size(list)); // List should be of length: 1

  ioopm_linked_list_append(list, &data_b, sizeof(int));
  CU_ASSERT_EQUAL(2, ioopm_linked_list_size(list)); // List should be of length: 2

  void *result_a;
  void *result_b;

  ioopm_linked_list_remove(list, 0, &result_a);
  CU_ASSERT_EQUAL(1, ioopm_linked_list_size(list)); // List should be of length: 1

  ioopm_linked_list_remove(list, 0, &result_b);
  CU_ASSERT_EQUAL(0, ioopm_linked_list_size(list)); // List should be of length: 0

  free(result_a);
  free(result_b);

  ioopm_linked_list_destroy(&list);
}

// Tests list size after clear
void test_linked_list_size_clear()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int data_a = 1;
  int data_b = 2;
  int data_c = 3;

  ioopm_linked_list_append(list, &data_a, sizeof(int));
  ioopm_linked_list_prepend(list, &data_b, sizeof(int));
  ioopm_linked_list_insert(list, 1, &data_c, sizeof(int));

  CU_ASSERT_EQUAL(3, ioopm_linked_list_size(list)); // List should be of length: 3

  ioopm_linked_list_clear(list);

  CU_ASSERT_EQUAL(0, ioopm_linked_list_size(list)); // List should be of length: 0

  ioopm_linked_list_destroy(&list);
}

// Checks empty list 
void test_linked_list_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  bool success = ioopm_linked_list_is_empty(list);

  CU_ASSERT_TRUE(success);
  ioopm_linked_list_destroy(&list);
}

// Tests empty on non empty list
void test_linked_list_empty_non_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);

  int data = 20;
  ioopm_linked_list_append(list, &data, sizeof(int));

  bool success = ioopm_linked_list_is_empty(list);
  CU_ASSERT_FALSE(success);

  ioopm_linked_list_destroy(&list);
}

#pragma endregion

#pragma region // iterator tests

void test_iterator_empty()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_alloc_simple_ptr, ioopm_free_simple_ptr);
  ioopm_list_iterator_t *iterator = ioopm_iterator_initialize(list);

  int test_val = 16;
  while (ioopm_linked_list_size(list) < 10)
  {
    ioopm_iterator_insert(iterator, &test_val, sizeof(test_val));

    void *result;
    ioopm_linked_list_get(list, ioopm_iterator_current(iterator), &result);

    CU_ASSERT_EQUAL(test_val, *(int *)result);

    free(result);

    ioopm_iterator_next(iterator);
  }
  
  CU_ASSERT_TRUE(ioopm_linked_list_size(list) == 10);

  ioopm_iterator_destroy(&iterator);
  ioopm_linked_list_destroy(&list);
}

#pragma endregion

// __________________________________________

int main(int argc, char *argv[]) 
{
  bool hash = false;
  bool list = false;
  bool iter = false;
  bool all = true;

  if (argc == 2)
  {
    hash = strcmp("hash", argv[1]) == 0;
    list = strcmp("list", argv[1]) == 0;
    iter = strcmp("iter", argv[1]) == 0;
    all = strcmp("all", argv[1]) == 0;

    if (!(hash || list || iter || all))
    {
      printf("Usage: %s suit_name, can be:\nhash\nlist\niter\nall\n", argv[0]);
      return 0;
    }
  }

  if (CU_initialize_registry() != CUE_SUCCESS) // set up cunit
  {
    return CU_get_error();
  }

  CU_pSuite hash_table_suit = CU_add_suite("Hash table test suite", init_suite, clean_suite);
  CU_pSuite linked_list_suit = CU_add_suite("Linked list test suite", init_suite, clean_suite);
  CU_pSuite iterator_suit = CU_add_suite("Iterator test suite", init_suite, clean_suite);
  if (hash_table_suit == NULL || linked_list_suit == NULL || iterator_suit == NULL) 
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (
    // add tests to linked list suit
    (CU_add_test(linked_list_suit, "Create and delete empty list", test_linked_list_create_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Create and delete list with basic elements", test_linked_list_create_full_basic_data) == NULL) ||
    (CU_add_test(linked_list_suit, "Create and delete list with non-baisc elements", test_linked_list_create_full_non_basic_data) == NULL) ||
    (CU_add_test(linked_list_suit, "Insertion into empty list", test_linked_list_insert_into_empty_list) == NULL) ||
    (CU_add_test(linked_list_suit, "Replacing value of first element", test_linked_list_insert_into_head) == NULL) ||
    (CU_add_test(linked_list_suit, "Replacing value of last element", test_linked_list_insert_into_last) == NULL) ||
    (CU_add_test(linked_list_suit, "Replacing value of a middle element", test_linked_list_insert_into_middle) == NULL) ||
    (CU_add_test(linked_list_suit, "Insertion with non-valid index", test_linked_list_insert_non_valid_index) == NULL) ||
    (CU_add_test(linked_list_suit, "Contains non-existing value", test_linked_list_contains_nonexisting_value) == NULL) ||
    (CU_add_test(linked_list_suit, "Contains all inserted", test_linked_list_insert_contains_all) == NULL) ||
    (CU_add_test(linked_list_suit, "Get element from empty list", test_linked_list_get_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Get first element in list", test_linked_list_get_first) == NULL) ||
    (CU_add_test(linked_list_suit, "Get last element in list", test_linked_list_get_last) == NULL) ||
    (CU_add_test(linked_list_suit, "Get a middle element in list", test_linked_list_get_middle) == NULL) ||
    (CU_add_test(linked_list_suit, "Get a middle non-basic element in list", test_linked_list_get_middle_non_basic) == NULL) ||
    (CU_add_test(linked_list_suit, "Get a non-existing value in list", test_linked_list_get_invalid_index) == NULL) ||
    (CU_add_test(linked_list_suit, "Append element", test_linked_list_append) == NULL) ||
    (CU_add_test(linked_list_suit, "Append element to empty list", test_linked_list_append_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Prepend element", test_linked_list_prepend) == NULL) ||
    (CU_add_test(linked_list_suit, "Prepend element to empty list", test_linked_list_prepend_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "All elements pass pred", test_linked_list_all_good) == NULL) ||
    (CU_add_test(linked_list_suit, "Not all elements pass pred", test_linked_list_not_all_good) == NULL) ||
    (CU_add_test(linked_list_suit, "All pred on empty list", test_linked_list_all_on_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Any element pass pred", test_linked_list_any_good) == NULL) ||
    (CU_add_test(linked_list_suit, "Not any elements pass pred", test_linked_list_not_any_good) == NULL) ||
    (CU_add_test(linked_list_suit, "Any pred on empty list", test_linked_list_any_on_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Apply mutation to list", test_linked_list_apply_to_all) == NULL) ||
    (CU_add_test(linked_list_suit, "Apply mutation to empty list", test_linked_list_apply_to_all_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Clear list with one element", test_linked_list_clear_one) == NULL) ||
    (CU_add_test(linked_list_suit, "Clear list with mulitple elements", test_linked_list_clear_mulitple) == NULL) ||
    (CU_add_test(linked_list_suit, "Clear empty list", test_linked_list_clear_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Clear list twice", test_linked_list_clear_double) == NULL) ||
    (CU_add_test(linked_list_suit, "Remove single element", test_linked_list_remove_single) == NULL) ||
    (CU_add_test(linked_list_suit, "Remove first element", test_linked_list_remove_first) == NULL) ||
    (CU_add_test(linked_list_suit, "Remove a middle element", test_linked_list_remove_middle) == NULL) ||
    (CU_add_test(linked_list_suit, "Remove last element", test_linked_list_remove_last) == NULL) ||
    (CU_add_test(linked_list_suit, "Remove element from empty list", test_linked_list_remove_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Remove a non-basic middle element", test_linked_list_remove_middle_non_basic) == NULL) ||
    (CU_add_test(linked_list_suit, "Size changes from inserts and removes", test_linked_list_size) == NULL) ||
    (CU_add_test(linked_list_suit, "Size changes from clear", test_linked_list_size_clear) == NULL) ||
    (CU_add_test(linked_list_suit, "Check empty list really empty", test_linked_list_empty) == NULL) ||
    (CU_add_test(linked_list_suit, "Check non empty list not empty", test_linked_list_empty_non_empty) == NULL) ||

    //add tests to iterator suit
    (CU_add_test(iterator_suit, "Insert mulitple values into empty iterator", test_iterator_empty) == NULL) ||
   
    //add tests to hash table suit
    (CU_add_test(hash_table_suit, "Create and delete hash table", test_create_destroy_empty) == NULL) ||
    (CU_add_test(hash_table_suit, "Add mulitple entries to one bucket", test_chain_inserts) == NULL) ||
    (CU_add_test(hash_table_suit, "Insert once", test_insert_once) == NULL) ||
    (CU_add_test(hash_table_suit, "Insert key already in use", test_insert_key_in_use) == NULL) ||
    (CU_add_test(hash_table_suit, "Lookup unused key", test_lookup_key_not_in_use) == NULL) ||
    (CU_add_test(hash_table_suit, "Destroy non-empty hash table", test_create_destroy_full) == NULL) ||
    (CU_add_test(hash_table_suit, "Lookup value with key already inserted", test_lookup_inserted_value) == NULL) ||
    (CU_add_test(hash_table_suit, "Lookup inserted invalid key", test_lookup_insert_invalid_key) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove one element", test_remove_one_element) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove the first element", test_remove_first_element) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove the middle element", test_remove_middle_element) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove the last element", test_remove_last_element) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove non-existing element", test_remove_non_existing_element) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove same element twice", test_remove_same_element_twice) == NULL) ||
    (CU_add_test(hash_table_suit, "Remove using invalid key", test_remove_invalid_key) == NULL) ||
    (CU_add_test(hash_table_suit, "Check that keys correspond to inserted keys", test_hash_table_keys) == NULL) ||
    (CU_add_test(hash_table_suit, "Check that values correspond to inserted values", test_hash_table_values) == NULL) ||
    (CU_add_test(hash_table_suit, "Check corresponding key/value order", test_hash_table_keys_and_values) == NULL) ||
    (CU_add_test(hash_table_suit, "Check if hash table is empty", test_hash_table_empty) == NULL) ||
    (CU_add_test(hash_table_suit, "Check size of hash table", test_hash_table_size) == NULL) ||
    (CU_add_test(hash_table_suit, "Check that hash table clears correctly", test_hash_table_clear) == NULL) ||
    (CU_add_test(hash_table_suit, "Try finding existing key and value", test_find_existing_value_and_key) == NULL) ||
    (CU_add_test(hash_table_suit, "Try finding non existant key and value", test_find_non_existing_value_and_key) == NULL) ||
    (CU_add_test(hash_table_suit, "Apply predicate on empty ht", test_has_any_empty_ht) == NULL) ||
    (CU_add_test(hash_table_suit, "Successfully apply predicate on ht", test_has_any_successful) == NULL) ||
    (CU_add_test(hash_table_suit, "Unsuccessfully apply predicate on ht", test_has_any_unsuccessful) == NULL) ||
    (CU_add_test(hash_table_suit, "Apply predicate on empty ht", test_has_all_empty_ht) == NULL) ||
    (CU_add_test(hash_table_suit, "Successfully apply predicate on ht", test_has_all_successful) == NULL) ||
    (CU_add_test(hash_table_suit, "Unsuccessfully apply predicate on ht", test_has_all_unsuccessful) == NULL) ||
    (CU_add_test(hash_table_suit, "Apply func to a full ht", test_apply_to_all) == NULL) ||
    (CU_add_test(hash_table_suit, "Apply func to an empty ht", test_apply_to_empty) == NULL) ||
    0
  )
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);

  if (all)
  {
    CU_basic_run_suite(hash_table_suit);
    CU_basic_run_suite(linked_list_suit);
    CU_basic_run_suite(iterator_suit);
  }
  else if (hash)
  {
    CU_basic_run_suite(hash_table_suit);
  }
  else if (list)
  {
    CU_basic_run_suite(linked_list_suit);
  }
  else if (iter)
  {
    CU_basic_run_suite(iterator_suit);
  }
  
  CU_cleanup_registry();
  return CU_get_error();
} 