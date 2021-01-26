#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <gmp.h>
#include <vector>
#include <sstream.h>

using namespace std;

/*Quarter Round on the ChaCha State 
  => actually it's a kind of "colum round" or "diagonal round"
*/
void quarterRound(uint32_t *state, int a_id, int b_id, int c_id, int d_id){
	state[a_id] += state[b_id]; state[d_id] ^=state[a_id]; state[d_id] = rot(state[d_id],16);
	state[c_id] += state[d_id]; state[b_id] ^=state[c_id]; state[b_id] = rot(state[b],12);
	state[a_id] += state[b_id]; state[d_id] ^=state[a_id]; state[d_id] = rot(state[d],8);
	state[c_id] += state[d_id]; state[b_id] ^=state[c_id]; state[b_id] = rot(state[b],7);
}

uint32_t rotation(uint32_t abcd, int n){
	n = n % 32;
	return (abcd << n) | (abcd >> (32-n));
}

// serialize function
void serialize(uint32_t * state, uint8_t *out){

	for(int i=0; i<16;i++){
		for(int j=0; j<4;j++)
			out[4*i + j] = (state[i] >> (8*j)) & 0xff;
	}
}

/* The ChaCha20 Block Function
   The ChaCha block function transforms a ChaCha state by running multiple quarter rounds.
   INPUTS:
   - A 256-bit key, treated as a concatenation of eight 32-bit little-
      endian integers.

   -  A 96-bit nonce, treated as a concatenation of three 32-bit little-
      endian integers.

   -  A 32-bit block count parameter, treated as a 32-bit little-endian
      integer.
   OUTPUT:
   - 64 random-looking bytes
 */

void block(uint32_t *key, uint32_t counter, uint32_t *nonce, uint8_t *out){

	uint32_t state[16]; // 16 integer numbers
	// initialisation

	// 0-3
	state[0] = 0x61707865;
	state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;

	// 4-11: key
	copy(key, key+8, state+4);

	// 12: counter
	state[12] = counter;

	// 13-15: nonce
	copy(nonce,nonce+3,state+13);

	uint32_t _state[16];
	copy(state, state+16, _state);
	
	// 20 rounds of quarter round
	for(int i=0; i<10;i++){
		quarterRound(_state,0,4,8,12);
		quarterRound(_state,1,5,9,13);
		quarterRound(_state,2,6,10,14);
		quarterRound(_state,3,7,11,15);
		quarterRound(_state,0,5,10,15);
		quarterRound(_state,1,6,11,12);
		quarterRound(_state,2,7,8,13);
		quarterRound(_state,3,4,9,14);
	}
    
	// add the original input with the output
	for(int i=0; i<16;i++){
		state[i] += _state[i];
	}
	
	// serialize the result
	serialize(state,out);
}

/* The ChaCha20 Encryption Algorithm
   INPUT:
   -  A 256-bit key

   -  A 32-bit initial counter.  This can be set to any number, but will
      usually be zero or one.  It makes sense to use one if we use the
      zero block for something else, such as generating a one-time
      authenticator key as part of an AEAD algorithm.

   -  A 96-bit nonce.  In some protocols, this is known as the
      Initialization Vector.

   -  An arbitrary-length plaintext

   OUTPUT:
   an encryption message of the same length
 */

void chacha20_encrypt(uint32_t * key, uint32_t counter, uint32_t * nonce, uint8_t * plaintext, uint64_t message_length, uint8_t * ciphertext){
	
	uint64_t nBlocks = message_length/64;
	uint8_t key_stream [64];

	for(uint64_t j=0; j<nBlocks; j++){
		block(key, counter+j, nonce, key_stream);
		for(uint64_t i=j*64; i<(j+1)*64;i++)
			ciphertext[i] = plaintext[i] ^ key_stream[i%64];
	}

	if(message_length % 64 !=0){
		uint64_t j = message_length/64;
		block(key, counter + j, nonce, key_stream);
		for(uint64_t i=j*64; i<message_length;i++)
			ciphertext[i] = plaintext[i] ^ key_stream[i%64];
	}

}


/* Poly1305 Algorithm
   The inputs to Poly1305 are:

   -  A 256-bit one-time key

   -  An arbitrary length message

   The output is a 128-bit tag.

 */

void poly1305(uint8_t * key, uint8_t * message, uint64_t message_length, uint8_t * out){   
	/*TODO*/
	uint8_t r_tab[16];
	copy(key,key+16,r_tab); // le bytes to num(key[0..15])
	clamp(r_tab);

	/* key r, s*/
    mpz_t r; 
    mpz_init (r);
    mpz_import (r, 16, -1, sizeof(r_tab[0]), 0, 0, r_tab);

	mpz_t s; 
	mpz_init (s);
	mpz_import (s, 16, -1, sizeof(key[16]), 0, 0, key+16);
    
	/*TODO*/
	mpz_t p;
	mpz_init(p);
	// p = (1<<130)-5
    mpz_set_str(p,"3fffffffffffffffffffffffffffffffb",16);
    
    mpz_t n;
    mpz_init(n);
    
    mpz_t a;
    mpz_init(a);
    
    uint64_t nBlocks = message_length/16;
    uint8_t messagesBytes[17];
    messagesBytes[16] = 0x01;
    for(uint64_t i=0; i<nBlocks; i++)
    {
        copy(message+i*16, message +(i+1)*16, messagesBytes);
        mpz_import (n, 17, -1, sizeof(messagesBytes[0]), 0, 0, messagesBytes);
        mpz_add(a,a,n);      
        mpz_mul(a,r,a);
        mpz_mod(a,a,p);
    }

    if(message_length%16 !=0)
    {
        messagesBytes[message_length%16] = 0x01;
        copy(message+nBlocks*16, message + message_length, messagesBytes);
        mpz_import (n, message_length%16+1, -1, sizeof(messagesBytes[0]), 0, 0, messagesBytes);
        mpz_add(a,a,n);
        mpz_mul(a,r,a);
        mpz_mod(a,a,p);
    }

    mpz_add(a,a,s);
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

void poly1305_key_gen(uint32_t * key, uint32_t * nonce, uint8_t * out)
{
    int counter = 0;
    uint8_t out_block[64];
    block(key, counter, nonce, out_block);
    copy(out_block, out_block+32, out);
}


void num_to_8_le_bytes(u_int64_t ac, u_int8_t * out)
{
    for(int i=0; i<8;i++)
        out[i] = (ac>>(8*i)) & 0xff;
}

/* AEAD Construction

   AEAD_CHACHA20_POLY1305 is an authenticated encryption with additional
   data algorithm.  The inputs to AEAD_CHACHA20_POLY1305 are:

   -  A 256-bit key

   -  A 96-bit nonce -- different for each invocation with the same key

   -  An arbitrary length plaintext

   -  Arbitrary length additional authenticated data (AAD)

   The output from the AEAD is twofold:

   -  A ciphertext of the same length as the plaintext.

   -  A 128-bit tag, which is the output of the Poly1305 function.

 */

void chacha20_aead_encrypt(uint8_t * aad, uint64_t alength, uint32_t * key, uint32_t *  iv, uint32_t constant, uint8_t * plaintext, uint64_t mlength)
{
    uint32_t nonce[3];
    nonce[0] = constant;
    copy(iv,iv+2, nonce +1);

    uint8_t otk[32];
    poly1305_key_gen(key, nonce, otk);

    uint8_t ciphertext[mlength];
    chacha20_encrypt(key, 1, nonce, plaintext, mlength, ciphertext);

    int aadpad = 16 - alength%16;
    int cipherpad = 16 - mlength%16;
    uint64_t total_length = aadpad + alength + mlength +cipherpad + 16;
    uint8_t mac_data[total_length];
    memset(mac_data,0,total_length);
    copy(aad,aad+alength, mac_data);
    copy(ciphertext,ciphertext+mlength,mac_data + aadpad +alength);
    num_to_8_le_bytes(alength, mac_data + aadpad + alength + mlength +cipherpad);
    num_to_8_le_bytes(mlength, mac_data + aadpad + alength + mlength +cipherpad + 8);
    
    uint8_t tag[16];
    poly1305(otk,mac_data,total_length,tag);

    cout<<"Cipher :\n";
    for(uint64_t i=0; i< mlength;i++)
    {
        cout<<setfill('0')<<setw(2)<<hex<<(int)(ciphertext[i] & 0xff)<<" ";
        if(i%16==15)
            cout<<"\n";
    }
    cout<<"\n";

    cout<<"Tag :\n";
    for(int i=0; i< 16;i++)
    {
        cout<<setfill('0')<<setw(2)<<hex<<(int)(tag[i] & 0xff)<<" ";
        if(i%16==15)
            cout<<"\n";
    }
}

void readFile(istream & file, uint32_t &out){
	char* buff = new char[3];
	stringstream str;
	int ac;
	for(int i=0;i<4;i++){
		file.get(buff,3);
		str.seekp(0,ios::beg);
		str.seekg(0,ios::beg);
		str.write(buff,2);
		str>>std::hex>>ac;
		out+=ac<<(8*i);
	}
}
void readAad(istream & file, vector<u_int8_t> & out)
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
		cout<<"Usage: ./"<< argv[0] << " yourkey"<<endl;
		return 1;
	}

	ifstream key;
	key.open(argv[1]);
	if(!key){
		cout<<"The key file cannot be opened !"<<endl;
		return 1;
	}

	uint32_t iv[2] = {0};
	uint32_t constant = 0;

	readFile(key,constant);
	key.get();
	for(int i=0;i<2;I==){
		readFile(key,iv[1]);
	}

	keyFile.get();
    for(int i=0; i<8; i++)
        readInt(keyFile, key[i]);
    keyFile.get();  
    readAad(keyFile, aad);
  
    vector<u_int8_t> input;
    char c;
	while (cin.get(c))
	{
		input.push_back(c);
	}

    u_int8_t aadAar[aad.size()];
    copy(aad.begin(),aad.end(),aadAar);
    u_int8_t plaintext[input.size()];
    copy(input.begin(), input.end(), plaintext);

    chacha20_aead_encrypt(aadAar,aad.size(),key,iv,constant,plaintext,input.size());

}
