#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"



void fermat(int pmin, int pmax){
    mpz_t x;
	mpz_t base;
	mpz_t rop;
	
	mpz_init(x);
	mpz_init(base);
	mpz_init(rop);

	mpz_set_ui(base,2U);
	
	for(mpz_set_ui(x,pmin);mpz_cmp_ui(x,pmax);mpz_set_ui(x,mpz_get_ui(x)+1)){
		//mpz_set_ui(x,i);
	    mpz_powm_ui(rop,base,mpz_get_ui(x)-1,x);
        //printf("%ld\n",mpz_get_ui(rop));
		
		if((mpz_get_ui(rop)==1U)&&(mpz_probab_prime_p(x,25)==0)){
		    printf("%ld\n",mpz_get_ui(x));
		}
	}

	mpz_clear(x);
	mpz_clear(base);
	mpz_clear(rop);
    
}


void Usage(char *cmd){
    fprintf(stderr, "Usage: %s pmin pmax\n", cmd);
}


int main (int argc, char *argv[]){
    if(argc < 3){
		Usage(argv[0]);
		return 0;
    }
	fermat(atoi(argv[1]), atoi(argv[2]));
    return 0;
}
