#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int pid;
    char *shmem;
    char *message = "Hello daddy";
    int size_before_mapping;
    int size_after_mapping;
    int size_after_unmapping;
    int size_after_malloc;
    char *new_memory;

    // Allocate memory in the parent process
    shmem = malloc(1000); 

    if (shmem == 0) {
        printf("sbrk failed\n");
        exit(1);
    }

    size_before_mapping = (uint64)sbrk(0);
    int ppid = getpid(); 

    if ((pid = fork()) < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        // Map the shared memory
        uint64 child_shmem_addr = map_shared_pages(ppid, getpid(), (uint64)shmem, 20);
        if (child_shmem_addr == (uint64)-1) {
            printf("map_shared_pages failed\n");
            exit(1);
        }
        size_after_mapping = (uint64)sbrk(0);

        char *child_shmem = (char *)child_shmem_addr;
        // Write the message in the child
        strcpy(child_shmem, message);

        // Unmap the shared memory
        if (unmap_shared_pages(getpid(), child_shmem_addr, 20) != 0) {
            printf("unmap_shared_pages failed\n");
            exit(1);
        }
        size_after_unmapping = (uint64)sbrk(0);

        // Allocate memory after unmapping to ensure malloc works
        new_memory = (char *)malloc(1000);
        if (new_memory == 0) {
            printf("malloc failed after unmapping\n");
            exit(1);
        }

        size_after_malloc = (uint64)sbrk(0);

        printf("Size before mapping: %d\n", size_before_mapping);
        printf("Size after mapping: %d\n", size_after_mapping);
        printf("Size after unmapping: %d\n", size_after_unmapping);
        printf("Size after malloc: %d\n", size_after_malloc);
        exit(0);
    } 
    else { 
        // Wait for the child to finish
        wait(0); 
        // Print the message from the child
        printf("%s\n", shmem);
    }
    exit(0);
}

