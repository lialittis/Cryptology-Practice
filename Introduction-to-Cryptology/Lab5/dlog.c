/****************************************************************/
/* dlog.c                                                       */
/* Author : Alain Couvreur                                      */
/* alain.couvreur@lix.polytechnique.fr                          */
/* Last modification October 26, 2017                           */
/****************************************************************/

#include <stdio.h>
#include "hash.h"
#include "dlog.h"
#include "gmp.h"


void babySteps(hash_table H, mpz_t nBabySteps, mpz_t base, mpz_t p){
    mpz_t key,value;
	mpz_inits(key,value,NULL);
	int addr;
	for(long i =0; i<mpz_get_ui(nBabySteps);i++){
	    mpz_set_ui(value,i);
		mpz_powm(key,base,value,p);
		
		hash_put_mpz(H,&addr,key,value,base,p);
	}
	mpz_clears(key,value,NULL);
}


void giantSteps(mpz_t vz, hash_table H, mpz_t nBabySteps, mpz_t base,
		mpz_t p, mpz_t kz){
    
    mpz_t inter,max_i;
	mpz_inits(inter,max_i,NULL);

	mpz_cdiv_q(max_i,p,nBabySteps);
	for(long i = 0; i<mpz_get_ui(max_i);i++){
	    mpz_mul_ui(inter,nBabySteps,mpz_get_ui(p)-2);
		mpz_mul_ui(inter,inter,i);
		mpz_powm(inter,base,inter,p);
		mpz_mul(inter,inter,kz);
		mpz_cdiv_r(inter,inter,p);
		mpz_add(inter,inter,p);
		if(hash_get_mpz(vz,H,inter,base,p)==HASH_FOUND){
		    mpz_set_ui(inter,i);
			break;
		}
	}
	mpz_addmul(vz,inter,nBabySteps);
	mpz_clears(inter,max_i,NULL);
}
