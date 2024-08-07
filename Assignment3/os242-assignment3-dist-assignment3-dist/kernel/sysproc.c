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
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
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
  argaddr(0, &p);
  return wait(p);
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


uint64
sys_map_shared_pages(void) {
  uint64 src_va;
  uint64 size;
  int src_pid;
  int dst_pid;

  argint(0, &src_pid);    
  argint(1, &dst_pid);
  argaddr(2, &src_va);
  argaddr(3, &size);

  if (src_pid == 0 || dst_pid == 0 || src_va == 0 || size == 0) { //Error
    return -1;
  }
  struct proc* src_proc; 
  struct proc* dst_proc;

  if(src_pid == myproc() -> pid){
    src_proc = myproc();
  }
  else
  {
    src_proc = find_proc(src_pid);
  }

  if(dst_pid == myproc() -> pid){
    dst_proc = myproc();
  }
  else
  {
    dst_proc = find_proc(src_pid);
  }

  if (src_proc == 0 || dst_proc == 0) {  //Error in find_proc
    return -1;
  }

  uint64 dst_va = map_shared_pages(src_proc, dst_proc, src_va, size);

  if(dst_proc == myproc()){
    release(&src_proc->lock);
  }
  else if(src_proc == myproc()){
    release(&dst_proc->lock);
  }

  if (dst_va == 0) {
    return -1;
  }
  
  return dst_va;
}

uint64
sys_unmap_shared_pages(void) {
    uint64 addr;
    uint64 size;
    int pid;

    argint(0, &pid);  
    argaddr(1, &addr); 
    argaddr(2, &size);

    if (pid == 0 || addr == 0 || size == 0) { //Error
        return -1;
    }

    struct proc* p = find_proc(pid);

    if (p == 0) { //Error in find_proc
        return -1;
    }
    unmap_shared_pages(p, addr, size);
    release(&p->lock);
    return 0;
}

