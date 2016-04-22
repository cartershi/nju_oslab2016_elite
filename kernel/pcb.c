#include "include/common.h"
#include "include/pcb.h"
#include "include/kvm.h"
struct PCB
{
	uint8_t kstack[4096];
	uint32_t PID;
	uint32_t FA_PID;
	uint8_t state;
	uint32_t time_slice;
	pde_t *pgdir;
	struct PCB *link;
};

static struct PCB process[NR_PCB];


static struct PCB *pcb_free_list;
static struct PCB *pcb_running_list;
static struct PCB *pcb_toberun_list;
static struct PCB *pcb_sleeping_list;
static struct PCB *pcb_unexec_list;

static uint32_t PID_cnt;

void init_pcb()
{
	int i;
	PID_cnt=0;
	pcb_free_list=NULL;
	for (i=NR_PCB-1; i>=0;i--)
	{
		process[i].link=pcb_free_list;
		pcb_free_list=&process[i];
	}
	pcb_running_list=NULL;
	pcb_toberun_list=NULL;
	pcb_sleeping_list=NULL;
	pcb_unexec_list=NULL;
}

struct PCB * alloc_pcb(pde_t *pgdir)
{
	struct PCB *allocted=pcb_free_list;
	if (pcb_free_list!=NULL)
		pcb_free_list=pcb_free_list->link;
	allocted->state=PRO_TOBERUN;
	allocted->time_slice=0;
	PID_cnt++;
	allocted->PID=PID_cnt;
	allocted->pgdir=pgdir;
	allocted->FA_PID=pcb_running_list->PID;
	return allocted;
}

uint32_t kstack_prepare(struct PCB* newpcb,uint32_t entry)
{
	uint32_t *kernel_stack=(uint32_t*)newpcb->kstack;
	*(kernel_stack+1023)=(SELECTOR_USER(SEG_USER_DATA));	//ss
	*(kernel_stack+1022)=0xbfffffff;	//esp
	*(kernel_stack+1021)=0x202;	//eflags
	*(kernel_stack+1020)=(SELECTOR_USER(SEG_USER_CODE));	//cs
	*(kernel_stack+1019)=entry;	//eip
	*(kernel_stack+1018)=0;	//error_code
	*(kernel_stack+1017)=0x80;	//irq	
	*(kernel_stack+1016)=(SELECTOR_USER(SEG_USER_DATA));	//ds
	*(kernel_stack+1015)=(SELECTOR_USER(SEG_USER_DATA));	//es
	*(kernel_stack+1014)=0;	
	*(kernel_stack+1013)=0;	
	*(kernel_stack+1012)=0;	
	*(kernel_stack+1011)=0;	
	*(kernel_stack+1010)=0;	
	*(kernel_stack+1009)=0;	
	*(kernel_stack+1008)=0;	
	*(kernel_stack+1007)=0;	
	change_tss((uint32_t )(&newpcb->kstack[4096]));
	uint32_t* tf=kernel_stack+1007;
	return (uint32_t)tf;
}

uint32_t process_prepare(pde_t *pgdir,uint32_t entry)
{
	struct PCB* newpcb=alloc_pcb(pgdir);
	return kstack_prepare(newpcb,entry);
}
