#include <stdio.h>
#include <assert.h>

#include "gmp.h"

#include "xgcd.h"

#define DEBUG 0



/* compute g, u and v s.t. a*u+b*v = g = gcd(a, b) */
void XGCD(mpz_t g, mpz_t u, mpz_t v, mpz_t a, mpz_t b){
    
	if(mpz_get_ui(b)==0){
	    mpz_set_ui(g,101);
		mpz_set_ui(u,1);
		mpz_set_ui(v,0);
		return ;
	}
	// define and initialize the remains, the quotient
	mpz_t r1,r2,r3;
	mpz_t q;
	// define and initialize the u and v
	mpz_t u1,u2,v1,v2;
	mpz_init(r1);mpz_init(r2);mpz_init(r3);
	mpz_init(q);
	mpz_init(u1);mpz_init(u2);
	mpz_init(v1);mpz_init(v2);

	// define two temporary values
	mpz_t temp1,temp2;
    mpz_init(temp1);
    mpz_init(temp2);

	// set the first round of euclid's algorithm
	mpz_set(r1,a);
	mpz_set(r2,b);
     
	// calculate the quotient and remain
    mpz_cdiv_qr(q,r3,r1,r2);
    
	// initial values of u and v
	mpz_set_si(u1,1);mpz_set_si(u2,0);
	mpz_set_si(v1,0);mpz_set_si(v2,1);
    
	while(mpz_get_si(r3)!=0){
		
		// tuning for the quotient and remain value
		// because of the definition of the output of mpz_cdiv_qr function
		mpz_sub_ui(q,q,1);
		mpz_add(r3,r2,r3);
		// extended euclidean algorithm
		// calculate the u by u1 and u2, the v by v1 and v2
        
		// firstly, get the multiple
		mpz_mul(temp1,q,u2);
		mpz_mul(temp2,q,v2);
		// secondly, do the subtraction
		mpz_sub(u,u1,temp1);
        mpz_sub(v,v1,temp2);
	    
		// update the remains
		mpz_set(r1,r2);
		mpz_set(r2,r3);
        
		// calculate the q and r
		mpz_cdiv_qr(q,r3,r1,r2);
		// printf("q:%d,r3:%d\n",mpz_get_ui(q),mpz_get_ui(r3));
		// update u and v
		mpz_set(u1,u2);
		mpz_set(u2,u);
		mpz_set(v1,v2);
		mpz_set(v2,v);
	}

	//printf("q:%d,r3:%d\n",mpz_get_ui(q),mpz_get_ui(r3));

	mpz_set(g,r2);
	mpz_mul(temp1,q,u2);mpz_mul(temp2,q,v2);
	mpz_sub(u,u1,temp1);
    mpz_sub(v,v1,temp2);
	//printf("%d,%d,%d",mpz_get_ui(g),mpz_get_ui(u),mpz_get_ui(v));

	mpz_clears(r1,r2,r3,q,temp1,temp2,u1,u2,v1,v2,NULL);
    
}


/* Solve a*x=b mod m if possible. 
   
*/
int linear_equation_mod(mpz_t x, mpz_t a, mpz_t b, mpz_t m){
    int status = 1;
	
    // To prove if there is a solution for a*x = b mod m
	// we need to calculate the gcd(a,m)
    mpz_t g,u,v;
	mpz_inits(g,u,v,NULL);
    XGCD(g,u,v,a,m);
	
	// define and initialize remain and multiple
	mpz_t r,mul;
	mpz_init(r);
	mpz_init(mul);

	mpz_mod(r,b,g);
	//printf("g is:%d,b is %d\n",mpz_get_ui(g),mpz_get_ui(b));
	if(mpz_get_ui(r)!=0){
	    return 0;
	}
    
    mpz_set_ui(x,status);
    
	mpz_mul(mul,a,x);
	mpz_mod(r,mul,m);

	while(mpz_cmp(b,r)!=0){
	    status += 1;
        mpz_set_ui(x,status);
		mpz_mul(mul,a,x);
		mpz_mod(r,mul,m);
	}

	mpz_clears(r,mul,NULL);
    mpz_clears(g,u,v,NULL);
    return status;
}
 
