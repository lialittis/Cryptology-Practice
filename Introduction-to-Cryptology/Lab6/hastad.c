#include <assert.h>
#include <stdio.h>
#include "gmp.h"
#include "xgcd.h"
#include "crt.h"
#include "rsa.h"
#include "hastad.h"

void Hastad(mpz_t decrypted, mpz_t* cipher_texts, mpz_t* moduli,
			int exponent){
	mpz_t C;
	mpz_init(C);
	CRT(C,cipher_texts,moduli,exponent);
	mpz_root(decrypted,C,exponent);
	mpz_clear(C);
}


void parse_challenge(const char *file_name, int exponent, mpz_t *moduli,
					 mpz_t *cipher_texts){
	
	FILE * in = fopen(file_name, "r");
	mpz_t *cursor_mod = moduli;
	mpz_t *cursor_text = cipher_texts;

	cursor_mod = moduli;
	cursor_text = cipher_texts;

	for(int i = 0; i < exponent; i++, cursor_mod++, cursor_text++){
		gmp_fscanf(in, "%Zd",*cursor_mod);
		gmp_fscanf(in, "%Zd", *cursor_text);		
	}
	
	fclose(in);
}


