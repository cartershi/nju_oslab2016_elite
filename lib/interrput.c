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

int fork()
{
	int pid;
	__asm__("movl $0x5, %%eax\n\t"
			"int $0x80\n\t"
			"movl %%eax, %0"
			:"=r"(pid)
			:
			:"%eax");
	return pid;
}

void sleep(int sleeptime)
{
	__asm__("movl $0x6, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"int $0x80"
			:
			:"r"(sleeptime)
			:"%eax","%ecx");
}

void exit(int num)
{
	__asm__("movl $0x7, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"int $0x80"
			:
			:"r"(num)
			:"%eax","%ecx");
}

void thread_create(void *loc,void *para)
{
	__asm__("movl $0x8, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"movl %1, %%edx\n\t"
			"int $0x80"
			:
			:"r"(loc),"r"(para)
			:"%eax","%ecx","%edx");
}

void sem_wait(void *semloc)
{
	__asm__("movl $0x9, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"int $0x80"
			:
			:"r"(semloc)
			:"%eax","%ecx");
}

void sem_post(void *semloc)
{
	__asm__("movl $0xA, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"int $0x80"
			:
			:"r"(semloc)
			:"%eax","%ecx");
}

int open(char * src)
{
	int fp;
	__asm__("movl $0xB, %%eax\n\t"
			"movl %1, %%ecx\n\t"
			"int $0x80\n\t"
			"movl %%eax, %0"
			:"=g"(fp)
			:"r"(src)
			:"%eax","%ecx");
	return fp;
}

void read(void *dest,int cnt,int fp)
{
	__asm__("movl $0xC, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"movl %1, %%edx\n\t"
			"movl %2, %%ebx\n\t"
			"int $0x80"
			:
			:"m"(dest),"m"(cnt),"m"(fp)
			:"%eax","%ecx","%edx","%ebx");
}

void close(int fp)
{
	__asm__("movl $0xD, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"int $0x80"
			:
			:"r"(fp)
			:"%eax","%ecx");
}

void lseek(int fp,int move)
{
	__asm__("movl $0xE, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"movl %1, %%edx\n\t"
			"int $0x80"
			:
			:"r"(fp),"r"(move)
			:"%eax","ecx");
}

void write(void *dest,int cnt,int fp)
{
	__asm__("movl $0xF, %%eax\n\t"
			"movl %0, %%ecx\n\t"
			"movl %1, %%edx\n\t"
			"movl %2, %%ebx\n\t"
			"int $0x80"
			:
			:"m"(dest),"m"(cnt),"m"(fp)
			:"%eax","%ecx","%edx","%ebx");
}
