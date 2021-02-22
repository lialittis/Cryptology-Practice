#include <gmp.h>
#include <gmpxx.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


typedef struct{
	mpz_class a;
	mpz_class b;
}coordinates_t;

/*
typedef struct{
    mpz_class f;
    int e, status;
} factor_t;

#define FACTOR_IS_PRIME          1
#define FACTOR_IS_PROBABLE_PRIME 2
#define FACTOR_IS_COMPOSITE      3
#define FACTOR_IS_UNKNOWN        4

#define FACTOR_NOT_FOUND         0
#define FACTOR_FOUND             1
#define FACTOR_FINISHED          2
#define FACTOR_ERROR             3

*/

/* gcd uses the Euclidean algorithm to compute the GCD of a and b*/
mpz_class gcd(mpz_class a, mpz_class b);

/* find all primes up to N*/
vector<mpz_class> get_primesUpTo(mpz_class N);

/* trial_division(N) finds any prime factors of N less than 10000
   1229 such primes;
   using the sieve of Eratosthenes;
   return the modified N;
 */
mpz_class trial_division(mpz_class N);

/* is_probable_prime returns if and only if the integer m passes ntrials iterations of 
   the Miller-Rabin primality test
 */
int is_probable_prime(mpz_class m,int ntrials);

/* factorization(N) prints out as many prime factors of N(with their exponents) as possible
   it will call trial_division first, to find any easy tiny factors,
   before making a number of calls to ECMTrial.
 */
void factorization(mpz_class N);
