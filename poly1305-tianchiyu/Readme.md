# Poly1305

We give three program in this part.

* poly1305-gen
* poly1305-check
* chacha20-poly1305

## Compile

make all

make clean

## How to run

The first program is the basic poly1305 generator without chacha20 or any other key generating ways.

Usage: poly1305-gen key < input

> poly1305-gen key_poly1305 < input_poly1305.txt

The second progrm is used to check the correctness of the last program.

Usage: poly1305-check keyFile messageFile authenticatorFile

> poly1305-check key_poly1305 input_poly1305.txt tag_answer.txt

The third program is the complete chacha20 poly1305 program, with the aead algorithm.

> chacha20-poly1305 key < input.txt






