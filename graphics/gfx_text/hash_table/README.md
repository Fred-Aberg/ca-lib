# Word frequency counter

Counts the frequency of words in .txt files with support of a hash table and a linked list.

## Description

The final product of this entire project is the word frequency counter, but we also decided to implement the hash table and linked list in a way that makes them useful for other projects as well. The linked list and hash table take generic data which is automatically saved on the heap. A disadvantage of this to keep in mind is that the list and hash table are fairly slow and not the most space efficient. Data is automatically freed when using delete and/or the given free function for each datatype.

The setup when creating a new linked list and/or hash table is somewhat tedious. There are a lot of arguments to enter which are documented in the .h files. Some of the more basic function pointers are pre-implemented and can be seen in the .h file.

For functions such as 'delete' and 'get' etc. it is important to remember to free the pointer being sent in as an argument to avoid memory leaks. Also keep in mind to ONLY free the pointer if the functions are successful. Their success can be seen from their return argument where true indicates that it succeeded and that the pointer should be freed.

## Executing program

### Running word frequency counter

To compile the frequency counter run
```
make perftest
```
The executable can then be used to count the word frequency of any .txt file by running 
```
./perftest "FILENAME.txt"
```

### Checking performance of the word frequency counter

The performance can be checked by running
```
make perftest_16k
```

### Running unit tests

To compile the unit tests enter
```
make unittest
```
To run the unit tests enter 
```
make all
```
The tests for hash_table.c, linked_list.c, and iterator.c can also be run seperately. To exclusively run the unit tests for hash_table enter
```
make hash
```
To exclusively run the unit tests for linked_list enter
```
make list
```
To exclusively run the unit tests for list_iterator enter
```
make iter
```

### Running valgrind

Valgrind can also be automatically be run for all files together and seperately.
To run all enter
```
make allmem
```
For hash_table
```
make hashmem
```
For linked_list
```
make listmem
```
And for list_iterator enter
```
make itermem
```

### Checking code coverage

The code coverage can be checked using gcov. Running the following command will create a html file that will automatically be opened on Firefox. 
```
make gcov_unittest
```

### Clean up auto generated files

To clean up most auto generated files from make execute
```
make clean
```

## Line and branch coverage

The coverage numbers were calculated by running gcov on unit_test.c executing
```
make cov
```

### Each .c files coverage

hash_table.c has line coverage of 95.6% and function coverage of 93.3%

linked_list.c has line coverage of 99.3% and function coverage of 100%

list_iterator.c has line coverage of 75% and function coverage of 62.5%

unit_test.c has line coverage of 98% and function coverage of 100%

### Overall coverage

The project has line coverage of 97.2% and function coverage of 96.5%

## Authors

Marta Björknäs

Fred Åberg
