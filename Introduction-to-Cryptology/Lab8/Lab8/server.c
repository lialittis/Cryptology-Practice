#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gmp.h"

#include "base64.h"
#include "buffer.h"
#include "bits.h"
#include "random.h"
#include "base64.h"

#include "operating_modes.h"
#include "aes.h"

#include "version.h"

#include "network.h"
#include "dh.h"
#include "channel.h"

#include "certificate.h"

#define DEBUG 1

/* INPUT: mesg = "AES" */
void CaseAES(const char *client_host, const int client_port){
    char *from_Alice = network_recv(-1);
    buffer_t in;
    buffer_t z, decrypted, key;
    mpz_t gab;
    int N64 = strlen(from_Alice);

    buffer_init(&key, BLOCK_LENGTH);
    mpz_init_set_str(gab, "12345612345678907890", 10);
    AES128_key_from_number(&key, gab);

    printf("AES/RECV: %s\n", from_Alice);
    buffer_init(&in, N64);
    buffer_from_string(&in, (uchar *)from_Alice, N64);
    buffer_init(&z, 1);
    buffer_from_base64(&z, &in);

    buffer_init(&decrypted, 1);
    aes_CBC_decrypt(&decrypted, &z, &key, 's');
    buffer_print(stdout, &decrypted);
    printf("\n");

    buffer_clear(&z);
    buffer_clear(&decrypted);
    buffer_clear(&key);
}

/* INPUT: mesg = "DH: ALICE/BOB CONNECT1 0x..." */
void CaseDH(const char *client_host, const int client_port, char *mesg){
    gmp_randstate_t state;
    char buf[1024], *tmp;
    mpz_t p, g, gb, b, ga, gab;
    buffer_t encrypted, decrypted, key, in;
    int i;

    mpz_inits(p, g, gb, b, ga, gab, NULL);
    channel_init(p, g);
    gmp_randinit_default(state);
#if DEBUG
    gmp_randseed_ui(state, 42);
#else
    gmp_randseed_ui(state, random_seed());
#endif
    buffer_init(&key, BLOCK_LENGTH);
	
    /* Bob <-- g^a -- Alice */
    if(msg_import_mpz(ga, mesg, "DH: ALICE/BOB CONNECT1 ", 0) <= 0)
	return;
	
    /* Bob -- g^b --> Alice */
    DH_init(b, state);
    mpz_powm_sec(gb, g, b, p);
    msg_export_mpz(buf, "DH: BOB/ALICE CONNECT2 ", gb, 0);
#if DEBUG > 0
    gmp_printf("ga=%Zd, gb=g^%Zd=%Zd\nbuf=%s\n", ga, b, gb, buf);
#endif
    network_send(client_host, client_port, buf);
    mpz_powm_sec(gab, ga, b, p);
    gmp_printf("gab=%#Zx\n", gab);
	
    /* receiving a message encrypted with AES key = gab */
    AES128_key_from_number(&key, gab);
    tmp = network_recv(-1);
    printf("Received: %s [%d]\n", tmp, (int)strlen(tmp));
    if(msg_import_string(buf, tmp, "DH: ALICE/BOB CONNECT3 ") <= 0)
	return;
    free(tmp);
    buffer_init(&in, strlen(buf));
    buffer_from_string(&in, (uchar *)buf, strlen(buf));
    buffer_init(&encrypted, 1);
    // printf("encrypted message(in base64) is:");
    // buffer_print(stdout,&in);
    buffer_from_base64(&encrypted, &in);
#if DEBUG > 1
    printf("enc="); buffer_print_int(stdout, &encrypted); printf("\n");
#endif
    buffer_init(&decrypted, 1);
    aes_CBC_decrypt(&decrypted, &encrypted, &key, 's');
    printf("check: ");
    for(i = 0; i < decrypted.length; i++)
	printf("%c", decrypted.tab[i]);
    printf("\n");
    buffer_clear(&decrypted);
    buffer_clear(&encrypted);
    buffer_clear(&in);
    buffer_clear(&key);
    mpz_clears(p, g, gb, b, ga, gab, NULL);
}

/* INPUT: mesg = "STS: ALICE/BOB CONNECT1 0x..." */
void CaseSTS(const char *client_host, const int client_port, char *mesg,
	     certificate_t *CB, mpz_t NB, mpz_t dB, mpz_t N_aut, mpz_t e_aut){
    certificate_t CA;
    gmp_randstate_t state;
    char buf[1024], *tmp, *from_Alice;
    mpz_t p, g, gb, b, ga, gab, signB, tmpA, sigmaA;
    buffer_t encrypted, decrypted, clear, z, key, IV, in, out;

    mpz_inits(p, g, gb, b, ga, gab, sigmaA, signB, tmpA, NULL);
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
	
    /* Bob <-- g^a -- Alice */
    if(msg_import_mpz(ga, mesg, "STS: ALICE/BOB CONNECT1 ", 0) <= 0)
	return;
    /* Step 2 */
    /**** Step 2.1 ****/
    DH_init(b, state);
    /**** Step 2.2 ****/
    mpz_powm_sec(gb, g, b, p);
#if DEBUG > 0
    gmp_printf("ga=%Zd, gb=g^%Zd=%Zd\n", ga, b, gb);
#endif
    /**** Step 2.3: gab = k <- (g^a)^b = ga^b ****/
    mpz_powm_sec(gab, ga, b, p);
    gmp_printf("gab=%#Zx\n", gab);
    AES128_key_from_number(&key, gab);
#if DEBUG > 0
    printf("key="); buffer_print_int(stdout, &key); printf("\n");
#endif
    /**** Step 2.4: sigma_B <- SIGN_{SK_B}(gb, ga) ****/
    SIGNSK(signB, gb, ga, p, NB, dB);
#if DEBUG > 0
    gmp_printf("sigmaB=%Zd\n", signB);
#endif
    /**** Step 2.5:       y <- ENCRYPT_k(sigma_B) ****/
    buffer_init(&clear, 1);
    buffer_from_mpz(&clear, signB);
#if DEBUG > 1
    printf("clear="); buffer_print_int(stdout, &clear); printf("\n");
#endif
    buffer_init(&encrypted, 1);
    aes_CBC_encrypt(&encrypted, &clear, &key, &IV, 's');
#if DEBUG > 1
    printf("enc="); buffer_print_int(stdout, &encrypted); printf("\n");
#endif
    /**** Step 2.6: send (y, gb, certif_B) ****/
    buffer_init(&out, 10);
    buffer_to_base64(&out, &encrypted);
    tmp = (char*)string_from_buffer(&out);
    buffer_clear(&out);
	
    printf("Server sending: %s\n", tmp);
    msg_export_string(buf, "STS: BOB/ALICE CONNECT2 ", tmp);
    free(tmp);
    network_send(client_host, client_port, buf);
    msg_export_mpz(buf, "STS: BOB/ALICE CONNECT2 ", gb, 0);
    network_send(client_host, client_port, buf);
    tmp = (char*)string_from_certificate(CB);
    msg_export_string(buf, "STS: BOB/ALICE CONNECT2 ", tmp);
    free(tmp);
    network_send(client_host, client_port, buf);

    /**** Step 4: Bob receives z and CA ****/
    from_Alice = network_recv(-1);
    /* from_Alice = "STS: ALICE/BOB CONNECT3 z" */
    if(msg_import_string(buf, from_Alice, "STS: ALICE/BOB CONNECT3 ") <= 0)
	return;
    free(from_Alice);
#if DEBUG > 0
    printf("server receives: %s\n", buf);
#endif
    buffer_init(&in, 1);
    buffer_from_string(&in, (uchar*)buf, strlen(buf));
    buffer_init(&z, 1);
    buffer_from_base64(&z, &in);
    buffer_clear(&in);
    from_Alice = network_recv(-1);
    /* from_Bob = "STS: ALICE/BOB CONNECT3 CA" */
    if(msg_import_string(buf, from_Alice, "STS: ALICE/BOB CONNECT3 ") <= 0)
	return;
#if DEBUG > 0
    printf("server receives: CA=%s\n", buf);
#endif
    init_certificate(&CA);
    certificate_from_string(&CA, buf);
    /**** Step 4.1: verify certif_A ****/
#if DEBUG > 0
    printf("Is CA-client valid: %d\n", valid_certificate(&CA, N_aut, e_aut));
#else
    
#endif
    /**** Step 4.2: verify sign_A ****/
    buffer_init(&decrypted, 1);
    /* decrypted <- dec(z) should be sigmaA */
    aes_CBC_decrypt(&decrypted, &z, &key, 's');
    /* feed sigmaA = SIGN_{SK_A}(ga || gb) */
    buffer_to_mpz(sigmaA, &decrypted);
    /* we should check sigmaA^e = ga || gb */
    concatenate_gb_ga(tmpA, ga, gb, p);
    mpz_powm_sec(sigmaA, sigmaA, CA.e, CA.N);
    printf("signature z is valid? %d\n", 1-mpz_cmp(tmpA, sigmaA));

    /* last message: "OK" */
    buffer_from_string(&clear, (uchar*)"OK", 2);
    aes_CBC_encrypt(&encrypted, &clear, &key, &IV, 's');
    buffer_init(&out, 1);
    buffer_to_base64(&out, &encrypted);
    tmp = (char*)string_from_buffer(&out);
    buffer_clear(&out);
#if DEBUG > 0
    printf("server sends: CONNECT4 %s\n", tmp);
#endif
    msg_export_string(buf, "STS: BOB/ALICE CONNECT4 ", tmp);
    free(tmp);
    network_send(client_host, client_port, buf);

    buffer_clear(&key);
    buffer_clear(&IV);
    mpz_clears(p, g, gb, b, ga, gab, sigmaA, signB, tmpA, NULL);
}

void Usage(char *s){
    fprintf(stderr, "Usage: %s\n", s);
    fprintf(stderr, "    client_host client_port "
	    	    "[try_send|try_send_aes|try_DH]\n");
    fprintf(stderr, "or\n");
    fprintf(stderr, "    client_host client_port "
	            "server_certificate.txt server_sk.txt auth_pk.txt\n");
}

int main(int argc, char *argv[])
{
    FILE *in;
    char *mesg, *client_host;
    int client_port, server_port = 1717;
    certificate_t CB;
    mpz_t NB, dB, N_aut, e_aut;

    if(argc == 1){
	Usage(argv[0]);
	return 0;
    }    
    printf("Server for version %s\n", VERSION);
    client_host = argv[1];
    client_port = atoi(argv[2]);
    network_init(server_port);
    while(1){
	mesg = network_recv(1);
	if (mesg == NULL)
	    printf("Nothing arrived!\n");
	else{
	    printf("RECV: %s\n", mesg);
	    if(strlen(mesg) >= 3 && strncmp(mesg, "AES", 3) == 0)
		CaseAES(client_host, client_port);
	    else if(strlen(mesg) > 3 && strncmp(mesg, "DH: ", 4) == 0)
		CaseDH(client_host, client_port, mesg);
	    else if(strlen(mesg) > 4 && strncmp(mesg, "STS: ", 5) == 0){
		if(argc < 3){
		    fprintf(stderr, "Missing argument: certifB.txt");
		    break;
		}
		if((in = fopen(argv[3], "r")) == NULL){
		    perror(argv[3]);
		    return -1;
		}
		init_certificate(&CB);
		extract_certificate(&CB, in);
		fclose(in);
		print_certificate(&CB);
		if(argc < 4){
		    fprintf(stderr, "Missing argument: SKB.txt");
		    break;
		}
		if((in = fopen(argv[4], "r")) == NULL){
		    perror(argv[4]);
		    return -1;
		}
		mpz_inits(NB, dB, N_aut, e_aut, NULL);
		read_secret_keys(NB, dB, in);
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
		printf("valid certificate for CB-server? %d\n",
		       valid_certificate(&CB, N_aut, e_aut));
		CaseSTS(client_host, client_port, mesg, &CB,
			NB, dB, N_aut, e_aut);
		mpz_clears(NB, dB, N_aut, e_aut, NULL);
		clear_certificate(&CB);
		free(mesg);
		break;
	    }
	}
    }
    network_clear();
    return 0;
}
