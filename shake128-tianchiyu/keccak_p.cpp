#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <bitset>
#include <math.h>
#include "keccak_p.h"

using namespace std;

/* Keccak uses a specific permutation function KECCAK-P.
   It is a composition of 5 very simple permutations (theta, rho, pi, chi, iota),
   repeated nr times over.

   KECCAK-P[b,nr](S)

   where the argument is S, the state of input;
   b is the length of S and nr is the number of rounds.
 */


/* Adjustment for the index of Slice/State.
   Because the index of 3-d array is not corresponding to the index of slice.
 */

int Keccak_p::get_index(int state_index) { 
	return (state_index + 2)  % 5;
}


/* Converting strings to state arrays
   For all triples(0<= x < 5, 0 <= y <5, and 0<= z < w)
   A[x,y,z] = S[w(5y+z)]
   For example, if b = 1600, so that w = 64
 */
state Keccak_p::convert_string_to_state(vector<bool> S, int w){
	state A(5, vector<vector<bool>>(5, vector<bool>(w)));
	// printf("A is \n");
	for(int y = 0; y<5; y++){
		int y_index = get_index(y);
		for(int x = 0; x<5 ; x++){
			int x_index = get_index(x);
			for(int z=0;z<w;z++){
				A[x_index][y_index][z] = S[w*(5*y + x)+z];
				// std::cout<<A[x_index][y_index][z];
			}
		}
	}
	return A;
}


/* Converting state arrays to strings
   For each pair of integers (i,j) s.t. 0 <= i < 5, 0<= j <5, define 
 */

vector<bool> Keccak_p::convert_state_to_string(state &A){
	int b = 25 * A[0][0].size();
	int w = A[0][0].size();

	vector<bool> S(b);
	int i = 0;
	// printf("S is\n");

	for(int y = 0; y<5 ; y++){
		int y_index = get_index(y);
		for(int x = 0; x<5; x++){
			int x_index = get_index(x);
			for(int z = 0; z< w;z++){
				S[i++] = A[x_index][y_index][z];
				// std::cout<<S[i];
			}
		}
	}
	return S;
}



/* Step Mappings
   The five step mappings that comprise a round of KECCAK-p[n,nr]
 */


/* Specification of theta
   Step 1: calculate C[x,z]
   Step 2: calculate D[x,z]
   Step 3: calculate A_new[x,y,z]
 */
state Keccak_p::theta(state &A){
	int w = A[0][0].size();

	auto C = get_C_theta(A,w);
	auto D = get_D_theta(C,w);

	/* Initialisation*/
	state A_new(5, vector<vector<bool>>(5, vector<bool>(w, false)));
	
	for(int x = 0; x<5;x++){
		int x_index = get_index(x);
		for(int y = 0; y < 5; y++){
			int y_index = get_index(y);
			for(int z = 0; z<w; z++){
				A_new[x_index][y_index][z] = A[x_index][y_index][z] ^ D[x_index][z];
				// std::cout<<A_new[x_index][y_index][z];
			}
		}
	}
	// printf("\n\n");
	return A_new;
}

vector<vector<bool>> Keccak_p::get_C_theta(state &A, int w){

	vector<vector<bool>> C(5,vector<bool>(w,false));
	
	for(int x = 0; x<5; x++){
		int x_index = get_index(x);
		for(int z = 0; z <w; z++){
			C[x_index][z] = A[x_index][0][z] ^ A[x_index][1][z] ^ A[x_index][2][z] ^ A[x_index][3][z] ^ A[x_index][4][z];
		}
	}
	return C;
}

vector<vector<bool>> Keccak_p::get_D_theta(vector<vector<bool>> &C, int w){

	vector<vector<bool>> D(5,vector<bool>(w,false));
	for(int x = 0; x < 5; x++) {
		int x_index = get_index(x);
		for(int z = 0; z < w; z++) {
			int x1 = (x - 1 + 5) % 5;
			int x2 = (x + 1) % 5;

			x1 = get_index(x1);
			x2 = get_index(x2);

			int z_offset  = (z - 1 + w) % w;
			D[x_index][z] = C[x1][z] ^ C[x2][z_offset];
		}
	}
	return D;	
}


/* Specification of rho
   step 1: set A_new
   step 2: let (x,y) = (1,0)
   step 3: update (x,y) and A_new recursively
 */

state Keccak_p::rho(state &A){
	int w = A[0][0].size();
	/*step 1*/
	state A_new(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
	
	int zero = get_index(0);
	for(int z = 0; z < w; z++){
		A_new[zero][zero][z] = A[zero][zero][z];
	}

	/*step 2*/
	int x = 1;
	int y = 0;
	
	/*step 3*/
	// int x_index = get_index(x);
	// int y_index = get_index(y);
	
	for(int t = 0; t <= 23; t++) {
		int x_index = get_index(x);
		int y_index = get_index(y);
		for(int z = 0; z < w; z++) {
			int z_index = (((z - ((t + 1) * (t + 2))/ 2) + w) % w + w) % w;
			A_new[x_index][y_index][z] = A[x_index][y_index][z_index];
			// std::cout<<A_new[x_index][y_index][z];
		}
		int tmp = y; y = (2*x + 3*y) % 5; x = tmp;
	}
	//printf("\n \n");
  
	return A_new;
}


/* Specification of pi
   step 1: A_new[x,y,z]
   step 2: Return A_new
 */
state Keccak_p::pi(state &A){
	int w = A[0][0].size();
	state A_new(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
	
	for(int x = 0; x < 5; x++) {
		int x_index = get_index(x);
		for(int y = 0; y < 5; y++) {
			int y_index = get_index(y);
			for(int z = 0; z < w; z++) {
				int x_update = get_index((x + x*y) % 5);
				A_new[x_index][y_index][z] = A[x_update][x_index][z];
			}
		}
	}
	return A_new;
}

/* Specification of chi
   step 1: A_new[x,y,z]
   step 2: Return A_new
 */
state Keccak_p::chi(state &A){
	int w = A[0][0].size();

	state A_new(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
	
	for(int x = 0; x < 5; x++) {
		int x_index = get_index(x);
		for(int y = 0; y < 5; y++) {
			int y_index = get_index(y);
			for(int z = 0; z < w; z++) {
				int x_update1 = get_index((x + 1) % 5);
				int x_update2 = get_index((x + 2) % 5);
				A_new[x_index][y_index][z] = A[x_index][y_index][z] ^ ((A[x_update1][y_index][z] ^ 1)& A[x_update2][y_index][z]);
			}
		}
	}
	return A_new;
}


/* Specification of iota

   Algorithm rd(t):
   step 1: if t mod 255 = 0, return 1
   step 2: R = 10000000
   step 3: update R
   step 4: return R

   Algorithm iota(A,ir):
   step 1: A_new = A
   step 2: Let RC = 0^w
   step 3: RC[2^j-1] = rc(j + 7ir)
   step 4: update A_new[0,0,z]
   step 5: return A_new
 */

state copy(state &A){
	int w = A[0][0].size();
	state A_new(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
	std::copy(A.begin(), A.end(), A_new.begin());
	return A_new;
}

state Keccak_p::iota(state &A, int ir){
	auto A_new = copy(A);

	int w = A[0][0].size();

	std::vector<bool> RC(w,false);
	int l = (int)log2(w);
	//printf("l is %d\n",l);

	for(int j = 0; j  <= l; j++){
		RC[pow(2,j)-1] = rc(j+7*ir);
	}

	int zero = get_index(0);
	for(int z= 0; z < w; z++){
		A_new[zero][zero][z] = A_new[zero][zero][z] ^ RC[z];
	}

	return A_new;
}

/*
bool Keccak_p::rc(int t){

	if((t % 255) == 0)  return 1;

	//std::bitset<9> R("010000000");

	vector<bool> R = {1,0,0,0,0,0,0,0};

	for(int i = 1; i < (t % 255); i++){
		R.insert(R.begin(),0);
		
		R[0] = R[0] ^ R[8];
		R[4] = R[4] ^ R[8];
		R[5] = R[5] ^ R[8];
		R[6] = R[6] ^ R[8];
		
		R[8] = R[8] xor R[0];
		R[4] = R[4] xor R[0];
		R[3] = R[3] xor R[0];
		R[2] = R[2] xor R[0];
		R.resize(8);
	}
	return R[0];
}
*/



bool Keccak_p::rc(int t) {
  if((t % 255) == 0) return 1;
  std::bitset<9> R("010000000");
  int limit = t % 255;
  for(int i = 1; i < limit; ++i) {
    R[8] = R[8] xor R[0];
    R[4] = R[4] xor R[0];
    R[3] = R[3] xor R[0];
    R[2] = R[2] xor R[0];
    std::bitset<9> tmp;
    R >>= 1;
  }
  return R[0];
}


/* Test functions
   print_state(A)
 */
void print_state(state&A) {
	int w = A[0][0].size();
	for (int x = 0; x <5; x++) {
		for (int y = 0; y <5; y++) {
			for (int z = 0; z < w; z++) {
				printf("%d ",A[x][y][z]);
			}
			printf("\n");
		}        
		printf("\n \n");
	}
}



/* One round of state update/transformation
 */
state Keccak_p::Rnd(state&A, int ir){
	// print_state(A);
	
	auto _theta = theta(A);
	// print_state(theta_A);
	auto _rho = rho(_theta);
	auto _pi = pi(_rho);
	auto _chi = chi(_pi);
	auto _iota = iota(_chi,ir);
	return _iota;
} 


/* KECCAK-p[b,nr](S)
 */
std::vector<bool> Keccak_p::keccak_p(int nr, std::vector<bool> S){
	
	int b = S.size();
	// printf("b is %d\n",b);
	int w = b / 25;
	int l = (int)log2(w);
	// here we take nr = 12 + 2*l
	// int nr = 12 + 2*l;

	int ir_low = 12 + 2*l - nr;

	int ir_high = 12 + 2*l -1;

	// printf("ir_high is %d\n",ir_high);

	auto A = convert_string_to_state(S,w);

	// print_state(A);

	for(int ir = ir_low; ir <= ir_high ; ir++){
		A = Rnd(A,ir);
	}

	// print_state(A);

	auto S_new = convert_state_to_string(A);

	/*test part*/
	/*
	for (int i = 0; i < S_new.size(); i += 8) {
		unsigned c = 0;
		for (int j = 0; j < 8; j++)
			if (S_new[i + j]) c |= (1 << j);
		printf("%02X ", c);

	}
	*/
	// printf("\n");
	return S_new;
}

