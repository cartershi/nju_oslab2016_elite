#ifndef __SYSBREAK_H__
#define __SYSBREAK_H__

void printf(const char *fmt,...);
void display_screen(uint8_t*);
int query_key();
int query_timer();
int fork();
void sleep(int);
void exit(int);
void thread_create();
void sem_wait(void *);
void sem_post(void *);
int open(char*);
void read(void*,int,int);
void close(int);
void lseek(int,int);

#endif
