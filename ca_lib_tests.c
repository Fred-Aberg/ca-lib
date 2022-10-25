#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "ca_lib.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}


char empty_vs_full(void *data_ptr)
{
    return data_ptr ? '#' : '~'; // return '~' if null, '#' if "filled" - hehe cowboy code
}


// Create, then destroy grid. Check that pointer is correct.
void test_create_destroy_grid()
{
    ca_lib_grid_t *grid = ca_lib_create_grid(10, 10, NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL(grid);
    grid = ca_lib_destroy_grid(grid);
    CU_ASSERT_PTR_NULL(grid);
}

void test_print_grid()
{
    ca_lib_grid_t *grid = ca_lib_create_grid(10, 10, NULL, NULL);
    ca_lib_print_grid(grid, &empty_vs_full);
    grid = ca_lib_destroy_grid(grid);
}

void test_insert_once()
{
    ca_lib_grid_t *grid = ca_lib_create_grid(3, 3, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
    bool bl = true;
    CU_ASSERT_PTR_NULL(ca_lib_get_cell_data(grid, 1, 1).data_ptr);
    ca_lib_insert_cell(grid, 1, 1, sizeof(bool), &bl);
    CU_ASSERT_PTR_NOT_NULL(ca_lib_get_cell_data(grid, 1, 1).data_ptr);
    CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 1, 1).data_ptr, true)
    grid = ca_lib_destroy_grid(grid);
}

int main()
{
    ca_lib_grid_t *grid = ca_lib_create_grid(20, 10, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
    bool bl = true;
    for (size_t x = 0; x < 20; x++)
    {
        for (size_t y = 0; y < 10; y++)
        {
            if (x % 2 != 0 && y % 2 == 0) {ca_lib_insert_cell(grid, x, y, sizeof(bool), &bl);}
            if (x % 2 == 0 && y % 2 != 0) {ca_lib_insert_cell(grid, x, y, sizeof(bool), &bl);}
        }   
    }
    ca_lib_print_grid(grid, &empty_vs_full);
    grid = ca_lib_destroy_grid(grid);

  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
    (NULL == CU_add_test(test_suite1, "test_create_destroy_grid", test_create_destroy_grid)) ||
    (NULL == CU_add_test(test_suite1, "test_print_grid", test_print_grid)) ||
    (NULL == CU_add_test(test_suite1, "test_insert_once", test_insert_once)) ||
    0
  )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}