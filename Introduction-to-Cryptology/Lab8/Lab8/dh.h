void DH_init(mpz_t a, gmp_randstate_t state);
void AES128_key_from_number(buffer_t *key, mpz_t n);
void buffer_from_mpz(buffer_t *buf, mpz_t n);
void buffer_to_mpz(mpz_t n, buffer_t *buf);
void concatenate_gb_ga(mpz_t tmp, mpz_t gb, mpz_t ga, mpz_t p);
void SIGNSK(mpz_t sigmaB, mpz_t gb, mpz_t ga, mpz_t p, mpz_t NB, mpz_t dB);
