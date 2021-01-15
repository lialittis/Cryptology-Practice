#include<vector>


class Keccak{
	public:

		// Keccak();

		std::vector<bool> keccak_c(std::vector<bool> &m, int len,int capacity);

		std::vector<bool> shake128(std::vector<bool> &M, int d);
};


