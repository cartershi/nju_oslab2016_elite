#include "fcb.h"

void fileinit();
void initfileread(unsigned char *,int,int);
void filefind(struct myFCB*, char*);
void fileread(unsigned char*, int,int,struct myFCB*);
void filewrite(unsigned char*, int,int,struct myFCB*);
//void fileread();
//void filewrite();
