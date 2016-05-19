#include "include/map.h"
#include "include/fs.h"
#include "include/filesystem.h"
#include "include/string.h"
#include "include/loader.h"
static struct dir root;
static struct bitmap diskmap;
static struct block tmpblock;
uint32_t initblock[512/4];

void fileinit()
{
	int i;
	//ide_read(diskoffset,diskmap.mask,256);
	readseg(diskmap.mask,512*256,diskoffset);
	for (i=0; i<=20; i++) printk("%x",diskmap.mask[i]);
	printk("read 1 begin\n");
	//ide_read(diskoffset+512*256,root.entries,1);		
	readseg((void *)root.entries,512,diskoffset+512*256);
	printk("read 1 end\n");
	//ide_read(diskoffset+baseoffset,initblock,1);
	readseg((void *)initblock,512,diskoffset+baseoffset);
}

void initfileread(unsigned char *loc,int cnt,int offset)
{
	int readserial=offset/512;
	int readloc=readserial*512;
	int startloc,neednum;
	while (cnt>0)
	{
		//ide_read(diskoffset+baseoffset+initblock[readserial]*512
		//		,tmpblock.data,1);
		readseg(tmpblock.data,512,
				diskoffset+baseoffset+initblock[readserial]*512);
		startloc=offset-readloc;	//start place of the block
		if (cnt>=512-startloc)	//num of needed bytes of the block
		{
			cnt=cnt-512+startloc;
			neednum=512-startloc;
		}
		else 
		{
			neednum=cnt-startloc;
			cnt=0;
		}
		memcpy(loc,&tmpblock.data[startloc],neednum);
		loc=loc+neednum;
		readloc+=SECTSIZE;
		offset=readloc;
		readserial++;
	}
}
void fileread(unsigned char *loc,int cnt,int offset)
{
	while (cnt!=0)
	{
		ide_read(diskoffset+baseoffset,loc,1);
		cnt=cnt-SECTSIZE;
		offset=offset+SECTSIZE;
	}
}
