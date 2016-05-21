#include "boot/boot.h"
#include "include/loader.h"
#include "include/pmap.h"
#include "include/kvm.h"
#include "include/common.h"
#include "include/pcb.h"
#include "include/memlayout.h"
#include "include/filesystem.h"
#define SECTSIZE 512

void readseg(unsigned char *, int, int);

void intogame(pde_t*,uint32_t);

void gameloader(void)
{
	//printk("file in\n");
	fileinit();
	//printk("file out\n");
	struct ELFHeader *elf;
	struct ProgramHeader *ph = NULL;
	unsigned char buf[4096];
	unsigned char page_buffer[4096];
	initfileread(buf, 4096, 0);
	elf = (void*)buf;
	unsigned int phoff=elf->phoff;
	unsigned int phentsize=elf->phentsize;
	unsigned int phnum=elf->phnum;
	//printk("%x\n",elf->magic);
	struct PageInfo *pp;
	pde_t *pgdir=page2kva(page_alloc(1));
	/* Load each program segment */
	int i=0;

	for(i=0;i<phnum;i++ ) {
		ph=(void *)((unsigned int)buf + phoff+i*phentsize);
		/* Scan the program header table, load each segment into memory */
		{
			unsigned int va = ph->vaddr;
			// the num of bytes already loaded
			unsigned int data_loaded = 0;

			while(va < (ph->vaddr + ph->memsz))
			{
				// allocate a new page
				unsigned int offset = va & 0xfff;
				// forced alignment on 4K boundary
				va &=0xfffff000;
				pp=page_alloc(1);
				unsigned int *addr=(unsigned int*)page2kva(pp);
				page_insert(pgdir,pp,(void*)va,PTE_U|PTE_W);
				// zero the page buffer
				memset(page_buffer, 0, 4096);

				// the number of bytes to load this time
				unsigned int load_byte_num = 4096 - offset;
				if((ph->filesz - data_loaded) < load_byte_num)
					load_byte_num = ph->filesz - data_loaded;
				
				//printk("va %x load_byte %x\n",va,load_byte_num);
				if (load_byte_num!=0)
				initfileread((void *)(page_buffer + offset),load_byte_num,
						ph->off + data_loaded);
				memcpy(addr, page_buffer, 4096);

				va += 4096;
				data_loaded += load_byte_num;
			}

		}
	}
	//boot_map_region(pgdir,0xc5000000,10*1024*1024,0x5000000,PTE_U);
	uint32_t stackss=0xc0000000-PGSIZE;
	pp=page_alloc(1);
	page_insert(pgdir,pp,(void *)stackss,PTE_U|PTE_W);
	for (i=0; i<128/4; i++)
	*(pgdir+0x300+i)=*((pte_t *)(rcr3()+KERNBASE)+0x300+i);
	*(pgdir)=*((pte_t *)(rcr3()+KERNBASE));
	lcr3((unsigned int)(pgdir)-KERNBASE);
	volatile uint32_t entry = elf->entry;
	intogame(pgdir,entry);
}

void intogame(pde_t* pgdir,uint32_t entry){
	struct TrapFrame ss;
	struct TrapFrame *tf=&ss;
	tf->edi=0;
	tf->esi=0;
	tf->ebp=0;
	tf->xxx=0;
	tf->ebx=0;
	tf->edx=0;
	tf->ecx=0;
	tf->eax=0;
	tf->eflags=0x202;
	tf->irq=0x80;
	tf->eip=entry;
	tf->cs=(SELECTOR_USER(SEG_USER_CODE));
	tf->esp=KERNBASE;//0xbfffffff;
	tf->ss=(SELECTOR_USER(SEG_USER_DATA));
	tf->ds=tf->es=(SELECTOR_USER(SEG_USER_DATA));
	process_prepare(pgdir,tf);
	runprocess();
	/*__asm__("movl %0,%%esp"::"r"(ref));
	__asm__("popa");
	__asm__("popl %es");
	__asm__("popl %ds");
	//__asm__("popl %0,");
	__asm__("addl $8, %%esp"::);
	__asm__("iret");
	*/
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* 等待磁盘完毕 */
}

/* 读磁盘的一个扇区 */
void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);
	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

/* 将位于磁盘offset位置的count字节数据读入物理地址pa */
void
readseg(unsigned char *pa, int count, int offset) {
	unsigned char *epa;
	epa = pa + count;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 1;
	for(; pa < epa; pa += SECTSIZE, offset ++)
		readsect(pa, offset);
}
