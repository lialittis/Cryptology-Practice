/**************************************************************/
/* testEx1.c                                                  */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification September 30, 2020 (FJM)                 */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "bits.h"



void mini_test1(uchar a, int position, uchar reference_bit){
    uchar bit = getBit(a, position);
    printf("--- Number %d has binary representation : ", a);
    printBin(&a, 1);
    printf("\n    getBit(%d, %d) is :  %d\n\n", a, position, bit);
    if(reference_bit == bit)
	printf("    [OK]\n\n");
    else{
	printf("    [FAILED] -- Result should be %d.\n\n", reference_bit);
    }
}

void test1(){
    printf("************ Testing getBit ****************\n\n");
    uchar a = 113;
    uchar b = 211;
    uchar c = 159;
    uchar d = 27;

    mini_test1(a, 0, 1);
    mini_test1(a, 3, 0);
    mini_test1(b, 2, 0);
    mini_test1(a, 5, 1);
    mini_test1(c, 3, 1);
    mini_test1(c, 7, 1);
    mini_test1(d, 0, 1);
}


void mini_test2(uchar a, uchar reference, int position, uchar bit){
    uchar result = setBit(a, position, bit);
    printf("--- Number %d has binary representation :       ", a);
    printBin(&a, 1);
    printf("\n    setBit(%d, %d, %d) is :  %d  or, in binary : ",
	   a, position, bit, result);
    if(result < 100)
	printf(" ");
    if(result < 10)
	printf(" ");
    printBin(&result, 1);
    printf("\n");
    if(result == reference)
	printf("    [OK]\n\n");
    else{
	printf("    [FAILED] -- Expected result is %d : ",
	       reference);
	printBin(&reference, 1);
	printf("\n\n");
    }
}


void test2(){
    printf("************ Testing setBit ****************\n\n");

    mini_test2(208, 240, 5, 1);
    mini_test2(43, 43, 2, 0);
    mini_test2(9, 9, 0, 1);
    mini_test2(9, 137, 7, 1);
}


void mini_test3(int h_a, int reference){
    printf("    Hamming weight of a = %d.\n", h_a);
    if(h_a == reference)
	printf("    [OK]\n\n");
    else
	printf("    [FAILED] -- expected result is %d.\n\n", reference);

}


void test3(){
    printf("************ Testing Hamming weight ****************\n\n");
    uchar a[5] = {113, 2, 19, 48, 157};
    uchar b[8] = {211, 201, 199, 0, 8, 11, 37, 255};
    buffer_t buf_1, buf_2;
    buffer_init(&buf_1, 5);
    buffer_init(&buf_2, 8);

    buffer_from_string(&buf_1, a, 5);
    buffer_from_string(&buf_2, b, 8);
	
    printf("--- a = ");
    printBin(buf_1.tab, 5);
    printf("\n");
    int h_a = HammingWeight(&buf_1);
    mini_test3(h_a, 15);
	
    printf("--- b = ");
    printBin(buf_2.tab, 8);
    printf("\n");
    int h_b = HammingWeight(&buf_2);
    mini_test3(h_b, 29);

    buffer_clear(&buf_1);
    buffer_clear(&buf_2);
}



void test4(){
    printf("************ Testing oneTimePad ****************\n\n");
    uchar msg[4] = {3, 223, 17, 29};
    uchar key[4] = {1, 2, 4, 32};
    uchar verif[4] = {2, 221, 21, 61};

    buffer_t buf_msg, buf_key, buf_encrypted, buf_verif;
    buffer_init(&buf_msg, 4);
    buffer_init(&buf_key, 4);
    buffer_init(&buf_encrypted, 4);
    buffer_init(&buf_verif, 4);

    for(int i = 0; i < 4; i++){
	buffer_append_uchar(&buf_msg, msg[i]);
	buffer_append_uchar(&buf_key, key[i]);
	buffer_append_uchar(&buf_verif, verif[i]);
    }

    oneTimePad(&buf_encrypted, &buf_msg, &buf_key);

    printf("Plain text : ");
    printDec(buf_msg.tab, buf_msg.length);
	
    printf("\nKey        : ");
    printDec(buf_key.tab, buf_key.length);
	
    printf("\n\nIn binary.\nPlain :\n");
    printBin(buf_msg.tab, buf_msg.length);

    printf("\nKey :\n");
    printBin(buf_key.tab, buf_key.length);

    printf("\nEncrypted : \n");
    printBin(buf_encrypted.tab, buf_encrypted.length);

    if(buffer_equality(&buf_encrypted, &buf_verif))
	printf("\n\nTest one time pad [OK].\n\n");
    else
	printf("\n\nTest one time pad [FAILED].\n\n");

    buffer_clear(&buf_msg);
    buffer_clear(&buf_key);
    buffer_clear(&buf_encrypted);
    buffer_clear(&buf_verif);
}



void test5(){
    printf("************ Testing oneTimePad on a string ****************\n\n");
    uchar plain[8] = "Bonjour";
    uchar key[7] = {1, 2, 3, 4, 5, 6, 7};
    uchar verif[8] = "Cmmnjsu";
	
    buffer_t buf_plain, buf_key, buf_encrypted, buf_verif;
    buffer_init(&buf_plain, 7);
    buffer_init(&buf_key, 7);
    buffer_init(&buf_encrypted, 7);
    buffer_init(&buf_verif, 7);

    buffer_from_string(&buf_plain, plain, 7);


    buffer_from_string(&buf_verif, verif, 7);
	
    for(int i = 0; i < 7; i++)
	buffer_append_uchar(&buf_key, key[i]);
	
	
    printf("Plain text : %s\n\nPlain Text (binary) : \n", plain);
    printBin(plain, 7);
    printf("\nKey : \n");
    printBin(key, 7);
	
    oneTimePad(&buf_encrypted, &buf_plain, &buf_key);
    printf("\nEncryption : \n");
    printBin(buf_encrypted.tab, 7);
	
    uchar *encrypted_string = string_from_buffer(&buf_encrypted);
    printf("\n\nCipher Text: %s\n\n", encrypted_string);
    if(buffer_equality(&buf_verif, &buf_encrypted))
	printf("[SUCCESS]\n\n");
    else
	printf("[FAILED]\n\n");

	
    free(encrypted_string);
    buffer_clear(&buf_plain);
    buffer_clear(&buf_key);
    buffer_clear(&buf_encrypted);
    buffer_clear(&buf_verif);
}




void usage(char *s){
    fprintf(stderr, "Usage: %s <test_number>\n", s);
}


int main(int argc, char *argv[]){
    if(argc == 1){
	usage(argv[0]);
	return 0;
    }
    int n = atoi(argv[1]);

    switch(n){
    case 1:
	test1();
	break;
    case 2:
	test2();
	break;
    case 3:
	test3();
	break;
    case 4:
	test4();
	break;
    case 5:
	test5();
	break;
    }
    return 0;
}
