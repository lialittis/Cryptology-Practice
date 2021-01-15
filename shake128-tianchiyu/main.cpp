#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include "keccak.h"


using namespace std;

static void read_binary_from_file(ifstream& fin, vector<bool>& input, vector<bool>::size_type b){
	for(vector<bool>::size_type i = 0; i < b;){
        unsigned char aggr;
        fin.read((char*)&aggr, sizeof(unsigned char));
        for(unsigned char mask = 1; mask > 0 && i < b; ++i, mask <<= 1)
            input.at(i) = aggr & mask;
    }
}

static void output_binary_as_ascii(vector<bool>& output) {
	for (int i = 0; i < output.size(); i += 8) {
		unsigned c = 0;
		for (int j = 0; j < 8; j++)
			if (output[i + j]) c |= (1 << j);
		printf("%02X ", c);
	}

	printf("\n");
}

int main(int argc, char *argv[]) 
{
	if (argc < 3) {
		printf("Usage: %s NUMBER_BYTES BINARY_FILENAME\n", argv[0]);
		return 1;
	}

	int nbits = 8 * atoi(argv[1]);
	string filename = (string)argv[2];

    long int start,end,file_size_binary;
	ifstream fin(filename, ios::binary);

	fin.seekg(0, ios::end);
	int file_size = fin.tellg();
	fin.seekg(0, ios::beg);
	file_size_binary = file_size * 8;

	//printf("file size is :%d\n",file_size_binary);

	vector<bool>::size_type length = file_size_binary;
    vector<bool> input_binary_vector(length);

	read_binary_from_file(fin,input_binary_vector,length);
	fin.close();

	Keccak keccak;

	output_binary_as_ascii(input_binary_vector);

	auto digest = keccak.shake128(input_binary_vector, nbits);

	assert(digest.size() == nbits);

	output_binary_as_ascii(digest);

	return 0;
}
