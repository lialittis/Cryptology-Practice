/****************************************************************/
/* test_dlog.c                                                  */
/* Author : Alain Couvreur                                      */
/* alain.couvreur@lix.polytechnique.fr                          */
/* Last modification October 26, 2017                           */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"

#include "hash.h"
#include "dlog.h"


int elementary_verif(unsigned int k, unsigned int v, mpz_t kz,
		     mpz_t vz, mpz_t verif, hash_table H, mpz_t base,
		     mpz_t p){

    mpz_set_ui(kz, k);
    mpz_set_ui(verif, v);
    if(hash_get_mpz(vz, H, kz, base, p) == HASH_NOT_FOUND){
	gmp_printf("[ERROR] : Hash table misses the pair [%Zd; %Zd].\n",
		   kz, verif);
	return 0;
    }
    else if(mpz_cmp(vz, verif) != 0){
	gmp_printf("[Error] : value corresponding to key : %Zd,", kz);
	gmp_printf(" should be %Zd and not %Zd.\n", verif, vz);
	return 0;
    }
    return 1;
}


void test_BS(mpz_t base, mpz_t n_baby_steps, mpz_t p){
    printf("\n----------- Testing the precomputation phase ----------\n");
    mpz_t sqrt, vz, kz, verif;
    mpz_inits(sqrt, vz, kz, verif, NULL);
    mpz_set_ui(sqrt, 7);
    mpz_set_ui(vz, 0);
	
    unsigned int size = mpz_get_ui(sqrt);
    hash_table H = hash_init(size);	

    fflush(stdout);
    babySteps(H, sqrt, base, p);
	
    if(H->nb_elts != 7){
	printf("[ERROR] : Hash table has not the good number of elements.\n");
	return;
    }
    else
	printf("Hash table contains the good number of elements : [OK].\n");
    if(!elementary_verif(1, 0, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [1, 0]  : [OK].\n");
    if(!elementary_verif(2, 1, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [2, 1]  : [OK].\n");
	
    if(!elementary_verif(4, 2, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [4, 2]  : [OK].\n");
   
    if(!elementary_verif(8, 3, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [8, 3]  : [OK].\n");
		
    if(!elementary_verif(16, 4, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [16, 4] : [OK].\n");
	
    if(!elementary_verif(32, 5, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [32, 5] : [OK].\n");
		
    if(!elementary_verif(27, 6, kz, vz, verif, H, base, p))
	return;
    else
	printf("Pair [27, 6] : [OK].\n");
	
    printf("\n[SUCCESS]\n\n");
    mpz_clears(sqrt, vz, kz, verif, NULL);
    hash_clear(H);
}


void test1(){
    mpz_t n_baby_steps, base, p;
    mpz_inits(n_baby_steps, base, p, NULL);
    mpz_set_ui(base, 2);	

    mpz_set_ui(n_baby_steps, 6);
    mpz_set_ui(p, 37);
    test_BS(base, n_baby_steps, p);
    mpz_clears(n_baby_steps, base, p, NULL);
}


int testBSGS(mpz_t kz, mpz_t base, mpz_t p){
    printf("---------------------------\n");
    gmp_printf("Log of %Zd in base %Zd modulo %Zd : \n", kz, base, p);
    mpz_t sqrt, result, testResult;
    mpz_inits(sqrt, result, testResult, NULL);
    mpz_sqrt(sqrt, p);
	
    unsigned int size = mpz_get_ui(sqrt);
    hash_table H = hash_init(size);	
    //hash_table H = hash_init(1);

    printf("Precomputation phase... ");
    fflush(stdout);
    babySteps(H, sqrt, base, p);
    printf("Done.\n");

    printf("Online phase... ");
    fflush(stdout);
    giantSteps(result, H, sqrt, base, p, kz);
    printf("Done.\n");
	
    gmp_printf("Discrete Log : %Zd\n", result);
	
    mpz_powm(testResult, base, result, p);
	

    int j = mpz_cmp(testResult, kz);
    if(j == 0)
	printf("[OK]\n\n");
    else
	printf("[FAILED]\n\n");
    mpz_clears(sqrt, result, testResult, NULL);
    hash_clear(H);
    return j == 0;
}

void test2(){
    mpz_t kz, base, p;
    mpz_inits(kz, base, p, NULL);
    mpz_set_ui(base, 2);	

    int i = 1;
    mpz_set_str(kz, "19", 10);
    mpz_set_str(p, "37", 10);
    if(!testBSGS(kz, base, p))
	i = 0;
	
	
    mpz_set_str(kz, "705995201", 10);
    mpz_set_str(p, "813140123", 10);
    if(!testBSGS(kz, base, p))
	i = 0;
	
    mpz_set_str(kz, "26559362869", 10);
    mpz_set_str(p, "651959218259", 10);
    if(!testBSGS(kz, base, p))
	i = 0;

	
    mpz_set_str(kz, "31399422437428", 10);
    mpz_set_str(p, "87997811728907", 10);
    if(!testBSGS(kz, base, p))
	i = 0;
	
    if(i)
	printf("\n\n[SUCCESS]\n\n");
    else
	printf("\n\n[FAILED]\n\n");
    mpz_clears(kz, base, p, NULL);
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
    }
    return 0;
}
