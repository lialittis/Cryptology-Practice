#include <gmp.h>
#include <gmpxx.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>

mpz_class get_prime();

coordinates_t ladder(mpz_class m, coordinates_t P, mpz_class p, mpz_class A);

coordinates_t cswap(mpz_class swap, mpz_class x_2,mpz_class x_3);


coordinates_t xADD(mpz_class x_1,mpz_class x_2,mpz_class x_3,mpz_class z_2,mpz_class z_3,mpz_class p);

coordinates_t xDBL(mpz_class x_2,mpz_class x_3,mpz_class z_2,mpz_class z_3,mpz_class p,mpz_class A_constant);



mpz_class get_a24(mpz_class p, mpz_class A_constant);

