/****************************************************************/
/* dlog.h                                                       */
/* Author : Alain Couvreur                                      */
/* alain.couvreur@lix.polytechnique.fr                          */
/* Last modification October 26, 2017                           */
/****************************************************************/


void babySteps(hash_table H, mpz_t nBabySteps, mpz_t base, mpz_t p);
void giantSteps(mpz_t result, hash_table H, mpz_t nBabySteps, mpz_t base,
				mpz_t p, mpz_t kz);
