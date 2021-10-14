#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "gmp.h"
#include "utils.h"
#include "refine.h"
#include "rho.h"


int PollardRhoSteps(mpz_t f, const mpz_t N,
		    void (*fct)(mpz_t, mpz_t, const mpz_t), long *iter,
		    long nrOfIterations, mpz_t x, mpz_t y){
    int status = FACTOR_NOT_FOUND;
    while(status == 0 && *iter<nrOfIterations){
	    fct(x,x,N);
		fct(y,y,N);
		fct(y,y,N);
		
		mpz_t sub;
        mpz_init(sub);
        mpz_sub(sub,x,y);
        mpz_gcd(f,sub,N);
		if(mpz_get_ui(sub)==0){
		    break;
		}
        //printf("sub = %ld,gcd = %ld,N=%ld\n",mpz_get_ui(sub),mpz_get_ui(f),N);  
        //printf("f is %ld\n",mpz_get_ui(f));
		if(mpz_get_ui(f)!= 1){
            status = FACTOR_FOUND;
        }
        mpz_clear(sub);
        (*iter)++;
	}
    return status;
}



int PollardRho_with_long(long *f, const long N, long nrOfIterations, long x, long y){
    int status = 0;
	/*
	 In the comment, it's another method of mpz;
	*/
	
	/*mpz_t X;
	mpz_t Y;

	mpz_inits(X,Y,NULL);
	mpz_set_ui(X,x);
	mpz_set_ui(Y,y);
	*/
	long count = 0;
	while(status == 0 && count <= nrOfIterations){
		/*mpz_pow_ui(X,X,2);
		mpz_add_ui(X,X,7);
		mpz_mod_ui(X,X,N);
		mpz_pow_ui(Y,Y,2);
		mpz_add_ui(Y,Y,7);
		mpz_pow_ui(Y,Y,2);
		mpz_add_ui(Y,Y,7);
		mpz_mod_ui(Y,Y,N);
		*/
		long fx = pow(x,2) + 7;
		x = fx%N;
		long fy = pow(y,2) + 7;
		long ffy = pow(fy,2) + 7;
		y = ffy%N;
		
		//printf("x=%ld,y=%ld\n",mpz_get_ui(X),mpz_get_ui(Y));
	    /*
		mpz_t sub,gcd,N_mpz;
		mpz_inits(sub,gcd,N_mpz,NULL);
		mpz_sub(sub,X,Y);
		mpz_set_ui(N_mpz,N);
		mpz_gcd(gcd,sub,N_mpz);
	    */

		long sub = abs(x - y);
		
		//printf("sub = %ld,gcd = %ld,N=%ld\n",mpz_get_ui(sub),mpz_get_ui(gcd),N);	
        //long N_changable = N;
	    
		gcd(f,sub,N);
		//*f = result;
		if(*f != 1){
		    status = FACTOR_FOUND;
		}
		/*
		mpz_clears(sub,gcd,N_mpz,NULL);
		*/
		count ++;
	}
	/*
	mpz_clears(X,Y,NULL);
	*/
	printf("factors of %ld : %ld, %ld\n",N,*f,N/(*f));
    return status;
 
}

void gcd(long *f,long a, long b){
    //*f = (b>0)?gcd(*f,b,a%b):a;
	
	if(b == 0 ) {
		*f = a;
	}
	else{
		gcd(f,b,a%b);
	}
}


int PollardRho(factor_t* result, int *nf, mpz_t cof, const mpz_t N,
	       void (*fct)(mpz_t, mpz_t, const mpz_t),
	       long nrOfIterations){
    int status = FACTOR_NOT_FOUND;
    mpz_t x, y, fact;
    long iter = 0;
    mpz_inits(x, y, fact, NULL);
    mpz_set_ui(x, 1);	
    mpz_set_ui(y, 1);
    status = PollardRhoSteps(fact, N, fct, &iter, nrOfIterations, x, y);
    if(status == FACTOR_FOUND){
	AddFactor(result, fact, 1, FACTOR_IS_UNKNOWN);
	(*nf)++;
	mpz_divexact(cof, N, fact);
    }
    mpz_clears(x, y, fact,NULL);
    return status;
}

