/**************************************************************/
/* diffusion.c                                                */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification October 12, 2018                         */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "random.h"
#include "bits.h"
#include "aes.h"
#include "diffusion.h"


double diffusion_test_for_key(buffer_t *key, int nr_tests){
	srand(random_seed());
	int length = key->length;
	int bit_length = key->length * BYTE_SIZE;
	double result = 0;
	// 1. Intialisation
	buffer_t msg, key2, encrypted, encrypted2;
	buffer_init(&msg, length);
	buffer_init(&key2, length);
	buffer_init(&encrypted, length);
	buffer_init(&encrypted2, length);
	int position;

    // 2. Perform
	
	// Generate one message
	uchar plain[17] = "Hello my friends";
    buffer_from_string(&msg, plain, length);
    
	// perform nr_tests times
	for(int i =0;i<nr_tests;i++){
	    aes_block_encrypt(&encrypted, &msg, key);
		//aes_key_generation(&key2, length);
	    position = rand() % bit_length;
		buffer_flip_bit(&key2,key,position);
		aes_block_encrypt(&encrypted2, &msg, &key2);
		for(int j = 0; j< encrypted2.length; j++){
		    *(encrypted2.tab) ^= *(encrypted.tab);
			encrypted2.tab ++;
			encrypted.tab++;
		}
		encrypted2.tab -= encrypted2.length;
		encrypted.tab -= encrypted.length; 
		result += (double) HammingWeight(&encrypted2);
	}

	// 3. Free memory
	buffer_clear(&msg);
	buffer_clear(&key2);	
	buffer_clear(&encrypted);	
	buffer_clear(&encrypted2);	
	return result / nr_tests;
}


double diffusion_test_for_msg(buffer_t *msg, int nr_tests){
	srand(random_seed());
	int length = msg->length;
	int bit_length = msg->length * BYTE_SIZE;
	double result = 0;
    
	// 1. Intialisation

	buffer_t msg2, key, encrypted, encrypted2;
	buffer_init(&msg2, length);
	buffer_init(&key, length);
	buffer_init(&encrypted, length);
	buffer_init(&encrypted2, length);
    int position;

	// 2. Perform
    
	for(int i = 0; i< nr_tests; i++){
	    // Generate the key
	    aes_key_generation(&key, length);
	    aes_block_encrypt(&encrypted, msg, &key);
        position = rand() % bit_length;
        buffer_flip_bit(&msg2,msg,position);
        aes_block_encrypt(&encrypted2, &msg2, &key);
        for(int j = 0; j< encrypted2.length; j++){
			*(encrypted2.tab) ^= *(encrypted.tab);
            encrypted2.tab ++;
            encrypted.tab++;
		}
		encrypted2.tab -= encrypted2.length;
		encrypted.tab -= encrypted.length;
		
		result += (double) HammingWeight(&encrypted2);
    }
    // 3. Free memory
	buffer_clear(&msg2);
	buffer_clear(&key);
	buffer_clear(&encrypted);
	buffer_clear(&encrypted2);

	return result / nr_tests;
}


double diffusion_test_nr_rounds(buffer_t *msg, int Nr, int nr_tests){
	srand(random_seed());
	int length = msg->length;
	int bit_length = msg->length * BYTE_SIZE;
	double result = 0;
    
    // 1. Intialisation

    buffer_t msg2, key, encrypted, encrypted2;
    buffer_init(&msg2, length);
    buffer_init(&key, length);
    buffer_init(&encrypted, length);
    buffer_init(&encrypted2, length);
    int position;

    // 2. Performi
    
	for(int i = 0; i< nr_tests; i++){
		// Generate the key
		aes_key_generation(&key, length);
		// encryption
		aes_block_encrypt_few_rounds(&encrypted, msg, &key, Nr);
		position = rand() % bit_length;
		// flip
		buffer_flip_bit(&msg2,msg,position);
		aes_block_encrypt_few_rounds(&encrypted2, &msg2, &key,Nr);
		for(int j = 0; j< encrypted2.length; j++){
			*(encrypted2.tab) ^= *(encrypted.tab);
			encrypted2.tab ++;
			encrypted.tab++;
		}
		encrypted2.tab -= encrypted2.length;
		encrypted.tab -= encrypted.length;
		result += (double) HammingWeight(&encrypted2);
	}

	
	
    // 3. Free memory
    buffer_clear(&msg2);
    buffer_clear(&key);
    buffer_clear(&encrypted);
    buffer_clear(&encrypted2);

	return result / nr_tests;
}



