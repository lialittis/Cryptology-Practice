#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "buffer.h"
#include "rsa.h"
#include "text_rsa.h"

#define DEBUG 0


void lengths(int *block_length, int *cipher_length, int *last_block_size,
			 buffer_t *msg, mpz_t N){
				 *block_length=(mpz_sizeinbase(N,2)/8)-1;
				 mpz_t msg_length,r,q;
				 mpz_inits(q,r,NULL);
				 mpz_init_set_ui(msg_length,msg->length);
				 mpz_cdiv_qr_ui(q,r,msg_length,*block_length);
				 *cipher_length=mpz_get_ui(q);
				 *last_block_size=*block_length-mpz_get_ui(r);
				 
				 mpz_clears(msg_length,r,q,NULL);
/* to be filled in */

}


void RSA_text_encrypt(mpz_t *cipher, int block_length,
					  int cipher_length, int last_block_size,
					  buffer_t *msg, mpz_t N, mpz_t e){
	// cipher is a table of mpz_t of length cipher_length.
	// Memory allocation and initialisation of the cells is
	// already done.
	int i;
	for(i=0;i<cipher_length;i++){
		mpz_t msg2;
		mpz_init(msg2);
		if (i<cipher_length-1){mpz_import(msg2,block_length,1,1,0,0,msg->tab);}
		else{mpz_import(msg2,last_block_size,1,1,0,0,msg->tab);}
		RSA_encrypt(*cipher,msg2,N,e);
        cipher++;
		if (i<cipher_length-1){msg->tab+=block_length;}
		else {msg->tab -= i*block_length;}
		mpz_clear(msg2);
	}
	// block_length denotes the size of blocks of uchar's
	// which will partition the message.
	// last_block_size denotes the size of the last block. It may
	// be 0.

/* to be filled in */
}



void RSA_text_decrypt(buffer_t *decrypted, mpz_t *cipher,
					  int cipher_length, int block_length,
					  int last_block_size,
					  mpz_t N, mpz_t d){

	// buffer decrypted is supposed to be initialised.
	buffer_reset(decrypted);
	int i;
	for(i=0;i<cipher_length;i++){
		mpz_t cipher2;
		size_t a ,b;
		mpz_inits(cipher2,NULL);
		a=block_length;
		b=last_block_size;
		RSA_decrypt(cipher2,*cipher,N,d);
		if (i<cipher_length-1){
			mpz_export(decrypted->tab,&a,1,1,0,0,cipher2);
		}
		else{
			mpz_export(decrypted->tab,&b,1,1,0,0,cipher2);
        }
		cipher++;
		if(i<cipher_length-1){
			decrypted->tab+=block_length;
			decrypted->length+=block_length;
		}
		else{
			decrypted->tab-=i*block_length;
			decrypted->length+=b;
		}
		mpz_clears(cipher2,NULL);
	}
	
/* to be filled in */
}


