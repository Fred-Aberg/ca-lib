# to compile unit test: 	make unittest
# to run perf test:			make perftest

# to run all tests:			make all
# to run all mem tests:		make allmem 
# to run hash unit test: 	make hash
# to run hash mem test:		make hashmem
# to run list unit test: 	make list
# to run list mem test:		make listmem
# to run iter unit test: 	make iter
# to run iter mem test:		make itermem

# to run gcov:				make gcov_unittest
# to run perf test:			make perftest_16k

# to clean up files:		make clean

main: unittest

C_COMPILER     		= gcc
C_OPTIONS          	= -Wall -pedantic -g
C_LINK_OPTIONS     	= -lm
CUNIT_LINK         	= -lcunit

%.o:  %.c   	# matches any .o pattern and transforms into .c
	$(C_COMPILER) $(C_OPTIONS) -c $^ 

clean:        	# cleans up all objectfiles
	rm -f *.o unittest
	rm -f *.o gcov_unittest
	rm -f *.o perftest
	rm -f *.gcda cov
	rm -f *.gcno cov
	rm -f *.out

unittest: unit_tests.o hash_table.o linked_list.o list_iterator.o
	$(C_COMPILER) $(C_LINK_OPTIONS) $^ -o $@ $(CUNIT_LINK)

perftest: perf_test.o hash_table.o linked_list.o
	$(C_COMPILER) $(C_LINK_OPTIONS) -pg $^ -o $@ $(CUNIT_LINK)

gcov_unittest: unit_tests.c hash_table.c linked_list.c list_iterator.c
	$(C_COMPILER) $(C_OPTIONS) -fprofile-arcs -ftest-coverage $^ -o $@ $(CUNIT_LINK)

cov: gcov_unittest
	./gcov_unittest all
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory out
	firefox out/index.html

all: unittest
	./unittest

hash: unittest
	./unittest hash

list: unittest
	./unittest list

iter: unittest
	./unittest iter

hashmem: unittest
	valgrind --leak-check=full ./unittest hash

listmem: unittest
	valgrind --leak-check=full ./unittest list

itermem: unittest
	valgrind --leak-check=full ./unittest iter

allmem: unittest
	valgrind --leak-check=full ./unittest

perftest_16k: perftest
	./perftest "freq-count/16k-words.txt"
	gprof ./perftest gmon.out