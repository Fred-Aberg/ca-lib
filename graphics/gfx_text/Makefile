
C_COMPILER     		= gcc
C_OPTIONS          	= -Wall -pedantic -g
C_LINK_OPTIONS     	= -lm
CUNIT_LINK         	= -lcunit

main:
	gcc example.c ../gfx/gfx.c gfx_text.c hash_table/hash_table.c hash_table/linked_list.c  -o example -lX11 -lm $(C_OPTIONS)
run: main
	./example
clean:
	rm -f *.o

