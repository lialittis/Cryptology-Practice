/****************************************************************/
/* QS.c                                                         */
/* Author : F. Morain                                           */
/* morainr@lix.polytechnique.fr                                 */
/* Last modification October 24, 2017                           */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "gmp.h"
#include "hash.h"
#include "utils.h"
#include "refine.h"
#include "QS.h"

#define DEBUG 0

int trial_div(char *tabex, mpz_t cof, const mpz_t Px, int* B, int cardB){
    int status = FACTOR_NOT_FOUND;
/* to be filled in */
    return status;
}

void StoreRelation(relation_t *rel, mpz_t kN, mpz_t g, 
		   int cardB, int x, char *tabex){
    char *tmp = (char *)malloc(cardB * sizeof(char));

    memcpy(tmp, tabex, cardB);
    mpz_init_set_si(rel->y, x);
    mpz_add(rel->y, rel->y, g);
    mpz_mod(rel->y, rel->y, kN);
    rel->tabex = tmp;
}

void AddRelation(relation_t *tabrels, mpz_t kN, mpz_t g, 
		 int cardB, int i, int x, char *tabex){
    StoreRelation(tabrels+i, kN, g, cardB, x, tabex);
#if DEBUG >= 0
    printf("%d:", i);
    for(i = 0; i < cardB; i++)
	printf(" %d", tabex[i]);
    printf("\n");
#endif
}

/* OUTPUT: the actual number of relations found <= nrelsmax */
int FindRelationsUsingTrialDivision(relation_t *tabrels, mpz_t kN, mpz_t g,
				    int *B, int cardB, int M, int nrelsmax){
    char *tabex = (char *)malloc(cardB * sizeof(char));
    int x, nrels = 0;
    mpz_t Px, cof;

    mpz_inits(Px, cof, NULL);
    for(x = -M; x <= M; x++){
	mpz_set_si(Px, x);
	mpz_add(Px, Px, g);
	mpz_mul(Px, Px, Px);
	mpz_sub(Px, Px, kN);
	if(trial_div(tabex, cof, Px, B, cardB) != 0){
	    gmp_printf("x=%d Px=%Zd\n", x, Px);
	    AddRelation(tabrels, kN, g, cardB, nrels, x, tabex);
	    nrels++;
	    if(nrels == nrelsmax)
		break;
	}
    }
    free(tabex);
    mpz_clears(Px, cof, NULL);
    return nrels;
}

/* to be filled in */

/* Sieving over [-M, M].
   OUTPUT: the actual number of relations found <= nrelsmax */
int FindRelationsUsingSieving(relation_t *tabrels, mpz_t kN, mpz_t g,
			      int *B, int cardB, int lpB, int M, int nrelsmax){
    int nrels = 0;
/* to be filled in */
    return nrels;
}

/* OUTPUT: the actual number of relations found <= nrelsmax */
int FindRelations(relation_t *tabrels, mpz_t kN, mpz_t g, int *B, 
		  int cardB, int M, int nrelsmax){
#if 0
    return FindRelationsUsingTrialDivision(tabrels, kN, g, B, cardB, M, nrelsmax);
#else
    int lpB = (mpz_cmp_ui(kN, 2000) <= 0 ? 0 : 100 * B[cardB-1]);
    return FindRelationsUsingSieving(tabrels,kN,g,B,cardB,lpB,M,nrelsmax);
#endif
}

void PrintMatrix(char **mat, int nrows, int ncols){
    int i, j;

    for(i = 0; i < nrows; i++){
	for(j = 0; j < ncols; j++)
	    printf(" %d", mat[i][j]);
	printf("\n");
    }
}

void PrintMatrices(char **mat, char **C, int nrows, int ncols){
/* to be filled in */
}

/* mat[i1] += mat[i2]; C[i1] += C[i2]. */
void AddRows(char **mat, char **C, int ncols, int i1, int i2, int j){
/* to be filled in */
}

void Gauss(char **mat, char **C, int nrows, int ncols){
/* to be filled in */
}

char **MatrixFromRelations(relation_t *tabrels, int nrows, int ncols){
    char **mat = (char **)malloc(nrows * sizeof(char *));
/* to be filled in */
    return mat;
}

char **BuildCompanionMatrix(int nrows){
    char **C = (char **)malloc(nrows * sizeof(char *));
/* to be filled in */
    return C;
}

int FinishFactorization(factor_t *tabf, int *nf, mpz_t N, mpz_t kN, mpz_t g, 
			relation_t *tabrels, char **mat, char **C,
			int nrelsmax, int *B, int cardB){
    int status = FACTOR_NOT_FOUND;
/* to be filled in */
    return status;
}

int QS_aux(factor_t *tabf, int *nf, mpz_t N, mpz_t kN, mpz_t g, int *B,
	   int cardB, int M, int phase){
    int nrelsmax = cardB+2, nrels, i, status = FACTOR_NOT_FOUND;
    relation_t *tabrels = (relation_t *)malloc(nrelsmax * sizeof(relation_t));
    char **mat, **C;

    nrels = FindRelations(tabrels, kN, g, B, cardB, M, nrelsmax);
    if(nrels < cardB){
	printf("Not enough relations: %d // %d\n", nrels, nrelsmax);
	return -1;
    }
    if(phase == 1)
	return 0;
    mat = MatrixFromRelations(tabrels, nrels, cardB);
#if DEBUG >= 0
    PrintMatrix(mat, nrels, cardB);
#endif
    if(phase == 2)
	return 0;
    C = BuildCompanionMatrix(nrels);
#if DEBUG >= 0
    PrintMatrices(mat, C, nrels, cardB);
#endif
    if(phase == 3)
	return 0;
    Gauss(mat, C, nrels, cardB);
    if(phase == 4)
	return 0;
    status = FinishFactorization(tabf, nf, N, kN, g, tabrels, mat, C, nrels, B, cardB);
    free(tabrels);
    for(i = 0; i < nrels; i++){
	free(mat[i]);
	free(C[i]);
    }
    free(mat);
    free(C);
    return status;
}

/* Source: Silverman87. */
int FindMultiplier(mpz_t N){
    int kopt = -1;
/* to be filled in */
    return kopt;
}

/* OUTPUT: NULL if some problem occurred, a factor base otherwise of size
   cardB, starting {-1, 2, ...}. */
int *BuildFactorBase(mpz_t kN, int k, int cardB, FILE *file){
    int *B = NULL;
/* to be filled in */
    return B;
}

/* This is from Silverman87, but for MPQS. */
void ChooseParameters(int *cardB, int *M, mpz_t N){
    size_t dd = mpz_sizeinbase(N, 10);
    int thresh[] = {10, 24, 30, 36, 42, 48, 54, 60, 66, 0};
    int tcardB[] = {50, 100, 200, 400, 900, 1200, 2000, 3000, 4500, 0};
    int tM[] = {1000, 5000, 25000, 25000, 50000, 100000, 250000, 350000, 500000, 0};
    int i;

    *cardB = -1; *M = -1;
    for(i = 0; thresh[i] != 0; i++){
	if(dd <= thresh[i]){
	    *cardB = tcardB[i];
	    *M = tM[i];
	    break;
	}
    }
}

int QS(factor_t *tabf, int *nf, mpz_t N, int k, int cardB, int M, FILE *file,
       int phase){
    int *B, status = FACTOR_NOT_FOUND;
    mpz_t kN, g;

    mpz_inits(kN, g, NULL);
    if(k == 0){
	k = FindMultiplier(N);
	printf("Best multiplier: %d\n", k);
    }
    mpz_mul_ui(kN, N, k);
    /* g = trunc(sqrt(k*N)) */
    mpz_sqrt(g, kN);
    if(cardB == 0 || M == 0)
	ChooseParameters(&cardB, &M, kN);
    B = BuildFactorBase(kN, k, cardB, file);
    if(B == NULL)
	return FACTOR_ERROR;
    printf("cardB=%d, M=%d\n", cardB, M);
    status = QS_aux(tabf, nf, N, kN, g, B, cardB, M, phase);
    
    mpz_clears(kN, g, NULL);
    free(B);
    return status;
}
