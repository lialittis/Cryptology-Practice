#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gmp.h"

#include "base64.h"
#include "buffer.h"
#include "bits.h"

#include "aes.h"
#include "dh.h"

/* make a > 1... */
void DH_init(mpz_t a, gmp_randstate_t state){
    do{
		mpz_urandomb(a, state, 4);
    } while(mpz_cmp_ui(a, 1) <= 0);
}

/* for AES 128 */
void AES128_key_from_number(buffer_t *key, mpz_t n){
    size_t nc, i;
    uchar *tmp = (uchar *)calloc(BLOCK_LENGTH, sizeof(uchar));
    mpz_export(tmp, &nc, 1, 1, 1, 0, n);
    uchar *cursor = tmp;
    for(i = 0; i < BLOCK_LENGTH; i++, cursor++)
	buffer_append_uchar(key, *cursor);
    free(tmp);
}

void AES128_number_from_key(mpz_t n, uchar key[BLOCK_LENGTH]){
}

/* TODO: move the two following functions to buffer.c */
void buffer_from_mpz(buffer_t *buf, mpz_t n){
    size_t nc, len = 1 + (mpz_sizeinbase(n, 2) / BYTE_SIZE); /* overshooting */
    buffer_resize(buf, len);
    mpz_export(buf->tab, &nc, 1, 1, 1, 0, n);
    assert(nc <= len);
    buf->length = nc;
}

void buffer_to_mpz(mpz_t n, buffer_t *buf){
    mpz_import(n, buf->length, 1, 1, 1, 0, buf->tab);
}

void concatenate_gb_ga(mpz_t tmp, mpz_t gb, mpz_t ga, mpz_t p){
    /* shift tmp! */
    mpz_mul_2exp(tmp, gb, mpz_sizeinbase(p, 2));
    mpz_add(tmp, tmp, ga);
    gmp_printf("gb||ga=%#Zx\n", tmp);
}

/* sigmaB <- SIGN_B(gb || ga).
   We force gb and ga to have the size of p, so that we can separate them in
   the client part.
   Rem: we need NB >= p^2.
*/
void SIGNSK(mpz_t sigmaB, mpz_t gb, mpz_t ga, mpz_t p, mpz_t NB, mpz_t dB){
    mpz_t tmp;

    mpz_init(tmp);
    concatenate_gb_ga(tmp, gb, ga, p);
    mpz_powm_sec(sigmaB, tmp, dB, NB);
    mpz_clear(tmp);
}
