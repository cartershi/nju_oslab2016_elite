#include "include/x86.h"
#include "include/mmu.h"
#include "include/kvm.h"
#include "include/stdio.h"
#include "include/string.h"
/* One TSS will be enough for all processes in ring 3. It will be used in Lab3. */
static TSS tss; 


inline static void
set_tss(struct Segdesc *ptr) {
	tss.ss0 = SELECTOR_KERNEL(SEG_KERNEL_DATA);		// only one ring 0 stack segment

	uint32_t base = (uint32_t)(&tss);
	uint32_t limit = sizeof(TSS) - 1;
	ptr->sd_lim_15_0  = limit & 0xffff;
	ptr->sd_base_15_0   = base & 0xffff;
	ptr->sd_base_23_16  = (base >> 16) & 0xff;
	ptr->sd_type = STS_T32A;
	ptr->sd_s = 0;
	ptr->sd_dpl = DPL_USER;
	ptr->sd_p = 1;
	ptr->sd_lim_19_16 = limit >> 16;
	ptr->sd_avl = 0;
	ptr->sd_rsv1 = 0;
	ptr->sd_db = 1;
	ptr->sd_g = 0;
	ptr->sd_base_31_24  = base >> 24;
}



/* GDT in the kernel's memory, whose virtual memory is greater than 0xC0000000. */
static struct Segdesc gdt[NR_SEGMENTS];

static void
set_segment(struct Segdesc *ptr, uint32_t pl, uint32_t type) {
	ptr->sd_lim_15_0  = 0xFFFF;
	ptr->sd_base_15_0   = 0x0;
	ptr->sd_base_23_16  = 0x0;
	ptr->sd_type = type;
	ptr->sd_s = 1;
	ptr->sd_dpl = pl;
	ptr->sd_p = 1;
	ptr->sd_lim_19_16 = 0xF;
	ptr->sd_avl = 0;
	ptr->sd_rsv1 = 0;
	ptr->sd_db = 1;
	ptr->sd_g = 1;
	ptr->sd_base_31_24  = 0x0;
}




void
init_segment(void) {
	memset(gdt, 0, sizeof(gdt));
	set_segment(&gdt[SEG_KERNEL_CODE], DPL_KERNEL, SEG_EXECUTABLE | SEG_READABLE);
	set_segment(&gdt[SEG_KERNEL_DATA], DPL_KERNEL, SEG_WRITABLE );
	set_segment(&gdt[SEG_USER_CODE], DPL_USER, SEG_EXECUTABLE | SEG_READABLE); 
	set_segment(&gdt[SEG_USER_DATA], DPL_USER, SEG_READABLE|SEG_WRITABLE); 
	uint16_t data[3];
	data[0]=sizeof(gdt)-1;
	data[1]=(uint32_t)gdt;
	data[2]=((uint32_t)gdt)>>16;
	lgdt(data);

	set_tss(&gdt[SEG_TSS]);
	//set_tss_ss0(SELECTOR_KERNEL(SEG_KERNEL_DATA));
	ltr( SELECTOR_USER(SEG_TSS) );
}

void change_tss(unsigned int loc)
{
	tss.esp0=loc;
}

