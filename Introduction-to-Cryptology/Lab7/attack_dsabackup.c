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
	/* My code*/
    mpz_t msg_c;
    mpz_init(msg_c);
    // p,q,a,x,k - known
	int size = hash_length(q);
	
	mpz_t tempk;
	mpz_init(tempk);
	// generate r
    mpz_powm(r,a,k,p);
    mpz_mod(r,r,q);

    // declare hash
    buffer_t hash;
    buffer_init(&hash, size);
    // generate hash
    buffer_hash(&hash, size, msg);
    mpz_import(msg_c,size,1,1,1,0,hash.tab);
	gmp_printf("msg is %Zd\n",msg_c);
    // generate s
    mpz_mul(s,x,r);
    mpz_add(s,s,msg_c);
	mpz_mod(s,s,q);
	gmp_printf("s is %Zd\n",s);


    // mpz_cdiv_q(s,s,k);
    mpz_invert(tempk,k,q);
    mpz_mul(s,s,tempk);
	gmp_printf("s is %Zd\n",s);

	mpz_mod(s,s,q);
	gmp_printf("s is %Zd\n",s);
    // mpz_clear(hash_c);
    mpz_clear(msg_c);
	mpz_clear(tempk);
}

void solve_system_modq(mpz_t x, mpz_t r1, mpz_t s1,
					   mpz_t r2, mpz_t s2, mpz_t h1, mpz_t h2,
					   mpz_t q){

	/* Solves the system with unkowns k, x:
	   s1.k - r1.x = h1
	   s2.k - r1.x = h2
	   and fills in x
	 */

	
	mpz_t temph1,temph2,tempr1,tempr2,h,r;
	mpz_inits(temph1,temph2,tempr1,tempr2,h,r,NULL);

	// gmp_printf("h1 is %Zd\n",h1);
	// gmp_printf("h2 is %Zd\n",h2);
	// gmp_printf("s1 is %Zd\n",s1);
	// gmp_printf("s2 is %Zd\n",s2);
	mpz_mul(temph1,s2,h1);
	// gmp_printf("h1 is %Zd\n",h1);
	mpz_mul(temph2,s1,h2);
	mpz_sub(h,temph2,temph1);
	// gmp_printf("h is %Zd\n",h);
	mpz_mod(h,h,q);
	gmp_printf("h is %Zd\n",h);
	mpz_mul(tempr1,r1,s2);
	mpz_mul(tempr2,r2,s1);
	mpz_sub(r,tempr1,tempr2);
	
	gmp_printf("r is %Zd\n",r);

	mpz_invert(r,r,q);
	mpz_mul(r,r,h);
	mpz_mod(x,r,q);
	mpz_clears(temph1,temph2,tempr1,tempr2,h,r,NULL);
	// gmp_printf("x is %Zd\n\n",x);
}


void dsa_attack(mpz_t x, buffer_t *msg1, buffer_t *msg2,
			    mpz_t p, mpz_t q, mpz_t a, mpz_t r1,
				mpz_t s1, mpz_t r2, mpz_t s2){
	/*My code*/
	// declare hash
    int size = hash_length(q);
	buffer_t hash1,hash2;
	buffer_init(&hash1,size);
	buffer_init(&hash2,size);
	// printf("---flag1---\n");

	// generate hash
	buffer_hash(&hash1, size, msg1);
	// printf("---flag1---\n");
	buffer_hash(&hash2, size, msg2);
	// printf("---flag1---\n");

	// import in mpz_t
	mpz_t msg_c1,msg_c2;
	mpz_inits(msg_c1,msg_c2,NULL);
	// printf("---flag1---\n");
	mpz_import(msg_c1,size,1,1,1,0,hash1.tab);
	mpz_import(msg_c2,size,1,1,1,0,hash2.tab);
	// printf("---flag1---\n");
	solve_system_modq(x, r1, s1, r2, s2, msg_c1, msg_c2, q);
	// printf("---flag1---\n");

	buffer_clear(&hash1);
	buffer_clear(&hash2);
	mpz_clears(msg_c1,msg_c2,NULL);

}
