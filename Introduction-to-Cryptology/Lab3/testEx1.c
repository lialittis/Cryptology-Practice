/*********************************************************************
TestEx1.c
Author : Francois Morain, morain@lix.polytechnique.fr
Last modification October 9, 2018

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "random.h"
#include "hashtable.h"
#include "easyhash.h"



void find_collisions(int imax){
    hash_table H;
    buffer_t buf; // The current buffer you work on
	buffer_t *tab; // A table to store buffers you treated
    unsigned long h;  
    int i, j;

	H = hash_init(imax);
    buffer_init(&buf, 4);
    
	//buffer_reset(tab);
/*		
    for(j = 0; j < imax; j++,tab++){
	    buffer_init(tab,4);
	}
	tab = tab -imax;
  */  	
    tab = (buffer_t*)malloc(imax*sizeof(buffer_t));

	hash_pair pair;
    hash_pair pair_found;

	for(i = 0; i < imax; i++){
		buffer_random(&buf,buf.size);
        h = easy_hash(&buf);
		pair.k = h;
		pair.v = i;
		int sign = hash_get(&pair_found,H,pair.k);
		if(sign == 2){
		    // Generate a hash table - H
			hash_put(H,pair.k,pair.v);
		}else{
		    buffer_print_int(stdout,&buf);
		}
		// Generate a table of buffers tab to store the buffers
        //buffer_append(tab,&buf);
		tab[i] = buf;
	}
	
	free(tab);
    //buffer_clear(tab);
	buffer_clear(&buf);
    hash_clear(H);
}


int main(int argc, char *argv[]){
    if(argc == 1)
		fprintf(stderr, "Usage: %s <imax>\n", argv[0]);
	srand(random_seed());
	find_collisions(atoi(argv[1]));
    return 0;
}
