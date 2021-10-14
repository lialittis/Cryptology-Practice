/**************************************************************/
/* operating_modes.h                                          */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification October 8, 2018                          */
/**************************************************************/

/* Definitions */
#define HASH_LENGTH 32

/* Functions */
void pad(buffer_t *padded, buffer_t *in, char mode);
void extract(buffer_t *out, buffer_t *padded, char mode);
void aes_raw_CBC_encrypt(buffer_t *encrypted, buffer_t *in, buffer_t *key,
						 buffer_t *IV);
void aes_raw_CBC_decrypt(buffer_t *decrypted, buffer_t *in, buffer_t *key);
void aes_CBC_encrypt(buffer_t *encrypted, buffer_t *plain, buffer_t *key,
					 buffer_t *IV, char mode);
void aes_CBC_decrypt(buffer_t *decrypted, buffer_t *encrypted, buffer_t *key,
					 char mode);
