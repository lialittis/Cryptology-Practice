# Collisions-N

## Description

The algorithm used is based on the article of `Van Oorschot and WienerFile`.

The basic approch to detecting a collision with Pollard's rho method is ues Floyd's cycle-finding algorithm. We start with two sequence , where one applies hash funcion twice per step and the other applies hash function once per step. Until the two sequences find the same value, the collision happens.

Because we want to use Floyd's method and update the state/input by the output of the last function. We have to make sure the input and output of the new hash function are with same format. Specifically, to realize it to the shake-128 function, we need to build a function `gm` which is an injective function to make pertubation of initial fixed message. 

In my program, I create one sentence/signature "tianchi.yu@ip-paris.fr:Try to find the collisions in me!" as the initial message. I set one vecor of booleans and one initial message as the input of gm, each time we run the gm function, the message will be changed according to the format of vector. For example, we define that "a" -> "4", "T" -> "7", etc, at the position followed by a specific defined method. In this way, we can keep the message with only small changes. I also create one hashtable to settle the "wrong" messages and corresponding input data with the excepted number of bytes as hash results.

* Note that: with the increase of the bytes, the requirement of gm becomes strict, which means we have to make sure that the gm is injective. But it takes a lot of time to do this step.


Then I tried to realise the parallelism for my program. Here we used OPEN MPI method.

In different thread, the initial vector are generated differently. Here I only realise one simple parallelism: build several thread and run the Floyd's algorithm at each thread separately, this improves the chance of getting the collisions.

* Note that: To be faster generate the collisions, the program was changing to be suitable for different number of bytes need to be outputed. In this perpose, we may get no collision in some thread. In other word, actually the perturbation function could be optimized.

## How to run

./make : compile the files

./collision NBytes : run the program without parallelism.

mpirun -np NProcesses ./collision NBytes : run the program with parallelism in NProcesses processes.

* Note that: you need to build the environment of MPI at first.

* Note that: the pairs of messages are stored in the files, which cause the collisions at specific number of bytes. (The input message should not have eof at the end)

## Details for N bytes collisions

### collisions-1

Using time: 0.64 seconds
tiaNchi.yu@Ip-paris.fr:Try to find the collisions in me!
tiancHi.yU@Ip-pArIs.fr:Try to find the collisions in me!

Using time: 1.73 seconds
tiancHi:yv@!p_par!s;fr:Try to find the collisions in me!
tiamcHi;yu@ip_paris.fr:Try to find the collisions in me!

### collisions-2


Using time: 22.16 seconds
tiancHi;yv@!p_par!s;fr:7rY 7o finD 7he collisions in me!
tianchi.yu@!p-p4ris.fR:trY 7o`finD 7he collisions in me!

### collisions-3

The collision is caused by:
Using time: 256.65 seconds
tianchi.yu@!p-paris;fR:7rY to`finD 7he (oll!sions`im me!
tiamcHi;yu@!p-par!s:fr:7rY 7o`f!nD 7h3 (ollisioms im me!

### collisions-4


