C_COMPILER     		= gcc
C_OPTIONS          	= -Wall -pedantic
C_LINK_OPTIONS     	= -lm
CUNIT_LINK        	= -lcunit

CFLAGS= -g

main:
	ca_test
	ca_mem

clean:        	# cleans up all objectfiles
	rm -f *.o unittest
	rm -f *.o gcov_unittest
	rm -f *.o perftest
	rm -f *.gcda cov
	rm -f *.gcno cov
	rm -f *.out

ca_test: ca_lib_tests.o ca_lib.o
	$(C_COMPILER) $(C_OPTIONS) $(C_LINK_OPTIONS) $^ -o $@ $(CUNIT_LINK)
ca_mem: ca_test
	valgrind --leak-check=full ./ca_test