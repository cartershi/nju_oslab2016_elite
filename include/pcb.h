#include "memlayout.h"

void process_prepare(pde_t*,struct TrapFrame*);
void init_pcb();
void runprocess();
void pcb_sleeping();
void sys_fork(struct TrapFrame*);
void pcb_tosleep(int,struct TrapFrame*);
