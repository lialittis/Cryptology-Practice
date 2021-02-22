# ECM

We implement "Stage 1" of Lenstra's ECM factorization method.

## Explaination

Appearently, the core function of our program is ECMTrail, in this function, we could generate A and base point randomly by Suyama’s parametrization. By generating integers X and Z modulo N, and using ladder function, we can compute our output results when it is a factor of N. If not, we need to re-generate our curve by A and base point.

To make the functions clearly, we also create several number-theoretic functions in the utils.cpp file.

Just to notify that, if you set a high bound, the accuracy will be higher, but it will takes a lot of time. If you chose a small B, the accuracy will be slow but it is rapide.

## Structure


Before compile:
.
├── ecm.cpp
├── Makefile
├── montgomery.cpp
├── montgomery.h
├── README.md
├── tianchi.yu-challenge.txt
├── utils.cpp
└── utils.h

## Compile commands

Compile for the whole program:
$ make

Remove the compiled files:
$ make clean

## Run commands

We could set two arguments (the first is a prime number, the second is the possible digits of factor we want) for this program. For example,

$ ./ecm 783027137976147078444 6

## Challenge results

### challenge : 6
783027137976147078444

N is 783027137976147078444
+++++++++++++++++++++++
>> small factor:2
+++++++++++++++++++++++
+++++++++++++++++++++++
>> small factor:3
+++++++++++++++++++++++
+++++++++++++++++++++++
>> small factor:2089
+++++++++++++++++++++++
+++++++++++++++++++++++
>> small factor:3167
+++++++++++++++++++++++
After trial division, N becomes to 1095888968311
we get one factor: 103951
Now A:379716127411; base point is (855483030559,1043802610979)
B:64
+++++++++++++++++++++++
BIG FACTOR:103951
+++++++++++++++++++++++
+++++++++++++++++++++++
BIG FACTOR(final):10542361
+++++++++++++++++++++++


TIME RUNNING: 10 - 40 seconds

### challenge : 10
5714107332848007508794276148

input success!
N is 5714107332848007508794276148
+++++++++++++++++++++++
>> small factor:2
+++++++++++++++++++++++
+++++++++++++++++++++++
>> small factor:3253
+++++++++++++++++++++++
+++++++++++++++++++++++
>> small factor:4073
+++++++++++++++++++++++
After trial division, N becomes to 107817666746644856273

--------------------------STOP HERE

## NOTE

I have a problem since factor digits of 10, where I chose the B as 300 - 500, but the curve can never find the correct factor.

I cannot find the fault or bugs in my code. I believe the my program should work well.





