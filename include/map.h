typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

#define diskoffset 0x400a00
#define mapnum 512*256
#define baseoffset 512*256+512


#pragma pack(0)
struct bitmap{
	uint8_t mask[mapnum];
};

struct dirent {
	char filename[24];
	uint32_t file_size;
	uint32_t inode_offset;
};  // sizeof(struct dirent) == 32

struct dir {
	  struct dirent entries[512 / sizeof(struct dirent)];
};  // sizeof(dir) == 512, nr_entries == 16

struct inode {
	  uint32_t data_block_offsets[512 / sizeof(uint32_t)];
};

struct block{
	uint8_t data[512];
};
#pragma pack()
