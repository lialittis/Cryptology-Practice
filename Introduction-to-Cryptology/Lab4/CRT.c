#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gmp.h"

#include "xgcd.h"
#include "CRT.h"

/* Given (r0, m0) and (r1, m1), compute n such that
   n mod m0 = r0; n mod m1 = r1.  If no such n exists, then this
   function returns 0. Else returns 1.  The moduli m must all be positive.
*/
int CRT2(mpz_t n, mpz_t r0, mpz_t m0, mpz_t r1, mpz_t m1){
    int status = 1;
    
	// firstly, we need to check if m0 and m1 are prime together

	mpz_t g,u,v;
	mpz_inits(g,u,v,NULL);
	XGCD(g,u,v,m0,m1);
    int gcd = mpz_get_ui(g);
	if(gcd!=1){
	    // m0 and m1 are not prime together
        
		// to prove the existence of solution, use gcd(m0,m1) | abs(r0 - r1)
        int dif = mpz_get_ui(r0)-mpz_get_ui(r1);
        //printf("---------dir:%d------gcd:%d\n",dif,gcd);
		//printf("res:%d\n",abs(dif) % gcd);
		if(abs(dif) % gcd !=0){
		    return 0;
		}
		else{

            mpz_t lcm;
			mpz_init(lcm);
			if(mpz_cmp(m0,m1)>0){
			    mpz_cdiv_q(lcm,m0,g);
				mpz_mul(lcm,lcm,m1);
			}else{
			    mpz_cdiv_q(lcm,m1,g);
				mpz_mul(lcm,lcm,m0);
			}

            // There should be one method to change for the n
            // But I didn't get it.

			mpz_clear(lcm);
		    return status;
		}



	}

	
	// then we take m0 and m1 are primtives for each other
    // we also need to prove if there is one solution for this question
	// we need to consider (n1= m0*x0 = r1 mod m1) and (n2 = m1*x1 = r0 mod m0)

	mpz_t n1,n2;
	mpz_inits(n1,n2,NULL);


	mpz_t x0,x1;
	mpz_inits(x0,x1,NULL);
    
	int findx0,findx1;
	findx0 = linear_equation_mod(x0, m0, r1, m1);
	findx1 = linear_equation_mod(x1, m1, r0, m0);
    if(findx0==0 || findx1==0){
	    
		return 0;
	}

	mpz_mul_ui(n1,m0,findx0);
	mpz_mul_ui(n2,m1,findx1);
	mpz_add(n,n1,n2);
    
	mpz_clears(n1,n2,x0,x1,NULL);

	mpz_clears(g,u,v,NULL);
	return status;
}


/* Given a list S of pairs (r,m), returns an integer n such that n mod
   m = r for each (r,m) in S.  If no such n exists, then this function
   returns 0. Else returns 1.  The moduli m must all be positive.
*/
int CRT(mpz_t n, mpz_t *r, mpz_t *m, int nb_pairs){
    int status = 1;
    //mpz_t *tab = (mpz_t *)malloc(nb_pairs*sizeof(mpz_t));
	
	mpz_t r_interate,m_interate;
	mpz_inits(r_interate,m_interate,NULL);

	mpz_set(r_interate,r[0]);
	mpz_set(m_interate,m[0]);

	for(int i = 0; i<nb_pairs-1;i++){
	    if(CRT2(n,r_interate,m_interate,r[i+1],m[i+1])==0){
			return 0;
		}
	
		mpz_mul(m_interate,m_interate,m[i+1]);
        mpz_mod(r_interate,n,m_interate);
		//mpz_set(r_interate,n);
	}
	
	// To get the minimum positive integer, do such calculation 
    mpz_set(n,r_interate);
    
	mpz_clears(r_interate,m_interate,NULL);
    return status;
}
