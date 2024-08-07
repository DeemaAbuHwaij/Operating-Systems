#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

int main() {
    //Print how many bytes of memory the running process is using by calling memsize().
    fprintf(1, "Memory size before allocation: %d bytes\n", memsize());
    
    //Allocate 20k more bytes of memory by calling malloc().
    void *ptr = malloc(20000);

    //Print how many bytes of memory the running process is using after the allocation.
    fprintf(1, "Memory size after allocation: %d bytes\n", memsize());
    
    //Free the allocated array.
    free(ptr);
    
    //Print how many bytes of memory the running process is using after the release.
    fprintf(1, "Memory size after release: %d bytes\n", memsize());

    exit(0, 0);
}
