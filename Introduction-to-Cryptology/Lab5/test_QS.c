/****************************************************************/
/* test_QS.c                                                    */
/* Author : F. Morain                                           */
/* morainr@lix.polytechnique.fr                                 */
/* Last modification November 26, 2018                          */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "utils.h"
#include "QS.h"

void testQS1(int phase){
    mpz_t N, kN, g;
    int B[] = {-1, 2, 5, 17, 23};
    int cardB = 5, M = 10, nf = 0;
    factor_t tabf[2];

    mpz_inits(N, kN, g, NULL);

    mpz_set_str(N, "143", 10);
    mpz_mul_ui(kN, N, 7);
    mpz_set_ui(g, 31);
    QS_aux(tabf, &nf, N, kN, g, B, cardB, M, phase);

    mpz_clears(N, kN, g, NULL);
}

void testQS(char *sN, int k, int cardB, int M, int phase){
    const char *ficdp = "1e7";
    FILE *file;
    mpz_t N;
    factor_t tabf[20];
    int nf = 0;

    if((file = fopen(ficdp, "r")) == NULL){
		perror(ficdp);
		return;
    }
    mpz_init_set_str(N, sN, 10);
    QS(tabf, &nf, N, k, cardB, M, file, 10);
    fclose(file);
}

int main(int argc, char *argv[]){
    int k = 0, cardB = 0, M = 0;

    if(argc == 1)
	testQS1(1);
    else if(argc == 2)
	testQS1(atoi(argv[1]));
    else{
	if(argc > 3){
	    k = atoi(argv[3]);
	    cardB = atoi(argv[4]);
	    M = atoi(argv[5]);
	}
	testQS(argv[2], k, cardB, M, atoi(argv[1]));
    }
    return 0;
}
