#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "stdlib.h"

uint64 sys_trace(void)
{
  if (myproc()->trace_flag == 0)
  {
    myproc()->trace_flag = 1;
    argint(0, &myproc()->trace_mask);
    printf("entered here\n");
  }
  return 0;
}

uint64 sys_sigalarm(void)
{
  // printf("arrived here\n");
  struct proc *currprocess;
  currprocess = myproc();
  if (currprocess->alarm_flag == 0)
  {
    int n;
    uint64 handlerfunc;

    argint(0, &n);
    argaddr(1, &handlerfunc);
    currprocess->alarm_interval = n;
    currprocess->handler = handlerfunc;
    currprocess->alarm_flag = 1;
  }
  return 0;
}

uint64 sys_sigreturn(void)
{
  memmove(myproc()->trapframe, myproc()->trapframe_2, sizeof(*(myproc()->trapframe)));
  myproc()->alarm_flag = 1;
  return myproc()->trapframe->a0;
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
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
  if (growproc(n) < 0)
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
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
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
