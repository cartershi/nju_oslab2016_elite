#include "map.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

uint32_t maploc();
void test_content();

struct dir root;
struct bitmap diskmap;

FILE *fp;

int main(int argc,char* argv[])
{
	FILE *pFile;
	int i,j,blockcnt;
	uint32_t readnum,serial;
	struct dirent* file;
	struct inode infoblock;	//restore inode
	struct block datablock;	//restore data
	fp=fopen("newdisk","wb");
	for (i=0; i<512/sizeof(struct dirent); i++) 
		root.entries[i].inode_offset=-1;	//free		
	for (j=0; j<argc-1; j++)
	{
		file=&(root.entries[j]);
		//printf("%s\n",argv[j+1]);
		i=0;	//name
		while (argv[j+1][i]!='\0')
		{
			file->filename[i]=argv[j+1][i];
			i++;
		}
		file->filename[i]='\0';
		blockcnt=0;
		//printf("%s\n",file->filename);
		file->inode_offset=maploc();
		//printf("inode %d\n",file->inode_offset);
		for (i=0; i<512/4; i++) 
			infoblock.data_block_offsets[i]=0;
		file->file_size=0;
		pFile=fopen(argv[j+1],"rb");
		readnum=fread(datablock.data,1,512,pFile);
		while (readnum!=0)
		{
			serial=maploc();
			infoblock.data_block_offsets[blockcnt]=serial; //info block update
			blockcnt++;
			file->file_size+=readnum;
			fseek(fp,baseoffset+serial*512,SEEK_SET);
			fwrite(datablock.data,1,512,fp);
			for (i=0; i<512; i++) datablock.data[i]=0;
			readnum=fread(datablock.data,1,512,pFile); //next block
		}
		fseek(fp,baseoffset+file->inode_offset*512,SEEK_SET);
		fwrite(infoblock.data_block_offsets,1,512,fp);
		//for (i=0; i<512/4; i++) printf("%u",
		//		diskdata[file->inode_offset]->data[i*4]);
		fclose(pFile);
	}
	
		test_content();
	return 0;
}
void test_content()
{
	fseek(fp,0,SEEK_SET);
	fwrite(diskmap.mask,1,sizeof(struct bitmap),fp);
	fwrite(root.entries,1,sizeof(struct dir),fp);
	/*int i,j,tmp,k,bc,s;
	uint32_t *kk;
	freopen("../out.txt","w",stdout);
	for (k=0; k<16; k++) 
	if (root.entries[k].inode_offset!=-1)
	{
		tmp=root.entries[k].inode_offset;
		printf("name %s\n",root.entries[k].filename);
		printf("size %u\n",root.entries[k].file_size);
		for (i=0; i<512/4; i++) 
			printf("%u ",diskdata[tmp]->data[i*4]);
		printf("\n");
		s=root.entries[k].file_size;
		bc=0;
		while (s>0)
		{
			if (s>=512)
			{
				kk=(uint32_t *)&(diskdata[tmp]->data[bc]);
				for (j=0; j<512; j++)
					printf("%c",diskdata[*kk]->data[j]);
				s=s-512;
			}
			else
			{
				kk=(uint32_t *)&(diskdata[tmp]->data[bc]);
				for (j=0; j<s; j++)
					printf("%c",diskdata[*kk]->data[j]);
				s=0;
			}
			bc=bc+4;
			//printf("\n");
		}
	}*/
}
uint32_t maploc()
{
	uint8_t bittest;
	uint32_t cnt=0;
	int i;
	while (cnt<mapnum)
	{
		bittest=0x80;
		for (i=7; i>=0; i--)
		{
			if ((diskmap.mask[cnt]&bittest)>>i==0)
			{
				diskmap.mask[cnt]+=bittest;
				//printf("mask %x\n",diskmap.mask[cnt]);
				//printf("block %d\n",cnt*8+7-i);
				return cnt*8+7-i;
			}
			else
				bittest=bittest>>1;
		}
		cnt++;
	}
	return 0;
}
