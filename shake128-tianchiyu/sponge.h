#include <vector>

class Sponge{
	public:
		std::vector<bool> pad101(const int x, const int m);

		std::vector<bool> sponge(std::vector<bool>& N, int d, int r, int b, int nr);

		/* Construction function*/
		// Sponge();
};
