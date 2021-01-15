#include <vector>

/* Define the state of the KECCAK-P function*/
typedef std::vector<std::vector<std::vector<bool>>> state;

class Keccak_p{
	/*
	private:
		state A0;
	*/
	public:
		int get_index(int state_index);
		
		state convert_string_to_state(std::vector<bool> S, int w);

		std::vector<bool> convert_state_to_string(state &A);

		state theta(state &A);

		std::vector<std::vector<bool>> get_C_theta(state &A, int w);

		std::vector<std::vector<bool>> get_D_theta(std::vector<std::vector<bool>> &C, int w);

		state rho(state &A);

		state pi(state &A);

		state chi(state &A);

		state iota(state &A, int ir);

		bool rc(int t);
		state Rnd(state&A, int ir);

		std::vector<bool> keccak_p(int nr, std::vector<bool> S);

		// Keccak_p(state);
		// Keccak_p();
};



