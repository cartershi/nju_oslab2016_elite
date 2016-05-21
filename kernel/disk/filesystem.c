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
	ide_read(diskoffset/512,diskmap.mask,256);
	//for (int i=0x00; i<0x50; i++) 
		//printk("%x",diskmap.mask[i]);
	//printk("read 1 begin\n");
	ide_read(diskoffset/512+256,root.entries,1);		
	//readseg((void *)root.entries,512,diskoffset+512*256);
	//printk("read 1 end\n");
	ide_read((diskoffset+baseoffset)/512,initblock,1);
	//readseg((void *)initblock,512,diskoffset+baseoffset);
}

void initfileread(unsigned char *loc,int cnt,int offset)
{
	int readserial=offset/512;
	int readloc=readserial*512;
	int startloc,neednum;
	while (cnt>0)
	{
		ide_read((diskoffset+baseoffset)/512+initblock[readserial]
				,tmpblock.data,1);
		//readseg(tmpblock.data,512,
		//		diskoffset+baseoffset+initblock[readserial]*512);
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

void filefind(struct myFCB *dest,char *openname)
{
	int i;
	//printk("%s\n",root.entries[1].filename);
	for(i=0; i<16; i++)
		if (strcmp(root.entries[i].filename,openname)==0) break;
	if (i==16) {printk("NO SUCH FCB TO OPEN\n"); return;}
	dest->size=root.entries[i].file_size;
	ide_read((diskoffset+baseoffset)/512+root.entries[i].inode_offset
			,dest->infoblock,1);
	//for (i=0; i<512/4; i++)
	//printk("%u",dest->infoblock[i]);
	return;
}

void fileread(unsigned char *loc,int cnt,int offset,struct myFCB *dest)
{
	dest->pointer=dest->pointer+cnt;
	if (dest->pointer>dest->size) dest->pointer=dest->size;
	int readserial=offset/512;
	int readloc=readserial*512;
	int startloc,neednum;
	while (cnt>0)
	{
		ide_read((diskoffset+baseoffset)/512+dest->infoblock[readserial]
				,tmpblock.data,1);
		//readseg(tmpblock.data,512,
		//		diskoffset+baseoffset+initblock[readserial]*512);
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
