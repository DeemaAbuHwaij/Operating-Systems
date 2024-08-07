#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    //sets its own affinity mask
    int affinity_mask = 1 | (1 << 2); 
    printf("The mask is set to be 0 and 2\n");
    set_affinity_mask(affinity_mask);
    // then runs a loop that prints its own process ID
    while (1)
    {
        printf("Process ID: %d\n", getpid());
    }
    exit(0,0);
}


//int affinity_mask = 1 | (1 << 2);  (The test in the assignment).
//CPU 0 (bit 0 is set: 0001)  --> 1 in binary is 0001.
//CPU 2 (bit 2 is set: 0100)  --> 1 << 2 shifts 1 left by 2 positions, resulting in 0100.
//So, the affinity_mask 0101 means that the process is allowed to run on CPU 0 and CPU 2.