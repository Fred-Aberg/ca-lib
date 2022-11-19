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

char true_vs_false(void *data_ptr)
{
  if (data_ptr)
  {
    return *(bool *)data_ptr ? '1' : '0'; // cell has true or false
  }
  return '~'; // cell empty
}

void reverse_bool(ca_lib_grid_t *grid, data_t *data)
{
  if (!data->ptr)
  {
    return;
  }
  *(bool *)data->ptr = !*(bool *)data->ptr;
}

void polarize(ca_lib_grid_t *grid, data_t *data)
{
  if (!data->ptr)
  {
    return;
  }

  if (*(bool *)data->ptr) // Move to the right if true
  {
    if (ca_lib_check_limits(grid, data->x + 1, data->y)) // Check limits
    {
      data_t r_cell_data = ca_lib_get_cell_data(grid, data->x + 1, data->y);
      if (r_cell_data.ptr == NULL)
      {
        ca_lib_move_cell(grid, data->x, data->y, data->x + 1, data->y);
      }
    }
  }
  else // Move to the left
  {
    if (ca_lib_check_limits(grid, data->x - 1, data->y)) // Check limits
    {
      data_t l_cell_data = ca_lib_get_cell_data(grid, data->x - 1, data->y);
      if (l_cell_data.ptr == NULL)
      {
        ca_lib_move_cell(grid, data->x, data->y, data->x - 1, data->y);
      }
    }
  }
}

ca_lib_grid_t *sample_grid()
{
  ca_lib_grid_t *grid = ca_lib_create_grid(5, 5, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
  bool bl = true;
  ca_lib_insert_cell(grid, 0, 0, sizeof(bool), &bl);
  bl = false;
  ca_lib_insert_cell(grid, 4, 4, sizeof(bool), &bl);
  return grid;
}

ca_lib_grid_t *sample_grid_cross()
{
  ca_lib_grid_t *grid = ca_lib_create_grid(5, 5, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
  bool bl = true;
  bool bl2 = false;
  ca_lib_insert_cell(grid, 0, 0, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 1, 1, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 2, 2, sizeof(bool), &bl2);
  ca_lib_insert_cell(grid, 3, 3, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 4, 4, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 4, 0, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 3, 1, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 1, 3, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 0, 4, sizeof(bool), &bl);
  return grid;
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
  grid = ca_lib_destroy_grid(grid);
}

void test_insert_once()
{
  ca_lib_grid_t *grid = ca_lib_create_grid(3, 3, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
  bool bl = true;
  CU_ASSERT_PTR_NULL(ca_lib_get_cell_data(grid, (size_t)1, (size_t)1).ptr);
  ca_lib_insert_cell(grid, 1, 1, sizeof(bool), &bl);
  CU_ASSERT_PTR_NOT_NULL(ca_lib_get_cell_data(grid, 1, 1).ptr);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 1, 1).ptr, true)
  grid = ca_lib_destroy_grid(grid);
}

void test_insert_first_n_last()
{
  ca_lib_grid_t *grid = ca_lib_create_grid(3, 3, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
  bool bl = true;
  CU_ASSERT_PTR_NULL(ca_lib_get_cell_data(grid, 0, 0).ptr);
  CU_ASSERT_PTR_NULL(ca_lib_get_cell_data(grid, 2, 2).ptr);
  ca_lib_insert_cell(grid, 0, 0, sizeof(bool), &bl);
  ca_lib_insert_cell(grid, 2, 2, sizeof(bool), &bl);
  CU_ASSERT_PTR_NOT_NULL(ca_lib_get_cell_data(grid, 0, 0).ptr);
  CU_ASSERT_PTR_NOT_NULL(ca_lib_get_cell_data(grid, 2, 2).ptr);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 0, 0).ptr, true)
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 2, 2).ptr, true)
  grid = ca_lib_destroy_grid(grid);
}

void test_insert_on_cell_twice()
{
  ca_lib_grid_t *grid = ca_lib_create_grid(3, 3, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
  bool bl = true;
  CU_ASSERT_PTR_NULL(ca_lib_get_cell_data(grid, 1, 1).ptr);
  ca_lib_insert_cell(grid, 1, 1, sizeof(bool), &bl);
  CU_ASSERT_PTR_NOT_NULL(ca_lib_get_cell_data(grid, 1, 1).ptr);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 1, 1).ptr, true)
  bl = false;
  ca_lib_insert_cell(grid, 1, 1, sizeof(bool), &bl);
  CU_ASSERT_PTR_NOT_NULL(ca_lib_get_cell_data(grid, 1, 1).ptr);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 1, 1).ptr, false)
  grid = ca_lib_destroy_grid(grid);
}

void test_move_cell()
{
  ca_lib_grid_t *grid = sample_grid();
  ca_lib_move_cell(grid, 0, 0, 2, 2); // Move (0, 0) to (2, 2)
  ca_lib_print_grid(grid, true_vs_false);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 2, 2).ptr, true);
  CU_ASSERT_TRUE(ca_lib_cell_empty(grid, 0, 0));
  CU_ASSERT_FALSE(ca_lib_cell_empty(grid, 2, 2));
  grid = ca_lib_destroy_grid(grid);
}

void test_move_cell_overwrite()
{
  ca_lib_grid_t *grid = sample_grid();
  ca_lib_move_cell(grid, 0, 0, 4, 4); // Move (0, 0) to (2, 2)
  ca_lib_print_grid(grid, true_vs_false);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 4, 4).ptr, true);
  CU_ASSERT_TRUE(ca_lib_cell_empty(grid, 0, 0));
  CU_ASSERT_FALSE(ca_lib_cell_empty(grid, 4, 4));
  grid = ca_lib_destroy_grid(grid);
}

void test_switch_cells()
{
  ca_lib_grid_t *grid = sample_grid();
  ca_lib_switch_cells(grid, 0, 0, 4, 4); // Switch (0, 0) and (4, 4)
  ca_lib_print_grid(grid, true_vs_false);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 0, 0).ptr, false);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 4, 4).ptr, true);
  grid = ca_lib_destroy_grid(grid);
}

void test_simulate_unabstract()
{
  ca_lib_grid_t *grid = sample_grid_cross();
  ca_lib_simulate_unabstract(grid, reverse_bool);
  ca_lib_print_grid(grid, true_vs_false);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 2, 2).ptr, true);
  CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 4, 4).ptr, false);
  grid = ca_lib_destroy_grid(grid);
}

void test_simulate_abstract()
{
  ca_lib_grid_t *grid = sample_grid_cross();
  for (size_t i = 0; i < 3; i++)
  {
    ca_lib_print_grid(grid, true_vs_false);
    ca_lib_simulate(grid, polarize);
  }
  // CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 2, 2).ptr, true);
  // CU_ASSERT_EQUAL(*(bool *)ca_lib_get_cell_data(grid, 4, 4).ptr, false);
  grid = ca_lib_destroy_grid(grid);
}

int main()
{
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
      (NULL == CU_add_test(test_suite1, "test_insert_first_n_last", test_insert_first_n_last)) ||
      (NULL == CU_add_test(test_suite1, "test_insert_on_cell_twice", test_insert_on_cell_twice)) ||
      (NULL == CU_add_test(test_suite1, "test_move_cell", test_move_cell)) ||
      (NULL == CU_add_test(test_suite1, "test_move_cell_overwrite", test_move_cell_overwrite)) ||
      (NULL == CU_add_test(test_suite1, "test_switch_cells", test_switch_cells)) ||
      (NULL == CU_add_test(test_suite1, "test_simulate_unabstract", test_simulate_unabstract)) ||
      (NULL == CU_add_test(test_suite1, "test_simulate_abstract", test_simulate_abstract)) ||
      0)
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}