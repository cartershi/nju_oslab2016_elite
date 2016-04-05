#include "boot/boot.h"
#include "include/loader.h"
#include "include/stdio.h"
#include "include/string.h"
#include "include/x86.h"
#include "include/mmu.h"
#include "include/pmap.h"
#include "include/memlayout.h"
#define SECTSIZE 512

void readseg(unsigned char *, int, int);

void gameloader(void)
{
	struct ELFHeader *elf;
	struct ProgramHeader *ph = NULL;
	unsigned char buf[4096];
	unsigned char page_buffer[4096];
	readseg(buf, 4096, 0x400800);
	elf = (void*)buf;
	unsigned int phoff=elf->phoff;
	unsigned int phentsize=elf->phentsize;
	unsigned int phnum=elf->phnum;
	struct PageInfo *pp;
	pde_t *pgdir=(void*) (0xc7000000);
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
				pp=page_alloc(0);
				unsigned int *addr=(unsigned int*)page2kva(pp);
				page_insert(pgdir,pp,(void*)va,PTE_U|PTE_W);
				// zero the page buffer
				memset(page_buffer, 0, 4096);

				// the number of bytes to load this time
				unsigned int load_byte_num = 4096 - offset;
				if((ph->filesz - data_loaded) < load_byte_num)
					load_byte_num = ph->filesz - data_loaded;

				readseg((void *)(page_buffer + offset),load_byte_num,
						0x400800+ph->off + data_loaded);
				memcpy(addr, page_buffer, 4096);

				va += 4096;
				data_loaded += load_byte_num;
			}

		}
	}
	boot_map_region(pgdir,0xc5000000,10*1024*1024,0x5000000,PTE_U);
	printk("stack end\n");
	*(pgdir+0x300)=*((pte_t *)(rcr3()+KERNBASE)+0x300);
	*(pgdir)=*((pte_t *)(rcr3()+KERNBASE));
	__asm("movl $0xc5200000, %%esp":::"%esp");
	lcr3((unsigned int)(pgdir)-KERNBASE);
	volatile uint32_t entry = elf->entry;
	printk("loader end\n");
	((void(*)(void))(entry))();
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
