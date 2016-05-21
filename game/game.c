#include "include/string.h"
#include "include/x86.h"
#include "include/video.h"
#include "include/sysbreak.h"
void add_node();
void move_node();
void draw_screen();
typedef struct Node_he{
	int locx,locy,color;
}Node;

static Node t[1000];
static int loc=100,seed=2, start_loc=0,end_loc=0;
static uint8_t screen[SCR_SIZE];

semaphore mutex=1;
semaphore empty=5;
semaphore full=0;
int thid[20];

char con[10]="consumer";
char pro[10]="producer";
static int rand()
{
	seed=0x015A4E35 * seed + 1;
	return (seed >> 16) & 0x7FFF;
}

void producer(int* st)
{
	printf("produce start id %d\n",*st);
	sem_wait(&empty);
	sem_wait(&mutex);
	sleep(200);
	sem_post(&mutex);
	printf("produce now %d id %d\n",full,*st);
	sem_post(&full);
	//printf("end now %d id %d\n",full,*st);
	exit(2);
}

void consumer(int *st)
{
	printf("consume start id %d \n",*st);
	sem_wait(&full);
	sem_wait(&mutex);
	printf("consume now %d id %d \n",full,*st);
	sleep(200);
	sem_post(&mutex);
	sem_post(&empty);
	exit(2);
}

int main(){
	/*int i=0;
	empty=5;
	full=0;
	mutex=1;
	for (i=1; i<=18; i++) thid[i]=i;
	thread_create(producer,&thid[1]);
	thread_create(producer,&thid[2]);
	thread_create(producer,&thid[3]);
	thread_create(producer,&thid[4]);
	thread_create(consumer,&thid[5]);
	thread_create(consumer,&thid[6]);
	thread_create(consumer,&thid[7]);
	thread_create(consumer,&thid[8]);
	//thread_create(consumer,con);
	//thread_create(producer,&thid[9]);
	//thread_create(consumer,con);
	thread_create(producer,&thid[10]);
	thread_create(producer,&thid[11]);
	//thread_create(consumer,con);
	//thread_create(consumer,con);
	sleep(10000);
	if (fork()==0)
	{
		while (1)
		{
			i++;
		printf("ping\n");
		sleep(200);
		if (i==5) exit(0);
		}
	}
	else
	{
		while (1)
		{
		printf("pong\n");
		i++;
		sleep(100);
		if (i==6) exit(0);
		}
	}
	exit(0);*/
	//printf("game in\n");
	int fp=open("1.txt");
	char readtest[520];
	read((void *)readtest,514,fp);
	for (int i=0; i<514; i++)
		printf("%c",readtest[i]);
	lseek(fp,-5);
	printf("\n");
	read((void *)readtest,514,fp);
	for (int i=0; i<514; i++)
		printf("%c",readtest[i]);
	close(fp);
	int now=0,target;
	t[0].locx=-10;
	t[0].locy=10;
	while (1)
	{
		target=query_timer();
		while (now<target)
		{
			if(now%5==0) {
				draw_screen();//redraw=true;
			}
			//if(now==0) 
			move_node();
			int moved=query_key();
			if (moved!=0)
			{
				loc=loc+moved*2;
				if (loc<0) loc=0;
				if (loc>314) loc=314;
				draw_screen();	
			}
			if(now%10==0) add_node();
			now++;
		}
	}
	return 0;
}

void add_node()
{
	end_loc++;
	t[end_loc].locx=-10;
	t[end_loc].locy=rand()%314;
}

void move_node()
{
	int i;
	for (i=start_loc; i<=end_loc; i++)
		t[i].locx=t[i].locx+1;
	if (t[start_loc].locx>=198) start_loc++;
}

void test_bomb()
{
	int num1=t[start_loc].locx;
	int num2=t[start_loc].locy;
	if (num1>=187)
		if ((num2+9>=loc)&&(num2<=loc+4)) 
		printf("hehe\n");//disable_interrupt();}
}

void draw_screen()
{
	memset(screen,0,SCR_SIZE);
	int i,j,num;
	test_bomb();
	for (j=start_loc; j<=end_loc; j++)
	{
		num=t[j].locx;
		for (i=0;i<10; i++)
			if (num+i>=0&&num+i<=199)
			//draw_line(num+i,t[j].locy,3,10);
			memset(screen+((num+i)<<8)+((num+i)<<6)+t[j].locy,3,10);
	}
	//draw_line(198,loc,2,5);
	memset(screen+(198<<8)+(198<<6)+loc,2,5);
	display_screen(screen);
}
