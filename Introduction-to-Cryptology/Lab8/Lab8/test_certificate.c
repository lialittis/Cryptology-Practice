#include <stdlib.h>
#include <stdio.h>

#include "gmp.h"

#include "base64.h"
#include "buffer.h"
#include "bits.h"
#include "certificate.h"

int main(int argc, char *argv[]){
    FILE *in = NULL;
    certificate_t C;
    mpz_t N, e;
    uchar *msg;

    if((in = fopen(argv[1], "r")) == NULL){
	perror(argv[1]);
	return -1;
    }
    mpz_inits(N, e, NULL);
    read_public_keys(N, e, in);
    fclose(in);
    
    if((in = fopen(argv[2], "r")) == NULL){
	perror(argv[2]);
	return -1;
    }
    init_certificate(&C);
    extract_certificate(&C, in);
    fclose(in);
    print_certificate(&C);
    msg = string_from_certificate(&C);
    printf("Certificate as a string:\n%s\n", msg);
    printf("-> %d\n", valid_certificate(&C, N, e));
    mpz_clears(N, e, NULL);
    free((char *)msg);
    clear_certificate(&C);
    return 0;
}
