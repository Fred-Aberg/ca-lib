#pragma once
#include <stdlib.h>
#include <stdbool.h>

// ca-lib user-reachable datatypes

struct data
{
    size_t x; // Read-Only
    size_t y; // Read-Only
    size_t size;
    void *ptr; 
};
typedef struct data data_t;

typedef struct grid ca_lib_grid_t;

/// @brief Allocates a space 'data_size' large and copies over the data from 'data_ptr' - returns the allocated pointer
typedef void *(*ca_lib_data_alloc_function_t)(void *data_ptr, size_t data_size);
/// @brief Frees the data stored in 'data_ptr' and returns null 
typedef void *(*ca_lib_data_free_function_t)(void *data_ptr);

/// @brief Decides how the cell will be represented when printed in the console
typedef char(*ca_lib_data_to_char_t)(void *data_ptr);

/// @brief Provided the data of a cell - implement desired simulation
typedef void(*ca_lib_simulate_cell_t)(ca_lib_grid_t *grid, data_t *data);


/*----FUNCTION HEADERS----*/

/// @brief Allocate a new pointer on the heap with size data.size, copy over 'data_ptr'
/// @param data_ptr the data_ptr that will be copied onto the heap
/// @param data_size size of 'data_ptr' i bytes
/// @return the allocated pointer
void *ca_lib_alloc_simple_ptr (void *data_ptr, size_t data_size);

/// @brief Frees the given pointer
/// @param data_ptr 
/// @return null
void *ca_lib_free_simple_ptr (void *data_ptr);

/// @brief Creates a 'width' by 'height' grid
/// @param width 
/// @param heigth 
/// @param alloc_func a function that allocates the cells' 'data_ptr' pointer
/// @param free_func a function that frees the cells' 'data_ptr' pointer
/// @return a pointer to the allocated grid
ca_lib_grid_t *ca_lib_create_grid(size_t width, size_t heigth, ca_lib_data_alloc_function_t alloc_func, ca_lib_data_free_function_t free_func);

size_t ca_lib_get_grid_width(ca_lib_grid_t *grid);

size_t ca_lib_get_grid_height(ca_lib_grid_t *grid);

bool ca_lib_check_limits(ca_lib_grid_t *grid, size_t x, size_t y);

/// @brief Frees the given grid and its cells' 'data_ptr' pointer and returns NULL
/// @param grid the given grid to be freed 
/// @return NULL
ca_lib_grid_t *ca_lib_destroy_grid(ca_lib_grid_t *grid);

/// @brief Empty cell at (x,y), freeing it's data
/// @param grid The grid to be operated on
/// @param x 
/// @param y 
void ca_lib_clear_cell(ca_lib_grid_t *grid, size_t x, size_t y);

/// @brief Inserts the given data_ptr into the cell at (x,y) in 'grid'
/// @param grid The given grid which the cell is to be inserted into
/// @param x 
/// @param y 
/// @param data_size The total size of 'data_ptr' in bytes
/// @param data_ptr a pointer to the data_ptr one wishes to store in the cell
void ca_lib_insert_cell(ca_lib_grid_t *grid, size_t x, size_t y, size_t data_size, void *data_ptr);

/// @brief Moves the cell at (x1,y1) to (x2, y2) - overwriting and freeing any potential cell at (x2, y2)
/// @param grid The given grid to be operated on
/// @param x1 Cell to be moved
/// @param y1 Cell to be moved
/// @param x2 Destination
/// @param y2 Destination
void ca_lib_move_cell(ca_lib_grid_t *grid, size_t x1, size_t y1, size_t x2, size_t y2);

/// @brief Switches the cells at (x1,y1) and (x2, y2)
/// @param grid The given grid to be operated on
/// @param x1 cell 1's x
/// @param y1 cell 1's y
/// @param x2 cell 2's x
/// @param y2 cell 2's y
void ca_lib_switch_cells(ca_lib_grid_t *grid, size_t x1, size_t y1, size_t x2, size_t y2);

/// @brief Retrieves the data_t 'data' from the cell at (x,y) in the given grid
/// @param grid The given grid to be searched
/// @param x 
/// @param y 
/// @return The cell's data_t 'data' NOTE: changing 'data.ptr' will change the data in the original cell
data_t ca_lib_get_cell_data(ca_lib_grid_t *grid, size_t x, size_t y);

/// @brief Check whether the cell at (x,y) in 'grid' is empty ('data_ptr' == NULL)
/// @param grid The given grid which the cell resides in
/// @param x 
/// @param y 
/// @return true if 'data_ptr' == NULL, otherwise false
bool ca_lib_cell_empty(ca_lib_grid_t *grid, size_t x, size_t y);

/// @brief Prints a simple representation of the given 'grid'
/// @param grid 
/// @param convert_func Determines what char the cell will be represented as based on 'data_ptr'
void ca_lib_print_grid(ca_lib_grid_t *grid, ca_lib_data_to_char_t convert_func);

/// @brief Applies the given simulation function to each cell, uses a buffer to make sure ALL cells are simulated and only ONCE - despite movement
/// @param grid The given grid to be operated on
/// @param sim_func The function which determines how the cells will behave
void ca_lib_simulate(ca_lib_grid_t *grid, ca_lib_simulate_cell_t sim_func);

/// @brief Applies the given simulation function to the grid without keeping track of movement 
/// @param grid The given grid to be operated on
/// @param sim_func The function which determines how the cells will behave
void ca_lib_simulate_unabstract(ca_lib_grid_t *grid, ca_lib_simulate_cell_t sim_func);
