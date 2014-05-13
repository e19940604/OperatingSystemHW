#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<signal.h>
#include<stdbool.h>
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINK 0
#define HUNGURY 1
#define EAT 2

typedef int semaphore;

int para[N];
int state[N] = {0};
pthread_mutex_t mutex;
pthread_t thread[N];
semaphore s[N] = {0};
bool flag = true;
int counter = 0;

void up( semaphore * sem )
{
	while( *sem == 1 );
		*sem -= 1;
}

void down( semaphore * sem )
{
	while( *sem == 0 );
	*sem += 1;
}

void test( int i )
{
	if( state[i] == HUNGURY && state[LEFT] != EAT && state[RIGHT] != EAT)
	{
		state[i] = EAT;
		up(&s[i]);
		
//		printf("the %d thread eatting \n",i);
	}
}

void think(int i )
{
//	printf("the %d thread thinking \n",i);
//	state[i] = THINK;	
	sleep(1);
}

void take_forks( int i )
{
	pthread_mutex_lock(&mutex);
	state[i] = HUNGURY;
	test(i);
	int j;
	printf("%2d: " , i+1);
	for( j = 0 ; j < N ; j++)
	{
		printf("%3d",state[j]);
		if( j == N-1)
			printf("\n");
	}
	pthread_mutex_unlock(&mutex);
	down(&s[i]);
}

void eat( int i )
{
	//printf("the %d thread eating \n",i);
	sleep(1);
}

void put_forks( int i )
{
	pthread_mutex_lock(&mutex);
//	printf("the %d thread stop eatting \n",i);
    state[i] = THINK;
    test(LEFT);
	test(RIGHT);
    pthread_mutex_unlock(&mutex);
}

void philosopher( int i )
{
	while(1)
	{
		if(flag == false )
		{
			printf("thread %d will be close \n" , i+1);
			return;
		}
		think(i);
		take_forks(i);
		eat(i);
		put_forks(i);
			
	}
}

void *callPhilo( void *arg)
{
	int *i;
	i = ((int *)arg);
	printf("thread %d creat \n",*i+1);
	philosopher( *i );
	return NULL;
}

void stopThread( int signum)
{	
	flag = false;
}

int main()
{
	int i;
	signal(SIGINT, stopThread );

	if( pthread_mutex_init(&mutex , NULL )!= 0)
	{
		printf(" mutex init fail ");
		return 1;
	}

	for( i = 0 ; i < N ; i++ )
	{
		para[i] = i;
		if( pthread_create(&thread[i], NULL , &callPhilo , &para[i] ) != 0 )
			printf("creat thread error!!");
	}

	for( i = 0 ; i <= N ; i++)
	{
		pthread_join(thread[i],NULL);
//		printf("thread %d close\n",i+1);
	}

	if( flag == false )
		pthread_mutex_destroy(&mutex);
	
	return 0;
}
