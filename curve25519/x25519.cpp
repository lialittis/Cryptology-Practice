#include <gmp.h>
#include <gmpxx.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include "global.h"
#include "montgomery.h"

using namespace std;

/*
void readFile(istream & file, uint32_t &out){
	char* buff = new char[3];
	stringstream str;
	int x;
	for(int i=0;i<4;i++){
		file.get(buff,3);
		str.seekp(0);
		str.seekg(0);
		str.write(buff,2);
		str>>hex>>x;
		// cout<<hex<<buff[0]<<buff[1]<<endl;
		out+=x<<(8*i);
		// cout<<hex<<out<<endl;
	}
}
*/


mpz_class decodeLittleEndian(vector<mpz_class> b,int bits){
	mpz_class output = 0;
	for(int i=0;i<(bits+7)/8;i++){
		b[i] = b[i] << (8*i);
		output += b[i];
	}
	return output;
}

mpz_class decodeScalar25519(mpz_class k){
	vector<mpz_class> k_list(32,1);
	for(int i = 0; i< 32; i++){
		k_list[i] = 255&(k>>(8*i));
	}
	k_list[0] &= 248;
	k_list[31] &= 127;
	k_list[31] |=64;
	return decodeLittleEndian(k_list,255);
}

mpz_class decodeUCoordinate(mpz_class u, int bits){
	vector<mpz_class> u_list(32,1);
	for(int i=0;i<32;i++){
		u_list[i] = 255&(u>>8*i);
	}
	if(bits%8==0){
		u_list[-1] &= (1<<(bits%8)) - 1;
	}
	return decodeLittleEndian(u_list,bits);
}

int get_file_size_in_byte(char* file_name){
	int size = 0;
	ifstream in(file_name);
	in.seekg(0,ios::end);
	size = in.tellg();
	in.seekg(0,ios::beg);
	return size;
}

int main(int argc,char ** argv)
{

	if(argc < 2){
		cout<<"Usage: "<< argv[0]<<" secretFile uFile(default=9)"<<endl;
	}
	
	ifstream secretFile;
	secretFile.open(argv[1]);
	if(!secretFile){
		cout<<"The secret File cannot be opened !"<<endl;
		return 1;
	}

	int secret_size = get_file_size_in_byte(argv[1]);
	cout<<secret_size<<endl;

	char key[secret_size-1];
	secretFile.read(key,secret_size-1);
	string key_string(key,secret_size-1);
	cout<<key_string<<endl;

	// rerverse the string
	reverse(key_string.begin(),key_string.end());

	// reverse for each two
	for(int i=0;i<key_string.length();i+=2){
		reverse(key_string.begin()+i,key_string.begin()+i+2);
	}

	cout<<"read scalar string success!"<<endl;
	secretFile.close();

/*
	uint32_t key[16];
	for(int i=0; i<32;i++){
		readFile(secretFile,key[i]);
	}
	secretFile.get();
	
*/

	mpz_class m = mpz_class(key_string,16);
	//cout<<"secret key is:"<<endl;
	// m = decodeScalar25519(m);
	// mpz_import(m.get_mpz_t(),8,-1,sizeof(key[0]),0,0,key);
	gmp_printf("m is %Zx\n",m);

	m = decodeScalar25519(m);
	gmp_printf("after decode m is %Zd\n",m);
	
	mpz_class u;
	if(argc == 3){
		secretFile.open(argv[2]);
		if(!secretFile){
			cout<<"wrong!"<<endl;
			return 1;
		}
		int u_size = get_file_size_in_byte(argv[2]);
		//cout<<secret_size<<endl;
		char u_char[u_size-1];
		secretFile.read(u_char,u_size-1);
		string u_string(u_char,u_size-1);
		cout<<u_string<<endl;
		cout<<"read u string success!"<<endl;
		// rerverse the string
		reverse(u_string.begin(),u_string.end());
		
		// reverse for each two
		for(int i=0;i<u_string.length();i+=2){
			reverse(u_string.begin()+i,u_string.begin()+i+2);
		}
		u = mpz_class(u_string,16);

	}else{
		u = 9;
	}



	gmp_printf("before decode u is %Zd\n",u);
	u = decodeUCoordinate(u,255);
	gmp_printf("after decode u is %Zd\n",u);

	//coordinates_t dot;

	/*Test for montgomery*/

	mpz_class p;
	mpz_class A;

	/*Test 1*/
	/*
	m = 77;
	p = 101;
	A = 49;
	u = 2;
	*/
	/*Test 2*/
	/*
	m = 947;
	p = 1009;
	A = 682;
	u = 7;
	*/

	
	/*Test 3: curve 25519*/

	/*
	p =((mpz_class) 1<< 255)-19;
	A = 486662;
	u = 9;
	m = 7;
	*/
	
    p =((mpz_class) 1<< 255)-19;
	A = 486662;
	
	cout<<"prime: p = "<<p<<endl;
	cout<<"curve constant: A = "<<A<<endl;
	auto output = ladder(m,u,p,A);
	
	cout<<"The X on (X,Z) coordinates is:"<<output.get_str()<<endl;

	auto temp = output.get_str(16);
	reverse(temp.begin(),temp.end());
	// reverse for each two
	for(int i=0;i<temp.length();i+=2){
		reverse(temp.begin()+i,temp.begin()+i+2);
	}

	cout<<"The output u-coordinate is "<<temp<<endl;

	return 0;
}


