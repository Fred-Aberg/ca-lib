#include <stdio.h> 
#include "ca_lib.h"

#define sim_width 20
#define sim_height 20

enum blocks {Rock = 5, Sand = 2, Water = 1};

int main(int argc, char *argv[]) 
{
    enum blocks block_1 = Rock;
    enum blocks block_2 = Rock;
    printf("block_1 == block_2 = %d\n", block_1 == block_2);

    return 0;
}