# X25519 key exchange

We implement X25519 key exchange according to RFC7748, but the core of the assignment is implementing Montgomery's elliptic curve arithmetic.

## Compile commands

Compile for the whole program:
$ make

Remove the compiled files:
$ make clean

## Run commands

We could set two arguments or one argument for this program. For example,

$ ./x25519 inputScalar.txt inputU.txt

or

$ ./x25519 inputScalar.txt

For the second case, the default value of u is 9.


## Test

There is partial code in the x25519.cpp file, which are used to do the tests. 
You should un-comment them if you want to do the test.


## Supplementation

There is one mistake in the official document of RFC 7748, which takes me a lot of time to debug. Thefault is about the xDBL function. In the document, it uses "z_2 = E * (AA + a24 * E)", however, according to the slide, we use the function like "z_2 = E * (BB + a24 * E)".

