#include <stdio.h>
#include "ca_lib.h"
#include <unistd.h>
#include "graphics/gfx/gfx.h"

// #define Sim_width 10
// #define Sim_height 10

enum blocks
{
    Rock = 5,
    Sand = 2,
    Water = 1,
    Air = 0
};
typedef enum blocks blocks_t;

struct sand_sim_meta_data
{
    int rock_count;
    int sand_count;
    int water_count;
    int air_count;
};
typedef struct sand_sim_meta_data ss_meta_data_t;

void greeting()
{
    puts("█▀ ▄▀█ █▄░█ █▀▄  █▀ █ █▀▄▀█\n▄█ █▀█ █░▀█ █▄▀  ▄█ █ █░▀░█");
}

void generate_cell_value(ca_lib_grid_t *grid, data_t *data)
{
    ss_meta_data_t *md = ca_lib_get_meta_data(grid);
    int r = rand() % 10;
    if (r > 2)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Air;
        data->size = sizeof(enum blocks);
        md->air_count++;
    }
    else if (r == 0)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Water;
        data->size = sizeof(enum blocks);
        md->water_count++;
    }
    else if (r == 1)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Sand;
        data->size = sizeof(enum blocks);
        md->sand_count++;
    }
    else if (r == 2)
    {
        data->ptr = calloc(1, sizeof(enum blocks));
        *(enum blocks *)data->ptr = Rock;
        data->size = sizeof(enum blocks);
        md->rock_count++;
    }
}

void update_water(ca_lib_grid_t *grid, data_t *data)
{
    // Water can move sideways horizontally freely and randomly
    bool sw_move = (ca_lib_check_limits(grid, data->x - 1, data->y) && *(blocks_t *)ca_lib_get_cell_data(grid, data->x - 1, data->y).ptr < *(blocks_t *)data->ptr);
    bool se_move = (ca_lib_check_limits(grid, data->x + 1, data->y) && *(blocks_t *)ca_lib_get_cell_data(grid, data->x + 1, data->y).ptr < *(blocks_t *)data->ptr);
    if (sw_move && se_move)
    {
        float r = rand() % 10000 + 2000;
        if (r == 0) // w
        {
            ca_lib_switch_cells(grid, data->x, data->y, data->x - 1, data->y);
            return;
        }
        if (r == 1) // e
        {
            ca_lib_switch_cells(grid, data->x, data->y, data->x + 1, data->y);
            return;
        }
    }
    else if (sw_move)
    {
        ca_lib_switch_cells(grid, data->x, data->y, data->x - 1, data->y);
        return;
    }
    else if (se_move)
    {
        ca_lib_switch_cells(grid, data->x, data->y, data->x + 1, data->y);
        return;
    }
}

void update_sand(ca_lib_grid_t *grid, data_t *data)
{
    // Sand can move down diagonally
    bool sw_move = (ca_lib_check_limits(grid, data->x - 1, data->y - 1) && *(blocks_t *)ca_lib_get_cell_data(grid, data->x - 1, data->y - 1).ptr < *(blocks_t *)data->ptr);
    bool se_move = (ca_lib_check_limits(grid, data->x + 1, data->y - 1) && *(blocks_t *)ca_lib_get_cell_data(grid, data->x + 1, data->y - 1).ptr < *(blocks_t *)data->ptr);
    if (sw_move && se_move)
    {
        int r = rand() % 2;
        if (r == 0) // w
        {
            ca_lib_switch_cells(grid, data->x, data->y, data->x - 1, data->y - 1);
            return;
        }
        if (r == 1) // e
        {
            ca_lib_switch_cells(grid, data->x, data->y, data->x + 1, data->y - 1);
            return;
        }
    }
    else if (sw_move)
    {
        ca_lib_switch_cells(grid, data->x, data->y, data->x - 1, data->y - 1);
        return;
    }
    else if (se_move)
    {
        ca_lib_switch_cells(grid, data->x, data->y, data->x + 1, data->y - 1);
        return;
    }
}

void update_block(ca_lib_grid_t *grid, data_t *data)
{
    // Skip if air
    if (*(blocks_t *)data->ptr == Air)
    {
        return;
    }

    blocks_t cell_block = *(blocks_t *)(ca_lib_get_cell_data(grid, data->x, data->y).ptr);
    
    // All blocks fall down if there's nothing below
    if (ca_lib_check_limits(grid, data->x, data->y - 1))
    {
        // Switch if block below is lighter
        blocks_t s_block = *(blocks_t *)(ca_lib_get_cell_data(grid, data->x, data->y - 1).ptr);
        if (cell_block > s_block)
        {
            ca_lib_switch_cells(grid, data->x, data->y, data->x, data->y - 1);
            return;
        }
    }

    // Apply further physics to water and sand
    if (cell_block == Water)
    {
        update_water(grid, data);
    }
    else if (cell_block == Sand)
    {
        update_sand(grid, data);
    }
}

char block_to_char(void *data_ptr)
{
    if (*(enum blocks *)data_ptr == Air)
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

ca_lib_grid_t *initialize_sand_sim_grid(size_t width, size_t height)
{
    ss_meta_data_t *meta_data = calloc(1, sizeof(ss_meta_data_t));
    ca_lib_grid_t *grid = ca_lib_create_grid(meta_data, width, height, ca_lib_alloc_simple_ptr, ca_lib_free_simple_ptr);
    ca_lib_simulate_unabstract(grid, generate_cell_value);
    return grid;
}

void sand_simulate(ca_lib_grid_t *grid)
{
    blocks_t insert_block = Water;
    ca_lib_insert_cell(grid, ca_lib_get_grid_height(grid) / 2, ca_lib_get_grid_height(grid) - 1, sizeof(blocks_t), &insert_block);
    ca_lib_simulate(grid, update_block);
    //sleep(1);
}

void color_convert(data_t *data, int *color)
{
    if ((*(blocks_t *)data->ptr) == Water) // Blue
    {
        color[0] = 0;
        color[1] = 30;
        color[2] = 200;
        return;
    }
    else if ((*(blocks_t *)data->ptr) == Rock) // Grey
    {
        color[0] = 100;
        color[1] = 100;
        color[2] = 100;
        return;
    }
    else if ((*(blocks_t *)data->ptr) == Sand) // Yellow
    {
        color[0] = 200;
        color[1] = 200;
        color[2] = 0;
        return;
    }
}

int main(int argc, char *argv[])
{
    size_t width = 1;
    size_t height = 1;
    size_t iterations = 1;
    size_t scale = 1;
    if (argc == 5)
    {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        iterations = atoi(argv[3]);
        scale = atoi(argv[4]);
    }
    else
    {
        printf("\nUsage: '%s [width] [height] [iterations] [scale]'", argv[0]);
        return 1;
    }
    greeting();
    ca_lib_grid_t *grid = initialize_sand_sim_grid(width, height);
    //printf("\033[%dB", (int)ca_lib_get_grid_height(grid) + 3);
    ss_meta_data_t *md = ca_lib_get_meta_data(grid);

    ca_lib_start_graphics_simulation(grid, color_convert, sand_simulate, iterations, scale);
    
    free(ca_lib_get_meta_data(grid));
    ca_lib_destroy_grid(grid);
    return 0;
}