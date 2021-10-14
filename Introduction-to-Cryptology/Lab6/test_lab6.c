#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gmp.h"
// #include "gmp-impl.h"

#include "buffer.h"
#include "random.h"
#include "rsa.h"
#include "text_rsa.h"
#include "hastad.h"

#define DEBUG 0



void print_key(mpz_t N, mpz_t p, mpz_t q, mpz_t e, mpz_t d, int nlen){
    printf("---------------------- RSA %d ----------------------\n", nlen);
    printf("\nPublic key (N, e) : \n\n");
    gmp_printf("N = %Zd\n\n", N);
    gmp_printf("e = %Zd\n\n", e);

    printf("\n**** \n\nSecret key (d; p, q) : \n\n");
    gmp_printf("d = %Zd\n\n", d);
    gmp_printf("p = %Zd\n\n", p);
    gmp_printf("q = %Zd\n\n", q);
    printf("----------------------------------------------------\n\n");
}


void test_encrypt_decrypt(){
    // 1. INIT
    mpz_t e, d, N, msg, cipher, decrypted;
    gmp_randstate_t state;
    gmp_randinit_default(state);
	
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif
    mpz_inits(e, d, N, msg, cipher, decrypted, NULL);

    // 2. Generate key and message
    mpz_set_str(N, "13407807929942597093760734805309010363963172602566944802519609325224163304582998294904721413435102703248310906226491221593579560484162277805870126943177997", 10);
    mpz_set_str(e, "57222688662856409396324072171762917393157734041088050228362504607465340519645", 10);
    mpz_set_str(d, "1878997391028620372412461175918902866075227951627683808774454416890098012337711393921327334759215491874986751914100470703224252001941679727301166707073653", 10);
    mpz_urandomm(msg, state, N);
    printf("---------------------------------\n");
    gmp_printf("Plain text = %Zd\n\n", msg);
	
    // 3. Encrypt and decrypt
    RSA_encrypt(cipher, msg, N, e);
    gmp_printf("Cipher text = %Zd\n\n", cipher);

    RSA_encrypt(decrypted, cipher, N, d);
    gmp_printf("Decrypted text = %Zd\n\n", decrypted);

    // 4. Final verification
    if(mpz_cmp(msg, decrypted) == 0)
	printf("[OK]\n\n");
    else
	printf("[FAILED]\n\n");
	
	
    // . Clear
    gmp_randclear(state);
    mpz_clears(e, d, N, msg, cipher, decrypted, NULL);
}


void basic_test(mpz_t p2, mpz_t q2, mpz_t e2, mpz_t d2, int nlen){
    if(!is_valid_key(p2, q2, e2, d2, nlen, 100))
	printf("[OK]\n");
    else
	printf("[FAILED]\n");


	/*For challenges*/



    printf("**************For challenge**********************\n");

    mpz_t y, N, temp_n, temp_y, x2,x,p1,q1;
	mpz_inits(y,N,temp_n,temp_y,x2,x,p1,q1, NULL);
	mpz_set_str(N,"7181295373543472625111416917887674160957356632750437602913071415629704829212417720775128715915328042845709932123915841626931791373637144374074108793890293",10);
	gmp_printf("-th encrypted = %Zd.\n\n", N);
	
	mpz_set_si(y,-1);
	do {
		mpz_add_ui(y, y,1);
		mpz_mul_ui(temp_n,N,4);
		mpz_pow_ui(temp_y,y,2);
		mpz_add(x2,temp_n,temp_y);
	} while(!mpz_perfect_square_p (x2));
	mpz_sqrt(x,x2);
	mpz_sub(p1,x,y);
	mpz_div_ui(p1,p1,2);
	mpz_add(q1,x,y);
	mpz_div_ui(q1,q1,2);
	gmp_printf("EXIT X2 = %Zd. p:%Zd q:%Zd\n\n", x2, p1, q1);


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


void test_valid_key(){
    // 1. Initialisations
    gmp_randstate_t state;
    gmp_randinit_default(state);

    mpz_t p, q, e, d, pm1, qm1, lambda, tmp;
    mpz_inits(p, q, e, d, pm1, qm1, lambda, tmp, NULL);

#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif

    // 2. First values for p, q, d, e
    mpz_set_str(p, "81877371507464127615356903751309751484031022336294425213947021730845873406133", 10);
    mpz_set_str(q, "115792089237316195423570985008687907853269984665640564039457584007913153127617", 10);
    mpz_set_ui(e, 3);
    mpz_set_ui(d, 17);
    int nlen = 512;

    // 3. p is not prime
    printf("Test when p is not prime...                                ");
    basic_test(p, q, e, d, nlen);

    mpz_set_str(p, "81877371507464127615356903751309751484031022336294425213947021730845873406131", 10);
    mpz_set_str(q, "115792089237316195423570985008687907853269984665640564039457584007913153127615", 10);

    // 4. q is not prime
    printf("Test when q is not prime...                                ");
    basic_test(p, q, e, d, nlen);
	
    mpz_set_str(q, "115792089237316195423570985008687907853269984665640564039457584007913153127617", 10);
    mpz_set_ui(e, 4);
	
    // 5. e is even
    printf("Test when e is even...                                     ");
    basic_test(p, q, e, d, nlen);

    // 6. e isn't prime to lambda
    mpz_set_ui(e, 17);
    mpz_sub_ui(pm1, p, 1);
    mpz_sub_ui(qm1, q, 1);
    mpz_lcm(lambda, pm1, qm1);
    printf("Test when e is not prime to lambda...                      ");
    basic_test(p, q, e, d, nlen);

    // 7. e is too small
    mpz_set_ui(e, (1 << 12) + 3);
    printf("Test when e is too small...                                ");
    basic_test(p, q, e, d, nlen);

    // 8. e is too large
    mpz_ui_pow_ui(e, 2, 256);
    mpz_add_ui(e, e, 3);
    printf("Test when e is too large...                                ");
    basic_test(p, q, e, d, nlen);

    // 9. ed = 1 mod lambda
    mpz_set_ui(e, (1 << 17) + 5);
    printf("Test when ed is not congruent to 1 modulo lambda...        ");
    basic_test(p, q, e, d, nlen);

    // 10. p is too small
    mpz_ui_pow_ui(tmp, 2, 50);
    mpz_nextprime(p, tmp);
    mpz_sub_ui(pm1, p, 1);
    mpz_lcm(lambda, pm1, qm1);
    mpz_invert(d, e, lambda);
    printf("Test when p is too small...                                ");
    basic_test(p, q, e, d, nlen);

    // 11. q is too small
    mpz_set(tmp, q);
    mpz_set(q, p);
    mpz_set(p, tmp);
    printf("Test when q is too small...                                ");
    basic_test(p, q, e, d, nlen);

    // 12. p, q are too close.
    mpz_set_str(p, "81877371507464127615356903751309751484031022336294425213947021730845873406131", 10);
    mpz_set_str(q, "81877371507464127615356903751309751484031022336294425213947021730845874454759", 10);	
    mpz_set_ui(e, (1 << 17) + 5);
    mpz_sub_ui(pm1, p, 1);
    mpz_sub_ui(qm1, q, 1);
    mpz_lcm(lambda, pm1, qm1);
    mpz_invert(d, e, lambda);
    printf("Test when p, q are too close to each other...              ");
    basic_test(p, q, e, d, nlen);
	
    // 13. Free memory
    gmp_randclear(state);
    mpz_clears(p, q, e, d, pm1, qm1, lambda, tmp, NULL);
}




void test_key_gen(){
    // 1. Initialisation
    mpz_t N, p, q, e, d;
    mpz_inits(N, p, q, e, d, NULL);
    gmp_randstate_t state;
    gmp_randinit_default(state);
	
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif

    // 2. Key generation
    RSA_generate_key(N, p, q, e, d, 256, 50, state);
    print_key(N, p, q, e, d, 256);

    printf("\n\n\n\n");

    RSA_generate_key(N, p, q, e, d, 1024, 100, state);
    print_key(N, p, q, e, d, 1024);

    // 3. Free memory
    gmp_randclear(state);
    mpz_clears(N, p, q, e, d, NULL);
}



void test_CRT_RSA(int nlen, int sec){
    // 1. Initialisation
    mpz_t p, q, N, d, e, msg, cipher, tmp;	
    gmp_randstate_t state;
    mpz_inits(p, q, N, d, e, msg, cipher, tmp, NULL);	

    gmp_randinit_default(state);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif

    // 2. Key generation
    RSA_generate_key(N, p, q, e, d, nlen, sec, state);
		
    // 3. Encryption
    mpz_urandomm(msg, state, N);
    RSA_encrypt(cipher, msg, N, e);

    // 4. Decryption tests
    printf("\nStandard decryption test...   ");
    RSA_decrypt(tmp, cipher, N, d);
    if(mpz_cmp(tmp, msg) == 0)
	printf("[OK]\n");
    else
	printf("[FAILED]\n");
	
    printf("CRT decryption test...        ");
    //mpz_set_ui(tmp, 0);
    RSA_decrypt_with_p_q(tmp, cipher, N, d, p, q);
    if(mpz_cmp(tmp, msg) == 0)
	printf("[OK]\n\n");
    else
	printf("[FAILED]\n\n");

    // 5. Free memory
    mpz_clears(p, q, N, d, e, msg, cipher, tmp, NULL);
    gmp_randclear(state);
}


void test_lengths(){
    // 1. Initialisation
    buffer_t msg;
    buffer_init(&msg, 211);
    mpz_t N, p, q, e, d;
    mpz_inits(N, p, q, e, d, NULL);
    int nlen = 256;
    gmp_randstate_t state;
    gmp_randinit_default(state);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif
	
    // 2. Key generation
    RSA_generate_key(N, p, q, e, d, nlen, 50, state);

    // 3. Message generation
    const char* str = "Wait a minute. Wait a minute, Doc. Ah... Are you telling me that you built a time machine... out of a DeLorean? The way I see it, if you're gonna build a time machine into a car, why not do it with some *style?*";
    buffer_from_string(&msg, (uchar *)str, 211);

    // 4. Test
    printf("------------- Testing lengths ----------------\n\n");
    int block_length, cipher_length, last_block_size;
    lengths(&block_length, &cipher_length, &last_block_size, &msg, N);
    printf("Plain text : \n\n%s\n\n", str);
    printf("**********************************************\n\n");
    printf("Size of the modulus N (in bits) = %ld\n",
	   mpz_sizeinbase(N, 2));
    printf("Length of the message = %ld bytes\n", msg.length);
    printf("block length = %d bytes\n", block_length);
    printf("cipher length = %d blocks\n", cipher_length);
    printf("last block size = %d bytes\n\n", last_block_size);

    int i = block_length == 31 && cipher_length == 7 &&
	last_block_size == 25;

    if(i)
	printf("[OK]\n\n");
    else
	printf("[FAILED]\n\n");

    // 5. Additional test with full last block.
    const char* str2 = "Wait a minute. Wait a minute, Doc. Ah... Are you telling me that you built a time machine... out of a DeLorean? The way I see it, if you're gonna build a time machine into a car, why not do it with some *style?*******";
    buffer_reset(&msg);
    buffer_from_string(&msg, (uchar *)str2, 217);
    lengths(&block_length, &cipher_length, &last_block_size, &msg, N);
    lengths(&block_length, &cipher_length, &last_block_size, &msg, N);
    i = ( block_length == 31 && cipher_length == 7 &&
	  last_block_size == 31 );

    printf("**********************************************\n\n");
    printf("Second test (slightly longer plain text): \n");
    printf("Length of the message = %ld bytes\n", msg.length);
    printf("block length = %d bytes\n", block_length);
    printf("cipher length = %d blocks\n", cipher_length);
    printf("last block size = %d bytes\n\n", last_block_size);
	
    if(i)
	printf("[OK]\n\n");
    else
	printf("[FAILED]\n\n");
	
    // 6. Free memory
    gmp_randclear(state);
    mpz_clears(N, p, q, e, d, NULL);
    buffer_clear(&msg);
}


	

void test_text(int full){
    // 0. Printing
    printf("---------------- Test text encryption ");
    if(full)
	printf("and decryption ");
    printf("----------------\n");

    // 1. Initialisation	
    mpz_t p, q, N, d, e;
    mpz_t* cipher;
    buffer_t msg;
    int block_length, cipher_length, last_block_size, nlen = 256;
    gmp_randstate_t state;
    mpz_inits(p, q, N, d, e, NULL);
    gmp_randinit_default(state);
    buffer_init(&msg, 211);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif

    // 2. Message generation
    const char* str = "Wait a minute. Wait a minute, Doc. Ah... Are you telling me that you built a time machine... out of a DeLorean?\nThe way I see it, if you're gonna build a time machine into a car, why not do it with some *style?*";
    buffer_from_string(&msg, (uchar *)str, 211);
    printf("\nPlain text :\n%s\n\n", str);
	
    // 3. Key generation
    RSA_generate_key(N, p, q, d, e, nlen, 50, state);
#if DEBUG
    print_key(N, p, q, e, d, nlen);
#endif
	
    // 4. Defining sizes and number of blocks for the cipher text
    lengths(&block_length, &cipher_length, &last_block_size,
	    &msg, N);
	
    // 4. Init target for ciphertext
    cipher = (mpz_t*)malloc(sizeof(mpz_t) * cipher_length);
    mpz_t* cursor = cipher;
    for(int i = 0; i < cipher_length; i++, cursor++)
	mpz_init(*cursor);
		

    // 5. Encryption
    RSA_text_encrypt(cipher, block_length, cipher_length,
		     last_block_size, &msg, N, e);

#if DEBUG
    printf("Encrypted text :\n");
    cursor = cipher;
    for(int i = 0; i < cipher_length; i++, cursor++)
	gmp_printf("%Zd\n", *cursor);
    printf("\n");
#endif

    // 6. (optionnal) Decryption
    if(full){
	printf("---------------- Decryption test ------------------\n\n");
	uchar *foo;
	buffer_t decrypted;
	buffer_init(&decrypted, 211);
	RSA_text_decrypt(&decrypted, cipher, cipher_length,
			 block_length, last_block_size, N, d);
	foo = string_from_buffer(&decrypted);
	printf("Decrypted text :\n%s\n\n", foo);
	free(foo);
	       
	if(buffer_equality(&decrypted, &msg))
	    printf("[OK]\n\n");
	else
	    printf("[FAILED]\n\n");
	buffer_clear(&decrypted);
    }
	
    // 7. Free memory
    cursor = cipher;
    for(int i = 0; i < cipher_length; i++, cursor++)
	mpz_clear(*cursor);
    free(cipher);
    buffer_clear(&msg);
    mpz_clears(p, q, N, d, e, NULL);
    gmp_randclear(state);
}

 
 
	
void test_Hastad(int exponent, int nlen){
    // 0. Definitions
    printf("------------- Test Hastad attack --------------\n\n");
    mpz_t *moduli = (mpz_t *)malloc(sizeof(mpz_t) * exponent);
    mpz_t *cipher_texts = (mpz_t *)malloc(sizeof(mpz_t) * exponent);
    gmp_randstate_t state;
    mpz_t msg, N, e, decrypted, prod, g;

    // 1. Initialisation
    mpz_inits(msg, N, e, decrypted, prod, g, NULL);
    mpz_set_ui(e, exponent);
    mpz_set_ui(prod, 1);
    gmp_randinit_default(state);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif
    mpz_t* cursor_mod = moduli;
    mpz_t* cursor_ciph = cipher_texts;
    mpz_urandomb(msg, state, nlen - 1);

    gmp_printf("Plain text : \n%Zd\n\n", msg);
		
    // 2. Creating the pairs (moduli, cipher text)
    for(int i = 0; i < exponent; i++, cursor_mod++, cursor_ciph++){
	mpz_inits(*cursor_mod, *cursor_ciph, NULL);
	do{
	    RSA_dummy_generate_key(*cursor_mod, e, nlen, state);
	    mpz_gcd(g, *cursor_mod, prod);
	}while(mpz_cmp_ui(g, 1) != 0);
		
	RSA_encrypt(*cursor_ciph, msg, *cursor_mod, e);
	assert(mpz_cmp(msg, *cursor_mod) < 0);
	mpz_mul(prod, prod, *cursor_mod);
#if DEBUG
	gmp_printf("%d-th encrypted = %Zd.\n\n", i,
		   *cursor_ciph);
#endif
    }
    printf("Key generation and encryption done.\n\n");
    fflush(stdout);
	
    // 3. Hastad
    Hastad(decrypted, cipher_texts, moduli, exponent);

    if(mpz_cmp(decrypted, msg) == 0)
	printf("[TEST OK]\n\n");
    else
	printf("[TEST FAILED]\n\n");
	
	
    // 3. Free memory
    mpz_clears(msg, N, e, decrypted, prod, g, NULL);
    cursor_mod = moduli;
    cursor_ciph = cipher_texts;
    for(int i = 0; i < exponent; i++, cursor_mod++, cursor_ciph++)
	mpz_clears(*cursor_mod, *cursor_ciph, NULL);
    free(moduli);
    free(cipher_texts);
    gmp_randclear(state);
}


static void usage(char *s, int ntests){
    fprintf(stderr, "Usage: %s <test_number in 1..%d>\n", s, ntests);
}

int main(int argc, char *argv[]){
    if(argc == 1){
	usage(argv[0], 6);
	return 0;
    }
    int n = atoi(argv[1]);
    switch(n){
    case 0:
/* to be filled in */
	test_encrypt_decrypt();
	break;
    case 1:
	test_key_gen();
	break;
    case 2:
	test_CRT_RSA(512, 100);
	break;
    case 3:
	test_lengths();
	break;
    case 4:
	test_text(0);
	break;
    case 5:
	test_text(1);
	break;
    case 6:
	test_Hastad(3, 32);
	break;
    default:
	usage(argv[0], 6);
    }
    return 0;
}
