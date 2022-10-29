/*
A simple example of using the gfx library.
CSE 20211
9/7/2011
by Prof. Thain
*/

#include <stdio.h>
#include "../gfx/gfx.h"
#include "gfx_text.h"

// draw an size x size cube
// void draw_cube(int x, int y, int size)
// {
// 	for (int _x = 0; _x < size; _x++)
// 	{
// 		for (int _y = 0; _y < size; _y++)
// 		{
// 			gfx_point(x + _x, y + _y);
// 		}
		
// 	}	
// }

int main()
{
	int ysize = 600;
	int xsize = 600;

	char c;

	// Open a new window for drawing.
	gfx_open(xsize,ysize,"Example Graphics Program");
	ioopm_hash_table_t *font = gfx_txt_initialize_font("font.txt");

	// Set the current drawing color to green.
	gfx_color(0,200,100);
	//gfx_txt_draw_text(10, 10, 3, "XYZxyz, (x;y). Tove? Fred! abc", font);

	// Draw a triangle on the screen.
	// gfx_line(100,100,200,100);
	// gfx_line(200,100,150,150);
	// gfx_line(150,150,100,100);


	gfx_color(200,0,0);

	gfx_txt_draw_text(5, 5, 2, "abcdefghijklmnopqrstuvwxyz?", font);
	gfx_txt_draw_text(5, 30, 2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ!", font);
	gfx_txt_draw_text(5, 55, 2, "0123456789-+(x)[x],.=", font);
	gfx_color(0,200,50);
	gfx_txt_draw_text(5, 80, 2, "\"&\% ca_lib 2022 (x,y) = 59\%", font);
	gfx_color(100,200,0);
	gfx_txt_draw_text(5, 105, 2, "--CA_LIB: Fred Aaberg 2022/10/29--", font);
	gfx_txt_draw_text(5, 200, 3, "Tove Marguerite Eglantina Jansson", font);
	gfx_txt_draw_text(5, 250, 2, "Jag har en liten ksisse ao den ae fin!", font);


	while(1) {
		// Wait for the user to press a character.
		c = gfx_wait();

		// Quit if it is the letter q.
		if(c=='q') break;
	}
	ioopm_hash_table_destroy(&font);
	return 0;
}