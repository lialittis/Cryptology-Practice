#include <stdio.h>
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

		/*For challenges*/



    printf("**************For challenge**********************\n");

	mpz_t y, N1;
	mpz_set_str(N1,"",10);
	mpz_set_str(y,"",10);

	
	
	/*
    mpz_t y, N1, temp_n, temp_y, x2,x,p1,q1;
	mpz_inits(y,N1,temp_n,temp_y,x2,x,p1,q1, NULL);
	//mpz_set_str(N1,"7181295373543472625111416917887674160957356632750437602913071415629704829212417720775128715915328042845709932123915841626931791373637144374074108793890293",10);
	mpz_set_str(N1,"6825281219945233680198141720428900367285477714138217512486079680139670593911629181355368778240562526516627363975476245753331345329074363463133025001367939",10);
	gmp_printf("-th encrypted = %Zd.\n\n", N1);
	
	mpz_set_si(y,-1);
	do {
		mpz_add_ui(y, y,1);
		mpz_mul_ui(temp_n,N1,4);
		mpz_pow_ui(temp_y,y,2);
		mpz_add(x2,temp_n,temp_y);
	} while(!mpz_perfect_square_p (x2));
	mpz_sqrt(x,x2);
	mpz_sub(p1,x,y);
	mpz_div_ui(p1,p1,2);
	mpz_add(q1,x,y);
	mpz_div_ui(q1,q1,2);
	gmp_printf("EXIT X2 = %Zd. p:%Zd q:%Zd\n\n", x2, p1, q1);

*/

	/*
    int i,r,q;
    mpz_t N,p,p1,n1,n2,n3;
    mpz_inits(p,n1,n2,n3,NULL);
    mpz_init_set_str(N,"111651639460724872028968427056355040383",10);
    mpz_add_ui(n3,n,1);
    printf("n3:%ld\n",mpz_get_ui(n3));
    mpz_sqrt(p1,n3);
    mpz_add_ui(p1,p1,1);

    printf("p1 :%ld",mpz_get_ui(p1));
    mpz_init_set_str(p,"10566533937896800795",10);
    gmp_printf("n:%Zd\n",n);

    printf("vasda\n");

    for(int i=0; ;i++){
        
		mpz_add_ui(p1,p,i);
        gmp_printf("p1:%Zd\n",p1);
        gmp_printf("p:%Zd\n",p);
        printf("vasda\n");
        mpz_pow_ui(p1,p1,2);
        mpz_sub(n1,p1,n);
        gmp_printf("n:%Zd\n",n);
        gmp_printf("n1:%Zd\n",n1);

        //mpz_set_str(n1,"100",10);

        gmp_printf("n1:%Zd\n",n1);
        mpz_sqrt(n2,n1);
        printf("vasda1\n");
        gmp_printf("n2:%Zd\n",n2);

        mpz_pow_ui(n2,n2,2);
        printf("vasda1\n");
        if (mpz_cmp(n2,n1)==0){
            break;
        }
        printf("i :%d\n",i);
        mpz_add_ui(n1,n,pow(i,2));
        mpz_sqrt(p,n1);
        gmp_printf("p:%Zd\n\n",p);
        mpz_pow_ui(p,p,2);
        gmp_printf("n1:%Zd\n",n1);
        gmp_printf("p:%Zd\n",p);
        if (mpz_cmp(p,n1)==0){
            break;
        }
    }

    printf("%d\n",i);
    mpz_sqrt(n2,n2);
    r=(mpz_get_ui(n2)+i);
    q=(mpz_get_ui(n2)-i);

    printf("prime is %d and %d",r,q);
    mpz_clears(n1,n2,n3,n,p,NULL);
    gmp_randclear(state);
*/


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
	
	/*
	printf("after decrypted:");
	for(int i=0; i< decrypted->size;i++){
		printf("%d",decrypted->tab[i]);
		//printf("%d",(uchar)decrypted->tab[i]);
	}
	*/
	
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


