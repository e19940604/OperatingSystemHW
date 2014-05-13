#include<unistd.h>
#include<cstdlib>
#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<signal.h>
#include<stdbool.h>
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINK 0
#define HUNGRY 1
#define EAT 2

typedef int semaphore;

bool stop_flag = true ;

class monitor{
private :
	pthread_mutex_t mutex;
	semaphore p[N];
	int status[5];

public :

	monitor(){
		for( int j = 0 ; j < N ; j++ )
			p[j] = 0;
	}
 
	void mu_lock(){
		pthread_mutex_lock( &mutex );
	}
	

	void mu_unlock(){
		pthread_mutex_unlock( &mutex ); 
	}

	void up( int i ){
		while( p[i] == 1 ){
			int tmp = rand()%3+1;
			printf("Thread %d wait %d seconds\n" , i+1 , tmp );
			sleep(tmp);
		}
		p[i] += 1;
	}
	
	void down( int i  ){
		while( p[i] == 0 ){
			int tmp = rand()%3+1;
			printf("Thread %d wait %d seconds\n" , i+1 , tmp );
			sleep(tmp);
		}
		p[i] -= 1;
	}

	void test( int i ){
		if( status[i] == HUNGRY && status[LEFT] != EAT && status[RIGHT] != EAT ){
			up( i );
			status[i] = EAT;
		}
	}

	void set_think( int i ){
		status[i] = THINK;
	}

	void set_hungry( int i ){
		status[i] = HUNGRY;
	}

	void set_eat( int i ){
		status[i] = EAT;
	}

	void print( int i ){
		printf("now %d : " , i+1 );
		for( int j = 0 ; j < N  ; j++){
			char c;
			if( status[j] == 0 )
				c = 'T';
			else if( status[j] == 1 )
				c = 'H';
			else if( status[j] == 2 )
				c = 'E';
			printf("%3c",c);
		}
		printf("\n");
	}
}mo;


void think(){
	sleep(1);
}

void take_forks( int i ){
	mo.mu_lock();
	mo.set_hungry(i);
	mo.test(i);
	mo.print(i);
	mo.mu_unlock();
	mo.down(i);
}

void eat(){
	sleep(1);
}

void put_forks( int i ){
	mo.mu_lock();
	mo.set_think( i );
	mo.test(LEFT);
	mo.test(RIGHT);
	mo.mu_unlock();
}

void dining( int i ){

	while( true ){
		if( stop_flag == false){
			printf("Thread %d will close\n" , i);
			return;
		}
		think();
		take_forks(i);
		eat();
		put_forks(i);
	}
}

void *interface( void *i ){
	int *tmp;
	tmp = ((int *) i );
	printf("Thread %d will create\n" , *tmp);
	dining( *tmp );
	return NULL;
}

void stop( int signum  ){
	stop_flag = false;
}

int main(){
	int i;
	int parameters[N];
	pthread_t thread[N]	;

	signal( SIGINT , stop );

	for( i = 0 ; i < N ; i++ ){
		parameters[i] = i;
		if( pthread_create( &(thread[i]), NULL , &interface , &parameters[i] ) ){
			printf("error to create thread\n");
		}
	}

	for( i = 0 ; i < N ; i++ ){
		pthread_join( thread[i] , NULL );
	}

	return 0;
}
