#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define PGSIZE 4096  // The size of a page in bytes

int main() {
    int pid;
    char *shmem;
    char *message = "Hello child";

    // Allocate memory in the parent process using sbrk
    shmem = (char *)sbrk(PGSIZE);
    if (shmem == (char *)-1) {
        printf("sbrk failed\n");
        exit(1);
    }
    
    strcpy(shmem, message);
    int ppid = getpid();
    
    if ((pid = fork()) < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        uint64 child_shmem_addr = map_shared_pages(ppid, getpid(), (uint64)shmem, PGSIZE);
        
        if (child_shmem_addr == (uint64)-1) {
            printf("map_shared_pages failed\n");
            exit(1);
        }

        char *child_shmem = (char *)child_shmem_addr;
        // Print the message in the child
        printf("%s\n", child_shmem);
        exit(0);
    } 
    else {// Wait for the child to finish
        wait(0);
    }
    exit(0);
}
