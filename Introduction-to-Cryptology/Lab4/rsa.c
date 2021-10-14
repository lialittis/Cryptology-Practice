#include <stdio.h>
#include <assert.h>

#include "gmp.h"

#include "xgcd.h"
#include "CRT.h"
#include "rsa.h"

void generate_probable_prime(mpz_t p, size_t nbits, gmp_randstate_t state){
}

void RSA_encrypt(mpz_t cipher, mpz_t msg, mpz_t N, mpz_t e){
}

void RSA_decrypt(mpz_t msg, mpz_t cipher, mpz_t N, mpz_t d){
}

/* Use CRT. */
void RSA_decrypt_with_p_q(mpz_t msg, mpz_t cipher, mpz_t N, mpz_t d, mpz_t p, mpz_t q){
}

/* Generate p and q with nbits. */
void RSA_generate_key(mpz_t N, mpz_t p, mpz_t q, mpz_t e, mpz_t d, size_t nbits, gmp_randstate_t state){
}
