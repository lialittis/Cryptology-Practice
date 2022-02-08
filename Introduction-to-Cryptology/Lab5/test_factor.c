/****************************************************************/
/* test_factor.c                                                */
/* Author : Alain Couvreur                                      */
/* alain.couvreur@lix.polytechnique.fr                          */
/* Last modification October 26, 2017                           */
/****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "gmp.h"

#include "utils.h"
#include "refine.h"
#include "trialdiv.h"
#include "rho.h"
#include "pminus1.h"
#include "factor.h"

void testTrial(mpz_t n, long bound, uint length, FILE* file){	
    factor_t* factors = (factor_t*)malloc(length * sizeof(factor_t));
    int i, nFactors;
    mpz_t cof;
    mpz_init(cof);
	
    printf("------------------------\n");
    gmp_printf("Factorization of %Zd with primes less than %ld:\n\n",
	       n, bound);
    trialDivision(factors, &nFactors, cof, n, bound, length, file);

    //gmp_printf("%Zd = ", n);
    factor_t* cursor = factors;
    for(i = 0; i < nFactors; i++, cursor++){
	gmp_printf("(%Zd^%d) * ", cursor->f, cursor->e);	
    }
    gmp_printf("(%Zd)\n\n", cof);
	
	
    mpz_clear(cof);
    factor_clear(factors, nFactors);
    free(factors);
}


void test1(){
    printf("\n************** Testing Trial division **************\n\n");

    FILE* file;
    mpz_t n;
    long bound;
    mpz_inits(n, NULL);

    file = fopen("1e7", "r");
    /* TEST 1.1 */
    mpz_set_ui(n, 3276);
    bound = 10;
    testTrial(n, bound, 10, file);
    rewind(file);
    /* END TEST 1.1 */

    /* TEST 1.2 */
    bound = 100;
    testTrial(n, bound, 10, file);
    rewind(file);
    /* END TEST 1.2 */

    /* TEST 1.3 */
    mpz_set_str(n, "29145271819831067830349386007355751792690423602074265137925000", 10);
    testTrial(n, bound, 10, file);
    rewind(file);
    /* END TEST 1.3 */

    /* TEST 1.4 */
    mpz_set_str(n, "12652209139612535291", 10);
    bound = 10000000;
    testTrial(n, bound, 10, file);
    rewind(file);
    /* END TEST 1.4 */
    
	/*CHALLENGES*/
	bound = 1000;
    mpz_set_str(n, "1267650600228229401496703205376", 10);
	testTrial(n,bound,10,file);
	rewind(file);
    mpz_set_str(n, "29247505418476877676739193879681879", 10);
    testTrial(n,bound,10,file);
    rewind(file);
    mpz_set_str(n, "42590701354653912816230992340304619", 10);
    testTrial(n,bound,10,file);
    rewind(file);
    mpz_set_str(n, "5112499772359909311953133845147", 10);
    testTrial(n,bound,10,file);
    rewind(file);
	mpz_set_str(n, "132005526175827246467188890663501859", 10);
	testTrial(n,bound,10,file);
	rewind(file);

    mpz_clears(n, NULL);
    fclose(file);
}



void testPollardRho_with_long(const mpz_t N, long nrOfIterations){
    //printf("Seeking a factor for N = %Zd\n", N);

    printf("--------------------\n");
    int status;
    double duration;
    clock_t start, finish;
    long x=1, y=1, f, n=mpz_get_si(N);
    start = clock();
    status= PollardRho_with_long(&f,n,100000,x,y);
    finish = clock();
    printf("Factor found:%d\n",status);
    duration = (double)(finish - start)/CLOCKS_PER_SEC;
    printf("Running time : %f seconds.\n\n", duration);
}

void test2(){
    printf("************** Testing Pollard rho for longs **********************\n\n");
    printf("Using function x |-> x^2 + 7.\n");

    mpz_t N;
    mpz_init_set_si(N,44);
    testPollardRho_with_long(N, 100000);
 
    mpz_set_si(N,126522);
    testPollardRho_with_long(N, 100000);

    mpz_set_si(N,448537);
    testPollardRho_with_long(N, 100000);


    /*CHALLENGES*/
/*	mpz_set_si(N,1267650600228229401496703205376);
	testPollardRho_with_long(N, 100000);


    mpz_set_si(N,29247505418476877676739193879681879);
    testPollardRho_with_long(N, 100000);


    mpz_set_si(N,42590701354653912816230992340304619);
	testPollardRho_with_long(N, 100000);


    mpz_set_si(N,5112499772359909311953133845147);
    testPollardRho_with_long(N, 100000);


    mpz_set_si(N,132005526175827246467188890663501859);
    testPollardRho_with_long(N, 100000);

*/




    mpz_clear(N);
}


void goodFunction(mpz_t out, mpz_t in, const mpz_t N){
    mpz_powm_ui(out, in, 2, N);
    mpz_add_ui(out, out, 7);
    mpz_mod(out, out, N);
}

void badFunction(mpz_t out, mpz_t in, const mpz_t N){
    mpz_powm_ui(out, in, 2, N);
    mpz_sub_ui(out, out, 3);
    mpz_mod(out, out, N);
}


void testPollardRho(mpz_t N, long nrOfIterations,
                    void (*f)(mpz_t, mpz_t, const mpz_t)){
    gmp_printf("Seeking a factor for N = %Zd\n", N);

    printf("--------------------\n");
    factor_t p[20];
    double duration;
    clock_t start, finish;
    mpz_t cof;
    int nf = 0, i;

    mpz_init(cof);
    start = clock();
    PollardRho(p, &nf, cof, N, f, 100000);
    finish = clock();

    for(i = 0; i < nf; i++)
        gmp_printf("Factor found : p[%d] = %Zd (%d),p[%d] = %d\n",
                   i, p[i].f, p[i].status,i,p[i].e);
    duration = (double)(finish - start)/CLOCKS_PER_SEC;
    printf("Running time : %f seconds.\n\n", duration);
    factor_clear(p, nf);
    mpz_clear(cof);
}


void test3(){
    printf("************** Testing Pollard rho **********************\n\n");
	
    mpz_t N;
    mpz_init(N);

    printf("Using function x |-> x^2 + 7.\n");

    mpz_set_str(N, "22145579", 10);
    testPollardRho(N, 100000, goodFunction);
    if(mpz_probab_prime_p(N, 25))
	printf("Petit polission\n");
	
    mpz_set_str(N, "12652209139612535291", 10);
    testPollardRho(N, 100000, goodFunction);
	
    mpz_set_str(N, "10541221091544233897", 10);
    testPollardRho(N, 100000, goodFunction);

    mpz_set_str(N, "633564754957339397639948337059", 10);
    testPollardRho(N, 1000000, goodFunction);
	
    mpz_set_str(N, "2035109857152735577711831203565763223283", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "72963193328043133999662344352921779599583554200941", 10);
    testPollardRho(N, 100000000000000000, goodFunction);


	/*CHALLENGES*/

    mpz_set_str(N, "1267650600228229401496703205376", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "29247505418476877676739193879681879", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "42590701354653912816230992340304619", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "5112499772359909311953133845147", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "132005526175827246467188890663501859", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "79528987121976283201617097672189051", 10);
	testPollardRho(N, 100000000000000000, goodFunction);


    mpz_set_str(N, "40349710116779839654655670340336403", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "51564512294371432076336997698478167", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "9162374311027372882834264790457997", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "22339673333864202368466864268439347", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "3791719778346641837035315956619073", 10);
    testPollardRho(N, 100000000000000000, goodFunction);
    mpz_set_str(N, "4222024863313658273315874330956629", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "1035569983117668667408792222006999", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "22737225347118377064588685012607249", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "52481289522732307432135031857769", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "15340673457658484298112629175463", 10);
    testPollardRho(N, 100000000000000000, goodFunction);
    mpz_set_str(N, "513020842067724881694500362555943", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "4887105378099555741815696377899469", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "236711601971735773816444451442583", 10);
    testPollardRho(N, 100000000000000000, goodFunction);

    mpz_set_str(N, "2599635550305542600301154541058269", 10);
    testPollardRho(N, 100000000000000000, goodFunction);




















/*


    printf("\n\nUsing function x |-> x^2 - 3.\n");

    mpz_set_str(N, "12652209139612535291", 10);
    testPollardRho(N, 100000, badFunction);
	
    mpz_set_str(N, "10541221091544233897", 10);
    testPollardRho(N, 100000, badFunction);

    mpz_set_str(N, "633564754957339397639948337059", 10);
    testPollardRho(N, 1000000, badFunction);
	
    mpz_set_str(N, "2904904137951823762898116102980679156667", 10);
    testPollardRho(N, 10000000, badFunction);

    mpz_set_str(N, "72963193328043133999662344352921779599583554200941", 10);
    testPollardRho(N, 100000000, badFunction);
*/	
    mpz_clear(N);
}

void testPminus1(mpz_t N, long bound1, long bound2, FILE* file){
	
    factor_t p[20];
    int i, nf = 0, status = FACTOR_NOT_FOUND;
    mpz_t cof;
    double duration;
    clock_t start, finish;
	
    gmp_printf("Seeking a factor for N = %Zd\n", N);
    printf("--------------------\n");
	
    mpz_init(cof);
    start = clock();
    status = PollardPminus1(p, &nf, cof, N, bound1, bound2, file);
    finish = clock();
	
    if(status == FACTOR_NOT_FOUND && nf == 0)
	printf("[FACTOR NOT FOUND]\n\n");
    else{
	for(i = 0; i < nf; i++)
	    gmp_printf("[FACTOR FOUND] : p[%d] = %Zd (%d), p[%d] = %d\n",
		       i, p[i].f, p[i].status,i,p[i].e);
	duration = (double)(finish - start)/CLOCKS_PER_SEC;
	printf("Running time : %f seconds.\n\n", duration);
	factor_clear(p, nf);
    }
    mpz_clear(cof);
}


void test4(){
    printf("***************** Testing Pollar p - 1 ********************\n\n");
    printf("PHASE 1:\n\n");
    FILE* file = fopen("1e7", "r");


    mpz_t N;
    long bound1, bound2;
    mpz_inits(N, NULL);

    mpz_set_ui(N, 2993);
    bound1 = 50;
    bound2 = 0;
    testPminus1(N, bound1, bound2, file);
    rewind(file);
	
    mpz_set_str(N, "12652209139612535291", 10);
    testPminus1(N, 1500, bound2, file);
    rewind(file);
	
    mpz_set_str(N, "10541221091544233897", 10);
    testPminus1(N, 1500, bound2, file);
    rewind(file);

    printf("PHASES 1 & 2:\n\n");

    mpz_set_str(N, "10541221091544233897", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

    mpz_set_str(N, "633564754957339397639948337059", 10);
    testPminus1(N, 1000000, 100000000, file);
    rewind(file);


    /*CHALLENGE*/
    mpz_set_str(N, "1267650600228229401496703205376", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

	mpz_set_str(N, "29247505418476877676739193879681879", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "42590701354653912816230992340304619", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "5112499772359909311953133845147", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "132005526175827246467188890663501859", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

    mpz_set_str(N, "79528987121976283201617097672189051", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

	mpz_set_str(N, "40349710116779839654655670340336403", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "51564512294371432076336997698478167", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "9162374311027372882834264790457997", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "22339673333864202368466864268439347", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

    mpz_set_str(N, "3791719778346641837035315956619073", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

	mpz_set_str(N, "4222024863313658273315874330956629", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "1035569983117668667408792222006999", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "22737225347118377064588685012607249", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);


	mpz_set_str(N, "52481289522732307432135031857769", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

	mpz_set_str(N, "15340673457658484298112629175463", 10);
    testPminus1(N, 1500, 1000000, file);
    rewind(file);

	mpz_set_str(N, "513020842067724881694500362555943", 10);
    testPminus1(N, 1000000, 100000000, file);
    rewind(file);


	mpz_set_str(N, "4887105378099555741815696377899469", 10);
    testPminus1(N, 1000000, 1000000000, file);
    rewind(file);


	mpz_set_str(N, "236711601971735773816444451442583", 10);
    testPminus1(N, 1000000, 1000000000, file);
    rewind(file);


	mpz_set_str(N, "2599635550305542600301154541058269", 10);
    testPminus1(N, 1000000, 100000000, file);
    rewind(file);





    fclose(file);
    mpz_clears(N, NULL);
}

void test5(){
    factor_t tabf[20];
    working_t tabw[20];
    int i, nf, nw;

    printf("************** Testing Refine **********************\n\n");

    for(i = 0; i < 3; i++)
	mpz_init(tabw[i].N);
	
    mpz_set_ui(tabw[0].N, 11);
    tabw[0].e = 3;
    tabw[0].status = FACTOR_IS_PRIME;
    tabw[0].nargs_method = 0;
    mpz_set_ui(tabw[1].N, 13);
    tabw[1].e = 4;
    tabw[1].status = FACTOR_IS_PRIME;
    tabw[1].nargs_method = 0;
    mpz_set_ui(tabw[2].N, 17);
    tabw[2].e = 5;
    tabw[2].status = FACTOR_IS_PRIME;
    tabw[2].nargs_method = 0;
    nw = 2; nf = 0;
    Refine(tabf, &nf, tabw, &nw);
    PrintFactorization(tabf, nf); printf(" vs. 11^3*13^4\n");
#if 0
    nw = 3; nf = 0;
    Refine(tabf, &nf, tabw, &nw);
    PrintFactorization(tabw, nf); printf(" vs. 11^3*13^4*17^5\n");
    mpz_set_ui(tabw[0].N, 13*13*11);
    tabw[0].e = 3;
    mpz_set_ui(tabw[1].N, 11*11*11*13);
    tabw[1].e = 4;
    mpz_set_ui(tabw[2].N, 13*11*11*13);
    tabw[2].e = 5;
    nw = 2; nf = 0;
    Refine(tabf, &nf, tabw, &nw);
    PrintFactorization(tabf, nf); printf(" vs. 11^15*13^10\n");
    nw = 3; nf = 0;
    Refine(tabf, &nf, tabw, &nw);
    PrintFactorization(tabf, nf); printf(" vs. 11^25*13^20\n");

    mpz_set_ui(tabw[0].N, 13*13*11*17);
    tabw[0].e = 3;
    mpz_set_ui(tabw[1].N, 11*11*11*13*19);
    tabw[1].e = 5;
    nw = 2; nf = 0;
    Refine(tabf, &nf, tabw, &nw);
    PrintFactorization(tabf, nf); printf(" vs. 19^5*11^18*13^11\n");
#endif
    for(i = 0; i < 3; i++)
	mpz_clear(tabw[i].N);
    for(i = 0; i < nf; i++)
	mpz_clear(tabf[i].f);
}

void test5_aux(const char *sN, const char *ficdp){
    mpz_t N;
    factor_t tabf[20];
    int nf, nfmax = 20, status, i;

    mpz_init_set_str(N, sN, 10);
    status = Factorization(tabf, &nf, nfmax, N, 20, ficdp);
    printf("Result is: %d\n", status);
    PrintFactorization(tabf, nf); printf("\n\n");
    for(i = 0; i < nf; i++)
	mpz_clear(tabf[i].f);
    mpz_clear(N);
}

void test6(const char *ficdp){
    printf("************** Testing Factorization **********************\n\n");
#if 1
    test5_aux("121", ficdp);
    test5_aux("242", ficdp);
    /* [ <2, 4>, <3119, 2>, <4261, 2> ] */
    test5_aux("2826010691575696", ficdp);
    /* [ <2, 4>, <5, 5>, <3119, 3>, <4261, 3> ] */
    test5_aux("117368277580224383768950000", ficdp);
    /* [ <863, 1>, <1857731, 1>, <7891739447, 1> ] */
    test5_aux("12652209139612535291", ficdp);
    /* [ <863, 1>, <1857731, 2>, <7891739447, 3> ] */
    test5_aux("1463843561081654702849572240516608674954604289", ficdp);
    /* [ <863, 1>, <1857731, 2>, <7891739447, 3>, <2342432342351, 3> ] */
    test5_aux("18814636586825406963583264985367130030123671190434606784914702775300928463513504239", ficdp);
    /* [ <2342432342351, 1>, <2342432342581, 1> ] */
    test5_aux("5486989279030751906947931", ficdp);
    /* [ <2342432342351, 1>, <2342432342581, 1>, <2342432342713, 1> ] */
    test5_aux("12852901151321119035361934194038276803", ficdp);
    /* p and 2*p-1, max(q|p-1) = 178091 => cause f == N in p-1 */
    test5_aux("10973978607372047110567153", ficdp);
    /** bugs **/
    /* not using Incoming factor on a prp at the end of TD */
    test5_aux("100000000000000000999", ficdp);
    /** same epact for x^2+1
	e=10: { 1041091, 1239377, 1265129 } 
	e=21: { 1035043, 1177571, 1370189, 1858313 }
    **/
    /* for this one: same epact, but different periods! */
    /* [ <1041091, 1>, <1239377, 1> ] */
    test5_aux("1290304240307", ficdp);
    /* [ <1041091, 2>, <1239377, 3> ] */
#endif
    test5_aux("2063420216991956978479916762873", ficdp);
#if 1
    /* 1041091 and 1265129 have the same period! */
    test5_aux("1632401313235354603", ficdp);
    test5_aux("2664734047452510294970979267153287609", ficdp);
    /* 1041091*1239377^2*1265129^3; caused some bug in TreatFactor2 */
    test5_aux("3238172571580723392063076166516060171", ficdp);
    /* 1041091^7*1239377*1265129^4; */
    test5_aux("4208850707681723174780478226063226495757294677460592707005419142586389147", ficdp);
    /* (1041091*1239377)*(1035043*1177571*1370189*1858313) */
    test5_aux("4004395461914579141809954027457975047", ficdp);
#endif
    /* (1041091^2*1239377^3)*(1035043^4*1177571^6) */
    test5_aux("6314571167404833639644963983938148557535992482617833482526608672748522079482461632506070433", ficdp);
}

void test7(const char *sNmin, const char *sNmax, const char *ficdp){
    mpz_t N, Nmax;
    factor_t tabf[20];
    int nf, nfmax = 20, i;// status;
    long cpt = 0;

    if(sNmin == NULL || sNmax == NULL){
	fprintf(stderr, "sNmin and sNmax should be non NULL\n");
	return;
    }
    mpz_init_set_str(N, sNmin, 10);
    mpz_init_set_str(Nmax, sNmax, 10);
    /* make N odd */
    if(mpz_tstbit(N, 0) == 0)
	mpz_add_ui(N, N, 1);
    while(mpz_cmp(N, Nmax) < 0){
	cpt++;
	if(cpt % 10000 == 0)
	    gmp_printf("%Zd\n", N);
	Factorization(tabf, &nf, nfmax, N, 20, ficdp);
#if 1
	gmp_printf("N:=%Zd;\nlf:=", N);
	PrintFactorization(tabf, nf); printf("; Foo(N, lf);\n\n");
	for(i = 0; i < nf; i++)
	    mpz_clear(tabf[i].f);
#endif
	mpz_add_ui(N, N, 2);
    }
    mpz_clears(N, Nmax, NULL);
}



void usage(char *s, int ntests){
    fprintf(stderr, "Usage: %s <test_number in 1..%d>\n", s, ntests);
}

int main(int argc, char *argv[]){
    const char *ficdp = "1e7";
    if(argc == 1){
	usage(argv[0], 7);
	return 0;
    }
    int n = atoi(argv[1]);
    
    
    // testgcd(15,5,3);
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
    case 6:
	test6(ficdp);
	break;
    case 7:
	test7(argv[2], argv[3], ficdp);
	break;
    }
    return 0;
}
