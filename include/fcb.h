#include "common.h"

#define FCB_FREE 0
#define FCB_USED 1

struct myFCB
{
	int name,pointer,state,size;
	uint32_t infoblock[512/4];
};

void fcbinit();
void sys_file_close(int fp);
void sys_file_read(unsigned char *loc,int fp,int cnt);
int sys_file_open(char *openname);
void sys_file_seek(int,int);
