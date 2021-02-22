#include <gmp.h>
#include <gmpxx.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "utils.h"


using namespace std;

mpz_class gcd(mpz_class a, mpz_class b){
	mpz_class f;
	if(b == 0 ) {
		f = a;
	}else{
		f = gcd(b,a%b);
	}
	return f;
}

/* is_probable_prime()
   returns 0 is m is composite number
   returns 1 is that m is probably a prime
   returns 2 is that m is definatly a prime
 */
int is_probable_prime(mpz_class m,int ntrials){
	int flag = mpz_probab_prime_p(m.get_mpz_t(),ntrials);
	if(flag==0)
		return 0;
	else if(flag ==1)
		return 1;
	else
		return 2;
}

mpz_class trial_division(mpz_class N){
	vector<mpz_class> smallprimes = get_primesUpTo((mpz_class)10000);

	for(int i=0;i<smallprimes.size();++i){
		mpz_class smallprime = smallprimes[i];
		int flag = 0;
		// cout<<"check "<<smallprime.get_str()<<endl;
		while(N%smallprime ==0){
			N = N/smallprime;
			flag = 1;
		}
		if(flag ==1){
			cout<<"+++++++++++++++++++++++"<<endl;
			cout<<">> small factor:"<<smallprime.get_str()<<endl;
			cout<<"+++++++++++++++++++++++"<<endl;
			flag = 0;
		}
	}

	return N;
}

vector<mpz_class> get_primesUpTo(mpz_class N){
    // Create a boolean array
    // "prime[0..n]" and initialize
    // all entries it as true.
    // A value in prime[i] will
    // finally be false if i is
    // Not a prime, else true.
	
	// cout<<"Calculate primes up to "<<N.get_si()<<endl;
    vector<bool> prime(N.get_si()+1,true);
	//cout<<"N is"<<N.get_si()<<endl;
    // memset(prime, true, sizeof(prime));

    for (int p = 2; p * p <= N; p++)
    {
        // If prime[p] is not changed,
        // then it is a prime
        if (prime[p] == true)
        {
            // Update all multiples
            // of p greater than or
            // equal to the square of it
            // numbers which are multiple
            // of p and are less than p^2
            // are already been marked.
			// cout<<"p"<<p<<endl;
            for (int i = p * p; i <= N; i += p){
                prime[i] = false;
			}
        }
    }

    // Print all prime numbers
	
	/*
    for (int p = 2; p <= N; p++){
        if (prime[p])
            cout << p << " ";
	}
	*/

	vector<mpz_class> primes;
	for(int p = 2; p<=N; p++){
		if(prime[p]){
			//cout<<p<<" ";
			primes.push_back(p);
		}
	}
	// cout<<endl;
	return primes;
}
