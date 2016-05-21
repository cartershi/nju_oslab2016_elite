#include "include/common.h"
#include "include/filesystem.h"
#define FCB_NUM 10
static int filecnt=0;
static struct myFCB fcblist[FCB_NUM];

void fcbinit()
{
	int i;
	for (i=0; i<FCB_NUM; i++)
		fcblist[i].state=FCB_FREE;
	filecnt=0;
}

int sys_file_open(char *openname)
{
	int i,allocted;
	//printk("%s\n",openname);
	for (i=0; i<FCB_NUM; i++)
		if (fcblist[i].state==FCB_FREE) break;
	if (i==FCB_NUM) { printk("FCB FULL\n"); return -1;}
	allocted=i;
	fcblist[allocted].state=FCB_USED;
	filecnt++;
	fcblist[allocted].name=filecnt;
	fcblist[allocted].pointer=0;
	filefind(&fcblist[allocted],openname);
	return filecnt;
}

void sys_file_close(int fp)
{
	int i;
	for (i=0; i<FCB_NUM; i++)
		if (fcblist[i].name==fp) break;
	if (i==FCB_NUM) {printk("NO SUCH FCB TO CLOSE\n"); return;}
	fcblist[i].state=FCB_FREE;
	return;
}

void sys_file_read(unsigned char *loc,int fp,int cnt)
{
	//printk("fp %d cnt %d \n",fp,cnt);
	int i;
	for (i=0; i<FCB_NUM; i++)
		if (fcblist[i].name==fp) break;
	if (i==FCB_NUM) {printk("NO SUCH FCB TO READ\n"); return;}
	fileread(loc,cnt,fcblist[i].pointer,&fcblist[i]);
}

void sys_file_seek(int fp,int move)
{
	int i;
	for (i=0; i<FCB_NUM; i++)
		if (fcblist[i].name==fp) break;
	if (i==FCB_NUM) {printk("NO SUCH FCB TO LSEEK\n"); return;}
	fcblist[i].pointer+=move;
	if (fcblist[i].pointer<0) fcblist[i].pointer=0;
	if (fcblist[i].pointer>fcblist[i].size) 
		fcblist[i].pointer=fcblist[i].size;
}
