/****************************************************************/
/* refine.c                                                     */
/* Author : F. Morain                                           */
/* morain@lix.polytechnique.fr                                  */
/* Last modification November 25, 2017                          */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "gmp.h"
#include "utils.h"
#include "refine.h"

#define DEBUG 1

#define SMAX 100

void PrintWorking(working_t fact){
    gmp_printf("[%Zd, %d, %d]", fact.N, fact.e, fact.status);
}

void PrintWorkings(working_t *fact, int nf){
    int i;

    printf("[");
    for(i = 0; i < nf; i++){
	if(i > 0)
	    printf(", ");
	PrintWorking(fact[i]);
    }
    printf("]");
}

void WorkingClear(working_t *W){
    int i;
    
    mpz_clear(W->N);
    for(i = 0; i < W->nargs_method; i++)
	mpz_clear(W->args_method[i]);
    if(W->args_method != NULL)
	free(W->args_method);
}

void WorkingCopy(working_t *fact, working_t W){
    int i;
    
    mpz_init_set(fact->N, W.N);
    fact->e = W.e;
    fact->status = W.status;
    fact->next_method = W.next_method;
    fact->nargs_method = W.nargs_method;
    fact->args_method = (mpz_t *)malloc(W.nargs_method * sizeof(mpz_t));
    for(i = 0; i < W.nargs_method; i++)
	mpz_init_set(fact->args_method[i], W.args_method[i]);
}

void StackClear(working_t *S, int indS){
}

void StackPrint(working_t *S, int indS){
    int i;
    
    for(i = indS-1; i >= 0; i--){
	printf("%d: ", i);
	PrintWorking(S[i]);
	printf("\n");
    }
}

void StackPush(working_t *S, int *indS, working_t fes){
    S[*indS] = fes;
    *indS += 1;
}

void StackPop(working_t *fes, working_t *S, int *indS){
    *indS -= 1;
    *fes = S[*indS];
}

/* tabw[0..nw-1[ <- refine(w1, w2) */
int RefinePair(working_t *tabw, working_t w2, working_t w1){
    working_t S[SMAX], me1, me2, tmp;
    mpz_t g;
    int indS = 0, nw = 0;

    mpz_inits(g, tmp.N, NULL);
    StackPush(S, &indS, w2);
    StackPush(S, &indS, w1);
    while(1){
	if(indS == 0){
	    // stack empty
	    WorkingCopy(tabw+nw, me1);
	    nw++;
	    break;
	}
	StackPop(&me1, S, &indS);
	while(indS > 0){
	    StackPop(&me2, S, &indS);
#if DEBUG >= 1
	    printf("L=[");
	    PrintWorkings(tabw, nw);
	    printf("]\nme1=");
	    PrintWorking(me1);
	    printf(", me2=");
	    PrintWorking(me2);
	    printf("\n");
	    StackPrint(S, indS);
#endif
	    if(mpz_cmp(me1.N, me2.N) == 0){
		/* (m1, e1+e2) */
#if DEBUG >= 1
		printf("m1 = m2\n");
#endif
		me1.e += me2.e;
		continue;
	    }
	    mpz_gcd(g, me1.N, me2.N);
#if DEBUG >= 1
	    gmp_printf("gcd(m1, m2)=%Zd\n", g);
#endif
	    if(mpz_cmp_ui(g, 1) == 0){
		/* we have finished the job with me1 */
		WorkingCopy(tabw+nw, me1);
		nw++;
		StackPush(S, &indS, me2);
		break;
	    }
	    else if(mpz_cmp(g, me1.N) == 0){
		/* (m1, e1+e2) (m2/g, e2) */
#if DEBUG >= 1
		printf("(m1, e1+e2) (m2/g, e2)\n");
#endif
		me1.e += me2.e;
		mpz_divexact(me2.N, me2.N, g);
		me2.status = PrimeStatus(me2.N);
	    }
	    else if(mpz_cmp(g, me2.N) == 0){
		/* (m1/g, e1) (m2, e1+e2) */
#if DEBUG >= 1
		printf("(m1/g, e1) (m2, e1+e2)\n");
#endif
		me2.e += me1.e;
		mpz_divexact(me1.N, me1.N, g);
		me1.status = PrimeStatus(me1.N);
	    }
	    else{
		/* (m1/g, e1) (g, e1+e2) (m2/g, e2) */
#if DEBUG >= 1
		printf("(m1/g, e1) (g, e1+e2) (m2/g, e2)\n");
#endif
		/* tmp <- m2/g */
		mpz_divexact(tmp.N, me2.N, g);
		tmp.e = me2.e;
		tmp.status = PrimeStatus(tmp.N);
		tmp.next_method  = me2.next_method;
		tmp.nargs_method = me2.nargs_method;
		tmp.args_method  = me2.args_method;
		StackPush(S, &indS, tmp);
		mpz_divexact(me1.N, me1.N, g);
		me1.status = PrimeStatus(me1.N);
		mpz_set(me2.N, g);
		me2.e += me1.e;
		me2.status = PrimeStatus(me2.N);
	    }
	    StackPush(S, &indS, me2);
	}
    }
    mpz_clears(g, tmp.N, NULL);
    return nw;
}

int RefineAugment(factor_t *tabf, int *nf, working_t *newtabw,
		  working_t *tabw, int nw, working_t mkp1){
    working_t c;
    int i, newnw = 0;

    WorkingCopy(&c, mkp1);
    for(i = 0; i < nw; i++){
	newnw += RefinePair(newtabw+newnw, tabw[i], c);
	//	    if REFINE_PRINT ge 1 then
	//	        printf "# we get Li=%o\n", Li;
	//	    end if;
	WorkingClear(&c);
	WorkingCopy(&c, newtabw[newnw-1]);
	WorkingClear(newtabw+(newnw-1));
	newnw--;
    }
    WorkingCopy(newtabw+newnw, c);
    WorkingClear(&c);
    return newnw+1;
}

/* tabw may receive more clients to factor. */
void Refine(factor_t *tabf, int *nf, working_t *tabw, int *nw){
    working_t *tabw1, *tabw2, *tmp;
    int nw1 = 0, i, nw2 = 0, sz = 20, j;

    if(*nw == 1)
	// should not happen!
	return;
    tabw1 = (working_t *)malloc(sz * sizeof(working_t));
    tabw2 = (working_t *)malloc(sz * sizeof(working_t));
    nw1 = RefinePair(tabw1, tabw[*nw-2], tabw[*nw-1]);
#if DEBUG >= 1
    printf("1st refined pair: ");
    PrintWorkings(tabw1, nw1);
    printf("\n");
#endif
    for(i = *nw-3; i >= 0; i--){
	int k;
	
	nw2 = RefineAugment(tabf, nf, tabw2, tabw1, nw1, tabw[i]);
	tmp = tabw1; tabw1 = tabw2; tabw2 = tmp;
	k = nw1; nw1 = nw2; nw2 = k;
#if DEBUG >= 1
	printf("ref_%d=\n", i);
	PrintWorkings(tabw1, nw1);
	printf("\n");
#endif
    }
    /* tabw1[0..nw1[ contains the sequence of refined factors from tabw. 
       We need to transfer tabw1 into tabw or tabf
     */
    j = 0;
    for(i = 0; i < nw1; i++){
	if(tabw1[i].status != FACTOR_IS_COMPOSITE){
	    AddFactor(tabf+(*nf), tabw1[i].N, tabw1[i].e, tabw1[i].status);
	    *nf += 1;
	}
	else{
	    if(j < *nw)
		WorkingClear(tabw+j);
	    WorkingCopy(tabw+j, tabw1[i]);
	    j++;
	}
    }
    for(i = 0; i < nw1; i++)
	WorkingClear(tabw1+i);
    free(tabw1);
    for(i = 0; i < nw2; i++)
	WorkingClear(tabw2+i);
    free(tabw2);
    for(i = j; i < *nw; i++)
	WorkingClear(tabw+i);
    *nw = j;
}
