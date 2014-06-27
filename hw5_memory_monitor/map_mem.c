#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<time.h>
#include<stdlib.h>
#define FILE_SIZE 255
#define TRUE 1

int main( int argc , const char *argv[] ){
	int random_num;
	int fd;
	void *file_memory;
	struct stat status;	



	if( access( "foo.txt", F_OK) != -1 ){
		printf("\"foo.txt\" is exist .\n");
		fd = open( "foo.txt" , O_RDWR );
	}	
	else{
		printf("\"foo.txt\" is not exist , creating .\n");
		fd = open( "foo.txt" , O_RDWR | O_CREAT , S_IRWXU | S_IRWXG | S_IRWXO );
		
		if( fd < 0 ){
			printf("build file error ! \n");
			return -1;
		}
		else{
			printf("Create \"foo.txt\" successfully !");
		}
	}
	
	lseek( fd , FILE_SIZE  , SEEK_SET );
	int write_state;
	write_state = write( fd , "" , 1);

	if( write_state > 0 ){
		printf("%d bytes was written in \"foo.txt\" \n" , FILE_SIZE );
	}
	else{
		printf("Error ! nothing is writen \n");
		return -1;
	}
	
	lseek( fd , 0 , SEEK_SET);
	
	file_memory = mmap( 0 , FILE_SIZE , PROT_WRITE , MAP_SHARED , fd , 0 );
	close(fd);

	if( file_memory == (void *)(-1) ){
		printf("Mapping error !\n");
		return -1;
	}
	
	char command;
	while( TRUE ){
		srand(time(NULL));
		random_num = rand() % 256;
		printf("#################################\n");
		printf("Type enter will write One random number ( 0~256) in \"foo.txt\"\n");
		printf("Type \"q\" will leave program \n");
		scanf("%c", &command );
		if( command == 'q' )
			break;
		sprintf( (char *)(file_memory) , "%d\n" , random_num  );
		msync( file_memory , FILE_SIZE , MS_SYNC | MS_INVALIDATE);
		printf("\"%d\" was write in \"foo.txt\"\n",random_num);
	}

	printf("Bye!\n");
	return 0;
}
