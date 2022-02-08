typedef struct{
    int x;
    char *tabex;
} relation_t;

extern int trial_div(char *tabex, const mpz_t Px, int* B, int cardB);
extern void QS_aux(mpz_t N, mpz_t kN, mpz_t g, int *B, int cardB, int M);
extern int QS(mpz_t N);
