#include "./include/string.h"
#include "./include/x86.h"
#include "./include/video.h"
#include "./include/keyboard.h"
#include "./include/timer.h"

int loc,locx;

void draw_screen();

void key_event(int code)
{
	disable_interrupt();
	if (code==0x4b)
	{
		if (loc>0) loc--;
		locx=locx+10;
		draw_screen();
	}
	else 
	if (code==0x4d)
	{
		if (loc<315) loc++;
		locx=locx+10;
		draw_screen();
	}
	enable_interrupt();
}

extern void init_intr();
extern void init_idt();
extern void init_serial();

int main(){
	//hlt();
	loc=0;
	locx=0;
	init_serial();
	init_idt();
	init_intr();
	set_timer_intr_handler(init_timer);
	set_keyboard_intr_handler(key_event);
	printk("first\n");
	enable_interrupt();
	printk("second\n");
	while (1);
	printk("third\n");
	return 0;
}

void draw_screen()
{
	prepare_buffer();
	int i;
	for (i=0;i<10; i++)
		draw_line(i+locx,0,3,10);
	draw_line(198,loc,2,5);
	display_buffer();
}
