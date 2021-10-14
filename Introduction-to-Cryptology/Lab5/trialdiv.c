#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "gmp.h"
#include "utils.h"
#include "trialdiv.h"

/* OUTPUT: 1 if factorization finished. */
int trialDivision(factor_t* factors, int *nf, mpz_t cof, const mpz_t N,
		  const long bound, uint length, FILE* file){
    int status = FACTOR_NOT_FOUND;
	char half_diff[10];
	int e = 0;
	int index = 0;
	mpz_set(cof,N);
	//mpz_t N_changable;
	//mpz_init(N_changable);
	//mpz_set(N_changable,N);
    //printf("bound is %ld,%ld\n",bound,b);
	int hd = 0;
	mpz_t sqrt_N;
	mpz_init(sqrt_N);
	mpz_sqrt(sqrt_N,N);
    for(long i = 2; i <= bound && i<=mpz_get_ui(sqrt_N);i += 2*hd){
		fgets(half_diff,10,file); // 10 is to make sure that we can get all the value of that line
		//printf("the half_diff is %d\n",atoi(half_diff));
	    hd = atoi(half_diff);	
		//printf("possible factor is %ld\n",i);
		int flag = 0;
		//long Num = mpz_get_ui(cof);
		while(mpz_divisible_ui_p(cof,i) !=0){
			//printf("N is %d\n",mpz_get_ui(N_changable));
		    e ++;
			mpz_cdiv_q_ui(cof,cof,i);
			flag = 1;
			//printf("i is %ld\n",i);
			//long Num = mpz_get_ui(cof);
			//printf("Num is %ld\n",Num);
		}
		if(flag == 1){
			//printf("success for %ld factor\n",i);
			//printf("the exponent is %d\n",e);
			mpz_t f;
			mpz_init(f);
			mpz_set_ui(f,i);
            status = FACTOR_FOUND;
			AddFactor(factors+index,f,e,status);
			//printf("add success!\n");
			index ++;
			mpz_clear(f);
		}
        
		e = 0; // reinitialize e
		if(i==2){
		    i = 1; // To avoid the error of the first half-difference
		}
		//printf("i is %ld\n",i);
		//printf("bound is %ld\n",bound);
	}

	*nf=index;
    return status;
}
