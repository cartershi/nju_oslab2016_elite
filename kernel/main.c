#include "include/stdio.h"
#include "include/keyboard.h"
#include "include/loader.h"
#include "include/timer.h"
#include "include/x86.h"
#include "include/pmap.h"
#include "include/kvm.h"
extern void init_intr();
extern void init_idt();
extern void init_serial();

int main()
{
	unsigned int num=KERNBASE;
	int i;
	init_serial();
	init_idt();
	init_intr();
	init_timer();
	set_keyboard_intr_handler(key_event);
	set_timer_intr_handler(timer_event);
	init_segment();
	page_init();
	for (i=1; i<=32; i++)
	{
		boot_map_region((void*)(rcr3()+0xc0000000),
				num,(unsigned long)128*1024*1024,0,PTE_P);
		num+=128*1024*1024;
	}
	enable_interrupt();
	gameloader();
	while (1);
}
