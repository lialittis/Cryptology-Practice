


static void fermat() {
  //long long y = -1;
  mpz_t y, N, temp_n, temp_y, x2,x,p,q;
  mpz_inits(y,N,temp_n,temp_y,x2,x,p,q, NULL);
  mpz_set_str(N,"13125617032138349372394352970905698273665597620287503814174732158970275446694307939305083958196843700425451532134031169644827022124389791898748203394712807",10);
  gmp_printf("-th encrypted = %Zd.\n\n", N);
  mpz_set_si(y,-1);
  do {
    mpz_add_ui(y, y,1);
    mpz_mul_ui(temp_n,N, 4);
    mpz_pow_ui(temp_y,y,2);
    mpz_add(x2,temp_n,temp_y);
  } while(!mpz_perfect_square_p (x2));
  mpz_sqrt(x,x2);
  mpz_sub(p,x,y);
  mpz_div_ui(p,p,2);
  mpz_add(q,x,y);
  mpz_div_ui(q,q,2);
  gmp_printf("EXIT X2 = %Zd. p:%Zd q:%Zd\n\n", x2, p, q);
}
