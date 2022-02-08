#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gmp.h"
#include "buffer.h"
#include "sha3.h"

#include "rsa.h"
#include "sign.h"
#include "dsa.h"

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
/* to be filled in */
  mpz_t r,temp,multiplier;
  mpz_inits(r,temp,multiplier, NULL);
  mpz_set_ui(r,1);
  do {
    generate_probable_prime(q, qsize, state);
	  mpz_rrandomb(multiplier, state, psize-qsize);
	  mpz_mul(p, q, multiplier);
    mpz_add_ui(p,p,1);
    if(!mpz_probab_prime_p(p, 20)) continue;

    mpz_sub_ui(temp,p,1);
    mpz_tdiv_r(r, temp, q);
  } while(mpz_cmp_ui(r,0));
}
	
void dsa_generate_keys(mpz_t p, mpz_t q, mpz_t a, mpz_t x, mpz_t y,
		       size_t psize, size_t qsize, gmp_randstate_t state){
/* to be filled in */
  mpz_t temp,g,tempq,priv;
  mpz_inits(temp,g,tempq,priv, NULL);
  mpz_set_ui(g,2);

  generate_pq(p, q, psize, qsize, state);
  mpz_sub_ui(temp,p,1);
  mpz_tdiv_q(temp, temp, q);
  mpz_powm(a,g,temp,p);

  mpz_sub_ui(tempq,q,1);
  //unsigned long tempd = mpz_get_ui(tempq);
  mpz_urandomm(x,state, tempq);

  mpz_powm(y,a,x,p);
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
/* to be filled in */
   buffer_t out;
   mpz_t hmsg,xr,tempq, u, v,g,k;
   mpz_inits(hmsg,xr,tempq, u,v,g,k,NULL);

   buffer_init(&out, hash_length(q)-1);
   buffer_hash(&out, hash_length(q)-1, msg); 
   mpz_import(hmsg, hash_length(q)-1, 1, 1, 1,0, out.tab);
   //gmp_printf("\n#S hmsg:%#Zx\n",hmsg);

   mpz_sub_ui(tempq,q,1);
   mpz_urandomm(k,state, tempq);

   mpz_powm(r,a,k,p);
   mpz_mod(r,r,q);

   mpz_mul(xr,x,r);
   mpz_add(xr, hmsg, xr);
   mpz_gcdext(g, u, v, k, q);
   mpz_mul(xr,u,xr);
   mpz_mod(s,xr,q);

   //gmp_printf("\n#S r:%#Zx  s:%#Zx\n",r, s);
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
/* to be filled in */
   buffer_t out;
   mpz_t hmsg,xr,tempq, u, v,g,k,w,u1,u2,temp;
   mpz_inits(hmsg,xr,tempq, u,v,g,k,w,u1,u2,temp,NULL);

   buffer_init(&out, hash_length(q)-1);
   buffer_hash(&out, hash_length(q)-1, msg); 
   mpz_import(hmsg, hash_length(q)-1, 1, 1, 1,0, out.tab);
   //gmp_printf("\n#V s:%#Zx hmsg:%#Zx\n",s, hmsg);
   //gmp_printf("\n#V r:%#Zx\n",r);

   mpz_gcdext(g, u, v, s, q);
   mpz_mod(w,u,q);
   
   mpz_mul(u1,hmsg,w);
   mpz_mod(u1,u1,q);

   mpz_mul(u2,r,w);
   mpz_mod(u2,u2,q);

   mpz_powm(u1,a,u1,p);
   mpz_powm(u2,y,u2,p);
   mpz_mul(temp,u2,u1);
   mpz_mod(v,temp,p);
   mpz_mod(v,v,q);

   verify = (mpz_cmp(v, r))?0:1;
   //gmp_printf("\n#V v:%#Zx r:%#Zx",v, r);
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
