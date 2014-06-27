#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<time.h>
#include<stdlib.h>
#define FILE_SIZE 255

int main( int argc , const char *argv[] ){
	int random_num;
	int fd;
	void *file_memory;
	struct stat status;		

	srand(time(NULL));

	if( access( "foo.txt", F_OK) != -1 ){
		printf("\"foo.txt\" is exist .\n");
		fd = open( "foo.txt" , O_RDWR );
	}	
	else{
		printf("\"foo.txt\" is not exist , creating .\n");
		fd = open( "foo.txt" , O_RDWR | O_CREAT , S_IRWXU | S_IRWXG | S_IRWXO );
	}

	stat( "foo.txt" , &status );

	if( fd < 0 ){
		printf("build file error ! \n");
		return -1;
	}
		
	file_memory = mmap( 0 , FILE_SIZE , PROT_READ | PROT_WRITE , MAP_SHARED , fd , 0 );
	if( file_memory == (void *)(-1) ){
		printf("Mapping error !\n");
		return -1;
	}
	
	printf("We get main memory address %p \n\n" , file_memory );

	char command;
	int i;
	time_t before_change = status.st_mtime;
	while(1){
		stat( "foo.txt" , &status );
		if( before_change != status.st_mtime ){
			int tmp;
			sscanf( file_memory , "%d" , &tmp );
			printf("\"foo.txt\" was change , number is %d \n" , tmp);
			before_change = status.st_mtime;
		}
	}
	
	printf("Bye!\n");
	return 0;
}
