#include <stdio.h>
#include "../gfx/gfx.h"
#include "hash_table/hash_table.h"
#include <ctype.h>
#include <string.h>

#define font_dim 5
#define font_len 25

void draw_cube(int x, int y, int size)
{
	for (int _x = 0; _x < size; _x++)
	{
		for (int _y = 0; _y < size; _y++)
		{
			gfx_point(x + _x, y + _y);
		}
		
	}	
}

void gfx_txt_draw_text(int x, int y, int font_size, char *txt, ioopm_hash_table_t *font)
{
    int x_offset = 0;
    for (size_t i = 0; i < strlen(txt); i++)
    {
        char *bitmap;
        if(!ioopm_hash_table_lookup(font, &(txt[i]), (void **)(&bitmap))) // load bitmap
        {
            char unknown_char = '@';
            ioopm_hash_table_lookup(font, &unknown_char, (void **)(&bitmap));
        } 
        int bitmap_index = 0;

        // Draw letter
        for (int y_bm = 0; y_bm < font_dim; y_bm++)
        {
            for (int x_bm = 0; x_bm < font_dim; x_bm++)
            {
                if (bitmap[bitmap_index] == 'x')
                {
                    draw_cube(x + x_offset + font_size*x_bm, y + font_size*y_bm, font_size); // draw bitmap-pixel
                }
                bitmap_index++;
            }
        }
        x_offset += font_dim * font_size; // make space for next character
        free(bitmap);
    }
    
}

int hash_char(void *c)
{
    int h = 0;
    return h + (int)(*(char *)c);
}

int order_char(void *a, void *b)
{
    if (*(char *)a < *(char *)b)
    {
        return -1;
    }
    else if (*(char *)a == *(char *)b)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void letter_alloc (void **data_loc, void *data, size_t data_size)
{
    *data_loc = calloc(1, data_size + 1);
    void *real_data = *data_loc;
    for (size_t i = 0; i < font_len; i++)
    {
        ((char *)real_data)[i] = ((char *)data)[i];
    }
}


void print_letter(void **font)
{
    int pixel_index = 0;
    for (size_t i = 0; i < font_dim; i++)
    {
        for (size_t q = 0; q < font_dim; q++)
        {
            char c = (*(char (*)[25])font[0])[pixel_index]; // I donno....
            printf("%c", c);
            pixel_index++;
        }
        printf("\n");
    }
}

void print_font(void *key, void **data, void *extra)
{
    printf("\n%c:\n", *(char *)key);
    print_letter(data);
}

ioopm_hash_table_t *process_file(FILE *f)
{
  ioopm_hash_table_t *font_table = ioopm_hash_table_create(10, order_char, ioopm_int_order, hash_char, ioopm_alloc_simple_ptr, ioopm_alloc_simple_ptr, ioopm_free_simple_ptr, ioopm_free_simple_ptr);
  while (!feof(f))
  {
    char *buf = NULL;
    size_t len = 0;
    getline(&buf, &len, f);
    if (buf[0] == '\n' || buf[0] == '#' || isblank(buf[0])) { continue; } // Comment or blank line
    if (buf[0] == ':')
    {
        char letter = buf[1];
        char *pixels = calloc(1, font_dim + 1);
        int pixel_index = 0;
        for (size_t i = 0; i < font_dim; i++)
        {
            getline(&buf, &len, f);
            for (size_t q = 0; q < font_dim; q++)
            {
                pixels[pixel_index] = buf[q] == 'x' ? 'x' : '.';
                pixel_index++;
            }
        }
        //print_font((void *)(&letter), (void **)(&pixels), NULL);
        ioopm_hash_table_insert(font_table, &letter, pixels, 1, font_len + 1);
        free(pixels);
    }
    free(buf);
  }
  return font_table;
}



ioopm_hash_table_t *gfx_txt_initialize_font(char *font_path)
{
    FILE *in = fopen(font_path, "r");
    ioopm_hash_table_t *font = process_file(in);
    //ioopm_hash_table_apply_to_all(font, print_font, NULL);

    char key = '0';
    char letter[25] = {0};
    ioopm_hash_table_lookup(font, &key, (void **)(&letter));
    //print_letter((void **)(&letter));
    fclose(in);
    return font;
}
