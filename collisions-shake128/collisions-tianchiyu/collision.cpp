#include <cstdlib>
#include <mpi.h>
#include <pthread.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "keccak.h"
#include "utilities.h"
#include <assert.h>
#include <list>
#include <map>
#include <time.h>

using namespace std;

typedef list<string> LISTMSG;

typedef long clock_t;

int flag = 0;

string message;
int nbTask;
int myRank;
map<vector<bool>, string> hashtable;
LISTMSG listmsg;

// static int NBits = 0;

vector<bool> binary_vector_init(int N){
	vector<bool> r(N,false);
	srand(myRank+1);
	int t = rand()%N;
	for(int i = 0; i< t;i++){
		srand(myRank+1);
		r[rand()%N] = true;
	}
	return r;
}

void get_message_init(){
	string message_init("tianchi.yu@ip-paris.fr:Try to find the collisions in me!");
	message = message_init;
}


string change_char_in(string m, int i)
{
    int id;
    int a = rand()% 4;
    id = (i*3+a)%m.length();
    char c = m[id];
    if (id/3 != 0) {
        if (islower(m[id])) {
          m[id] = toupper(m[id]);
        } else {
          m[id] = tolower(m[id]);
        }
    } else {
      switch (c)
      {
		case 'm': m[id] = 'n'; break;
		case 'n': m[id] = 'm'; break;
        case 'a': m[id] = '4'; break;
        case '4': m[id] = 'a'; break;
		case '-': m[id] = '_'; break;
		case '_': m[id] = '-'; break;
		case 'p': m[id] = 'q'; break;
		case 'q': m[id] = 'p'; break;
        case 'e': m[id] = '3'; break;
        case '3': m[id] = 'e'; break;
        case 'i': m[id] = '!'; break;
        case 'o': m[id] = '0'; break;
        case 'A': m[id] = '4'; break;
        case 'E': m[id] = '3'; break;
        case 'I': m[id] = '1'; break;
        case '1': m[id] = 'I'; break;
        case 'O': m[id] = '0'; break;
        case '0': m[id] = 'O'; break;
        case 's': m[id] = '$'; break;
        case 'S': m[id] = '$'; break;
        case '$': m[id] = 's'; break;
        case 'g': m[id] = '9'; break;
        case 'G': m[id] = '9'; break;
        case '9': m[id] = 'g'; break;
        case 'l': m[id] = '|'; break;
        case 'L': m[id] = '|'; break;
        case '|': m[id] = 'l'; break;
        case 'c': m[id] = '('; break;
        case 'C': m[id] = '('; break;
        case '(': m[id] = 'c'; break;
        case 't': m[id] = '7'; break;
        case 'T': m[id] = '7'; break;
        case '7': m[id] = 't'; break;
        case 'z': m[id] = '2'; break;
        case 'Z': m[id] = '2'; break;
        case '2': m[id] = 'z'; break;
        case 'u': m[id] = 'v'; break;
        case 'U': m[id] = 'V'; break;
        case 'v': m[id] = 'u'; break;
        case 'V': m[id] = 'U'; break;
        case ':': m[id] = ';'; break;
        case ';': m[id] = ':'; break;
        case '.': m[id] = ';'; break;
        case '@': m[id] = '*'; break;
        case '*': m[id] = '@'; break;
		case '`': m[id] = ' '; break;
		case ' ': m[id] = '`'; break;
		case '!': m[id] = 'i'; break;
        default:
		if (islower(m[id])) {
                	m[id] = toupper(m[id]);
                } else {
                	m[id] = tolower(m[id]);
                } break;
      	}
    }
    // map<vector<bool>,string>::iterator it;
    // it = hashtable.begin();
    return m;
}
static void string_to_binary(string message, vector<bool> &input,vector<bool>::size_type b){
	int j = 0;
	printf("message length is:%d\n",message.size() );
	for(vector<bool>::size_type i = 0; i< b, j< message.length();j++){
		unsigned char aggr;
		aggr = message.at(j);
		// cout<<"b is "<< b <<", i is "<<i<<endl;
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


void gm(vector<bool> &r,int N){
	get_message_init();
	int len = message.length();
	map< vector<bool>,string>::iterator it;

	string space = " ";
/*
	while(true){
		// printf("good !\n");
		int index = rand();
		if (index/2 == 0){
			message = change_char_in(message,index);
		} else{
			// message = change_char_in(message,index);
			message = change_char_in(message,index+1);
		}
		// cout<<message<<endl;	
		it = hashtable.begin();
		while(it != hashtable.end()){
			if(message==it->second){
				flag = 1;
				break;
			} 
			it++;
		}
		if(flag == 1) {message.append(space);flag=0;continue;}
		else {flag=0; break;}
	}
*/

	for(int i = 0; i <N; i++){
		if(r[i] == true ){
			message = change_char_in(message,i%len);
		} else {
			//message = change_char_in(message,(i-1)%len);
			message = change_char_in(message,(i+1)%len);
		}
	}
/*	
	it = hashtable.begin();
	while(it != hashtable.end()){
		if(message==it->second){
			int r = rand();
			message = change_char_in(message,r%len);
		} 
		it++;
	}

*/	
	it = hashtable.find(r);
	if (it == hashtable.end()){
		// cout<<"not found"<<endl;
		hashtable[r] = message;
	}
	cout<<message<<endl;
	cout<<"--------"<<endl;


}


vector<bool> run_hash(vector<bool> &r, int N){

	// printf("start hashing\n");
	
	
	gm(r,N);
	
	vector<bool>::size_type length = 8 * message.length();
	vector<bool> input_binary_vector(length);
	
	string_to_binary(message,input_binary_vector,length);
	
	auto digest = shake128(input_binary_vector,N);

	assert(digest.size() == N);
	// cout<<"in running---for"<<endl;
	output_binary_as_ascii(r);
	output_binary_as_ascii(digest);

	return digest;
}

void Floyd(int NBits){
	clock_t t1 = clock();
	int N = NBits;
	int s = 1;	
	vector<bool> r = binary_vector_init(N);
	vector<bool> T = r;
	vector<bool> H = r;
	do{
		s+=1;
		T = run_hash(T,N);
		H = run_hash(H,N);
		H = run_hash(H,N);
	}while(T!=H);

	vector<bool> T1 = T;
	vector<bool> T2 = r;

	printf("Catch you !\n");

	vector<bool> T1_p = run_hash(T1,N);
	vector<bool> T2_p = run_hash(T2,N);
	while(T1_p != T2_p){
		T1 = T1_p;
		T2 = T2_p;
		T1_p = run_hash(T1,N);
		T2_p = run_hash(T2,N);	
	}

/*
	for(std::map<string,vector<bool>>::iterator it = hashtable.begin(); it!=hashtable.end();it++) 
	{
		if(it->second==T){
			message1 = it->first;
			cout<<"message 1"<<it->first<<endl;
		}
		if(it->second==H){
			message2 = it->first;
			cout<<"message 2"<<it->first<<endl;
		}
	} 
*/
	flag = 1;
	printf("The collision is caused by:\n");
	
	char path1[100] = "collisions-%d/ex-%d.A";
	char path2[100] = "collisions-%d/ex-%d.B";

	cout <<"Using time: "<< (clock() - t1) * 1.0 / CLOCKS_PER_SEC<<" seconds" << endl;
	map<vector<bool>,string>::iterator it;
        it = hashtable.find(T1);
	string message1 = it->second;
        cout<<message1<<endl;

        it = hashtable.find(T2);
        string message2 = it->second;
	cout<<message2<<endl;
	if(message1 == message2) {printf("Not found collision for this thread!\n"); return;}
	for(int n = 0; n< 100; n++){
		sprintf(path1,"collisions-%d/ex-%d.A",NBits/8,n);
		sprintf(path2,"collisions-%d/ex-%d.B",NBits/8,n);
		fstream fin(path1);
		if(fin){
			fin.close();
			continue;
		} else {
			ofstream ofile1(path1),ofile2(path2);
        		ofile1 << message1;
        		ofile1.close();
        
        		ofile2 <<message2;
        		ofile2.close();
			break;
		}
	}

}



// #ifdef COLLISION_

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s\n NBytes", argv[0]);
		return 1;
	}

	int NBits = 8*atoi(argv[1]);
	get_message_init();

	MPI_Init(&argc, &argv);
	cout<<"The original message is:";
	cout<<message<<endl;
	MPI_Comm_size(MPI_COMM_WORLD, &nbTask);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	printf ("From rank %d\n", myRank);
	if (myRank == 0) printf("MPI World size = %d processes\n", nbTask);	
	
	Floyd(NBits);

	MPI_Finalize();
	// printf("%s\n",message.c_str());
	// cout<<message<<endl;
	
	// My method
	// output_binary_as_ascii(digest);

/*
	pthread_t ThreadA, ThreadB, ThreadC;

	pthread_create(&ThreadA, NULL, Floyd, NULL);
	pthread_create(&ThreadB, NULL, Floyd, NULL);
	pthread_create(&ThreadC, NULL, Floyd, NULL);

	while(flag == 0){
	}

	pthread_join(ThreadA, NULL);
	pthread_join(ThreadB, NULL);
	pthread_join(ThreadC, NULL);

*/
	
	// Floyd(NBits);

	return 0;
}

// #endif
