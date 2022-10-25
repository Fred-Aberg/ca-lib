/*
A simple example of using the gfx library.
CSE 20211
9/7/2011
by Prof. Thain
*/

#include <stdio.h>
#include "gfx.h"

// draw an size x size cube
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

int main()
{
	int ysize = 600;
	int xsize = 600;

	char c;

	// Open a new window for drawing.
	gfx_open(xsize,ysize,"Example Graphics Program");

	// Set the current drawing color to green.
	gfx_color(0,200,100);

	// Draw a triangle on the screen.
	gfx_line(100,100,200,100);
	gfx_line(200,100,150,150);
	gfx_line(150,150,100,100);

	draw_cube(100, 100, 50);

	gfx_color(200,0,0);
	draw_cube(300, 300, 5);

	while(1) {
		// Wait for the user to press a character.
		c = gfx_wait();

		// Quit if it is the letter q.
		if(c=='q') break;
	}

	return 0;
}