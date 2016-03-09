#include "include/x86.h"
#include "include/stdarg.h"
#include "include/stdio.h"

/*
 * You may refer to lib/printfmt.c
 * to implement the printk() and vprintk().
 * You can also implement a simplier version
 * by yourself.
 */
#define PORT 0x3f8

static void init_serial() {
	   outb(PORT + 1, 0x00);
	   outb(PORT + 3, 0x80);
	   outb(PORT + 0, 0x03);
	   outb(PORT + 1, 0x00);
	   outb(PORT + 3, 0x03);
	   outb(PORT + 2, 0xC7);
	   outb(PORT + 4, 0x0B);
}

static int is_serial_idle() {
	   return inb(PORT + 5) & 0x20;
}

void putchar(char ch)
{
	init_serial();
	while (!is_serial_idle());
	outb(PORT,ch);
}
static void putch(int ch, int *cnt){
	/*	;
	*
	 * Call the output function(such as putchar(ch)) to display character 'ch'.
	 */
	putchar(ch);
//	*cnt++;

}

int	vprintk(const char *fmt,va_list ap){ 
	//hlt();
	int cnt;
	vprintfmt((void*)putch,&cnt,fmt,ap);
	/*
	 * uncomment the hlt()
	 * after your completement.
	 * refer to manual.
	 */
	return cnt;
}



int	printk(const char *fmt, ...){
	//hlt();
	va_list ap;
	va_start(ap,fmt);
	int cnt=vprintk(fmt,ap);
	va_end(ap);
	/*
	 * uncomment the hlt()
	 * after your completement.
	 * refer to manual.
	 *
	 * Hint:Use va_list to get the parameters and call vprintk().
	 *		You may also build the actual string here and call a
	 *		function which would print a string.
	 */
	return cnt;
}
