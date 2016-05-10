#include "include/x86.h"
#include "include/stdio.h"
#include "include/assert.h"
#include "include/video.h"
#include "include/keyboard.h"
#include "include/timer.h"
#include "include/pcb.h"

static void (*do_timer)(void);
static void (*do_keyboard)(int);

void
set_timer_intr_handler( void (*ptr)(void) ) {
	do_timer = ptr;
}
void
set_keyboard_intr_handler( void (*ptr)(int) ) {
	do_keyboard = ptr;
}

void do_syscall(struct TrapFrame *tf);
/* TrapFrame的定义在include/memory.h
 * 请仔细理解这段程序的含义，这些内容将在后续的实验中被反复使用。 */
void
irq_handle(struct TrapFrame *tf) {
	if (tf->irq==0x80){
		do_syscall(tf);
	}
	else 
	{
		if(tf->irq < 1000) {
		if(tf->irq == -1) {
			printk("%s, %d: Unhandled exception!\n", __FUNCTION__, __LINE__);
		}
		else {
			printk("%s, %d: Unexpected exception #%d!\n", __FUNCTION__, __LINE__, tf->irq);
		}
		assert(0);
		}

	if (tf->irq == 1000) {
		//printk("timer\n");
		do_timer();
		pcb_sleeping();		
	} else if (tf->irq == 1001) {
		uint32_t code = inb(0x60);
		uint32_t val = inb(0x61);
		outb(0x61, val | 0x80);
		outb(0x61, val);

		//printk("%s, %d: key code = %x\n", __FUNCTION__, __LINE__, code);
		do_keyboard(code);
	} else {
		if (tf->irq!=1014)assert(0);
	}
	}
}

void do_syscall(struct TrapFrame *tf){
	switch(tf->eax){
		case 1:
			vprintk((char*)tf->ecx,(va_list)tf->ebx);
			break;
		case 2:
			sys_display((uint32_t*)tf->ecx);
			break;
		case 3:
			tf->ecx=sys_key_query();
			break;
		case 4:
			tf->ecx=sys_timer_query();
			break;
		case 5:
			sys_fork(tf);
			runprocess();
			break;
		case 6:
			pcb_tosleep(tf->ecx,tf);
			runprocess();
		case 7:
			pcb_exit(tf->ecx);
			runprocess();
		case 8:
			//printk("thread\n");
			sys_thread_create(tf,tf->ecx,tf->edx);
			runprocess();
		break;
		default: break;
		//case SYS_prink:
	}
}
