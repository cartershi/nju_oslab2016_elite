#include "./include/string.h"
#include "./include/x86.h"
#include "./include/video.h"
#include "./include/keyboard.h"
#include "./include/timer.h"

void draw_screen();
void add_node();
void move_node();
static void init();
typedef struct Node_he{
	int locx,locy,color;
}Node;

static Node t[1000];
static int loc=0,seed=2,tick=0,start_loc=0,end_loc=0;

void key_event(int code)
{
	if (code==0x4b&&loc>1) loc=loc-2;
	else 
	if (code==0x4d&&loc<314) loc=loc+2;
}

void timer_event(){
	tick++;
}
static int rand()
{
	seed=0x015A4E35 * seed + 1;
	return (seed >> 16) & 0x7FFF;
}
int main(){
	int now=0,target;
	init();
	while (1)
	{
		disable_interrupt();
		if (now==tick)
		{
			enable_interrupt();
			continue;
		}
		target=tick;
		enable_interrupt();
		while (now<target)
		{
			if(now%5==0) draw_screen();//redraw=true;
			//if(now==0) 
			move_node();
			if(now%5==0) add_node();
			now++;
		}
	}
	return 0;
}

void add_node()
{
	end_loc++;
	t[end_loc].locx=-10;
	t[end_loc].locy=rand()%320;
}

void move_node()
{
	int i;
	for (i=start_loc; i<=end_loc; i++)
		t[i].locx=t[i].locx+2;
	if (t[start_loc].locx>=198) start_loc++;
}

void test_bomb()
{
	int num1=t[start_loc].locx;
	int num2=t[start_loc].locy;
	if (num1>=187)
		if ((num2+9>=loc)&&(num2<=loc+4)) 
		{printk("hehe\n");disable_interrupt();}
}
void draw_screen()
{
	prepare_buffer();
	int i,j,num;
	test_bomb();
	for (j=start_loc; j<=end_loc; j++)
	{
		num=t[j].locx;
		for (i=0;i<10; i++)
			if (num+i>=0&&num+i<=199)
			draw_line(num+i,t[j].locy,3,10);
	}
	draw_line(198,loc,2,5);
	display_buffer();
}

extern void init_intr();
extern void init_idt();
extern void init_serial();
void init()
{
	init_serial();
	init_idt();
	init_intr();
	t[0].locx=-10;
	t[0].locy=10;
	set_timer_intr_handler(timer_event);
	set_keyboard_intr_handler(key_event);
	enable_interrupt();
}
