#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"



uint64
sys_exit(void)
{
  uint64 exit_exit_msg_ptr;
  argaddr(1, &exit_exit_msg_ptr);
  char exit_msg[32];
  //If the process passes 0 as the string address, the exit_exit_msg field
  //is set to the constant string “No exit message”.
  if (!exit_exit_msg_ptr || fetchstr(exit_exit_msg_ptr, exit_msg, 32) <= 0)
  //copy the string from the user address to exit_msg and returns a non-positive value on failure.
    strncpy(exit_msg, "No exit message", 16);
  int n;
  //fetches the first argument (the exit code) passed by the user process.
  argint(0, &n);
  exit(n, exit_msg);
  return 0;
}


uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}


uint64
sys_wait(void)
{
  uint64 p;
  uint64 exit_msg;
  argaddr(0, &p);
  argaddr(1, &exit_msg);
  return wait(p,exit_msg);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//return the size of the running process’ memory in bytes
uint64
sys_memsize(void) {
  struct proc *curproc = myproc(); // Get the current process
  uint64 size;

  if (curproc == 0)  //Check if current process exists, if not return size = 0.
    return 0; 

  acquire(&curproc->lock); //To ensure consistency while accessing its attributes
  size = curproc->sz; //Retrieve the size of the process' memory (curproc->sz) and store it in a local variable size.
  release(&curproc->lock);
  return size;
}


//Write the set_affinity_mask() system call 
//that allows a process to set its own affinity mask.
uint64
sys_set_affinity_mask(void)
{
  int affinity_mask;
  argint(0, &affinity_mask); //retrieves the first argument passed to the system call and stores it in the variable mask
  set_affinity_mask(affinity_mask);  //sets the affinity mask for the current process
  return 0;
}





