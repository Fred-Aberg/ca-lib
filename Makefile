C_COMPILER     		= gcc
FLAGS				= -Wall -std=c17 -g
C_OPTIONS          	= -Wall -pedantic -g
C_LINK_OPTIONS     	= -lm
CUNIT_LINK        	= -lcunit
OBJECTS				= packages/hash_table/linked_list.c ca_lib.c graphics/gfx/gfx.c

CFLAGS= -g -lX11 -lm

main:
	ca_test
	ca_mem

clean:        	# cleans up all objectfiles
	rm -f *.o ca_test
	rm -f *.o ca_perf
	rm -f *.o sand_sim
	rm -f *.gcda cov
	rm -f *.gcno cov
	rm -f *.out

ca_test: ca_lib_tests.c $(OBJECTS)
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) $^ -o $@ $(CUNIT_LINK)
ca_mem: ca_test
	valgrind --leak-check=full ./ca_test
ca_perf: sand_sim.c $(OBJECTS)
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) $^ -o $@ $(CUNIT_LINK)
ca_perf_mem: ca_perf
	valgrind --leak-check=full ./ca_perf

sand_sim:
	gcc sand_sim.c $(OBJECTS) -o sand_sim -lX11 -lm
