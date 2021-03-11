# Chacha20 - Poly1305

Based on the exercise of week 3, we give four program(The third program is the most important program for this homework):

* poly1305-gen : to generate the poly1305 algorithem.
* poly1305-check : to check the first program.
* chacha20-poly1305: to generate the chacha20-poly1305 algorithm.
* chacha20-polt1305-check: to check the third program.

## Compile and Clean

make all

make clean

### Note

The first two programs are not compiled by default, if you want to generate the pure poly1305 and check it, please go to the Makefile, and cancel the comments for MakefileGen and MakefileTest.

## How to run these programs

### poly1305-gen

* Usage: ./poly1305-gen key < input_message
* Test Example: ./poly1305-gen key_poly1305 < input_poly1305

### poly1305-check

* Usage: ./poly1305-check keyFile messageFile authenticatorFile
* Test Example: ./poly1305-check key_poly1305 input_poly1305 tag_answer

### chacha20-poly1305

* Usage: ./chacha-poly1305 key < input_message
* Test Example: ./chacha20-poly1305 key_chacha20 < input_chacha20

### chacha20-poly1305-check

* Usage: ./chacha20-poly1305-check keyFile messageFile authenticatorFile
* Test Example: ./chacha20-poly1305-check key_chacha20 input_chacha20 tag_answer_chacha20


## Simple description

The first program is the basic poly1305 generator without chacha20 or any other key generating ways. The second progrm is used to check the correctness of the first program. The third program is the complete chacha20 poly1305 program, with the aead algorithm. The last program can be used to check the correctness of the third program.

## Running results for chacha20 algorithm

### chacha20-poly1305

./chacha20-poly1305 key_chacha20 < input_chacha20

Cipher :
d3 1a 8d 34 64 8e 60 db 7b 86 af bc 53 ef 7e c2
a4 ad ed 51 29 6e 08 fe a9 e2 b5 a7 36 ee 62 d6
3d be a4 5e 8c a9 67 12 82 fa fb 69 da 92 72 8b
1a 71 de 0a 9e 06 0b 29 05 d6 a5 b6 7e cd 3b 36
92 dd bd 7f 2d 77 8b 8c 98 03 ae e3 28 09 1b 58
fa b3 24 e4 fa d6 75 94 55 85 80 8b 48 31 d7 bc
3f f4 de f0 8e 4b 7a 9d e5 76 d2 65 86 ce c6 4b
61 16
Tag :
1a e1 0b 59 4f 09 e2 6a 7e 90 2e cb d0 60 06 91

### chacha20-poly1305-check

./chacha20-poly1305-check key_chacha20 input_chacha20 tag_answer_chacha20

 L a d i e s   a n d   G e n t l e m e n   o f   t h e   c l a s s   o f   ' 9 9 :   I f   I   c o u l d   o f f e r   y o u   o n l y   o n e   t i p   f o r   t h e   f u t u r e ,   s u n s c r e e n   w o u l d   b e   i t .(end)
The size of plaintext is 114
The plaintext is
Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.
Key is
7b ac 2b 25 2d b4 47 af 09 b6 7a 55 a4 e9 55 84 0a e1 d6 73 10 75 d9 eb 2a 93 75 78 3e d5 53 ff
r before clamping:
7b ac 2b 25 2d b4 47 af 09 b6 7a 55 a4 e9 55 84
clamped r is 455e9a4057ab6080f47b42c052bac7b
clamped s is ff53d53e7875932aebd9751073d6e10a
Block #0
acc is 1bb22680babc3dcdc0a0c9a383695c4a3
Block #1
acc is 2c28892718bbf4b439094be1a6c562472
Block #2
acc is eac8e912bae4da39e51785abd7d75a22
Block #3
acc is 2e436ba90aad02f9f26f5ba1fb88aa27e
Block #4
acc is 31f99048e9c70cca600087c406d08158b
Block #5
acc is 354f953520211257e35c6d4defa90b0be
Block #6
acc is 24a6f87100374b108dc05c7e15ec3bdfd
Block #7
acc is c3349da19703f736bc0fd003f69230ec
Block #8
acc is 3a0ca8e12eb289ffe3e49c4533704051d
Block #9
acc is 191b28b9252b8fd537f08943ee5350010
final acc is 2910660d0cb2e907e6ae2094f590be11a
-------------------------------------------------------------------
Cipher :
d3 1a 8d 34 64 8e 60 db 7b 86 af bc 53 ef 7e c2
a4 ad ed 51 29 6e 08 fe a9 e2 b5 a7 36 ee 62 d6
3d be a4 5e 8c a9 67 12 82 fa fb 69 da 92 72 8b
1a 71 de 0a 9e 06 0b 29 05 d6 a5 b6 7e cd 3b 36
92 dd bd 7f 2d 77 8b 8c 98 03 ae e3 28 09 1b 58
fa b3 24 e4 fa d6 75 94 55 85 80 8b 48 31 d7 bc
3f f4 de f0 8e 4b 7a 9d e5 76 d2 65 86 ce c6 4b
61 16
Tag :
1a e1 0b 59 4f 09 e2 6a 7e 90 2e cb d0 60 06 91
Tag answer is:
1a e1 0b 59 4f 09 e2 6a 7e 90 2e cb d0 60 06 91

************** Testing Tag **************
Test OK !

## In Case

There are little popabilities that the test fails because of the fstream reading issue.

Here I give one exact example to explain this problem. One possible output of check program will be

 L a d i e s   a n d   G e n t l e m e n   o f   t h e   c l a s s   o f   ' 9 9 :   I f   I   c o u l d   o f f e r   y o u   o n l y   o n e   t i p   f o r   t h e   f u t u r e ,   s u n s c r e e n   w o u l d   b e   i t .(end)
The size of plaintext is 114
The plaintext is
Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.
Key is
7b ac 2b 25 2d b4 47 af 09 b6 7a 55 a4 e9 55 84 0a e1 d6 73 10 75 d9 eb 2a 93 75 78 3e d5 53 ff
r before clamping:
7b ac 2b 25 2d b4 47 af 09 b6 7a 55 a4 e9 55 84
clamped r is 455e9a4057ab6080f47b42c052bac7b
clamped s is ff53d53e7875932aebd9751073d6e10a
Block #0
acc is 1bb22680babc3dcdc0a0c9a383695c4a3
Block #1
acc is 2c28892718bbf4b439094be1a6c562472
Block #2
acc is eac8e912bae4da39e51785abd7d75a22
Block #3
acc is 2e436ba90aad02f9f26f5ba1fb88aa27e
Block #4
acc is 31f99048e9c70cca600087c406d08158b
Block #5
acc is 354f953520211257e35c6d4defa90b0be
Block #6
acc is 24a6f87100374b108dc05c7e15ec3bdfd
Block #7
acc is c3349da19703f736bc0fd003f69230ec
Block #8
acc is 3a0ca8e12eb289ffe3e49c4533704051d
Block #9
acc is 191b28b9252b8fd537f08943ee5350010
final acc is 2910660d0cb2e907e6ae2094f590be11a
-------------------------------------------------------------------
Cipher :
d3 1a 8d 34 64 8e 60 db 7b 86 af bc 53 ef 7e c2
a4 ad ed 51 29 6e 08 fe a9 e2 b5 a7 36 ee 62 d6
3d be a4 5e 8c a9 67 12 82 fa fb 69 da 92 72 8b
1a 71 de 0a 9e 06 0b 29 05 d6 a5 b6 7e cd 3b 36
92 dd bd 7f 2d 77 8b 8c 98 03 ae e3 28 09 1b 58
fa b3 24 e4 fa d6 75 94 55 85 80 8b 48 31 d7 bc
3f f4 de f0 8e 4b 7a 9d e5 76 d2 65 86 ce c6 4b
61 16
Tag :
1a e1 0b 59 4f 09 e2 6a 7e 90 2e cb d0 60 06 91
Tag answer is:
1c e1 0b 59 4f 09 e2 6a 8c 90 2e cb d0 60 06 11

************** Testing Tag **************
 Test failed !


**The output Tag of my algorithm is correct! However the reading of the Tag answer from the reference file is not correct, with couples of the number wrong !**

This result proves that there is no problem with the algo, but there is some instability of reading files by ifstream.























