#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#define MAX_COM 100
#define MAX_PAR 40
#define MAX_PAR_LEN 40
#define TRUE 1
#define STD_INPUT 0
#define STD_OUTPUT 1

void type_place();
void read_command();
void process_one();
int command_type();
void process_pipe ();
int execvpe(const char *file, char *const argv[],char *const envp[]);
void process_back();
void process_redir();
void process_cd();
void welcome();
void help( char *envp[] );

/* global */
char command[MAX_COM], *parameters[MAX_PAR];
int type;
char *process[MAX_PAR];
char *para[MAX_PAR][MAX_PAR];
char *redirCommand,*fileIn,*fileOut;
int pipe_count;
int flag;

int main( int argc , char **argv , char*envp[] )
{
	welcome();

	while(TRUE)
    {
		type = 0;
		flag = 0;
		memset(command,0,MAX_COM);
		memset(parameters,0,MAX_PAR);

        type_place();
		read_command();

		if( !strcmp( command , "exit") )
			exit(0);
	
		if( flag == 2 )
		{
			help( envp );
			continue;
		}	
	
		if( flag == 1 )
		{
			process_cd();
			continue;
		}	

		if(type == 1)
			process_one();
//			printf("%s\n%s",command,parameters[1]);
		else if(type == 2)
		{
//			int i;
//			for( i = 0 ; process[i] ; i++ )
//			{
//				printf("%s %s %s\n",process[i],para[i][0],para[i][1]);
//			}
			process_pipe();
		}
		else if( type == 3)
		{
			process_back();
		}
		else if( type <= 7)
		{
			process_redir();
		}

    }
}

void welcome()
{
    char here[200];
    char host[200];
    getcwd( here , 200 );
	
	printf("\n==================================================\n");
	printf("welcome my shell \n");
	printf("type \"exit\" will leave the shell\n");
	printf("type \"help\" will show what programs you can use\n");
	printf("==================================================\n\n");
}

void type_place()
{
	char here[200];
	printf("[ Xgnid's shell]");
    getcwd( here , 200 );
	printf( "@%s~# " ,  here ); 
}

void read_command()
{
	//	memset(proc1_para,0,MAX_PAR);
 //   memset(proc2_para,0,MAX_PAR);

	fgets(command,MAX_COM,stdin);
	type = command_type();
    char *tmp;
    int i = 0;
	pipe_count = 0;

	if( type == 2)
	{
		// count the number of commands
		for( i = 0 ; command[i] ; i++)
			if( command[i] == '|' )
				pipe_count++;

		// strtok the command by |
		process[0] = strtok(command,"|\n");
		for( i = 1 ; i < pipe_count+1 ; i++)
			process[i] = strtok(NULL,"|\n");
	
		// strtok every command into their parameters
		for( i = 0 ; i < pipe_count+1 ; i++)
		{
			int k = 0;
			tmp = strtok(process[i]," \n");
			for( k = 0 ;  tmp != NULL ; k++ )
            {
                para[i][k] = tmp;
                tmp = strtok(NULL," \n");
            }
		}

		// remove the empty bit 
		for( i = 1 ; i < pipe_count+1 ; i++)
		{
			process[i] = para[i][0];
		}
	}
	else
	{
		if( type == 3)
		    tmp = strtok(command," \n&");
		else if( type == 1)
			tmp = strtok(command," \n");
		else if( type == 4)
		{
			redirCommand = strtok(command,"<\n");
			fileIn = strtok(NULL,"< \n");
			tmp = strtok(redirCommand," \n");
		}
		else if( type == 5)
		{
			redirCommand = strtok(command,">\n");
			fileOut = strtok(NULL,"> \n");
			tmp = strtok(redirCommand," \n");
		}
		else if( type == 6)
		{
			redirCommand = strtok(command,"><\n");
			fileIn = strtok(NULL,">< \n");
			fileOut = strtok(NULL,">< \n");
			tmp = strtok(redirCommand," \n");
		}
		else if( type == 7)
		{
			redirCommand = strtok(command,"><\n");
            fileOut = strtok(NULL,">< \n");
            fileIn = strtok(NULL,">< \n");
			tmp = strtok(redirCommand," \n");
		}

	    while(tmp != NULL)
	    {
	        parameters[i] = tmp;
	        //printf("%s\n",parameters[i]);
			if( type == 3)
		        tmp = strtok(NULL," \n&");
			else
				tmp = strtok(NULL," \n");
	        i++;
	    }
		if(!strcmp(command,"ls"))
			parameters[i] = "--color=auto";
		
		if(!strcmp(command,"cd"))
			flag = 1;
		
		if(!strcmp(command,"help"))
			flag = 2;
	}
}

int command_type()
{
	int i = 0;
	for( ; i < MAX_COM ; i++)
	{
		if(command[i] == '|' )
			return 2;
		else if(command[i] == '&' )
			return 3;
		else if(command[i] == '<' )
		{
			int j = i+1;
			for( ; j < MAX_COM ; j++)
			{
				if(command[j] == '>')
					return 6;
			}
			return 4;
		}
		else if(command[i] == '>' )
		{
			int j = i+1;	
			for( ; j < MAX_COM ; j++)
			{
				if(command[j] == '<')
					return 7;
			}
			return 5;
		}
	}
	return 1;
}


void process_one( char *envp[] )
{
	int for_wait = 0;
	if( fork() != 0)
    {
   		/* parent process */
    	wait(&for_wait);
    }
    else
    {
     	/* chile process */
    	execvp(command,parameters);
		exit(0);
    }
}

void process_pipe( char *envp[])
{
	pid_t pid;
	
	if( ( pid = fork() ) < 0 )
	{
		printf("fork error\n");
	}
	else if( pid == 0 )
	{
		int fd[2];
		pipe(fd);
		if( ( pid = fork() ) == 0 )
		{
			/* in grand child */	
			close( fd[0] );
			if( dup2( fd[1] , STD_OUTPUT ) < 0 )
				printf("dup error in child\n");
			close( fd[1] );
			execvp( process[0] , para[0]);
			printf("Command Error !!\n");
	        exit(0);
		}	
		else if( pid > 0 )
		{
			/* in child */
			close( fd[1] );
			if( dup2( fd[0] , STD_INPUT ) < 0 )
	            printf("dup error in grand child\n");
	        close( fd[0] );
	        execvp( process[1] , para[1]);
	        printf("exec Error!\n");
			printf("Command Error !!\n");
	        exit(0);
		}
		else
		{
			printf("fork error\n");
		}
	}
	else
	{
		int status;
		wait(&status);
	}
}

void process_back()
{
	//printf("%d\n%s\n%s\n",type,command,parameters[1]);
	pid_t pid;
	pid = fork();

	if( pid == 0 )
	{
		setsid();
		signal( SIGHUP , SIG_IGN );
		execvp(command,parameters);
		printf("Command Error !!\n");
        exit(0);
	}
	else
	{
		printf("[1] %d\n",pid);
	}
}

void process_redir()
{
//	printf("%s\n%s\n",redirCommand,parameters[1]);
	pid_t pid;
	pid = fork();
	int fd[2];

	if( pid < 0 )
	{
		printf("fork Error ! \n");
		exit(0);
	}
	else if( pid == 0)
	{
		int fd_in,fd_out;
		if( type == 4 || type == 6 || type == 7 )
		{
			fd_in = open( fileIn , O_RDONLY);
			dup2( fd_in , STD_INPUT);
			close(fd_in);
		}
		
		if( type == 5 || type == 6 || type == 7  )
		{
			fd_out = open( fileOut , O_WRONLY | O_CREAT , 0664 );
        	dup2( fd_out , STD_OUTPUT );
			close( fd_out );
		}

		execvp(redirCommand,parameters);
	}
	else
	{
		waitpid(pid,NULL,0);
	}

}

void process_cd()
{
	chdir(parameters[1]);
}

void help( char *envp[] )
{
	char *path;
	path = strtok(envp[7],"=:");
	path = strtok(NULL,"=:");

	printf("\nYou can use programs under these folders : \n");	
	while( path )
	{
		printf("%s\n" , path);
		path = strtok(NULL,"=:");
	}
	printf("\n");
}
