#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gmp.h"
#include "buffer.h"
#include "sha3.h"

#include "rsa.h"
#include "sign.h"
#include "dsa.h"
#include "attack_dsa.h"

#define DEBUG 1

void dsa_sign_dummy(buffer_t *msg, mpz_t p,
				  mpz_t q, mpz_t a, mpz_t x, mpz_t r, mpz_t s,
					mpz_t k){
						//initialize
                 buffer_t hash;
                 buffer_init(&hash,100);
                 mpz_t hashm,r1,invert_k,xr;
                 mpz_inits(hashm,r1,invert_k,xr,NULL);
                 
                 //create hash(m)
                 buffer_hash(&hash,hash_length(q),msg);
                 mpz_import(hashm,hash_length(q),1,1,1,0,hash.tab);


                 //create r
                 
                 mpz_powm_sec(r1,a,k,p);
                 mpz_mod(r,r1,q);

                 //create s   
                 mpz_invert(invert_k,k,q);
                 mpz_mul(xr,x,r);
                 mpz_add(xr,hashm,xr);
                 mpz_mul(xr,xr,invert_k);
                 mpz_mod(s,xr,q);

                 //clear
                 mpz_clears(hashm,r1,invert_k,xr,NULL);
                 buffer_clear(&hash);
/* to be filled in */
}

void solve_system_modq(mpz_t x, mpz_t r1, mpz_t s1,
					   mpz_t r2, mpz_t s2, mpz_t h1, mpz_t h2,
					   mpz_t q){
						   mpz_t a1,a2,b1,b2,c;
						   printf("---flag---\n");
						   mpz_inits(a1,a2,b1,b2,c,NULL);
						   printf("---flag---\n");
						   mpz_mul(a1,s2,r1);
						   printf("---flag---\n");
						   mpz_mul(a2,s1,r2);
						   printf("---flag---\n");
						   mpz_sub(a1,a1,a2);
						   printf("---flag---\n");
						   // printf("b1, b2:");
						   gmp_printf("b1, b2: %Zd,%Zd\n",a1,a2);
						   gmp_printf("b1, b2: %Zd,%Zd\n",s1,h2);
						   mpz_mul(b1,s1,h2);
						   printf("---flag---\n");
						   gmp_printf("b1, b2: %Zd\n\n",b1);
						   gmp_printf("h2, s1: %Zd\n\n",h2);
						   mpz_mul(b2,s2,h1);
						   printf("---flag---\n");
						   gmp_printf("b1, b2: %Zd,%Zd\n\n",b1,b2);
						   mpz_sub(b1,b1,b2);
						   gmp_printf("b1, b2: %Zd,%Zd\n\n",b1,b2);
						   printf("---flag---\n");
						   mpz_invert(a2,a1,q);
						   printf("---flag---\n");
						   mpz_mul(b2,b1,a2);
						   printf("---flag---\n");
						   mpz_mod(x,b2,q);
						   printf("---flag---\n");
						   
						   mpz_clears(a1,a2,b1,b2,c,NULL);

	/* Solves the system with unkowns k, x:
	   s1.k - r1.x = h1
	   s2.k - r1.x = h2
	   and fills in x
	 */
/* to be filled in */
}


void dsa_attack(mpz_t x, buffer_t *msg1, buffer_t *msg2,
			    mpz_t p, mpz_t q, mpz_t a, mpz_t r1,
				mpz_t s1, mpz_t r2, mpz_t s2){
					//initialize
                 buffer_t hash1,hash2;
                 buffer_init(&hash1,100);
				 buffer_init(&hash2,100);
                 mpz_t hashm1,hashm2;
                 mpz_inits(hashm1,hashm2,NULL);
                 
                 //create hash(m)
                 buffer_hash(&hash1,hash_length(q),msg1);
                 mpz_import(hashm1,hash_length(q),1,1,1,0,hash1.tab);
                 buffer_hash(&hash2,hash_length(q),msg2);
                 mpz_import(hashm2,hash_length(q),1,1,1,0,hash2.tab);

                 //attack
				 solve_system_modq(x,r1,s1,r2,s2,hashm1,hashm2,q);

                 //clear
				 buffer_clear(&hash1);
				 buffer_clear(&hash2);
				 mpz_clears(hashm1,hashm2,NULL);
/* to be filled in */
}
