#include "include/string.h"
#include "include/x86.h"
#include "include/sysbreak.h"
#include "include/stdarg.h"
void printf(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	
	__asm__("movl $0x1, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"movl %1, %%edx\n\t"
			"int $0x80"
			:
			:"m"(fmt),"r"(ap)
			:"%eax","%ecx","%edx");
	va_end(ap);
}

void display_screen(uint8_t *vref)
{
	__asm__("movl $0x2, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"int $0x80"
			:
			:"m"(vref)
			:"%eax","%ecx");
}

int query_key()
{
	int moved;
	__asm__("movl $0x3, %%eax\n\t"
			"int $0x80\n\t"
			"movl %%ecx, %0"
			:"=r"(moved)
			:
			:"%eax","%ecx");
	return moved;
}

int query_timer()
{
	int time;
	__asm__("movl $0x4, %%eax\n\t"
			"int $0x80\n\t"
			"movl %%ecx, %0"
			:"=r"(time)
			:
			:"%eax","%ecx");
	return time;
}

