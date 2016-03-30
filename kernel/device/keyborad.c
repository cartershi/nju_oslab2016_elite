#include "include/keyboard.h"
#include "include/stdio.h"
static int right_move=0;

void key_event(int code)
{
	if (code==0x4b) right_move=right_move-2;
	else
		if (code==0x4d) right_move=right_move+2;
}

int sys_key_query()
{
	int move=right_move;
	right_move=0;
	return move;
}
