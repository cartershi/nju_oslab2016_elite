#ifndef __SYSBREAK_H__
#define __SYSBREAK_H__

void printf(const char *fmt,...);
void display_screen(uint8_t*);
int query_key();
int query_timer();
int fork();
void sleep(int);

#endif
