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

extern void serial_printc(char);

static void putch(int ch, int *cnt){
	/*	;
	*
	 * Call the output function(such as putchar(ch)) to display character 'ch'.
	 */
	serial_printc(ch);
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
