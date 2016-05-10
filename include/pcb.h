#include "memlayout.h"

#define process_exit 0
#define thread_exit 2

void process_prepare(pde_t*,struct TrapFrame*);
void init_pcb();
void runprocess();
void pcb_sleeping();
void sys_fork(struct TrapFrame*);
void pcb_tosleep(int,struct TrapFrame*);
void pcb_exit(int);
void sys_thread_create();
