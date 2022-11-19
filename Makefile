C_COMPILER     		= gcc
C_OPTIONS          	= -Wall -pedantic
C_LINK_OPTIONS     	= -lm
CUNIT_LINK        	= -lcunit
OBJECTS				= packages/hash_table/linked_list.c ca_lib.c

CFLAGS= -g

main:
	ca_test
	ca_mem

clean:        	# cleans up all objectfiles
	rm -f *.o ca_test
	rm -f *.gcda cov
	rm -f *.gcno cov
	rm -f *.out

ca_test: ca_lib_tests.c $(OBJECTS)
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) $^ -o $@ $(CUNIT_LINK)
ca_mem: ca_test
	valgrind --leak-check=full ./ca_test
ca_perf: sand_sim.c $(OBJECTS)
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) $^ -o $@ $(CUNIT_LINK)