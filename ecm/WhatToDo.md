#What to do

For this assignment, you will implement "Stage 1" of Lenstra's ECM factorization method.

The heart of the algorithm is (again) the Montgomery ladder, and you should be able to recycle a lot of your code from the X25519 assignment.  Beware, however, that this time around the fixed field prime p = 2^255-19 is replaced by the number N to be factored, and the Montgomery curve coefficient A is no longer fixed: it varies in each ECM trial.  The test vectors from Week 4 will still be useful for checking your ladder code.

You will be given a series of challenge numbers to try and factor. The goal is to print out a list of prime powers that are factors of a given number.

You will need several number-theoretic functions.  If they're already included in your big-integer library then great, but otherwise you will need to define them yourself, but luckily most of them are quite short, and all are easy to find.  These functions are:

gcd(a,b), which uses the Euclidean algorithm to compute the GCD of a and b
trial_division(N), which finds any prime factors of N less than 10000 (there are 1229 such primes; you can compute them using the sieve of Eratosthenes, and even hard-code them into your algorithm if you want)
is_probable_prime(m, ntrials), which returns true if and only if the integer m passes ntrials iterations of the Miller-Rabin primality test (this will require a modular exponentiation algorithm, like Python's three-argument pow(x,e,n))
factorization(N), which prints out as many prime factors of  N (with their exponents) as possible.  factorization will call trial_division first, to find any easy (tiny) factors, before making a number of calls to ECMTrial.
The main function that you need to code yourself is ECMTrial(N, A, bound), which should

Generate integers X and Z modulo N such that (X:*:Z) is a projective point on an elliptic curve BY^2Z = X(X^2 + AXZ + Z^2) over Z/NZ, using the given A (the y-coordinate * is irrelevant, as is the curve parameter B);
Compute (X_m,Z_m) such that (X_m:*:Z_m) = [m](X:*:Z) where m is the product of all prime powers less than bound; and
Return gcd(Z_m, N).
It's up to you to decide how many calls to ECMTrial you make for a given N, and how large the bound parameter should be (and also when to make the bound larger!).

What to submit: your source code, plus a text file containing, for each number that you can factor

The factors you found;
The A-parameter of the curve that found the factor
the (X:Z) coordinates of the base point that you used to find the factor
The smoothness bound B that you used to find the factor.
