/**************************************************************/
/* test_for_prof.c                                            */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification October 12, 2018                         */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "random.h"
#include "bits.h"
#include "aes.h"
#include "sha3.h"
#include "operating_modes.h"

void test_flip_bit(){
	buffer_t key, modified_key;
	buffer_init(&key, BLOCK_LENGTH);
	buffer_init(&modified_key, BLOCK_LENGTH);
	aes_key_generation(&key, BLOCK_LENGTH);
	printf("Key : ");
	buffer_print_int(stdout, &key);
	printf("\n\n");

	// Tests when position is out of range
	buffer_flip_bit(&modified_key, &key, 157);

	printf("\n");

	// Tests a good position
	printBin(key.tab, 5);
	printf("\n");
	buffer_flip_bit(&modified_key, &key, 10);
	printBin(modified_key.tab, 5);
	printf("\n");	
	
	buffer_clear(&key);
	buffer_clear(&modified_key);	
}

void test_aes(){
	// 1. Initialisation
	buffer_t msg, key, encrypted, decrypted;
	buffer_init(&msg, BLOCK_LENGTH);
	buffer_init(&key, BLOCK_LENGTH);
	buffer_init(&encrypted, BLOCK_LENGTH);
	buffer_init(&decrypted, BLOCK_LENGTH);

	// 2. Message generation
	uchar plain[17] = "Hello my friends";
	buffer_from_string(&msg, plain, BLOCK_LENGTH);
	printf("Plain text : %s\n\n", plain);

	// 3. Key generation
	aes_key_generation(&key, BLOCK_LENGTH);
	printf("Key : ");
	buffer_print_int(stdout, &key);
	printf("\n\n");

	// 4. Encryption
	aes_block_encrypt(&encrypted, &msg, &key);
	printf("Encrypted : %s\n\n", string_from_buffer(&encrypted));

	// 5. Decryption
	aes_block_decrypt(&decrypted, &encrypted, &key);
	printf("Decrypted : %s\n\n", string_from_buffer(&decrypted));

	// 6. Verification
	if(buffer_equality(&msg, &decrypted))
		printf("[OK]\n\n");
	else
		printf("[FAILED]\n\n");
	
	// 7. Free memory
	buffer_clear(&msg);
	buffer_clear(&key);
	buffer_clear(&encrypted);
	buffer_clear(&decrypted);
}


void test_keccak(){
	char *in = (char *)"If my calculations are correct, when this baby hits 88 miles per hour... you're gonna see some serious shit.";
	buffer_t buf, hash;
	buffer_init(&buf, 100);
	buffer_init(&hash, 32);
	
	buffer_from_string(&buf, (uchar *)in, strlen(in));
	buffer_hash(&hash, 16, &buf);
	printf("\n\nInput : %s\n\n", in);
	printf("Output : ");
	buffer_print_int(stdout, &hash);
	printf("\n\n");
	buffer_clear(&hash);
	buffer_clear(&buf);
}


void test_pad(char mode){
	buffer_t msg1, msg2, padded1, padded2;
	buffer_init(&msg1, 16);
	buffer_init(&msg2, 21);
	buffer_init(&padded1, 32);
	buffer_init(&padded2, 32);
	
	buffer_random(&msg1, 16);
	buffer_random(&msg2, 21);

	printf("Message 1 : ");
	buffer_print_int(stdout, &msg1);
	printf("\n\n");
	printf("Message 2 : ");
	buffer_print_int(stdout, &msg2);
	printf("\n\n");

	pad(&padded1, &msg1, mode);
	pad(&padded2, &msg2, mode);

	printf("Message 1 padded : ");
	buffer_print_int(stdout, &padded1);
	printf("\n\n");
	printf("Message 2 padded : ");
	buffer_print_int(stdout, &padded2);
	printf("\n\n");

	
	buffer_clear(&msg1);
	buffer_clear(&msg2);
	buffer_clear(&padded1);
	buffer_clear(&padded2);
}


void test_extract(char mode){
	buffer_t msg, padded, extracted;
	buffer_init(&msg, 100);
	buffer_init(&padded, 100);
	buffer_init(&extracted, 100);

	buffer_from_string(&msg, (uchar *)"Is there a problem with Earth's gravitational pull in the future? Why is everything so heavy?", -1);
	//
	printf("[test_extract] : Length = %d.\n", (int)msg.length);
	pad(&padded, &msg, mode);
	printf("[test_extract] : Length padded = %d.\n", (int)padded.length);

	extract(&extracted, &padded, mode);
	printf("Message :   %s\n", string_from_buffer(&msg));
	printf("Extracted : %s\n", string_from_buffer(&extracted));

	if(buffer_equality(&msg, &extracted))
		printf("\n\n[OK]\n\n");
	else
		printf("\n\n[FAILED]\n\n");
	buffer_clear(&msg);	
	buffer_clear(&padded);
	buffer_clear(&extracted);
}


void test_raw_CBC_encrypt(){
	// 1. Initialisation
	buffer_t key, msg, padded, encrypted, IV;
	buffer_init(&key, BLOCK_LENGTH);
	buffer_init(&IV, BLOCK_LENGTH);
	buffer_init(&msg, BLOCK_LENGTH);
	buffer_init(&padded, BLOCK_LENGTH);
	buffer_init(&encrypted, BLOCK_LENGTH);

	// 2. Fill in buffers
	aes_key_generation(&key, BLOCK_LENGTH);
	buffer_random(&IV, BLOCK_LENGTH);
	buffer_from_string(&msg, (uchar *)"Is there a problem with Earth's gravitational pull in the future? Why is everything so heavy?", -1);
	pad(&padded, &msg, 's');

	// 3. Encrypt
	aes_raw_CBC_encrypt(&encrypted, &padded, &key, &IV);

	// 4. Print
	printf("Plain : %s\n\n", string_from_buffer(&msg));
	printf("Encrypted (length = %d): %s\n\n", (int)encrypted.length,
		   string_from_buffer(&encrypted));
	
	// 5. Free Memory
	buffer_clear(&key);
	buffer_clear(&IV);
	buffer_clear(&msg);
	buffer_clear(&padded);
	buffer_clear(&encrypted);	
}


void test_raw_CBC_decrypt(){
	// 1. Initialisation
	buffer_t key, msg, padded, encrypted, decrypted, extracted, IV;
	buffer_init(&key, BLOCK_LENGTH);
	buffer_init(&IV, BLOCK_LENGTH);
	buffer_init(&msg, BLOCK_LENGTH);
	buffer_init(&padded, BLOCK_LENGTH);
	buffer_init(&encrypted, BLOCK_LENGTH);
	buffer_init(&decrypted, BLOCK_LENGTH);
	buffer_init(&extracted, BLOCK_LENGTH);

	// 2. Fill in buffers
	aes_key_generation(&key, BLOCK_LENGTH);
	buffer_random(&IV, BLOCK_LENGTH);
	buffer_from_string(&msg, (uchar *)"Is there a problem with Earth's gravitational pull in the future? Why is everything so heavy?", -1);
	pad(&padded, &msg, 's');

	// 3. Encrypt
	aes_raw_CBC_encrypt(&encrypted, &padded, &key, &IV);

	// 4. Print
	printf("Plain (length = %d): %s\n\n", (int)msg.length, string_from_buffer(&msg));
	printf("Encrypted (length = %d): %s\n\n", (int)encrypted.length,
		   string_from_buffer(&encrypted));

	// 5. Decrypt and extract
	aes_raw_CBC_decrypt(&decrypted, &encrypted, &key);
	extract(&extracted, &decrypted, 's');
	
	// 6. Print again
	printf("Decrypted (length = %d): %s\n\n", (int)extracted.length,
		   string_from_buffer(&extracted));

	// 7. Check
	if(buffer_equality(&extracted, &msg))
		printf("\n\n[OK]\n\n");
	else
		printf("\n\n[FAILED]\n\n");
	
	// 8. Free Memory
	buffer_clear(&key);
	buffer_clear(&IV);
	buffer_clear(&msg);
	buffer_clear(&padded);
	buffer_clear(&encrypted);	
	buffer_clear(&decrypted);	
	buffer_clear(&extracted);	
}


void test_CBC_en_and_decrypt(){
	// 1. Initialisation
	buffer_t key, msg, encrypted, decrypted, IV;
	buffer_init(&key, BLOCK_LENGTH);
	buffer_init(&IV, BLOCK_LENGTH);
	buffer_init(&msg, BLOCK_LENGTH);
	buffer_init(&encrypted, BLOCK_LENGTH);
	buffer_init(&decrypted, BLOCK_LENGTH);

	// 2. Fill in buffers
	aes_key_generation(&key, BLOCK_LENGTH);
	buffer_random(&IV, BLOCK_LENGTH);
	buffer_from_string(&msg, (uchar *)"Is there a problem with Earth's gravitational pull in the future? Why is everything so heavy?", -1);
	//buffer_random(&msg, rand() % 1024);

	// 3. Encrypt
	aes_CBC_encrypt(&encrypted, &msg, &key, &IV, 's');

	// 4. Print
	printf("Plain (length = %d): %s\n\n", (int)msg.length, string_from_buffer(&msg));
	printf("Encrypted (length = %d): %s\n\n", (int)encrypted.length,
		   string_from_buffer(&encrypted));

	// 5. Decrypt
	aes_CBC_decrypt(&decrypted, &encrypted, &key, 's');
	
	// 6. Print again
	printf("Decrypted (length = %d): %s\n\n", (int)decrypted.length,
		   string_from_buffer(&decrypted));

	// 7. Check
	if(buffer_equality(&decrypted, &msg))
		printf("\n\n[OK]\n\n");
	else
		printf("\n\n[FAILED]\n\n");
	
	// 8. Free Memory
	buffer_clear(&key);
	buffer_clear(&IV);
	buffer_clear(&msg);
	buffer_clear(&encrypted);	
	buffer_clear(&decrypted);	
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
		test_flip_bit();
		break;
	case 2:
		test_aes();
		break;
	case 3:
		test_keccak();
		break;
	case 4:
		test_pad('s');
		break;
	case 5:
		test_pad('R');
		break;
	case 6:
		test_extract('s');
		break;
	case 7:
		test_extract('R');
		break;
	case 8:
		test_raw_CBC_encrypt();
		break;
	case 9:
		test_raw_CBC_decrypt();
		break;
	case 10:
		test_CBC_en_and_decrypt();
		break;
	}
	return 0;
}
