#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gmp.h"
#include "buffer.h"
#include "sha3.h"

#include "rsa.h"
#include "sign.h"
#include "dsa.h"

#include <math.h>
#define DEBUG 1

void generate_probable_prime(mpz_t p, int psize,
			     gmp_randstate_t state){
    do{
	mpz_rrandomb(p, state, psize);
	mpz_nextprime(p, p);
    }while(mpz_sizeinbase(p, 2) < psize);
}

void generate_pq(mpz_t p, mpz_t q, size_t psize, size_t qsize,
		 gmp_randstate_t state){

	
	mpz_t mul,r,pm1;
	mpz_inits(mul,r,pm1,NULL);
	do{
		// generate_probable_prime(p,(int)psize,state);
	    // mpz_rrandomb(q,state,(int)qsize);
		generate_probable_prime(q,(int)qsize,state);
		mpz_rrandomb(mul,state,psize-qsize);
		mpz_mul(pm1,q,mul);
		//gmp_printf("mul : %Zd\n\n",mul);

		// gmp_printf("p-1 is %Zd\n\n",pm1);
		mpz_add_ui(p,pm1,1);
		// mpz_tdiv_r(r,pm1,q);
		mpz_set_ui(r,1);
		if(mpz_probab_prime_p(p,25)){
			mpz_sub_ui(pm1,p,1);
			mpz_tdiv_r(r,pm1,q);
		}
		
		// gmp_printf("p is %Zd\n\n",p);
	}while(mpz_cmp_ui(r,0));

	mpz_clears(mul,r,pm1,NULL);

}
	
void dsa_generate_keys(mpz_t p, mpz_t q, mpz_t a, mpz_t x, mpz_t y,
		       size_t psize, size_t qsize, gmp_randstate_t state){
	// generate p,q
    generate_pq(p,q,psize,qsize,state);

	mpz_t pm1,g;
	mpz_inits(pm1,g,NULL);

	mpz_sub_ui(pm1,p,1);
	mpz_tdiv_q(pm1,pm1,q);
	mpz_set_ui(g,2);
	// calculate a
	mpz_powm(a,g,pm1,p);
    // generate x

	mpz_sub_ui(pm1,q,1);
	mpz_urandomm(x,state,pm1);
	// old method
	// mpz_rrandomb(x,state,(int)(qsize-1));
	// generate y
	mpz_powm(y,a,x,p);

	mpz_clears(pm1,g,NULL);
}


void dsa_generate_key_files(const char* pk_file_name, const char* sk_file_name,
			    size_t psize, size_t qsize,
			    gmp_randstate_t state){
    // 1. INITS
    mpz_t p, q, a, x, y;
    mpz_inits(p, q, a, x, y, NULL);
    FILE* pk = fopen(pk_file_name, "w");
    FILE* sk = fopen(sk_file_name, "w");
	
    // 2. Key generation
    dsa_generate_keys(p, q, a, x, y, psize, qsize, state);

    // 3. Printing files
    fprintf(pk, "#DSA public key (%lu bits, %lu bits):\n", psize, qsize);
    gmp_fprintf(pk, "p = %#Zx\nq = %#Zx\na = %#Zx\ny = %#Zx\n", p, q, a, y);
    fprintf(sk, "#DSA Private Key (%lu bits, %lu bits):\n", psize, qsize);
	// printf("------flag------------\n\n");
	// printf("------flag------------\n\n");
	// printf("------flag------------\n\n");
    gmp_fprintf(sk, "p = %#Zx\nq = %#Zx\na = %#Zx\nx = %#Zx\n", p, q, a, x);
	
    // 4. Cleaning
    mpz_clears(p, q, a, x, y, NULL);
    fclose(pk);
    fclose(sk);
}


void dsa_key_import(const char* key_file_name, mpz_t p, mpz_t q, mpz_t a,
		    mpz_t xy){
    FILE* key = fopen(key_file_name, "r");
	
    // Go to second line, then move from 6 characters to the right
    while(fgetc(key) != '\n');
    fseek(key, 6, SEEK_CUR);

    // Scan the modulus p
    gmp_fscanf(key, "%Zx", p);

    // Same for q
    while(fgetc(key) != '\n');
    fseek(key, 6, SEEK_CUR);
    gmp_fscanf(key, "%Zx", q);

    // Same for a
    while(fgetc(key) != '\n');
    fseek(key, 6, SEEK_CUR);
    gmp_fscanf(key, "%Zx", a);

    // Same for x or y
    while(fgetc(key) != '\n');
    fseek(key, 6, SEEK_CUR);
    gmp_fscanf(key, "%Zx", xy);

    fclose(key);
}


void dsa_sign_buffer(buffer_t *msg, mpz_t p,
		     mpz_t q, mpz_t a, mpz_t x, mpz_t r, mpz_t s,
		     gmp_randstate_t state){
	/* my code */

	mpz_t msg_c;
	mpz_init(msg_c);
	// p,q,a,x - known

	int size = hash_length(q);
	// generate k with size<q;
	mpz_t k,hash_c,k_size;
	mpz_inits(k,hash_c,k_size,NULL);

	mpz_sub_ui(k_size,q,1);
	mpz_urandomm(k,state,k_size);

	// generate r
	mpz_powm(r,a,k,p);
	mpz_mod(r,r,q);

	// declare hash
	buffer_t hash;
	buffer_init(&hash, size);

	// generate hash
	buffer_hash(&hash, size, msg);
	mpz_import(msg_c,size,1,1,1,0,hash.tab);

	// generate s
	mpz_mul(s,x,r);
	mpz_add(s,s,msg_c);
	mpz_mod(s,s,q);
	// mpz_cdiv_q(s,s,k);
	mpz_invert(k,k,q);
	mpz_mul(s,s,k);
	mpz_mod(s,s,q);

	buffer_clear(&hash);
	mpz_clears(k,hash_c,k_size,NULL);
	mpz_clear(msg_c);
}


void dsa_sign(const char* file_name, const char* key_file_name,
	      const char* signature_file_name,
	      gmp_randstate_t state){
    // 1. Initialisation
    mpz_t p, q, a, x, r, s;
    buffer_t msg;
    mpz_inits(r, s, p, q, a, x, NULL);
    buffer_init(&msg, 100);
	
    // 2. Import the message
    buffer_from_file(&msg, file_name);
#if DEBUG
    printf("Length of the message = %lu.\n", msg.length);
#endif

    /* 3. Parse the secret key */
    dsa_key_import(key_file_name, p, q, a, x);
#if DEBUG > 0
    gmp_printf("p = %#Zx\nq = %#Zx\n", p, q);
#endif
	
    /* 4. Sign */
    dsa_sign_buffer(&msg, p, q, a, x, r, s, state);

    /* 5. Write signature in a file */
    FILE* sgn = fopen(signature_file_name, "w");
    gmp_fprintf(sgn, "#DSA signature:\nr = %#Zx\ns = %#Zx\n", r, s);
	
    /* . Cleaning */
    mpz_clears(p, q, a, x, r, s, NULL);
    fclose(sgn);
    buffer_clear(&msg);
}


int dsa_verify_buffer(buffer_t *msg, mpz_t p, mpz_t q,
		      mpz_t a, mpz_t r, mpz_t s, mpz_t y){
    int verify = 0;
	/* my code */

	mpz_t msg_c;
	mpz_init(msg_c);

	mpz_t w,u1,u2,v;
	mpz_inits(w,u1,u2,v,NULL);

	gmp_printf("\nwe have r : %#Zx\n\n",r);
	int size = hash_length(q);
	// declare hash
	buffer_t hash;
	buffer_init(&hash, size);

	// generate hash
	buffer_hash(&hash, size, msg);
	mpz_import(msg_c,size,1,1,1,0,hash.tab);
	// mpz_export(hash_vrf_sgn.tab,&output_length,1,1,1,0,vrf_sgn);

    // generate w
	mpz_invert(w,s,q);
	// gmp_printf("we get w : %Zd\n\n",w);
/*
	// generate u1
	mpz_mul(u1,msg_c,w);

	// mpz_powm_ui(u1,u1,1,q);
    mpz_mod(u1,u1,q);

	gmp_printf("we get u1 : %Zd\n\n",u1);
	// generate u2
	mpz_mul(u2,r,w);
	// mpz_powm_ui(u2,u2,1,q);
	
	mpz_mod(u2,u2,q);
	// gmp_printf("we get u2 : %Zd\n\n",u2);

	//generate v
	mpz_t v1,v2;
	mpz_inits(v1,v2,NULL);
	mpz_powm(v1,a,u1,p);
	mpz_powm(v2,y,u2,p);
*/
    mpz_mul(u1,msg_c,w);
    mpz_mod(u1,u1,q);

    mpz_mul(u2,r,w);
    mpz_mod(u2,u2,q);

    mpz_powm(u1,a,u1,p);
    mpz_powm(u2,y,u2,p);
    mpz_mul(v,u2,u1);
    mpz_mod(v,v,p);
    mpz_mod(v,v,q);

	// gmp_printf("we get v1: %Zd\n\n",v1);
	// gmp_printf("we get v2 : %Zd\n\n",v2);
	
	// mpz_mul(v,v1,v2);

	// mpz_mod(v,v,p);
	// printf("------flag------------\n\n");
	// mpz_mod(v,v,p);
	// mpz_mod(v,v,q);
	// mpz_clears(v1,v2,NULL);
	gmp_printf("we get v : %#Zx\n\n",v);

	mpz_clear(msg_c);

	// condition
	if (mpz_cmp(v,r)==0){
		verify = 1;
	    // printf("------flag------------\n\n");
	}

	mpz_clears(w,u1,u2,v,NULL);
	mpz_clears(w,u1,u2,v);
	buffer_clear(&hash);

    return verify;
}


void dsa_import_signature(mpz_t r, mpz_t s, const char* signature_file_name){
    FILE* sgn = fopen(signature_file_name, "r");
    while(fgetc(sgn) != '\n');
    fseek(sgn, 6, SEEK_CUR);
    gmp_fscanf(sgn, "%Zx", r);
	
    while(fgetc(sgn) != '\n');
    fseek(sgn, 6, SEEK_CUR);
    gmp_fscanf(sgn, "%Zx", s);
	
    fclose(sgn);
}


int dsa_verify(const char* file_name, const char* key_file_name,
	       const char* signature_file_name){
    // 1. INIT
    mpz_t p, q, a, y, r, s;
    buffer_t msg;
    mpz_inits(p, q, a, y, r, s, NULL);
    buffer_init(&msg, 100);
	
    // 2. Imports the message
    buffer_from_file(&msg, file_name);
	
    // 3. Parse the public key 
    dsa_key_import(key_file_name, p, q, a, y);

#if DEBUG > 0
    gmp_printf("\n\np = %#Zx\nq = %#Zx\n\n", p, q);
#endif
	
    // 4. Parse the signature 
    dsa_import_signature(r, s, signature_file_name);
    int verify = dsa_verify_buffer(&msg, p, q, a, r, s, y);
	
    // 5. Cleaning and return
    mpz_clears(p, q, a, y, r, s, NULL);
    buffer_clear(&msg);
    return verify;
}
