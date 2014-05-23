#include<stdio.h>
#include<stdlib.h>
#ifndef __MY_MM_H_INCLUDED__
#define __MY_MM_H_INCLUDED__
#include <stddef.h>
#define NALLOC 1024
typedef long Align;
union header {
    struct {
        union header *ptr;
        unsigned size;
    }s;
    Align x;
};

typedef union header Header;
static Header base;
static Header *freep = NULL;

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void *mycalloc(size_t nmemb, size_t size);

#endif
