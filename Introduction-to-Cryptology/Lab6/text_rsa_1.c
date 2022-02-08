#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "buffer.h"
#include "rsa.h"
#include "text_rsa.h"

#define DEBUG 0

void lengths(int *block_length, int *cipher_length, int *last_block_size,
			 buffer_t *msg, mpz_t N){
/* to be filled in */
  long int count = 0;

  *last_block_size = 0;

	 count =  mpz_sizeinbase(N, 2) - 8;
  *block_length = count/8;
  *last_block_size = ((msg->length*8)%count)/8;
  *cipher_length = ((msg->length*8)/count) + ((*last_block_size)?1:0); 
  *last_block_size = ((*last_block_size)?*last_block_size:*block_length);
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

/* to be filled in */
  int i = 0;
  mpz_t msg_mblk;
  mpz_inits(msg_mblk,NULL);
  char *msg_blk = malloc(sizeof(block_length)+1);

  printf("\nblock_length:%d cipher_length:%d last_blk_size:%d\n",block_length,cipher_length,last_block_size);
  for (i = 0; i<cipher_length-1; i++) {
    memset(msg_blk,0,block_length);
    memcpy(msg_blk, &msg->tab[i*block_length],block_length);
    printf("msg_blk:%s\n",msg_blk);
    //mpz_set_str(msg_mblk,"100",0);
    mpz_import(msg_mblk,block_length,1,1,0,0,msg_blk);
    //mpz_export(msg_blk,(size_t *)0,1,block_length,0,0,msg_mblk);
    //printf("#exported msg_blk:%s\n",msg_blk);
    RSA_encrypt(cipher[i], msg_mblk, N, e);
  }
  memset(msg_blk,0,block_length);
  memcpy(msg_blk, &msg->tab[i*block_length],last_block_size);
  printf("msg_blk:%s\n",msg_blk);
  mpz_import(msg_mblk,last_block_size,1,1,0,0,msg_blk);
  RSA_encrypt(cipher[i], msg_mblk, N, e);

  free(msg_blk);
}



void RSA_text_decrypt(buffer_t *decrypted, mpz_t *cipher,
					  int cipher_length, int block_length,
					  int last_block_size,
					  mpz_t N, mpz_t d){

	// buffer decrypted is supposed to be initialised.
	buffer_reset(decrypted);
	
/* to be filled in */
  int i = 0, temp =0;
  mpz_t msg_mblk;
  mpz_inits(msg_mblk,NULL);
  char *msg_blk = malloc(sizeof(block_length)+1);
  printf("size fo block_length is: %d",sizeof(block_length));
  printf("\nblock_length:%d cipher_length:%d last_blk_size:%d\n",block_length,cipher_length,last_block_size);

  for (i = 0; i<cipher_length-1; i++) {
    memset(msg_blk,0,block_length);
    RSA_decrypt(msg_mblk, cipher[i], N, d);
    mpz_export(msg_blk,(size_t *)0,1,1,0,0,msg_mblk);
    printf("msg_blk:%s size:%d i:%d\n",msg_blk,decrypted->length,i);
    memcpy(&decrypted->tab[i*block_length],msg_blk,block_length);
    decrypted->length += block_length;
  }
  memset(msg_blk,0,block_length);
  RSA_decrypt(msg_mblk, cipher[i], N, d);
  mpz_export(msg_blk,(size_t *) 0,1,1,0,0,msg_mblk);
  printf("msg_blk:%s %d %d\n",msg_blk, last_block_size, i);
  memcpy(&decrypted->tab[i*block_length],msg_blk, last_block_size);
  decrypted->length += last_block_size;

  free(msg_blk);
}

