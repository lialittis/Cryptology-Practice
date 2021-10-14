#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gmp.h"
#include "buffer.h"
#include "rsa.h"
#include "text_rsa.h"

#define DEBUG 0


void lengths(int *block_length, int *cipher_length, int *last_block_size,
			 buffer_t *msg, mpz_t N){
	// mpz_sizeinbase Method: Return the size of op measured in number of digits in the given base.

	size_t bit_size_N = mpz_sizeinbase(N,2);
	
	// Actually, here I think it's better to code like:
	// *block_length = bit_size_N/(sizeof(int)) -1;
	// To pass the test, replace sizeof(int) by 8
	*block_length = bit_size_N/8 - 1;
	*cipher_length = (msg->size) / (*block_length);
	if (msg->size % (*block_length) != 0){
		*cipher_length = *cipher_length + 1;
		*last_block_size = msg->size - (*cipher_length - 1) * (*block_length);
	}else{
		*last_block_size = *block_length;
	}
}


void RSA_text_encrypt(mpz_t *cipher, int block_length,
					  int cipher_length, int last_block_size,
					  buffer_t *msg, mpz_t N, mpz_t e){
	// cipher is a table of mpz_t of length cipher_length.
	// Memory allocation and initialisation of the cells is
	// already done.

	// block_length denotes the size of blocks of uchar's
	// which will partition the message.
	// last_block_size denotes the size of the last block. It may
	// be 0.

	// buffer_init(msg,(block_length*(cipher_length-1)+last_block_size)*8)
    for(int i=0;i<msg->size;i++){
		printf("%d",msg->tab[i]);
	}
	printf("size is %ld\n",msg->size);
	uchar *foo;
    foo = string_from_buffer(msg);
	printf("Plain text :\n%s\n\n", foo);

	free(foo);
	for(int i=0;i<cipher_length;i++){
		mpz_t msg_t;
		mpz_init(msg_t);
		
		if(i<cipher_length - 1){
			mpz_import(msg_t,block_length,1,1,1,0,msg->tab);
			msg->tab += block_length;
		}else{
			mpz_import(msg_t,last_block_size,1,1,1,0,msg->tab);
			msg->tab -=(i)*block_length;
		}
		RSA_encrypt(cipher[i],msg_t,N,e);
		gmp_printf("the %d part of msg is : %Zd\n",i,msg_t);
		gmp_printf("the %d part of cipher is : %Zd\n",i,cipher[i]);
		mpz_clear(msg_t);
	}
	// printf("msg is : %d\n",*msg->tab);
	// gmp_printf("cipher is: %Zd\n",*cipher);
}



void RSA_text_decrypt(buffer_t *decrypted, mpz_t *cipher,
					  int cipher_length, int block_length,
					  int last_block_size,
					  mpz_t N, mpz_t d){

	// buffer decrypted is supposed to be initialised.
	buffer_reset(decrypted);

	// printf("size of block is %d\n",sizeof(block_length));
	
	for(int i=0; i< cipher_length;i++){
		mpz_t decrypted_t;
		size_t b_l;
		size_t l_b_l;
		b_l = (size_t)block_length;
		l_b_l = (size_t)last_block_size;
		mpz_init(decrypted_t);
		RSA_decrypt(decrypted_t,cipher[i],N,d);

		if(i < cipher_length -1){
			mpz_export(decrypted->tab,&b_l,1,1,1,0,decrypted_t);
			decrypted->tab += block_length;
			decrypted->length += block_length;
		}else{
			mpz_export(decrypted->tab,&l_b_l,1,1,1,0,decrypted_t);
			decrypted->tab -= i*block_length;
			decrypted->length += last_block_size;
		}
		gmp_printf("the %d part of decryted is : %Zd\n",i,decrypted_t);
		mpz_clear(decrypted_t);	
	}

	printf("size is : %ld\n",decrypted->size);
	gmp_printf("cipher is : %Zd\n",*cipher);
	
	printf("after decrypted:");
	for(int i=0; i< decrypted->size;i++){
		printf("%d",decrypted->tab[i]);
		//printf("%d",(uchar)decrypted->tab[i]);
	}
	//printf("tmp %s",(char*)decrypted);
	
	/*
	uchar *tmp = (uchar*)malloc((decrypted->length+1)*sizeof(uchar));
	if(tmp != NULL){
		memcpy((char*)tmp,(char*)decrypted->tab,decrypted->length);
		tmp[decrypted->length]='\0';
	}
	printf("tmp is \n%s\n\n,",tmp);
    */
	
}


