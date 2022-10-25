
C_COMPILER     		= gcc
C_OPTIONS          	= -Wall -pedantic -g
C_LINK_OPTIONS     	= -lm
CUNIT_LINK         	= -lcunit

main:
	gcc example.c gfx.c -o example -lX11 -lm
run: main
	./example
clean:
	rm -f *.o

