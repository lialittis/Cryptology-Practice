/**************************************************************/
/* bits.c                                                     */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification September 30, 2020 (FJM)                 */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "bits.h"

void printDec(uchar* u, int length){
    printf("[");
    if(length == 0){
	printf("]");
	return;
    }
    for(int i  = 0; i < length - 1; i++){
	printf("%d, ", *(u++));
    }
    printf("%d]", *u);
}


void printHexa(uchar* u, int length){
    printf("[");
    if(length == 0){
	printf("]");
	return;
    }
    for(int i  = 0; i < length - 1; i++){
	if(*u < 0x10)
	    printf("0x0%x, ", *(u++));
	else
	    printf("0x%x, ", *(u++));
    }
    if(*u < 0x10)
	printf("0x0%x]", *(u++));
    else
	printf("0x%x]", *(u++));
}


void printBin(uchar* u, int length){
    printf("[ ");
    if(length == 0){
	printf("]");
	return;
    }
    int i, j;
    for(i  = 0; i < length - 1; i++){
	for(j = BYTE_SIZE - 1; j >= 0; j--){
	    printf("%d ", (*u >> j) & 1);
	}
	printf("| ");
	u++;
    }
    for(j = BYTE_SIZE - 1; j >= 0 ; j--){
	printf("%d ", (*u >> j) & 1);
    }
    printf("]");
}



uchar getBit(uchar t, int position){
	return  ((t&(1<<position))>>position);
}



uchar setBit(uchar t, int position, uchar value){
	if(value==(uchar)0){
	    return (t&=~(1<<position));
	}
	else if(value==(uchar)1){
	    return (t|=(1<<position));
	}
	else{
	    return t;
	}
}


int HammingWeightByte(uchar c){
	int count = 0;
	
	while(c!=(uchar)0){
	  count += c & 1;
	  c >>= 1;
	}
	return count;
}


int HammingWeight(buffer_t *buf){
    int count = 0;
    for(int i =0; i<buf->size;i++){
	    uchar c =*(buf->tab);
		count += HammingWeightByte(c);
		buf->tab ++;
	}
	buf->tab -= buf->size;
	return count;
}



void oneTimePad(buffer_t *encrypted, buffer_t *msg, buffer_t *key) {
    buffer_reset(encrypted);
	buffer_append(encrypted,msg);
	uchar *state = encrypted->tab;
    for(int i=0;i<msg->length;i++,state++){
	    *state = *state ^ (key->tab[i]);
	}	
	return;
    
}

