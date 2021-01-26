#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "keccak.h"
#include "utilities.h"
#include <assert.h>

using namespace std;

vector<bool> binary_vector_init(int NBits){
	vector<bool> r(NBits,false);
	return r;
}

string get_message_init(){
	string message_init("I don't think that you can succeed, but anyway, if you find this collision, it means you win!");
	return message_init;
}

static void string_to_binary(string message, vector<bool> &input,vector<bool>::size_type b){
	int j = 0;
	for(vector<bool>::size_type i = 0; i< b;j++){
		unsigned char aggr;
		aggr = message.at(j);
		cout<<"b is "<< b <<", i is "<<i<<endl;
		for(unsigned char mask = 1; mask > 0 && i<b ; i++, mask <<= 1){
			input.at(i) = aggr & mask;
		}
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


static string gm(vector<bool> &r,int N){

	string message = get_message_init();
	string space = " ";
	string shortline = "-";

	int len = message.length();
	for(int i = 0; i <N; i++){
		if(r[i] == true && i < len){
			message.insert(i,space);
		} else if(i>=len){
			message.append(message);
		} else{
			message = message;
		}
	}

	return message;
}


vector<bool> run_hash(vector<bool> &r, int N){

	printf("start hashing\n");
	string message = gm(r,N);
	
	vector<bool>::size_type length = 8 * message.length();
	vector<bool> input_binary_vector(length);
	
	string_to_binary(message,input_binary_vector,length);


	// My method 
	// Keccak keccak;
	// auto digest = keccak.shake128(input_binary_vector, NBits);
	
	cout << "applying shake128" << endl;
	
	auto digest = shake128(input_binary_vector,N);
	
	/*
	auto res = shake128(input_binary_vector,N);
	ofstream out;
	
	out.open("digest.bin");
	std::cout << "writing the result in digest.bin" << std::endl;
	
	binary_write(out, res);
	
	out.close();
	
	in.open("digest.bin", std::ios::binary);
	
	auto vsize = get_file_size_in_byte("digest.bin");
	
	std::vector<bool> ver(vsize);
	
	binary_read(in, ver, vsize);
	std::cout << "uncomment the last lines to display the result" << std::endl;
	
	auto str = vec_of_bool_string(ver);
	auto ss = reverse_string(str);
	//  std::cout << vsize << std::endl;
	std::string display = ss; //std::string(ss.begin(), ss.begin() + N/4);
	std::transform(display.begin(), display.end(), display.begin(), ::tolower);
	std::cout << display << std::endl;
	*/


    // auto digest = keccak.shake128(input_binary_vector, NBits);
	assert(digest.size() == N);

	output_binary_as_ascii(digest);

	return digest;
}

void Floyd(int N){
	int s = 1;
	vector<bool> r = binary_vector_init(N);
	vector<bool> T = r;
	vector<bool> H = r;
	do{
		T = run_hash(T,N);
		H = run_hash(H,N);
		H = run_hash(H,N);
	}while(T!=H);

	vector<bool> T1 = T;
	vector<bool> T2 = r;

	printf("Catch you !\n");

	vector<bool> T1_p = run_hash(T1,N);

	vector<bool> T2_p = run_hash(T2,N);
	
	do{
		T1 = T1_p;
		T2 = T2_p;
		T1_p = run_hash(T1,N);
		T2_p = run_hash(T2,N);	
	}while(T1_p!=T2_p);

	string message1 = gm(T1,N);
	string message2 = gm(T2,N);

	printf("The collision is caused by:\n");
	cout<<message1<<endl;
	cout<<message2<<endl;
}



#ifdef COLLISION_

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s\n", argv[0]);
		return 1;
	}

	int NBits = 8*atoi(argv[1]);

	// printf("%s\n",message.c_str());
	// cout<<message<<endl;
	
	// My method
	// output_binary_as_ascii(digest);

	Floyd(NBits);

	return 0;
}

#endif
