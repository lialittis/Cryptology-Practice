/****************************************************************/
/* factor.c                                                     */
/* Author : F. Morain                                           */
/* morain@lix.polytechnique.fr                                  */
/* Last modification November 5, 2017                           */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gmp.h"
#include "utils.h"
#include "refine.h"
#include "trialdiv.h"
#include "rho.h"
#include "pminus1.h"
#include "QS.h"
#include "factor.h"

#define DEBUG 1 /* special debug value: 100 */

/* use a stack */
#define IF_INIT     0
#define IF_TRIALDIV 1
#define IF_RHO      2
#define IF_PMINUS1  3
#define IF_QS       4

void IFStackPush(working_t *S, int *indS, working_t W){
#if DEBUG != 0
    gmp_printf("pushing (%Zd, %d, %d)\n", W.N, W.e, W.status);
#endif
    mpz_init_set(S[*indS].N, W.N);
    S[*indS].e = W.e;
    S[*indS].status = W.status;
    S[*indS].next_method  = W.next_method;
    S[*indS].nargs_method = W.nargs_method;
    if(S[*indS].nargs_method == 0)
	S[*indS].args_method = NULL;
    else{
	int i;
	
	S[*indS].args_method =
	    (mpz_t *)malloc(S[*indS].nargs_method * sizeof(mpz_t));
	for(i = 0; i < S[*indS].nargs_method; i++)	
	    mpz_init_set(S[*indS].args_method[i], W.args_method[i]);
    }
    *indS += 1;
}

void IFStackPushInit(working_t *S, int *indS, mpz_t N, int e, int status, int method){
    working_t W;

    mpz_init_set(W.N, N);
    W.e = e;
    W.status = status;
    W.next_method = method;
    switch(method){
    case IF_INIT:
	/* powers, etc. */
	W.nargs_method = 0;
	W.args_method  = NULL;
	break;
    case IF_TRIALDIV:
	W.nargs_method = 1;
	W.args_method  = (mpz_t *)malloc(sizeof(mpz_t));
	mpz_init_set_ui(W.args_method[0], 10000);
	break;
    case IF_RHO:
	W.nargs_method = 4;
	W.args_method  = (mpz_t *)malloc(W.nargs_method * sizeof(mpz_t));
	mpz_init_set_ui(W.args_method[0], 1);     // x
	mpz_init_set_ui(W.args_method[1], 1);     // y
	mpz_init_set_ui(W.args_method[2], 1);     // it
	mpz_init_set_ui(W.args_method[3], 10000); // itmax
	break;
    case IF_PMINUS1:
	W.nargs_method = 4;
	W.args_method  = (mpz_t *)malloc(W.nargs_method * sizeof(mpz_t));
	mpz_init_set_ui(W.args_method[0], 2);       // b
	mpz_init_set_ui(W.args_method[1], 1);       // p
	mpz_init_set_ui(W.args_method[2], 10000);  // B1
	mpz_init_set_ui(W.args_method[3], 200000); // B2
	break;
    case IF_QS:
	break;
    }
    IFStackPush(S, indS, W);
    WorkingClear(&W);
}

void IFStackPop(working_t *W, working_t *S, int *indS){
    *indS -= 1;
    mpz_init_set(W->N, S[*indS].N);
    mpz_clear(S[*indS].N);
    W->e = S[*indS].e;
    W->status = S[*indS].status;
    W->next_method  = S[*indS].next_method;
    W->nargs_method = S[*indS].nargs_method;
    if(W->nargs_method == 0)
	W->args_method = NULL;
    else{
	int i;
	
	W->args_method = (mpz_t *)malloc(W->nargs_method * sizeof(mpz_t));
	for(i = 0; i < W->nargs_method; i++){
	    mpz_init_set(W->args_method[i], S[*indS].args_method[i]);
	    mpz_clear(S[*indS].args_method[i]);
	}
	free(S[*indS].args_method);
    }
}

void IncomingFactor(working_t* W){
    W->status = PrimeStatus(W->N);
    if(W->status == FACTOR_IS_COMPOSITE){
	int e1;
	if((e1 = IsPerfectPower(W->N, W->N)) > 1){
	    W->status = PrimeStatus(W->N);
	    W->e *= e1;
	}
    }
}

/* W.N is a probable prime; we inspect S for multiples of W.N. */
void IFStackPurge(working_t *S, int *indS, working_t *W){
    int i, e;

    for(i = 0; i < *indS; i++){
	e = 0;
	while(mpz_divisible_p(S[i].N, W->N)){
	    e++;
	    mpz_divexact(S[i].N, S[i].N, W->N);
	}
	if(e > 0){
#if DEBUG != 0
	    gmp_printf("%Zd divides a composite in the stack\n", W->N);
#endif
	    W->e += S[i].e * e;
	    if((e = IsPerfectPower(S[i].N, S[i].N)) > 1)
		S[i].e *= e;
	    S[i].status = PrimeStatus(S[i].N);
	}
    }
}

/* OUTPUT: 1 if a non-trivial factor was found; 0 for f == N. 
   If f (resp. cof) is composite, push it.
   If f (resp. cof) is prprime, purge stack.
   If f and cof are composite (rare event), refine!.
*/
int TreatFactor2(factor_t *tabf, int *nf, working_t *S, int *indS,
		 working_t W, mpz_t f){
    mpz_t cof;
    int e = W.e, e_cof, status_f, status_cof;

    if(mpz_cmp(f, W.N) >= 0){
	gmp_printf("# WARNING: f == N = %Zd\n", W.N);
	return 0;
    }
    mpz_divexact(W.N, W.N, f);
    IncomingFactor(&W);
    mpz_init_set(cof, W.N);
    e_cof = W.e;
    if((status_cof = W.status) == FACTOR_IS_COMPOSITE)
	IFStackPush(S, indS, W);

    mpz_set(W.N, f);
    W.e = e;
    IncomingFactor(&W);
    if((status_f = W.status) == FACTOR_IS_COMPOSITE)
	IFStackPush(S, indS, W);
    else{
	IFStackPurge(S, indS, &W);
	AddFactor(tabf+(*nf), W.N, W.e, FACTOR_IS_PROBABLE_PRIME);
	*nf += 1;
    }
    if(status_cof != FACTOR_IS_COMPOSITE){
	mpz_set(W.N, cof);
	W.e = e_cof;
	W.status = status_cof;
	IFStackPurge(S, indS, &W);
	AddFactor(tabf+(*nf), W.N, W.e, FACTOR_IS_PROBABLE_PRIME);
	*nf += 1;
    }
    mpz_clear(cof);
    if(status_f == FACTOR_IS_COMPOSITE && status_cof == FACTOR_IS_COMPOSITE){
	/* refine at this point, since at least two composite factors
	   are sitting in S */
	printf("We must refine!\n");
	Refine(tabf, nf, S, indS);
    }
    return 1;
}

static void goodFunction(mpz_t out, mpz_t in, const mpz_t N){
    mpz_mul(out, in, in);
    mpz_add_ui(out, out, 1);
    mpz_mod(out, out, N);
}

int Split(factor_t *tabf, int *nf, mpz_t N, uint length, FILE *file){
    working_t S[20], W;
    mpz_t N1;
    int indS = 0, status = FACTOR_NOT_FOUND;
    
    mpz_inits(N1, NULL);
    IFStackPushInit(S, &indS, N, 1, PrimeStatus(N), IF_INIT);
    while(indS > 0){
	IFStackPop(&W, S, &indS);
#if DEBUG >= 1
	gmp_printf("popping (%Zd, %d; %d)\n", W.N, W.e, W.status);
#endif
	if(mpz_cmp_ui(W.N, 1) == 0)
	    continue;
	if(W.next_method == IF_INIT){
	    IncomingFactor(&W);
	    IFStackPushInit(S, &indS, W.N, W.e, W.status, IF_TRIALDIV);
	}
	else if(W.next_method == IF_TRIALDIV){
	    long bound = mpz_get_si(W.args_method[0]);
#if DEBUG != 0
	    printf("# ==> trial division\n");
#endif
	    rewind(file);
	    trialDivision(tabf, nf, N1, W.N, bound, length, file);
#if DEBUG != 0
	    gmp_printf("# TD: cof=%Zd\n", N1);
#endif
	    /* N1 = 1 means finished */
	    if(mpz_cmp_ui(N1, 1) > 0){
		mpz_set(W.N, N1);
		IncomingFactor(&W);
		IFStackPushInit(S, &indS, W.N, W.e, W.status, IF_RHO);
	    }
	}
	else if(W.next_method == IF_RHO){
	    long it = mpz_get_ui(W.args_method[2]);
	    long itmax = mpz_get_si(W.args_method[3]);

#if DEBUG != 0
	    printf("# ==> rho(%ld)\n", it);
#endif
	    status = PollardRhoSteps(N1, W.N, goodFunction, &it, itmax,
				     W.args_method[0], W.args_method[1]);
#if DEBUG != 0
	    gmp_printf("# <== rho(%ld): %Zd\n", it, N1);
#endif
	    mpz_set_ui(W.args_method[2], it+1);
	    if(status == FACTOR_FOUND){
#if DEBUG != 0
		gmp_printf("# rho.factor: %Zd for it=%d\n", N1, it);
#endif
		TreatFactor2(tabf, nf, S, &indS, W, N1);
	    }
	    else if(it > itmax)
#if DEBUG != 100
		IFStackPushInit(S, &indS, W.N, W.e, W.status, IF_PMINUS1);
#else
	    break;
#endif
	    else /* quite rare! */
		IFStackPush(S, &indS, W);
	}
#if DEBUG != 100
	else if(W.next_method == IF_PMINUS1){
	    long B1 = mpz_get_ui(W.args_method[2]);
	    long B2 = mpz_get_ui(W.args_method[3]);
	    mpz_t b, p;

	    mpz_init_set(b, W.args_method[0]);
	    mpz_init_set(p, W.args_method[1]);
#if DEBUG != 0
            gmp_printf("# ==> p-1 with p=%Zd, B1=%ld, B2=%ld\n", p, B1, B2);
#endif
	    rewind(file);
	    if(mpz_cmp_ui(p, B1) < 0){
#if DEBUG != 0
		printf("# p-1 step1\n");
#endif
		status = PollardPminus1Step1(N1, W.N, B1, file, b, p);
	    }
	    else if(mpz_cmp_ui(p, B2) < 0){
#if DEBUG != 0
		printf("# p-1 step2\n");
#endif
		mpz_sub_ui(p, p, 1); // trick to position file correctly
		status = PollardPminus1Step2(N1, W.N, B2, file, b, p);
	    }
	    mpz_set(W.args_method[0], b);
	    mpz_set(W.args_method[1], p);
	    if(status == FACTOR_FOUND)
		TreatFactor2(tabf, nf, S, &indS, W, N1);
	    else if(mpz_cmp_ui(p, B2) >= 0)
		IFStackPushInit(S, &indS, W.N, W.e, W.status, IF_QS);
	    else
		IFStackPush(S, &indS, W);
	    mpz_clears(b, p, NULL);
	}
	else if(W.next_method == IF_QS){
	}
#endif /* DEBUG != 100 */
	WorkingClear(&W);
    }
    assert(indS == 0);
    mpz_clears(N1, NULL);
    return status;
}

/* PROB_PRIME factors are put first into tabf. */
int Factorization(factor_t *tabf, int *nf, int nfmax, mpz_t N, uint length, const char *ficdp){
    FILE *file;
    mpz_t N0;
    int status, e, i;

#if DEBUG >= 1
    gmp_printf("Entering Factorization with N=%Zd\n", N);
#endif
    *nf = 0;
    if(mpz_cmp_ui(N, 1) == 0)
	return 0;
    mpz_init(N0);
    if(mpz_tstbit(N, 0) == 0){
	/* N is even */
	e = 0;
	do{
	    e++;
	} while(mpz_tstbit(N, e) == 0); /* trick */
	mpz_fdiv_q_2exp(N0, N, e);
	gmp_printf("N0=%Zd\n", N0);
	AddSmallFactor(tabf, 2, e, FACTOR_IS_PRIME);
	status = Factorization(tabf+1, nf, nfmax-1, N0, length-1, ficdp);
	*nf += 1;
    }
    else if((e = IsPerfectPower(N0, N)) > 1){
	status = Factorization(tabf, nf, nfmax, N0, length, ficdp);
	for(i = 0; i < *nf; i++)
	    tabf[i].e *= e;
    }
    else{
	if((file = fopen(ficdp, "r")) == NULL){
	    perror(ficdp);
	    return FACTOR_ERROR;
	}
	status = Split(tabf, nf, N, length, file);
	fclose(file);
    }
    mpz_clear(N0);
    return status;
}

int DoFactorization(mpz_t N, uint length){
    int nfmax = mpz_sizeinbase(N, 2); // overshooting!!
    int nf;
    factor_t *tabf = (factor_t *)malloc(nfmax * sizeof(factor_t));
    int status = Factorization(tabf, &nf, nfmax, N, length, "1e7"), i;

    PrintFactorization(tabf, nf);
    printf("\n");
    for(i = 0; i < nf; i++)
	mpz_clear(tabf[i].f);
    free(tabf);
    return status;
}
