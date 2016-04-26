#include "include/common.h"
#include "include/pcb.h"
#include "include/kvm.h"
#include "include/pmap.h"
struct PCB
{
	uint8_t kstack[4096];
	uint32_t PID;
	uint32_t FA_PID;
	uint8_t state;
	uint32_t time_slice;
	uint32_t sleeptime;
	pde_t *pgdir;
	struct TrapFrame *tf;
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
	for (i=NR_PCB-1; i>=1;i--)
	{
		process[i].link=pcb_free_list;
		pcb_free_list=&process[i];
	}
	pcb_running_list=&process[0];
	process[0].PID=0;
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
	allocted->link=pcb_toberun_list;
	pcb_toberun_list=allocted;
	return allocted;
}

void sys_fork(struct TrapFrame *tf)
{
	void* pgva=0;
	void* pteva=0;
	int i,j;
	pde_t *pgdir=(pde_t *)(rcr3()+KERNBASE);
	pde_t *newpgdir=page2kva(page_alloc(1));
	pte_t *reptedir;
	pte_t *newptedir;
	uint32_t *addr;
	struct PageInfo *new_pt;
	for (i=0; i<128/4; i++)	//kernel low copy
    	*(newpgdir+0x300+i)=*((pte_t *)(pgdir+0x300+i));
    *(newpgdir)=*((pte_t *)pgdir);
	for (i=1; i<=1024; i++)	//pgae deep copy
	{
		pde_t *repgdir=pgdir+PDX(pgva);
		if (((*repgdir)&PTE_P)==1&&
				((*(newpgdir+PDX(pgva)))&PTE_P)==0)
		{
			pteva=pgva;
			reptedir=KADDR(PTE_ADDR(*repgdir));
			new_pt=page_alloc(1);
			new_pt->pp_ref=1;
			*(newpgdir+PDX(pgva))=page2pa(new_pt)|PTE_P|PTE_U|PTE_W;
			newptedir=page2kva(new_pt);
			for (j=1; j<=1024; j++)
			{
				if(((*(reptedir+PTX(pteva)))&PTE_P)==1){
					new_pt=page_alloc(1);
					new_pt->pp_ref=1;
					*(newptedir+PTX(pteva))=page2pa(new_pt)|PTE_P|PTE_U|PTE_W;
					addr=(uint32_t *) page2kva(new_pt);
					memcpy(addr,pteva,4096);
				}
				pteva=pteva+(1<<12);
			}
		}
		pgva=pgva+(1<<22);
	}
	pcb_running_list->tf=tf;
	struct PCB* allocted=alloc_pcb(newpgdir);
	for (i=0; i<4096; i++)	//stack deep copy
		allocted->kstack[i]=pcb_running_list->kstack[i];
	allocted->tf=(struct TrapFrame *)
		((void *)tf-(void*)pcb_running_list->kstack+(void *)allocted->kstack);
	allocted->tf->eax=0;
}

void kstack_prepare(struct PCB* newpcb,struct TrapFrame *tf)
{
	uint32_t *kernel_stack=(uint32_t*)newpcb->kstack;
	*(kernel_stack+1023)=tf->ss;	//ss
	*(kernel_stack+1022)=tf->esp;	//esp
	*(kernel_stack+1021)=tf->eflags;	//eflags
	*(kernel_stack+1020)=tf->cs;	//cs
	*(kernel_stack+1019)=tf->eip;	//eip
	*(kernel_stack+1018)=tf->error_code;	//error_code
	*(kernel_stack+1017)=(uint32_t)tf->irq;	//irq	
	*(kernel_stack+1016)=tf->ds;	//ds
	*(kernel_stack+1015)=tf->es;	//es
	*(kernel_stack+1014)=tf->eax;	
	*(kernel_stack+1013)=tf->ecx;	
	*(kernel_stack+1012)=tf->edx;	
	*(kernel_stack+1011)=tf->ebx;	
	*(kernel_stack+1010)=tf->xxx;	
	*(kernel_stack+1009)=tf->ebp;	
	*(kernel_stack+1008)=tf->esi;	
	*(kernel_stack+1007)=tf->edi;	
	newpcb->tf=(struct TrapFrame *)(kernel_stack+1007);
}

void pcb_sleeping()
{
	struct PCB* node=pcb_sleeping_list;
	struct PCB* tmphead=&process[0];	//process[0] is reserved
	tmphead->link=pcb_sleeping_list;
	if (node==NULL) return; //empty 
	struct PCB* nextnode=pcb_sleeping_list->link;
	if (nextnode==NULL) //only one process
	{
		node->time_slice=node->time_slice+1;
		if (node->time_slice==node->sleeptime) 
		{
			node->state=PRO_TOBERUN;
			node->link=pcb_toberun_list;
			pcb_toberun_list=node;
			pcb_sleeping_list=NULL;
		}
		return;
	}
	node=tmphead;
	nextnode=tmphead->link;
	while (nextnode!=NULL) 
	{
		nextnode->time_slice=nextnode->time_slice+1;
		if (nextnode->time_slice==nextnode->sleeptime)//nextnode is awake
		{
			node->link=nextnode->link;
			nextnode->state=PRO_TOBERUN;
			nextnode->link=pcb_toberun_list;
			pcb_toberun_list=nextnode;
			nextnode=node->link;
		}
		else
		{
			node=nextnode;
			nextnode=node->link;
		}
	}
	pcb_sleeping_list=tmphead->link;
}

void updaterunninglist()
{
	struct PCB* node;
	while (pcb_toberun_list!=NULL)
	{
		node=pcb_toberun_list;
		pcb_toberun_list=pcb_toberun_list->link;
		node->link=pcb_running_list;
		pcb_running_list=node;
		node->state=PRO_RUNNING;
	}
}
void runprocess()
{
	while (pcb_running_list==process) {//printk("on guy");
		__asm__("sti");updaterunninglist();}
	lcr3((uint32_t)(pcb_running_list->pgdir)-KERNBASE);
	change_tss((uint32_t )(pcb_running_list->kstack+4096));
	__asm__("movl %0,%%esp"::"r"(pcb_running_list->tf));
	__asm__("popa");
	__asm__("popl %es");
	__asm__("popl %ds");
	__asm__("addl $8, %%esp"::);
	__asm__("sti");
	__asm__("iret");
}

void pcb_tosleep(int sleeptime,struct TrapFrame *tf)
{
	struct PCB* node;
	node=pcb_running_list;
	pcb_running_list=pcb_running_list->link;
	node->sleeptime=sleeptime;
	node->time_slice=0;
	node->tf=tf;
	node->link=pcb_sleeping_list;
	node->state=PRO_SLEEPING;
	pcb_sleeping_list=node;
}

void process_prepare(pde_t *pgdir,struct TrapFrame* tf)
{
	struct PCB* newpcb=alloc_pcb(pgdir);
	kstack_prepare(newpcb,tf);
}
