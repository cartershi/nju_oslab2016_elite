#include "include/keyboard.h"
#include "include/loader.h"
#include "include/pcb.h"
#include "include/timer.h"
#include "include/common.h"
#include "include/pmap.h"
#include "include/kvm.h"
extern void init_intr();
extern void init_idt();
extern void init_serial();

int main()
{
	init_serial();
	init_idt();
	init_intr();
	init_timer();
	set_keyboard_intr_handler(key_event);
	set_timer_intr_handler(timer_event);
	init_segment();
	page_init();
		boot_map_region((void*)(rcr3()+0xc0000000),
				KERNBASE,(unsigned long)128*1024*1024,0,PTE_P);
	enable_interrupt();
	init_pcb();
	gameloader();
	while (1);
}
