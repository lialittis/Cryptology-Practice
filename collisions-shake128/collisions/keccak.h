#pragma once
#include "utilities.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <bitset>
#include <cmath>
#include <unordered_map>
#include <sstream>

const int slize_size = 25;

// representation of the state as 3d vector
typedef std::vector<std::vector<std::vector<bool>>> state;
typedef std::bitset<8> byte;
typedef std::bitset<4> half_byte;

/**
* This function converts from the indexing of the 3d
* state to the the 3d array.
*/
int to_index(int slice_index);

/**
* This function converts from 1d bits string(represented as a vector of boolean)
* to the 3d state
*/
state convert_string_to_state(std::vector<bool> s, int w);

/**
* This function converts the 3d state to 1d bits string represented as vector
* of bool
*/
std::vector<bool> convert_state_to_string(state& A);

/**
* this function returns the parity of each column in the state.
* i.e it returns the array C of step one in the theta algorithm
*/
std::vector<std::vector<bool>> theta_C(const state& A, int w);

/**
* this function returns the D array of step two in theta algorithm.
*/
std::vector<std::vector<bool>> theta_D(std::vector<std::vector<bool>> &C, int w);

/**
* this function returns A'used in rho algorithm
*/
state get_A_prime(state& A);

// theta step
state theta(state& A);
// rho step
state   rho(state& A);
// pi step
state    pi(state& A);
// chi step
state   chi(state& A);
// iota step
state iota(state& A, int ir);

/**
* this is rc algorithm used in iota
*/
bool rc(int t);

/**
* this function compute the RC constant
*/
std::vector<bool> RC_array(int w, int ir);

/*
* this function applies iota( chi(pi(rho(theta(A)))),ir)
*/
state Rnd(state& A, int ir);

/**
* this function applies Rnd on the state of S for ir from 0 to 24
*/
std::vector<bool> keccak_p(int nr, std::vector<bool> S);

/**
* The padding rule used in sponge
*/
std::vector<bool> pad101(const int x, const int m);

/**
* this function constructs an vector of length r starting from source[i]
*/
std::vector<bool> get_Pi(const std::vector<bool> &source, const int i, const int r);

// sponge construction
std::vector<bool> sponge(std::vector<bool>& N, int d, int r, int b, int nr);


std::vector<bool> keccak_c(std::vector<bool> &N, int d, int c);
std::vector<bool> shake128(std::vector<bool> &M, int d);
