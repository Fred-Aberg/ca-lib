#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "ca_lib.h"
#include "packages/hash_table/linked_list.h"
#include "graphics/gfx/gfx.h"

/*----USER NON-REACHAcarts.c ../database/db.c ../hash_and_list/hash_table.c ../hash_and_list/linked_list.cBLE DATATYPES----*/

struct cell_struct
{
    size_t x;
    size_t y;
    data_t data;
};

typedef struct cell_struct cell_t;

struct grid
{
    void *meta_data; // data pertaining to the whole grid, muste be alloc:ed/freed by the user
    size_t width; // Width of the simulation
    size_t height; // Height of the simulation
    ca_lib_data_alloc_function_t alloc_func; // A 'ca_lib_data_alloc_function_t' that allocates the cells' data
    ca_lib_data_free_function_t free_func; // A 'ca_lib_data_free_function_t' that frees the cells' data
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
    pos.y = i / width; // C autimatically "floors" the result 
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
        empty_cell.data.x = pos.x;
        empty_cell.data.y = pos.y;
        grid->cells[i] = empty_cell; // Value transfer, not identity
    }
    
}

static void clear_cell(ca_lib_grid_t *grid, cell_t *cell)
{
    if (!cell->data.ptr) {return;}
    cell->data.ptr = grid->free_func(cell->data.ptr);
    cell->data.size = 0;
    cell->data.x = cell->x;
    cell->data.y = cell->y;
}

static void alloc_data_allocated(void **data_loc, void *data, size_t data_size)
{
    *data_loc = data; // data_loc has already been alloc:ed
}

static void dont_free(void **data_loc)
{
    return; // Reject work, do nothing, sleep.
}

static ioopm_list_t *create_cell_buffer(ca_lib_grid_t *grid)
{
    ioopm_list_t *buf = ioopm_linked_list_create(alloc_data_allocated, dont_free);
    for (size_t i = 0; i < grid->height * grid->width; i++)
    {
        ioopm_linked_list_append(buf, (void *)(&grid->cells[i].data), sizeof(data_t));
    }
    return buf;
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

ca_lib_grid_t *ca_lib_create_grid(void *meta_data, size_t width, size_t height, ca_lib_data_alloc_function_t alloc_func, ca_lib_data_free_function_t free_func)
{
    size_t cells_mem_size = width * height * sizeof(cell_t); // memory required for the cells
    ca_lib_grid_t *grid = calloc(1, cells_mem_size + sizeof(ca_lib_grid_t));

    grid->meta_data = meta_data;

    grid->height = height;
    grid->width = width;
    grid->alloc_func = alloc_func;
    grid->free_func = free_func;

    fill_grid_empty_cells(grid);

    return grid;
}

void *ca_lib_get_meta_data(ca_lib_grid_t *grid)
{
    return grid->meta_data;
}

size_t ca_lib_get_grid_width(ca_lib_grid_t *grid)
{
    return grid->width;
}

size_t ca_lib_get_grid_height(ca_lib_grid_t *grid)
{
    return grid->height;
}

bool ca_lib_check_limits(ca_lib_grid_t *grid, size_t x, size_t y)
{
    return ((x >= 0 && x < grid->width) && (y >= 0 && y < grid->height));
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
    if (!ca_lib_check_limits(grid, x, y))
    {
        return;
    }

    cell_t *cell = &grid->cells[pos_to_i(grid->width, x, y)]; // Get pointer to cell at (x,y)
    cell->data.size = data_size; // Change cell's data_size
    cell->data.x = x;
    cell->data.y = y;

    // Free previous data if present
    if (cell->data.ptr) { cell->data.ptr = grid->free_func(cell->data.ptr); }

    // Allocate for new data
    cell->data.ptr = grid->alloc_func(data_ptr, data_size);
}

// Moves the cell's data at (x1,y1) to (x2, y2) - overwriting and freeing any potential cell at (x2, y2)
void ca_lib_move_cell(ca_lib_grid_t *grid, size_t x1, size_t y1, size_t x2, size_t y2)
{
    if (!ca_lib_check_limits(grid, x1, y1) || !ca_lib_check_limits(grid, x2, y2))
    {
        return;
    }

    // TODO: Optimise: don't need to clear and reinsert, just moooooove

    // Grab cell at (x1, y1)
    cell_t *cellxy = &grid->cells[pos_to_i(grid->width, x1, y1)];

    // Insert data from cellxy at (x2, y2)
    ca_lib_insert_cell(grid, x2, y2, cellxy->data.size, cellxy->data.ptr);

    clear_cell(grid, cellxy);
}

// Switches the cells' data at (x1,y1) and (x2, y2)
void ca_lib_switch_cells(ca_lib_grid_t *grid, size_t x1, size_t y1, size_t x2, size_t y2)
{
    if (!ca_lib_check_limits(grid, x1, y1) || !ca_lib_check_limits(grid, x2, y2))
    {
        return;
    }
    
    // No real memory management is needed, simply switch the 'data'
    cell_t *cell_1 = &grid->cells[pos_to_i(grid->width, x1, y1)];
    cell_t *cell_2 = &grid->cells[pos_to_i(grid->width, x2, y2)];
    data_t data_1 = cell_1->data;
    data_1.x = x2;
    data_1.y = y2;
    data_t data_2 = cell_2->data;
    data_2.x = x1;
    data_2.y = y1;
    cell_1->data = data_2;
    cell_2->data = data_1;
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
void ca_lib_print_grid(ca_lib_grid_t *grid, ca_lib_data_to_char_t convert_func)
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
    puts(""); // new line
}

void ca_lib_simulate(ca_lib_grid_t *grid, ca_lib_simulate_cell_t sim_func)
{
    ioopm_list_t *cell_buf = create_cell_buffer(grid);
    while (!ioopm_linked_list_is_empty(cell_buf))
    {
        data_t *current_data;
        ioopm_linked_list_remove(cell_buf, 0, (void **)(&current_data));
        sim_func(grid, current_data);
    }
    ioopm_linked_list_destroy(&cell_buf);
}

// Applies the given simulation function to the grid without keeping track of movement
// It naïvely applies the sim_func on each cell - good for simple automatas where movement isn't implemented
void ca_lib_simulate_unabstract(ca_lib_grid_t *grid, ca_lib_simulate_cell_t sim_func)
{
    for (size_t i = 0; i < grid->height * grid->width; i++)
    {
        sim_func(grid, &grid->cells[i].data);
    }
}

/// GRAPHICS ///

// draw an size x size cube
static void draw_cell(int x, int y, int size)
{
	for (int _x = 0; _x < size; _x++)
	{
		for (int _y = 0; _y < size; _y++)
		{
			gfx_point(x + _x, y + _y);
		}	
	}
}

static void render_cell(ca_lib_grid_t *grid, data_t *data, ca_lib_cell_to_color_t color_convert_func, size_t scale)
{
    // Set Color
    int color[3] = {0,0,0}; // Initialize three integers
    color_convert_func(data, color); // Determine color from data
    gfx_color(color[0], color[1], color[2]); // Set color

    // Rotate coordinates
    int x = (int)data->x; 
    int y = (grid->height - 1) - (int)data->y;

    draw_cell(x * scale, y * scale, scale); // Draw the cell to scale
}

static void render_grid(ca_lib_grid_t *grid, ca_lib_cell_to_color_t color_convert_func, size_t scale)
{
    for (size_t i = 0; i < grid->height * grid->width; i++)
    {
        render_cell(grid, &grid->cells[i].data, color_convert_func, scale);
    }
}

void ca_lib_start_graphics_simulation(ca_lib_grid_t *grid, ca_lib_cell_to_color_t color_convert_func, ca_lib_simulate_grid_t sim_func, int iterations, size_t scale)
{
	gfx_open(grid->width * scale,grid->height * scale, "CA-Lib Simulation");
    char c;
    if (iterations == 0)
    {
        while (1)
        {
            // Simulate
            render_grid(grid, color_convert_func, scale);
            sim_func(grid);
            c = gfx_wait();
            if (c == 'c') continue;
            if (c == 'q') break;
        }
    }
    else
    {
        for (size_t i = 0; i < iterations; i++)
        {
            // Simulate
            render_grid(grid, color_convert_func, scale);
            sim_func(grid);
        }
        puts("\n      --SIMULATION DONE--\n     Press 'q' to quit");
        while(1) 
        {
		// Wait for the user to press a character.
		c = gfx_wait();

		// Quit if it is the letter q.
		if(c=='q') break;
	    }
    }




    // while(1) {
	// // Wait for the user to press a character.
	// c = gfx_wait();

	// // Quit if it is the letter q.
	// if(c=='q') break;
	// }
}
