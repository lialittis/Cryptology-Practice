#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gmp.h"

#include "rsa.h"

static void Usage(char *s){
    fprintf(stderr, "Usage: %s <size_of_primes>\n", s);
}

int main(int argc, char *argv[]){
    size_t nbits;
    mpz_t p, q, N, e, d, msg, cipher, tmp;
    gmp_randstate_t state;

    if(argc == 1){
		Usage(argv[0]);
		return 0;
    }
    
    nbits = (size_t)atoi(argv[1]);
    mpz_inits(p, q,N, e, d, msg, cipher, tmp, NULL);

    gmp_randinit_default(state);
    RSA_generate_key(N, p, q, e, d, nbits, state);
    gmp_printf("N:=%Zd;\np:=%Zd;\nq:=%Zd;\ne:=%Zd;\nd:=%Zd;\n", N, p, q, e, d);

    /* check */
    mpz_urandomm(msg, state, N);
    RSA_encrypt(cipher, msg, N, e);

    printf("\nStandard decryption:    ");
    RSA_decrypt(tmp, cipher, N, d);
    if(mpz_cmp(tmp, msg) == 0)
		printf("[SUCCESS]\n");
	else
		printf("[FAILED]\n");
    
    printf("CRT decryption:         ");
    RSA_decrypt_with_p_q(tmp, cipher, N, d, p, q);
    if(mpz_cmp(tmp, msg) == 0)
		printf("[SUCCESS]\n\n");
	else
		printf("[FAILED]\n\n");
	
    mpz_clears(p, q,N, e, d, msg, cipher, tmp, NULL);
    return 0;
}
