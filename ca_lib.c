#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "ca_lib.h"

/*----USER NON-REACHABLE DATATYPES----*/

struct cell_struct
{
    size_t x;
    size_t y;
    data_t data;
};

typedef struct cell_struct cell_t;

struct grid
{
    size_t width;
    size_t height;
    ca_lib_data_alloc_function_t alloc_func;
    ca_lib_data_free_function_t free_func;
    cell_t cells[]; // Allocate for 'witdh' * 'height' cells
};

// Mainly used in i_to_pos to return 2 numbers
struct position
{
    size_t x;
    size_t y;
};
typedef struct position pos_t;


/*----STATIC HELPER FUNCTIONS----*/

// Convert array index of cell to cell's coordinates
static pos_t i_to_pos(size_t width, int i)
{
    pos_t pos;
    pos.y = i % width;
    pos.x = i - (pos.y * width);
    return pos;
}

static int pos_to_i(size_t width, size_t x, size_t y)
{
    return x + y * width;
}

static void fill_grid_empty_cells(ca_lib_grid_t *grid)
{
    size_t width = grid->width;
    // A cell with no data_ptr and undefined coordinates
    cell_t empty_cell; 
    empty_cell.data.size = 0;
    empty_cell.data.ptr = NULL;

    for (size_t i = 0; i < width * grid->height; i++)
    {
        pos_t pos = i_to_pos(width, i);
        empty_cell.x = pos.x; // Working on a local variable is faster than one on the heap
        empty_cell.y = pos.y;
        grid->cells[i] = empty_cell; // Value transfer, not identity
    }
    
}

static void clear_cell(ca_lib_grid_t *grid, cell_t *cell)
{
    if (!cell->data.ptr) {return;}
    cell->data.ptr = grid->free_func(cell->data.ptr);
    cell->data.size = 0;
}

/*----PUBLIC LIBRARY FUNCTIONS----*/


/*--DEFAULT HELP-FUNCTIONS--*/

// Allocates a new pointer with the value of data_ptr copied over - returns resulting pointer
void *ca_lib_alloc_simple_ptr (void *data_ptr, size_t data_size)
{
    void *new_data = calloc(1, data_size);
    new_data = memcpy(new_data, data_ptr, data_size);
    return new_data;
}

// Free the given data_ptr pointer and return null
void *ca_lib_free_simple_ptr (void *data_ptr)
{
    free(data_ptr);
    return NULL;
}
/*--------------------------*/

ca_lib_grid_t *ca_lib_create_grid(size_t width, size_t height, ca_lib_data_alloc_function_t alloc_func, ca_lib_data_free_function_t free_func)
{
    size_t cells_mem_size = width * height * sizeof(cell_t); // memory required for the cells
    ca_lib_grid_t *grid = calloc(1, cells_mem_size + sizeof(ca_lib_grid_t));

    grid->height = height;
    grid->width = width;
    grid->alloc_func = alloc_func;
    grid->free_func = free_func;

    fill_grid_empty_cells(grid);

    return grid;
}

// Frees the given grid and its cells' 'data_ptr' pointer and returns NULL
ca_lib_grid_t *ca_lib_destroy_grid(ca_lib_grid_t *grid)
{
    // Free all cells' 'data_ptr' pointers
    for (size_t i = 0; i < grid->height * grid->height; i++)
    {
        clear_cell(grid, &grid->cells[i]);
    }
    free(grid);
    return NULL;
}

//Empty cell at (x,y), freeing it's data
void ca_lib_clear_cell(ca_lib_grid_t *grid, size_t x, size_t y)
{
    cell_t *cellxy = &grid->cells[pos_to_i(grid->width, x, y)];
    clear_cell(grid, cellxy);
}

// Inserts the given data_ptr into the cell at (x,y) in 'grid'
void ca_lib_insert_cell(ca_lib_grid_t *grid, size_t x, size_t y, size_t data_size, void *data_ptr)
{
    cell_t *cell = &grid->cells[pos_to_i(grid->width, x, y)]; // Get pointer to cell at (x,y)
    cell->data.size = data_size; // Change cell's data_size

    // Free previous data if present
    if (cell->data.ptr) { cell->data.ptr = grid->free_func(cell->data.ptr); }

    // Allocate for new data
    cell->data.ptr = grid->alloc_func(data_ptr, data_size);
}

// Moves the cell's data at (x1,y1) to (x2, y2) - overwriting and freeing any potential cell at (x2, y2)
void ca_lib_move_cell(ca_lib_grid_t *grid, size_t x1, size_t y1, size_t x2, size_t y2)
{
    // Grab cell at (x1, y1)
    cell_t *cellxy = &grid->cells[pos_to_i(grid->width, x1, y1)];

    // Insert data from cellxy at (x2, y2)
    ca_lib_insert_cell(grid, x2, y2, cellxy->data.size, cellxy->data.ptr);

    clear_cell(grid, cellxy);
}

// Switches the cells' data at (x1,y1) and (x2, y2)
void ca_lib_switch_cells(ca_lib_grid_t *grid, size_t x1, size_t y1, size_t x2, size_t y2)
{
    // No real memory management is needed, simply switch the 'data'
    cell_t *cell_1 = &grid->cells[pos_to_i(grid->width, x1, y1)];
    cell_t *cell_2 = &grid->cells[pos_to_i(grid->width, x2, y2)];
    void *ptr_1 = cell_1->data.ptr;
    void *ptr_2 = cell_2->data.ptr;
    cell_1->data.ptr = ptr_2;
    cell_2->data.ptr = ptr_1;
}

// Get the data_t 'data' from cell at (x,y) in grid
data_t ca_lib_get_cell_data(ca_lib_grid_t *grid, size_t x, size_t y)
{
    return grid->cells[pos_to_i(grid->width, x, y)].data;
}

// Check whether the cell at (x,y) in 'grid' is empty ('data_ptr' == NULL)
bool ca_lib_cell_empty(ca_lib_grid_t *grid, size_t x, size_t y)
{
    return grid->cells[pos_to_i(grid->width, x, y)].data.ptr == NULL;
}

// Prints a simple representation of the given 'grid'
void ca_lib_print_grid(ca_lib_grid_t *grid, ca_lib_data_to_char convert_func)
{
    printf("\n| GRID [%d , %d] |\n", (int)grid->width, (int)grid->height);
    for (int y = (int)grid->height - 1; y >= 0; y--)
    {
        for (int x = 0; x < (int)grid->width; x++)
        {
            char c = convert_func(grid->cells[pos_to_i(grid->width, (size_t)x, (size_t)y)].data.ptr);
            x == 0 ? printf("\n%c", c) : printf("%c", c); // Print new line if x == 0
        }
    }
}