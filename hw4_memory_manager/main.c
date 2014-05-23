#include"mm.c"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(){
	int *i = mymalloc( 4 * sizeof(int) );
	myfree(i);
	int *j = mycalloc( 8 , sizeof(int) );
	myrealloc( j , 16 );
	myfree(j);
	int *k = mymalloc( 16 * sizeof(int) );
	k = myrealloc( k , 4  );
	myfree(k);
	return 0;
}
