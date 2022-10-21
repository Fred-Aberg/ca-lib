# Cellular Automata Library

## ca-lib

A simple cellular automata library in c with the goal of providing a framework for making cellular automatas. The library provides the user with functions to manipulate a  grid of cells - some of the basic ones include:

- Creating/deleting a *x* by *y* grid of cells 

- Switching cells' coordinates

- "Overwriting" a cell with another

- Creating a cell from scratch - user provides the *data* pointer.

## Design & Planning

### Data-representation

**Types accessible to the user use the format: 'ca_lib_[name]'**

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
```

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
```

```C
ca_lib_grid_t *ca_lib_create_grid(size_t width, size_t heigth, 
ca_lib_alloc_function_t alloc_func, ca_lib_free_function_t free_func);
```



How homogenous does the cells of the grid need to be? Can the *'data'* pointer of cells iin the same grid be of different structs? Default free/alloc functions could be given to the grid - and if the user wants to have different datatypes in *'data'* then they can use functions which they can provide specific free/alloc functions depending on the nature of the data stored in *'data'*. If we allow this then cells will most likely need to store their own free/alloc functions.

```c
typedef void(*ca_lib_alloc_function_t)(void *data, size_t data_size);
typedef void(*ca_lib_free_function_t)(void *data);

// Default static functions that are used when freeing/allocating 
static void alloc_cell(size_t x, size_t y, void *data);
static void free_cell(size_t x, size_t y, void *data);
```

## Function Documentation

```c
void example_insert_cell_bla(long ur_mom)
{
    char *bla;
}
```
