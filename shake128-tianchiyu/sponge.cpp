#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdint>
#include "sponge.h"
#include "keccak_p.h"


using namespace std;

/* pad 10*1 function
   x is positive integer
   m is non-negative integer
 */
std::vector<bool> Sponge::pad101(const int x, const int m) {

	int j = ((((-1 * m - 2) + x) % x) + x) % x;  // To realise the mod
	int len = 2 + j;
	std::vector<bool> P(len, false);
	P[0] = true;
	P[P.size() - 1] = true;
	return P;
}

std::vector<bool> concat(std::vector<bool> &N, std::vector<bool> &pad) {
    std::vector<bool> v;
	v.reserve(N.size() + pad.size());

	std::copy(N.begin(), N.end(), std::back_inserter(v));

	std::copy(pad.begin(), pad.end(), std::back_inserter(v));

	return v;
}


/* 
   SPONGE[f,pad,r](N,d)
 */

std::vector<bool> Sponge::sponge(std::vector<bool>& N, int d, int r, int b, int nr) {
    
	// separate P to P0, P1, ... P_n-1
	std::vector<bool> S(b); // zero_b

	Keccak_p keccak_p;
	int len_N = N.size();
	auto pad = pad101(r, len_N);
	auto P = concat(N, pad);
	
	// printf("P is %x\n");
	/*
	for(int i=0; i<P.size();i++){
		std::cout<<P[i];
	}
	*/
	int n = P.size()/r;
	int c = b - r;

	std::vector<bool> zero_c(c);
	// separate P to P0, P1, ... P_n-1

	for(int i = 0; i < n; ++i) {
		std::vector<bool> Pi(r);
		std::copy(P.begin() + i * r, P.begin() + i * r + r, Pi.begin());
		auto Pi_zero_c = concat(Pi, zero_c);
		std::vector<bool> S_Pi(S.size());
		for(size_t i = 0; i < S.size(); ++i) S_Pi[i] = S[i] xor Pi_zero_c[i];
		S = keccak_p.keccak_p(nr,S_Pi);
	}
	
	std::vector<bool> Z;
	
	while(true) {
    
		std::vector<bool> trunc_r(r);
    
		std::copy(S.begin(), S.begin() + r, trunc_r.begin());
		Z = concat(Z, trunc_r);
		if(d <= Z.size()) {
      
			std::vector<bool> trunc_d(d);
      
			std::copy(Z.begin(), Z.begin() + d, trunc_d.begin());
      
			return trunc_d;
		}
		S = keccak_p.keccak_p(nr,S);
	}
  
	return Z;
} 


