#include<stdio.h>
#include<stdlib.h>

int main()
{
	char x[2000];
	fgets(x,2000,stdin);
	printf("%s\n",x);
}
