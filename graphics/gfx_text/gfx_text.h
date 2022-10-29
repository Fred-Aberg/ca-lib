#include <stdio.h>
#include "../gfx/gfx.h"
#include "hash_table/hash_table.h"
#include <ctype.h>

ioopm_hash_table_t *gfx_txt_initialize_font(char *font_path);

void gfx_txt_draw_text(int x, int y, int font_size, char *txt, ioopm_hash_table_t *font);