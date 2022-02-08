#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gmp.h"

#include "base64.h"
#include "buffer.h"
#include "bits.h"
#include "random.h"

#include "operating_modes.h"
#include "aes.h"

#include "version.h"

#include "network.h"
#include "dh.h"
#include "channel.h"

#include "certificate.h"

#define DEBUG 2

void try_send(const char *host, const int port){
    network_send(host, port, "Hello!");
    network_send(host, port, "I am the client sending to the server.");
}

void try_aes(){
    uchar *msg = (uchar*)"It's a long way to Tipperary!!";
    buffer_t clear, encrypted, key, IV, decrypted;
    mpz_t gab;

    buffer_init(&clear, strlen((char*)msg));
    buffer_init(&encrypted, 32);
    buffer_init(&key, BLOCK_LENGTH);
    buffer_init(&IV, BLOCK_LENGTH);
	
    mpz_init_set_str(gab, "12345612345678907890", 10);
    AES128_key_from_number(&key, gab);
    buffer_random(&IV, BLOCK_LENGTH);
    buffer_from_string(&clear, msg, strlen((char*)msg));

    aes_CBC_encrypt(&encrypted, &clear, &key, &IV, 's');

    buffer_init(&decrypted, 32);
    aes_CBC_decrypt(&decrypted, &encrypted, &key, 's');
    buffer_print(stdout, &decrypted);
    printf("\n");
	
    buffer_clear(&clear);
    buffer_clear(&encrypted);
    buffer_clear(&decrypted);
    buffer_clear(&key);
    buffer_clear(&IV);
    mpz_clear(gab);
}

void send_with_aes(const char *host, const int port, uchar *msg, mpz_t gab){
/* to be filled in */
}

void try_send_aes(const char *host, const int port){
    uchar *msg = (uchar*)"It's a long way to Tipperary";
    mpz_t gab;

    mpz_init_set_str(gab, "12345612345678907890", 10);
    send_with_aes(host, port, msg, gab);
    mpz_clear(gab);
}

void prepare_cipher(buffer_t *encrypted, buffer_t *clear, buffer_t *key){
    buffer_t IV;
    buffer_init(&IV, BLOCK_LENGTH);
    buffer_random(&IV, BLOCK_LENGTH);
    aes_CBC_encrypt(encrypted, clear, key, &IV, 's');
    buffer_clear(&IV);
}

void CaseDH(const char *server_host, const int server_port){
/* to be filled in */
}

void CaseSTS(const char *server_host, const int server_port, 
	     certificate_t *CA, mpz_t NA, mpz_t dA, mpz_t N_aut, mpz_t e_aut){
/* to be filled in */
}

void Usage(char *s){
    fprintf(stderr, "Usage: %s\n", s);
    fprintf(stderr, "    server_host server_port "
	    "[try_send|try_send_aes|try_DH]\n");
    fprintf(stderr, "or\n");
    fprintf(stderr, "    server_host server_port client_certificate.txt"
	    " client_sk.txt auth_pk.txt\n");
    fprintf(stderr, "In this last version, STS is started\n");
    
}

int main(int argc, char *argv[])
{
    const char *server_host;
    int client_port = 1789, server_port;
#ifndef USE_DH
    FILE *in;
    certificate_t CA;
    mpz_t NA, dA, N_aut, e_aut;
#endif
    //    try();
    if(argc == 1){
	Usage(argv[0]);
	return 0;
    }
    printf("Client for version %s\n", VERSION);
    server_host = argv[1];
    server_port = atoi(argv[2]);
    network_init(client_port);
    if(argc > 3 && strcmp(argv[3], "try_send") == 0){
	try_send(server_host, server_port);
	return 0;
    }
    if(argc > 3 && strcmp(argv[3], "try_send_aes") == 0){
	try_send_aes(server_host, server_port);
	return 0;
    }
    if(argc > 3 && strcmp(argv[3], "try_aes") == 0){
	try_aes();
	return 0;
    }
    if(argc > 3 && strcmp(argv[3], "try_DH") == 0){
	printf("Go for DH: \n");
	CaseDH(server_host, server_port);
	return 0;
    }
	
    if((in = fopen(argv[3], "r")) == NULL){
	perror(argv[3]);
	return -1;
    }
    init_certificate(&CA);
    extract_certificate(&CA, in);
    fclose(in);
    print_certificate(&CA);
    if((in = fopen(argv[4], "r")) == NULL){
	perror(argv[4]);
	return -1;
    }
    mpz_inits(NA, dA, N_aut, e_aut, NULL);
    read_secret_keys(NA, dA, in);
    fclose(in);
    if((in = fopen(argv[5], "r")) == NULL){
	perror(argv[5]);
	return -1;
    }
    read_public_keys(N_aut, e_aut, in);
    fclose(in);
#if DEBUG > 0
    gmp_printf("N_aut:=%Zd; e_aut:=%Zd;\n", N_aut, e_aut);
#endif
    printf("valid certificate for CA-client? %d\n",
	   valid_certificate(&CA, N_aut, e_aut));
    CaseSTS(server_host, server_port, &CA, NA, dA, N_aut, e_aut);
    clear_certificate(&CA);
    mpz_clears(NA, dA, N_aut, e_aut, NULL);
    network_clear();
    return 0;
}
