# Cellular Automata Library

## ca-lib

A simple cellular automata library in c with the goal of providing a framework for making cellular automatas. The library provides the user with functions to manipulate a  grid of cells - some of the basic ones include:

- Creating/deleting a *x* by *y* grid of cells 

- Switching cells' coordinates

- "Overwriting" a cell with another

- Creating a cell from scratch - user provides the *data* pointer.

## ca-lib-graphics

A library that interacts with ca-lib - using [gfx]([gfx: A Simple Graphics Library (V2)](https://www3.nd.edu/~dthain/courses/cse20211/fall2013/gfx/)) to render grids from ca-lib.

## Design & Planning

### Documentation & code formatting

Types and functions accessible to the user use the naming format: **'ca_lib_name'**

Variable names use the format: ***'name'*** in documentation. (italics + '')

Types in use italics: ***name_t*** in documentation.

### Dictionary

- **grid** - the *x* * *y* grid which the CA simulates on.

- ***cell_t* / cell** - The grid is made up of *x* * *y* cells, they can be manipulated and moved in different ways. Each cells "place" in the grid is constant - what is moved is it's *unique properties*. Each cell is part of the grid struct - they do not need to be freed/allocated by themselves.

- **unique properties** - is the properties of a cell that changes through the simulation - the *'data_size'* and the *'data'* pointer will be switched between cells as the "abstract" cell moves.

- **abstract cell** - In the grid each *cell_t* is set in place, the only thing that "moves" is the cell's data. But in abstraction we can imagine that the identity of the cell is defined by the data and therefore, in an abstract sense, the cells are actually "moving" as one would imagine they'd do in a cellular automata.

- ***'data'* / data** - The cell's data pointer - which is the only thing that differentiates all the cells except for their coordinates. Here the user can point to any struct they want. just "data" can refer to *'data'* and *'data_size'* together.

### Data-representation

A cell could be interpreted as empty if *'data'* is *NULL*.

```c
struct cell
{
    size_t x;
    size_t y;
    size_t data_size;
    void *data; 
};
typedef struct cell cell_t;

void ca_lib_insert_cell(ca_lib_grid_t grid, size_t x, size_t y, size_t data_size, void *data);

bool ca_lib_cell_empty(ca_lib_grid_t grid, size_t x, size_t y);
```

The *cell_t* data type isn't reachable by the user - all interaction is done through library functions.

The *'data'* pointer contains a struct or type of the user's choosing. Alocation and freeing of these pointers need to be done by functions given by parameters in the creation of a grid.

```c
struct grid
{
    size_t width;
    size_t height;
    ca_lib_alloc_function_t alloc_func;
    ca_lib_free_function_t free func;
    cell_t cells[]; // Allocate for 'witdh' * 'height' cells
};

typedef struct grid ca_lib_grid_t;

ca_lib_grid_t *ca_lib_create_grid(size_t width, size_t heigth, ca_lib_alloc_function_t alloc_func, ca_lib_free_function_t free_func);
```

~~How homogenous does the cells of the grid need to be? Can the *'data'* pointer of cells iin the same grid be of different structs? Default free/alloc functions could be given to the grid - and if the user wants to have different datatypes in *'data'* then they can use functions which they can provide specific free/alloc functions depending on the nature of the data stored in *'data'*. If we allow this then cells will most likely need to store their own free/alloc functions.~~

If the user wants heterogeneity in their cells - they'll need to use an enum as a top structure in their *'data'* pointer.

```c
typedef void(*ca_lib_alloc_function_t)(void *data, size_t data_size);
typedef void(*ca_lib_free_function_t)(void *data);

// Default static functions that are used when freeing/allocating 
static void alloc_cell(size_t x, size_t y, void *data);
static void free_cell(size_t x, size_t y, void *data);
```

## Function Documentation
