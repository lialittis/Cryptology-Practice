#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "gmp.h"
#include "utils.h"
#include "refine.h"
#include "pminus1.h"

#define DEBUG 0

static void ReadDifFile(mpz_t p, FILE *file){
    mpz_t q;
    int dp;

    /* we have to read file while read_p <= p */
    mpz_init_set_ui(q, 1);
    while(fscanf(file, "%d", &dp) != EOF){
	mpz_add_ui(q, q, dp << 1);
	if(mpz_cmp(q, p) > 0)
	    break;
    }
    mpz_set(p, q);
    mpz_clear(q);
}

/* Starting from nextprime(p) >= p+1. 
   On unsuccessful exit, p is the smallest prime > bound1.
*/
int PollardPminus1Step1(mpz_t f, const mpz_t N,	long bound1, FILE* file,
			mpz_t b, mpz_t p){
    int status = FACTOR_NOT_FOUND;
    char half_diff[10];
	while(mpz_cmp_ui(p,bound1)<=0){
	    mpz_t q,qp1;
		mpz_init(q);
		mpz_init(qp1);
		long k = 0;
		//mpz_pow_ui(q,p,k);
		//mpz_pow_ui(qp1,p,k+1);
		//printf("bound1 is %ld\n",bound1);
		for(k = 0;mpz_cmp_ui(q,bound1)>0 || mpz_cmp_ui(qp1,bound1)<=0;k++){
		    mpz_pow_ui(q,p,k);
			mpz_pow_ui(qp1,p,k+1);
		}
		//printf("p is %ld,q is %ld\n", mpz_get_ui(p),mpz_get_ui(q));
		mpz_powm(b,b,q,N);
		mpz_sub_ui(b,b,1);
		mpz_gcd(f,b,N);
		if(mpz_get_ui(f)>1){
		    status = FACTOR_FOUND;
			break;
		}
		mpz_add_ui(b,b,1);
		mpz_clear(q);
		mpz_clear(qp1);
		fgets(half_diff,10,file);
		if(mpz_get_ui(p)==2){
		    mpz_set_ui(p,1);
		}
		int hd = atoi(half_diff);
		mpz_add_ui(p,p,2*hd);
	}
	
	return status;
}

int PollardPminus1Step2(mpz_t f, const mpz_t N, long bound2, FILE* file,
			mpz_t b, mpz_t p){
    mpz_t bm1;
    unsigned long d;
    int dp, status = FACTOR_NOT_FOUND;
    int B = (int)log((double)bound2);
    B = B * B;

    mpz_init(bm1);
    ReadDifFile(p, file);
    /* Precomputations */
    mpz_t* precomputations = (mpz_t*)malloc(B * sizeof(mpz_t));
    mpz_t* cursor = precomputations;
		
    for(int i = 0; i < B; i++, cursor++){
	mpz_init(*cursor);
	mpz_powm_ui(*cursor, b, i, N);
    }
#if DEBUG >= 1
    printf("# Precomputation of phase 2 done.\n");
#endif
    mpz_powm(b, b, p, N);
    while(mpz_cmp_ui(p, bound2) <= 0){
	mpz_sub_ui(bm1, b, 1);
	mpz_gcd(f, bm1, N);
	if(mpz_cmp_ui(f, 1) > 0){
	    status = FACTOR_FOUND;
	    break;
	}
	fscanf(file, "%d", &dp);
	d = dp << 1;
	mpz_add_ui(p, p, d);		
	if(d < B){
	    mpz_mul(b, b, precomputations[d]);
	    mpz_mod(b, b, N);
	}
	else{
	    printf("Cramer's rule Failed!\n");
	    printf("WRITE A PAPER!!!\n");
	    return 1;
	    //mpz_powm(b, b, precomputations[d], cof);
	}
    }			
    cursor = precomputations;
    for(int i = 0; i < B; i++, cursor++){
	mpz_clear(*cursor);
    }
    free(precomputations);
    mpz_clear(bm1);
    return status;
}

int PollardPminus1(factor_t* result, int *nf, mpz_t cof, const mpz_t N,
		   long bound1, long bound2, FILE* file){
    mpz_t b, p, factor;
    mpz_inits(b, p, factor, NULL);
    mpz_set_ui(b, 2);
    mpz_set_ui(p, 2);
    int status = PollardPminus1Step1(factor, N, bound1, file, b, p);
    if(status != FACTOR_FOUND)
	status = PollardPminus1Step2(factor, N, bound2, file, b, p);
    if(status == FACTOR_FOUND){
	AddFactor(result + *nf, factor, 1, FACTOR_IS_UNKNOWN);
	(*nf)++;
    }
    mpz_clears(b, p, factor, NULL);
    return status;
}
