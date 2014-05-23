#include "mm.h"
#include<string.h>
static Header *morecore( unsigned nu ){
    char *cp , *sbrk(int);
    Header *up;

    if( nu < NALLOC )
    // minimun  units to request
        nu = NALLOC;

    cp = sbrk( nu * sizeof(Header) );
    // no space at all
    if( cp == (char *) -1)
        return NULL;

    up = (Header *)cp;
    up->s.size = nu;
    myfree(( void * )( up + 1 ));
    return freep;
}

void *mymalloc(size_t size){
	Header *p , *prevp;
    Header *morecore( unsigned );
    unsigned nunits;

    nunits = ( size + sizeof(Header)-1) / sizeof(Header) +1 ;

    // no freelist yet
    if( (prevp = freep) == NULL ){
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

	for( p = prevp->s.ptr ;; prevp = p , p = p->s.ptr ){
        // big enough
		if( p->s.size >= nunits ){
			// exactly
            if( p->s.size == nunits )
                prevp->s.ptr = p->s.ptr;
            // too big
            else{
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p+1);
        }
		if( p == freep )
    	        if( ( p = morecore(nunits)) == NULL )
    	            return NULL;
    }
	

}

void myfree(void *ptr){
    Header  *bp,*p;

    bp = ( Header * )ptr - 1;
    for( p = freep ; !( bp > p && bp < p->s.ptr ); p = p->s.ptr )
        if( p >= p->s.ptr && ( bp > p || bp < p->s.ptr ))
            break;
    // join to list front & make the first of free list be a part of bp and combine them
    if( bp + bp->s.size == p->s.ptr ){
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
    // can't combine , so let bp next point to the first of freelist
        bp->s.ptr = p->s.ptr;

    // join to list back % make bp be a part of the free list back and combine them
    if( p + p->s.size == bp ){
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else
    // can't combine , so let last block point to bp
        p->s.ptr = bp;

    // new first block of free list
    freep = p;

}

void *myrealloc(void *ptr, size_t size){ 
	myfree(ptr);
	ptr = mymalloc( size );
	return ptr;
}
void *mycalloc(size_t nmemb, size_t size){
	void * block = mymalloc( nmemb * size );
	memset( block , 0 , size);
	return block;
}

