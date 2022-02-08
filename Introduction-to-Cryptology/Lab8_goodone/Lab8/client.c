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
    network_send(host, port, "This is the worst world !");
    network_send(host, port, "......");
    network_send(host, port, "But, it's also the best world !");
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
    /* Done */
   
    char buf[1024]; 
    printf("Sending: AES\n");
    buffer_t clear,encrypted,key,IV,out;
    // mpz_t msg_c;

    buffer_init(&clear, strlen((char*)msg));
    buffer_init(&encrypted, 32);
    buffer_init(&key, BLOCK_LENGTH);
    buffer_init(&IV, BLOCK_LENGTH);
    buffer_init(&out, BLOCK_LENGTH);
    // mpz_init(msg_c);    

    AES128_key_from_number(&key, gab);
    buffer_random(&IV, BLOCK_LENGTH);
    buffer_from_string(&clear, msg, strlen((char*)msg));

    aes_CBC_encrypt(&encrypted, &clear, &key, &IV, 's');

    // buffer_print(stdout, &encrypted);
    // printf("\n");

    buffer_to_base64(&out,&encrypted);
    printf("Sending: ");
    buffer_print(stdout, &out);
    printf("\n");

    msg_export_string(buf, "Send with aes: ", (char*)string_from_buffer(&out));
    //printf(tmp);
    network_send(host,port,buf);
    
    buffer_clear(&clear);
    buffer_clear(&encrypted);
    buffer_clear(&key);
    buffer_clear(&IV);
    buffer_clear(&out);


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
    /* DONE */
    uchar *msg = (uchar*)"Nice job ! You solve the DH problem !";
    gmp_randstate_t state;
    char buf[1024], *tmp;
    mpz_t p, g, gb, a, ga, gab;

    mpz_inits(p, g, gb, a, ga, gab, NULL);
    buffer_t clear,encrypted,key,IV,out;

    buffer_init(&clear, strlen((char*)msg));
    buffer_init(&encrypted, 32);
    buffer_init(&key, BLOCK_LENGTH);
    buffer_init(&IV, BLOCK_LENGTH);
    buffer_init(&out, BLOCK_LENGTH);


    channel_init(p, g);
    gmp_randinit_default(state);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif
	
    /* Bob <-- g^a -- Alice */
    DH_init(a, state);
    mpz_powm_sec(ga, g, a, p);
    msg_export_mpz(buf, "DH: ALICE/BOB CONNECT1 ", ga, 0);
    network_send(server_host, server_port, buf);

    /* Bob -- g^b --> Alice */
    tmp = network_recv(-1);
    /*
    while(1){
        tmp = network_recv(-1);
        if(msg_import_mpz(gb, tmp, "DH: BOB/ALICE CONNECT2 ", 0) <= 0)
            continue;
        else
    }
    */
    if(msg_import_mpz(gb, tmp, "DH: BOB/ALICE CONNECT2 ", 0) <= 0)
        return;
    free(tmp);
#if DEBUG > 0
    gmp_printf("ga=g^%Zd=%Zd, gb=%Zd\nbuf=%s\n", a, ga, gb, buf);
#endif
    mpz_powm_sec(gab, gb, a, p);
    gmp_printf("gab=%#Zx\n",gab);
	
    /* sending a message encrypted with AES key = gab */
    // send_with_aes(server_host,server_port,msg,gab);
    printf("Sending message encrypted with AES key\n");

    AES128_key_from_number(&key, gab);
    buffer_random(&IV, BLOCK_LENGTH);
    buffer_from_string(&clear, msg, strlen((char*)msg));

    aes_CBC_encrypt(&encrypted, &clear, &key, &IV, 's');

    // buffer_print(stdout, &encrypted);
    // printf("\n");

    buffer_to_base64(&out,&encrypted);
    printf("Sending: ");
    buffer_print(stdout, &out);
    printf("\n");

    msg_export_string(buf, "DH: ALICE/BOB CONNECT3 ", (char*)string_from_buffer(&out));
    //printf(tmp);
    network_send(server_host,server_port,buf);
    
    buffer_clear(&clear);
    buffer_clear(&encrypted);
    buffer_clear(&key);
    buffer_clear(&IV);
    buffer_clear(&out);
    
    mpz_clears(p, g, gb, a, ga, gab, NULL); 
}

void CaseSTS(const char *server_host, const int server_port, 
	     certificate_t *CA, mpz_t NA, mpz_t dA, mpz_t N_aut, mpz_t e_aut){
    /*DONE*/
    /*I am Alice, I am client*/
    certificate_t CB;
    gmp_randstate_t state;
    char buf[1024], *tmp, *from_Bob;
    mpz_t p, g, ga, a, gb, gab, signA, tmpB, sigmaB;
    buffer_t encrypted, decrypted, clear, y, key, IV, in, out;
    mpz_inits(p, g, gb, a, ga, gab, signA, tmpB, sigmaB, NULL);
    channel_init(p, g);
    gmp_randinit_default(state);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif

    buffer_init(&key, BLOCK_LENGTH);
    buffer_init(&IV, BLOCK_LENGTH);
    buffer_random(&IV, BLOCK_LENGTH);


    /* Step 1*/
    /*Alice -- g^a --> Bob*/
    /**** Step 1.1 ****/
    DH_init(a, state);
    /**** Step 1.2 ****/
    mpz_powm_sec(ga, g, a, p);
    msg_export_mpz(buf, "STS: ALICE/BOB CONNECT1 ", ga, 0);
    network_send(server_host, server_port, buf);
    /* Step 3 : Alice receives encrypted key, gb  and CB*/
    /* Alice <-- g^b -- Bob*/
    /**** grab encrypted key from Bob ****/
    from_Bob = network_recv(-1);
    /*from_Bob = "STS: BOB/ALICE CONNECT2 y"*/
    if(msg_import_string(buf, from_Bob, "STS: BOB/ALICE CONNECT2 ") <= 0)
    return;
    free(from_Bob);
#if DEBUG > 0
    printf("client receives: %s\n",buf);
#endif
    buffer_init(&in, 1);
    buffer_from_string(&in, (uchar*)buf, strlen(buf));
    buffer_init(&y, 1);
    buffer_from_base64(&y, &in);
    buffer_clear(&in);
    
    /**** grab g^b mod p from Bob ****/
    from_Bob = network_recv(-1);
    /*from_Bob = "STS: BOB/ALICE CONNECT2 g^b mod p"*/
    if(msg_import_mpz(gb, from_Bob, "STS: BOB/ALICE CONNECT2 ",0) <= 0)
    return;
    free(from_Bob);
    
    /**** grap certificat CB from Bob ****/
    from_Bob = network_recv(-1);
    /*from_Bob = "STS: BOB/ALICE CONNECT2 CB"*/
    if(msg_import_string(buf, from_Bob, "STS: BOB/ALICE CONNECT2 ") <= 0)
    return;
    free(from_Bob);
#if DEBUG > 0
    printf("client receives: CB=%s\n",buf);
#endif
    init_certificate(&CB);
    certificate_from_string(&CB,buf);
    /**** Step 3.1 : verify certif_B****/
#if DEBUG > 0
    printf("Is CB-server valid: %d\n", valid_certificate(&CB, N_aut, e_aut));
#else
    
#endif

     /**** Step 3.2 : generate k <- (g^b)^a = gb^a ****/
    mpz_powm_sec(gab,gb,a,p);
    gmp_printf("gab=%#Zx\n",gab);
    AES128_key_from_number(&key, gab);
#if DEBUG > 0
    printf("key="); buffer_print_int(stdout, &key); printf("\n");
#endif

    /**** Step 3.3 : decrypt y (by key) to get sigmaB and verify sigmaB ****/
    buffer_init(&decrypted, 1);
    /* decrypted <- dec(y) should be sigmaB */
    aes_CBC_decrypt(&decrypted, &y, &key, 's');
    /* feed sigmaB = SIGN_{SK_B}(ga || gb) */
    buffer_to_mpz(sigmaB, &decrypted);
    /* we should check sigmaB^e = ga || gb */
    concatenate_gb_ga(tmpB, ga, gb, p);
    mpz_powm_sec(sigmaB, sigmaB, CB.e, CB.N);
    printf("signature y is valid? %d\n", 1-mpz_cmp(tmpB, sigmaB));
    
    /**** Step 3.4 : generate the signature sigmaA ****/
    /*sigmaA <- SIGN_{SK_A}(gb,ga)*/
    SIGNSK(signA,gb,ga,p,NA,dA);
#if DEBUG > 0
    gmp_printf("sigmaA=%Zd\n", signA);
#endif
    /**** Step 3.5 : z <- ENCRYPT_k(sigma_A) ****/
    buffer_init(&clear, 1);
    buffer_from_mpz(&clear, signA);

#if DEBUG > 1
    printf("clear="); buffer_print_int(stdout, &clear); printf("\n");
#endif
    buffer_init(&encrypted, 1);
    aes_CBC_encrypt(&encrypted, &clear, &key, &IV, 's');
#if DEBUG > 1
    printf("enc="); buffer_print_int(stdout, &encrypted); printf("\n");
#endif
    /**** Step 3.6: send (z, certif_A) ****/
    buffer_init(&out, 10);
    buffer_to_base64(&out, &encrypted);
    tmp = (char*)string_from_buffer(&out);
    buffer_clear(&out);

    printf("Client sending: %s\n", tmp);
    msg_export_string(buf, "STS: ALICE/BOB CONNECT3 ", tmp);
    free(tmp);
    network_send(server_host, server_port, buf);
    tmp = (char*)string_from_certificate(CA);
    msg_export_string(buf, "STS: ALICE/BOB CONNECT3 ", tmp);
    free(tmp);
    network_send(server_host, server_port, buf);

    /**** Step 5 : receive OK****/
    from_Bob = network_recv(-1);
    /*from_Bob = "STS: BOB/ALICE CONNECT2 y"*/
    if(msg_import_string(buf, from_Bob, "STS: BOB/ALICE CONNECT4 ") <= 0)
    return;
    free(from_Bob);
#if DEBUG > 0
    printf("client receives: %s\n",buf);
#endif
    buffer_init(&in, 1);
    buffer_from_string(&in, (uchar*)buf, strlen(buf));
    buffer_init(&clear, 1);
    buffer_from_base64(&clear, &in);
    buffer_clear(&in);
    buffer_init(&decrypted, 1);
    /* decrypted <- dec(clear) should be sigmaB */
    aes_CBC_decrypt(&decrypted, &clear, &key, 's');
    /* feed sigmaB = SIGN_{SK_B}(ga || gb) */
    tmp = (char*)string_from_buffer(&decrypted); 
    buffer_clear(&decrypted);

    printf("The successful signal for the connection by STS protocol: %s\n", tmp);
    
    free(tmp);
    buffer_clear(&key);
    buffer_clear(&IV);
    mpz_clears(p, g, gb, a, ga, gab, sigmaB, signA, tmpB, NULL);

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
