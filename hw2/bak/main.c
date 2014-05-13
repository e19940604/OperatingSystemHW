#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i-1)%N
#define THINK 0
#define HUNGURY 1
#define EAT 2

int state[N];
pthread_mutex_t mutex;
pthread_t s[N];
int counter = 0;

void *philosopher( void *arg )
{
	pthread_mutex_lock(&mutex);
	counter += 1;
	int i = counter;

	printf("thread %d progressing \n" , i );
	sleep(1);
	

	pthread_mutex_unlock(&mutex);
	return NULL;
}

void think( int i )
{
	state[i] = THINK;
}

void eat( int i )
{
	state[i] = EAT;
}

int main()
{
	int i;
	if( pthread_mutex_init(&mutex , NULL )!= 0)
	{
		printf(" mutex init fail ");
		return 1;
	}

	for( i = 0 ; i < N ; i++ )
		if( pthread_create(&s[i], NULL , &philosopher , NULL ) != 0 )
			printf("creat thread error!!");

	for( i = 0 ; i < N ; i++)
		pthread_join(s[i],NULL);
	pthread_mutex_destroy(&mutex);
	
	return 0;
}
