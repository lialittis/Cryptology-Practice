#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gmp.h"

#include "buffer.h"
#include "sha3.h"
#include "rsa.h"
#include "sign.h"

#define DEBUG 0


void RSA_generate_key_files(const char *pk_file_name,
			    const char *sk_file_name,
			    size_t nbits, int sec, gmp_randstate_t state){


	// mpz_rrandomb Method : Generate a random integer with long strings of zeros
    // and ones in the binary representation.
    
	FILE *fp1;
	fp1 = fopen(pk_file_name,"wr");
	FILE *fp2;
	fp2 = fopen(sk_file_name,"wr");
	
	mpz_t N,e,d,p,q;
	mpz_inits(N,e,d,p,q,NULL);

    RSA_generate_key(N,p,q,e,d,(int)nbits,sec,state);

	gmp_fprintf(fp1, "#RSA Public key (256 bits):\n");
	gmp_fprintf(fp1, "N = %#Zx\n",N);
	gmp_fprintf(fp1, "e = %#Zx", e);
    
	gmp_fprintf(fp2, "#RSA Secret key (256 bits):\n");
	gmp_fprintf(fp2, "N = %#Zx\n",N);
	gmp_fprintf(fp2, "d = %#Zx", e);

	fclose(fp1);
	fclose(fp2);
	mpz_clears(N,e,d,p,q,NULL);
	
}


void RSA_key_import(mpz_t N, mpz_t ed, const char *key_file_name){
    FILE *key = fopen(key_file_name, "r");
    /* Go to second line, then move from 6 characters to the right */
    while(fgetc(key) != '\n');
    fseek(key, 6, SEEK_CUR);

    /* Scan the modulus N */
    gmp_fscanf(key, "%Zx", N);

    /* Same for e or d*/
    while(fgetc(key) != '\n');
    fseek(key, 6, SEEK_CUR);
    gmp_fscanf(key, "%Zx", ed);

    fclose(key);
}


int hash_length(mpz_t N){
    int bit_size_N = mpz_sizeinbase(N, 2);
    return (bit_size_N % BYTE_SIZE == 0) ?
	bit_size_N / BYTE_SIZE - 1 : (bit_size_N / BYTE_SIZE);
}


void RSA_sign_buffer(mpz_t sgn, buffer_t *msg,
		     mpz_t N, mpz_t d){
	
	size_t output_length;
	output_length = hash_length(N);
	buffer_t hash;
    
	buffer_init(&hash, hash_length(N));
	
	hash.length = (int)output_length;
	// printf("the length of hash is %ld\n\n",hash.length);
	buffer_hash(&hash,hash_length(N),msg);
	mpz_t msg_c;
	mpz_init(msg_c);
	mpz_import(msg_c,output_length,1,1,1,0,hash.tab);
	mpz_powm(sgn,msg_c,d,N);

	buffer_clear(&hash);

    // gmp_printf("After the signature algo, the hash message is : %#Zx\n\n", msg_c);
    // gmp_printf("After the signature algo, the signature is : %#Zx\n\n", sgn);

}


int RSA_verify_signature(mpz_t sgn, buffer_t *msg,
			 mpz_t N, mpz_t e){
    int verify = 0;

	size_t output_length;
	output_length = hash_length(N);

	buffer_t hash;
	buffer_init(&hash,hash_length(N));
	buffer_hash(&hash,hash_length(N),msg);

	mpz_t vrf_sgn;
	mpz_init(vrf_sgn);
    mpz_powm(vrf_sgn,sgn,e,N);

	buffer_t hash_vrf_sgn;
	buffer_init(&hash_vrf_sgn,hash_length(N));

    mpz_export(hash_vrf_sgn.tab,&output_length,1,1,1,0,vrf_sgn);
	hash_vrf_sgn.length = (int)output_length;

	// printf("the length of hash_vrf_sgn is %ld\n\n",hash.length);
	// printf("the length of hash_vrf_sgn is %ld\n\n",hash_vrf_sgn.length);
	// gmp_printf("After the verification algo, the hash message is : %#Zx\n\n", vrf_sgn);

    // printf("hash is %s\n\n",hash.tab);
	// printf("hash_vrf_sgn is %s\n\n",hash_vrf_sgn.tab);
	if(buffer_equality(&hash,&hash_vrf_sgn)){
		verify = 1;
	}
	buffer_clear(&hash);
	buffer_clear(&hash_vrf_sgn);
    return verify;
}


void RSA_signature_import(mpz_t S, const char* signature_file_name){
    FILE *sgn = fopen(signature_file_name, "r");
    while(fgetc(sgn) != '\n');
    fseek(sgn, 6, SEEK_CUR);
    gmp_fscanf(sgn, "%Zx", S);
    fclose(sgn);
}


void RSA_sign(const char* file_name, const char* key_file_name,
	      const char* signature_file_name){
    // 1. Initialisation
    buffer_t msg;
    mpz_t N, d, signature;
    mpz_inits(N, d, signature, NULL);
    buffer_init(&msg, 100);

    // 2. Import the message in a buffer
    buffer_from_file(&msg, file_name);
	
    // 3. Parse the secret key
    RSA_key_import(N, d, key_file_name);

    // 4. Sign the buffer
    RSA_sign_buffer(signature, &msg, N, d);

    // 5. Exports the signature in a file
    FILE* sgn = fopen(signature_file_name, "w");
    gmp_fprintf(sgn, "#RSA signature\nS = %#Zx\n", signature);
	
    // 6. Close and free
    fclose(sgn);
    mpz_clears(N, d, signature, NULL);
    buffer_clear(&msg);
}


int RSA_verify(const char* file_name, const char* key_file_name,
	       const char* signature_file_name){
    // 1. Initialisation
    buffer_t msg;
    mpz_t N, e, S;	
    buffer_init(&msg, 100);
    mpz_inits(N, e, S, NULL);	

	
    // 2. Import the message into a buffer
    buffer_from_file(&msg, file_name);

    // 3. Import the public key
    RSA_key_import(N, e, key_file_name);

    // 4. Parse the signature
    RSA_signature_import(S, signature_file_name);
	
    // 5. Verify
    int verify = RSA_verify_signature(S, &msg, N, e);
	
    // 6. Close, free and return
    mpz_clears(S, N, e, NULL);
    buffer_clear(&msg);
    return verify;
}
