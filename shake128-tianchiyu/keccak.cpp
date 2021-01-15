#include <vector>
#include "keccak.h"
#include "sponge.h"
#include <math.h>
#include <iostream>

#define B 1600
#define CAPACITY 256

std::vector<bool> Keccak::keccak_c(std::vector<bool> &N, int d, int c){
	Sponge sponge;

	int b = B;
	int w = b / 25;
	int l = (int)log2(w);
	// Here we take nr = 12 + 2 *l
	int nr = 12 + 2*l;
	std::cout << "nr is: "<< nr<< std::endl;
	int r = b - c;

	return sponge.sponge(N, d, r, b, nr);

}

std::vector<bool> Keccak::shake128(std::vector<bool> &M, int d) {

	M.push_back(1); M.push_back(1); M.push_back(1); M.push_back(1);

	return keccak_c(M, d, CAPACITY);

}

