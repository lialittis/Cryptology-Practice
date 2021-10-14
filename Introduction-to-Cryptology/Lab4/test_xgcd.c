#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"

#include "xgcd.h"

void testab(const char *sa, const char *sb, const char *sg){
    mpz_t a, b, g, u, v, gg;
    int ok;

    mpz_init_set_str(a, sa, 10);
    mpz_init_set_str(b, sb, 10);
    mpz_init_set_str(gg, sg, 10);
    mpz_inits(g, u, v, NULL);

    XGCD(g, u, v, a, b);

    ok = mpz_cmp(g, gg) == 0;

    mpz_clears(a, b, g, gg, u, v, NULL);
    if(ok)
		printf("[OK]\n");
	else
		printf("[FAILED]\n");
	
}


void testlem(const char *sa, const char *sb, const char *sm, const char *sx){
    mpz_t a, b, m, x, tmp;
    int ok = 1, status;

    mpz_init_set_str(a, sa, 10);
    mpz_init_set_str(b, sb, 10);
    mpz_init_set_str(m, sm, 10);
    mpz_init_set_str(x, sx, 10);

    mpz_init(tmp);
    status = linear_equation_mod(tmp, a, b, m);
    if(status == 0)
		ok = mpz_sgn(x) == 0;
    else
		ok = mpz_cmp(tmp, x) == 0;

    mpz_clears(a, b, m, x, tmp, NULL);

    if(ok)
		printf("[OK]\n");
	else
		printf("[FAILED]\n");
}

int main(int argc, char *argv[]){
    int n = atoi(argv[1]);

    switch(n){
    case 1:
		printf("test 1.1:      ");
		testab("101", "11", "1");
		printf("test 1.2:      ");
		testab("101", "101", "101");
		printf("test 1.3:      ");
		testab("101", "0", "101");
		printf("test 1.4:      ");
		testab("1010", "35", "5");
		break;
    case 2:
		printf("test 2.1:      ");
		testlem("1", "50", "101", "50");
		printf("test 2.2:      ");
		testlem("11", "50", "101", "78");
		printf("test 2.3:      ");
		testlem("11", "50", "135", "115");
		printf("test 2.4:      ");
		testlem("10", "53", "135", "0");
		printf("test 2.5:      ");
		testlem("10", "50", "135", "5");
    }
    return 0;
}
