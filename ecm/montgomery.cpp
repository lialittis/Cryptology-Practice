#include <stdint.h>
#include <gmp.h>
#include <gmpxx.h>
#include <vector>
#include "utils.h"
#include "montgomery.h"

using namespace std;

mpz_class get_prime(){
	mpz_class p;
	mpz_class two = 2;
	mpz_pow_ui(p.get_mpz_t(),two.get_mpz_t(),255);
	p = p-19;
	return p;
}

coordinates_t ladder(mpz_class m, coordinates_t P, mpz_class p, mpz_class A){
	
	mpz_class u;
	mpz_class x_0 = P.a;
	mpz_class z_0 = P.b;

	/*
	mpz_class reverse;

	mpz_invert(reverse.get_mpz_t(),z_0.get_mpz_t(),p.get_mpz_t());

	cout<<"reverse is "<<reverse.get_str()<<endl;
	u = reverse*x_0%p;

	if(u == 0){
		cout<<"Failed ! The problem comes from X and Z"<<endl;
	}else{
		cout<<"u is "<<u<<endl;
	}
	*/
	mpz_class x_1 = x_0; 
	mpz_class x_2 = 1;
	mpz_class x_3 = x_0; 
	mpz_class z_2 = 0;
	mpz_class z_3 = z_0;
	mpz_class swap = 0;

	int bits_m = m.get_str(2).length();
	//int bits_m = u.get_ui();

	// set prime p
	// mpz_class p = get_prime();


	for(int i=bits_m-1;i>=0;i--){
		mpz_class k_t = (m >> i) & 1;
		// cout<<"\nk_t:"<<k_t<<endl;

		swap ^= k_t;
		coordinates_t temp_1 = cswap(swap, x_2, x_3);
		x_2 = temp_1.a;
		x_3 = temp_1.b;

		coordinates_t temp_2 = cswap(swap, z_2, z_3);
		z_2 = temp_2.a;
		z_3 = temp_2.b;

		swap = k_t;

		coordinates_t update_add = xADD(x_1,x_2,x_3,z_2,z_3,p);
		coordinates_t update_dbl = xDBL(x_2,x_3,z_2,z_3,p,A);
		x_3 = update_add.a;
		z_3 = update_add.b;
		x_2 = update_dbl.a;
		z_2 = update_dbl.b;
		

		/*
        if (k_t==0){
            auto coor4=XDBL(x_2,x_3,z_2,Z_3,p,A);
            auto coor5=XADD(coor2,coor3,u,modulo);
            coor2=coor4;
            coor3=coor5;
        }
        else {
            auto coor5=XDBL(coor3,A,modulo);
            auto coor4=XADD(coor2,coor3,u,modulo);
            coor2=coor4;
            coor3=coor5;
        }
		*/
		// cout<<"output 2 is ("<<x_2<<","<<z_2<<")"<<endl;
		// cout<<"output 3 is ("<<x_3<<","<<z_3<<")"<<endl;
	}

	coordinates_t temp_3 = cswap(swap, x_2, x_3);
	x_2 = temp_3.a;
	x_3 = temp_3.b;

	coordinates_t temp_4 = cswap(swap, z_2, z_3);
	z_2 = temp_4.a;
	z_3 = temp_4.b;

	mpz_class p_2 = p-2;
	//  calculate output of x_2*(z_2^(p-2))
	
	coordinates_t output;
	mpz_class z_temp = 0;
	mpz_powm(z_temp.get_mpz_t(),z_2.get_mpz_t(),p_2.get_mpz_t(),p.get_mpz_t());
	// cout<<"all is ok\n"<<endl;
	
	output.a = x_2;
	output.b = z_2;
	//cout<<"output of ladder is ("<<(x_2*z_temp)%p<<","<<z_2<<")"<<endl;
	return output;
}


coordinates_t cswap(mpz_class swap, mpz_class x_2,mpz_class x_3){
	mpz_class dummy;
	dummy = (0- swap) & (x_2 ^ x_3);
	// cout<<"dummy is:"<<dummy<<endl;
	x_2 = x_2 ^ dummy;
	x_3 = x_3 ^ dummy;
	coordinates_t output;
	output.a = x_2;
	output.b = x_3;
	return output;
}

coordinates_t xADD(mpz_class x_1,mpz_class x_2,mpz_class x_3,mpz_class z_2,mpz_class z_3,mpz_class p){
	/*Xp = x_2, Xq = x_3, Zp = z_2, Zq = z_3*/
	// mpz_class p = get_prime();
	mpz_class A = (x_2 + z_2)%p;
	mpz_class B = (x_2 - z_2)%p;
	mpz_class C = (x_3 + z_3)%p;
	mpz_class D = (x_3 - z_3)%p;
	mpz_class U = (B*C)%p;
	mpz_class V = (A*D)%p;
	x_3 = ((V+U)*(V+U))%p;
	z_3 = (x_1 * (V - U)*(V-U))%p;
	coordinates_t X;
	X.a = x_3;
	X.b = z_3;
	// cout<<"after add:"<<X.a.get_str()<<" "<<X.b.get_str()<<endl;
	return X;
}

coordinates_t xDBL(mpz_class x_2,mpz_class x_3,mpz_class z_2,mpz_class z_3,mpz_class p,mpz_class A_constant){
	// mpz_class p = get_prime();
	mpz_class A = (x_2 + z_2)%p;
	mpz_class B = (x_2 - z_2)%p;
	mpz_class Q = (A*A)%p;
	mpz_class R = (B*B)%p;
	mpz_class S = (Q - R)%p;
	x_2 = (Q*R)%p;
	//  consider the reverse in a24
	mpz_class a24 = get_a24(p,A_constant);
	// cout<<"\na24 = "<<a24.get_str()<<endl;
	z_2 = (S*(R + a24 *S))%p;
	coordinates_t X;
	X.a = x_2;
	X.b = z_2;
	// cout<<"after dbl:"<<X.a.get_str()<<" "<<X.b.get_str()<<endl;
	return X;
}

mpz_class get_a24(mpz_class p, mpz_class A_constant){
	mpz_class a24;
	mpz_class invert_four;

	a24= (A_constant + 2)%p;
	// mpz_invert(rop,op1,op2)
	// op1*rop ≡ 1 (mod op2) which implies that (op1*rop - 1) mod op2 = 0

	mpz_class four = 4;
	mpz_invert(invert_four.get_mpz_t(),four.get_mpz_t(),p.get_mpz_t());
	// a24 = (A+2)/4 (mod p)
	a24 = (a24*invert_four)%p;
	return a24;
}
