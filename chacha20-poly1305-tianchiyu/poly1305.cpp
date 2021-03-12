#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <gmp.h>
#include <gmpxx.h>
#include <vector>
#include <sstream>


using namespace std;

void num_to_16_le_bytes(uint64_t ac, uint8_t * out);


/*
   Adapted from poly1305aes_test_clamp.c version 20050207
   D. J. Bernstein
   Public domain.
*/
void clamp(uint8_t * r);


/* Poly1305 Algorithm
   The inputs to Poly1305 are:

   -  A 256-bit one-time key

   -  An arbitrary length message

   The output is a 128-bit tag.

 */

void poly1305(uint8_t * key, uint8_t * message, uint64_t message_length, uint8_t * out){   
	uint8_t r_tab[16];
	copy(key,key+16,r_tab); // le bytes to num(key[0..15])
	
	cout<<"r before clamping:"<<endl;
	for(uint16_t i=0; i< 16;i++){
		//cout<<i<<endl;
		cout<<setfill('0')<<setw(2)<<hex<<(int)(r_tab[i] & 0xff)<<" ";
		if(i%16==15){
			cout<<"\n";
		}
	}

	clamp(r_tab);


	/* key r, s*/
    mpz_t r; 
    mpz_init (r);
    mpz_import (r, 16, -1, sizeof(r_tab[0]), 0, 0, r_tab);

	gmp_printf("clamped r is %Zx",r);
	cout<<"\n";

	mpz_t s; 
	mpz_init (s);
	mpz_import (s, 16, -1, sizeof(key[16]), 0, 0, key+16);
    
	gmp_printf("s is %Zx",s);
	cout<<"\n";
	mpz_t p;
	mpz_init(p);
	// p = (1<<130)-5;
    mpz_set_str(p,"3fffffffffffffffffffffffffffffffb",16);
    
    mpz_t n;
    mpz_init(n);
    
    mpz_t a;
    mpz_init(a);
    
    uint64_t nBlocks = message_length/16;
    uint8_t messagesBytes[17];
    messagesBytes[16] = 0x01;
    
	for(uint64_t i=0; i<nBlocks; i++){
        copy(message+i*16, message +(i+1)*16, messagesBytes);
        mpz_import (n, 17, -1, sizeof(messagesBytes[0]), 0, 0, messagesBytes);
        mpz_add(a,a,n);      
        mpz_mul(a,r,a);
        mpz_mod(a,a,p);
		cout<<"Block #"<<i<<endl;
	    gmp_printf("acc is %Zx",a);
		cout<<endl;
		
    }

    if(message_length%16 !=0){
		cout<<"message_length%16 "<< message_length%16<<endl;
		messagesBytes[message_length%16] = 0x01;
        copy(message+nBlocks*16, message + message_length -0, messagesBytes);
        mpz_import (n, message_length%16+1, -1, sizeof(messagesBytes[0]), 0, 0, messagesBytes);
		mpz_add(a,a,n);
        mpz_mul(a,r,a);
        mpz_mod(a,a,p);
        gmp_printf("n is %Zx \n",n);
		cout<<"Block #"<<nBlocks<<endl;
        gmp_printf("a is %Zx",a);
		cout<<endl;
	}

    mpz_add(a,a,s);
	gmp_printf("final acc is %Zx",a);
	cout<<"\n";
    size_t countp = 0;
    mpz_export(out, &countp, -1, 1, 1, 0, a);
}


/* key part 1: r
   -  r[3], r[7], r[11], and r[15] are required to have their top four
      bits clear (be smaller than 16)

   -  r[4], r[8], and r[12] are required to have their bottom two bits
      clear (be divisible by 4)

 */
/*
   Adapted from poly1305aes_test_clamp.c version 20050207
   D. J. Bernstein
   Public domain.
*/
void clamp(uint8_t * r){
	r[3] &= 15;
	r[7] &= 15;
    r[11] &= 15;
    r[15] &= 15;
    r[4] &= 252;
    r[8] &= 252;
    r[12] &= 252;
}

/* Poly1305 Key Generation*/

void poly1305_key_gen(uint32_t * key, uint8_t * out){
	for(int i=0; i<8; i++){
		for(int j=0; j<4; j++)
			out[4*i+j] = (key[i] >> (8*j)) & 0xff ;
	}
}

void num_to_16_le_bytes(uint64_t ac, uint8_t * out){
    for(int i=0; i<16;i++)
        out[i] = (ac>>(8*i)) & 0xff;
}

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

void readAad(istream & file, vector<uint8_t> & out)
{
    char* buff = new char[3];
    stringstream ss_buff;
    int ac;

    while(file.get(buff, 3).gcount() ==2){
        ss_buff.seekp(0); //VERY important lines
        ss_buff.seekg(0); //VERY important lines
        ss_buff.write(buff, 2);
        ss_buff >> std::hex >> ac;
        out.push_back(ac);
    }   
}


int main (int argc, char *argv[])
{
	if(argc != 2){
		cout<<"Usage: ./"<< argv[0] << " yourkey < input_message"<<endl;
		return 1;
	}

	ifstream keyFile;
	keyFile.open(argv[1]);
	if(!keyFile){
		cout<<"The key file cannot be opened !"<<endl;
		return 1;
	}
    
	uint32_t key[8];
    uint8_t outkey[32];

	cout<<"Key is(before operated):"<<endl;
	

    for(int i=0; i<8; i++){
		readFile(keyFile, key[i]);
		cout<<hex<<key[i]<<endl;
	}
	keyFile.close();
	keyFile.clear(ios::goodbit);

	// keyFile.get();

	poly1305_key_gen(key, outkey);
    
	// readAad(keyFile, aad);
	cout<<"Key is:"<<endl;
	for(uint16_t i=0; i< 32;i++){
		//cout<<i<<endl;
		cout<<setfill('0')<<setw(2)<<hex<<(int)(outkey[i] & 0xff)<<" ";
		if(i%32==31){
			cout<<"\n";
		}
		//cout<<"here";
	}

    vector<uint8_t> input;
    char c;
	while (cin.get(c)){
		input.push_back(c);
	}
	input.pop_back();

    // uint8_t aadAar[aad.size()];
    // copy(aad.begin(),aad.end(),aadAar);
    uint8_t plaintext[input.size()];
    copy(input.begin(), input.end(), plaintext);
	
	uint64_t message_length = input.size();

	uint8_t tag[16];
	poly1305(outkey,plaintext,message_length,tag);

	cout<<"-------------------------------------------"<<endl;
	cout<<"Tag :\n";
	for(uint16_t i=0; i< 16;i++){
		//cout<<i<<endl;
		cout<<setfill('0')<<setw(2)<<hex<<(int)(tag[i] & 0xff)<<" ";
		if(i%16==15){
			cout<<"\n";
		}
		//cout<<"here";
	}
	//cout<<"finish"<<endl;
	return 0;
}
