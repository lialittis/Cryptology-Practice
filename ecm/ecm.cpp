#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <math.h>
#include <string>
#include <sys/timeb.h>
#include <cstdlib>
#include "utils.h"
#include "montgomery.h"

using namespace std;

/* Suyama’s parametrization
   Montgomery credits Suyama with the now-standard way to chose a random curve for ECM
 */
void randomcurve(mpz_class* A, coordinates_t* P, mpz_class N){
	mpz_class sigma = 0;
	int size = mpz_size(N.get_mpz_t());

	// generate random sigma
	gmp_randstate_t state;
	gmp_randinit_default(state);
	struct timeb timeSeed;
	ftime(&timeSeed);
	unsigned int seed = timeSeed.time * 1000 + timeSeed.millitm;
	gmp_randseed_ui(state, seed);

	mpz_urandomm(sigma.get_mpz_t(),state,N.get_mpz_t());
	gmp_randclear(state);

	if(sigma>=0 && sigma <5) sigma += 5; // avoid {0,1,2,3,4}
	mpz_class u = (sigma*sigma - 5)%N;
	mpz_class v = 4*sigma%N ;
	mpz_class x_p = 0;
	mpz_class y_p = 0;
	mpz_class three = 3;
	mpz_powm(x_p.get_mpz_t(),u.get_mpz_t(),three.get_mpz_t(),N.get_mpz_t());
	mpz_powm(y_p.get_mpz_t(),v.get_mpz_t(),three.get_mpz_t(),N.get_mpz_t());
	// cout<<"x_p is "<<x_p.get_str()<<", y_p is "<<y_p.get_str()<<endl;
	mpz_class v_u = (v - u)%N;
	mpz_powm(A->get_mpz_t(),v_u.get_mpz_t(),three.get_mpz_t(),N.get_mpz_t());
	*A = ((*A)*(3*u + v))%N;

	mpz_class reverse;
	mpz_powm(reverse.get_mpz_t(),u.get_mpz_t(),three.get_mpz_t(),N.get_mpz_t());
	reverse = reverse * 4 * v % N;
	// cout<<"reverse is "<<reverse.get_str()<<" and N is "<<N.get_str()<<endl;

	/*
	if(mpz_invert(reverse.get_mpz_t(),reverse.get_mpz_t(),N.get_mpz_t()) == 0){
		//  cout<<"------"<<endl;
		randomcurve(A,P,N); 
		// To avoid the case when there is no invert for reverse.
		// For example, 1/2 mod 10 = ?
	}else{
		*A = (*A) * reverse % N;
		*A = (*A) - 2;
		*A = ((*A) + N)%N;
		P->a = x_p;
		P->b = y_p;
	}
	*/
	
	
	mpz_invert(reverse.get_mpz_t(),reverse.get_mpz_t(),N.get_mpz_t());
	*A = (*A) * reverse % N;
	*A = (*A) - 2;
	*A = ((*A) + N)%N;
	P->a = x_p;
	P->b = y_p;
	
	// cout<<"So we have A as "<<(*A).get_str()<<" and base point is ("<<P->a.get_str()<<","<<P->b.get_str()<<")"<<endl;

}


/* Here we use the suggestion of Zimmermann and Dodson 
   to set the value of Bound B, without calculation of L-notation
 */

mpz_class generate_B( mpz_class N, int nbits){
	// int nbits = mpz_size(N.get_mpz_t());
	// nbits = nbits/2;
	mpz_class B;
	if(nbits < 10){
		B = 64;
	}else if(nbits <15){
		B = 500;
	}else if(nbits <=20){
		B = 30000;
	}else if (nbits <= 40){
		B = 3*10e6;
	}else if(nbits <= 45 && nbits > 40){
		B = 11*10e6;			
	}else if(nbits <= 50 && nbits > 45){
		B = 43*10e6;
	}else if(nbits <= 55 && nbits > 50){
		B = 110*10e6;
	}else if(nbits <= 60 && nbits > 55){
		B = 260*10e6;
	}else if(nbits <= 65 && nbits > 60){
		B = 850*10e6;
	}else{\
		cout<<"B is too big I think!"<<endl;
	}

	//cout<<"B is "<<B.get_str()<<endl;
	return B;
}


mpz_class ecmTrial(mpz_class N, mpz_class B){

	mpz_class A;
	coordinates_t P;

	// (A,P) <- randomcurve
	// cout<<"\nStart to get the random curve! "<<endl;
	randomcurve(&A,&P, N);
	//cout<<"So we have A as "<<A.get_str()<<" and base point is ("<<P.a.get_str()<<","<<P.b.get_str()<<")"<<endl;

	
	if(gcd(A*A-4,N)!=1){
		cout<<"Luck factor:"<<gcd(A*A - 4,N)<<endl;
		return gcd(A*A - 4,N);
	}
	

	// Primeupto B

	// cout<<"\nStart to get the list of primes up to "<<B.get_str()<<endl;
	vector<mpz_class> primeUpToB;
	primeUpToB = get_primesUpTo(B);


	//cout<<"\n";

	// Q <- P
	coordinates_t Q = P;
	coordinates_t temp = P;
	// cout<<"Before iteration, Q is ("<<Q.a.get_str()<<","<<Q.b.get_str()<<")"<<endl;
	for(int i = 0; i< primeUpToB.size();++i){
		mpz_class l = primeUpToB[i];
		// cout<<"primte of B "<<l.get_str()<<endl;
		// using uint32_t: 0 ~ 4294967295
		// using mpz_get_ui
		double k = log(B.get_si())/log(l.get_si());
		int ik = floor(k);
		// cout<<"k is "<<ik<<endl;
		mpz_class lk;
		mpz_pow_ui(lk.get_mpz_t(),l.get_mpz_t(),ik);
		// cout<<"check the inputs of ladder:"<<endl;
		// cout<<"lk(scalar) is "<<lk<<", base point is ("<<Q.a.get_str()<<","<<Q.b.get_str()<<")"<<" N is "<<N.get_str()<<" A is "<<A.get_str()<<endl;
		Q = ladder(lk,Q,N,A);
		// cout<<"OUTPUT Q is ("<<Q.a.get_str()<<","<<Q.b.get_str()<<")"<<endl;
		if(Q.b==0) {
			cout<<"OUTPUT Q is ("<<Q.a.get_str()<<","<<Q.b.get_str()<<")"<<endl;
			cout<<"Curve cannot find the factor Or we don't need the curve !"<<endl;
			return 0;
		}
	}

	mpz_class g = gcd(Q.b,N);
	// cout<<"g is "<<g<<endl;
	if(g>1 && g< N){
		cout<<"we get one factor: "<<g<<endl;
		cout<<"Now A:"<<A<<"; base point is ("<<temp.a.get_str()<<","<<temp.b.get_str()<<")"<<endl;
		cout<<"B:"<<B<<endl;
		return g;
	}else{
		g = 1;
		//cout<<"Curve cannot find the factor Or we don't need the curve !"<<endl;
	}

	return g;

}

/* factorization(N) 
   prints out as many prime factors of  N (with their exponents) as possible
 */

void factorization(mpz_class N,int n){

	N = trial_division(N);
	cout<<"After trial division, N becomes to "<<N.get_str()<<endl;

	while(is_probable_prime(N,100)==0 && N!=1){
		// cout<<"running"<<endl;
		mpz_class B = generate_B(N,n);
		// cout<<"\nB:"<<B.get_str()<<endl;
		mpz_class prime = ecmTrial(N,B);
		// cout<<"prime result is "<<prime<<endl;
		if(prime > 1 && prime < N){
		//if(is_probable_prime(prime,100)==1){
			cout<<"+++++++++++++++++++++++"<<endl;
			cout<<"BIG FACTOR:"<<prime.get_str()<<endl;
			cout<<"+++++++++++++++++++++++"<<endl;
			N = N/prime;
		}

	}

	cout<<"+++++++++++++++++++++++"<<endl;
	cout<<"BIG FACTOR(final):"<<N.get_str()<<endl;
	cout<<"+++++++++++++++++++++++"<<endl;
}



int main(int argc,char ** argv)
{

	if(argc < 3){
		cout<<"Usage: "<< argv[0]<<" YourNumber digitsNumber"<<endl;
	}

	mpz_class N;
	N = argv[1];
	
	int n;
	n = atoi(argv[2]);

	cout<<"input success!"<<endl;
	cout<<"N is "<<N<<endl;
	factorization(N,n);

	//coordinates_t dot;

	/*Test for montgomery*/

	/*Test 1*/
	/*
	mpz_class m = 77;
	mpz_class p = 101;
	mpz_class A = 49;
	mpz_class x = 2;
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
	x = 9;
	m = 7;
	*/
	
	/*
    p =((mpz_class) 1<< 255)-19;
	A = 486662;
	*/
	/*
	cout<<"prime: p = "<<p<<endl;
	cout<<"curve constant: A = "<<A<<endl;
	coordinates_t u;
	u.a = x;
	u.b = 1;
	coordinates_t output = ladder(m,u,p,A);
	*/
	// cout<<"The X on (X,Z) coordinates is:"<<output.a.get_str()<<" and "<<output.b.get_str()<<endl;

	return 0;
}


