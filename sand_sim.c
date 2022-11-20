#include <stdio.h>
#include "ca_lib.h"

#define Sim_width 20
#define Sim_height 10

enum blocks
{
    Rock = 5,
    Sand = 2,
    Water = 1
};
typedef enum blocks blocks_t;

void greeting()
{
    puts("█▀ ▄▀█ █▄░█ █▀▄  █▀ █ █▀▄▀█\n▄█ █▀█ █░▀█ █▄▀  ▄█ █ █░▀░█");
}

void generate_cell_value(ca_lib_grid_t *grid, data_t *data)
{
    int r = rand() % 10;
    if (r > 2)
    {
        data->ptr = NULL;
    }
    else if (r == 0)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Water;
        data->size = sizeof(enum blocks);
    }
    else if (r == 1)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Sand;
        data->size = sizeof(enum blocks);
    }
    else if (r == 2)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Rock;
        data->size = sizeof(enum blocks);
    }
}

void update_block(ca_lib_grid_t *grid, data_t *data)
{
    // Skip if no block
    if (data->ptr == NULL)
    {
        return;
    }
    
    // All blocks fall down if there's nothing below
    if (ca_lib_check_limits(grid, data->x, data->y - 1))
    {
        data_t s_data = ca_lib_get_cell_data(grid, data->x, data->y - 1);
        if (s_data.ptr == NULL)
        {
            ca_lib_move_cell(grid, data->x, data->y, data->x, data->y - 1);
        }
    }

    //blocks_t cell_type = *(blocks_t *)ca_lib_get_cell_data(grid, data->x, data->y).ptr;
    
}

char block_to_char(void *data_ptr)
{
    if (data_ptr == NULL)
    {
        return '.';
    }
    if (*(enum blocks *)data_ptr == Water)
    {
        return '~';
    }
    if (*(enum blocks *)data_ptr == Sand)
    {
        return '#';
    }
    if (*(enum blocks *)data_ptr == Rock)
    {
        return '@';
    }

    return '?'; // Non-standard behaviour
}

ca_lib_grid_t *initialize_sand_sim_grid()
{
    ca_lib_grid_t *grid = ca_lib_create_grid(Sim_width, Sim_height, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
    ca_lib_simulate_unabstract(grid, generate_cell_value);
    return grid;
}

int main(int argc, char *argv[])
{
    greeting();
    ca_lib_grid_t *grid = initialize_sand_sim_grid();
    for (size_t i = 0; i < 10; i++)
    {
        ca_lib_print_grid(grid, block_to_char);
        ca_lib_simulate(grid, update_block);
    }
    
    ca_lib_destroy_grid(grid);
    return 0;
}