# shake128

This program is the basic implementation for shake128 according to FIPS202.
However, it needs some optimization because 

## How to use

- make
- ./shake128 NUMBER_BYTES BINARY_FILENAME


> Explaination: The program is coded by C++, we offer one `Makefile` to compile the code and enerate an executable file shake128. To run the program, just provide or build a command-line `shake128 NUMBER_BYTES BINARY_FILENAME`, where `NUMBER_BYTES` is the number of bytes to be output. Input file should be read from the standard input stream as plain binary data (that is, a stream of bytes).

> Example: 

./shake128 32 test.bin

./shake128 32 hello.txt

## File structure

- `main.cpp`
- `keccak.h`
- `keccak.cpp`
- `sponge.h`
- `sponge.cpp`
- `keccak_p.h`
- `keccak_p.cpp`

## Problem

The final result is not correct, while the reading and writing processes have no problem.
For example, `$./shake128 32 test.bin` should return 40 7B 0E 00 C6 CE 95 48 E1 EC C8 48 2E 67 E3 8F 78 EB A7 E5 70 2C 92 F7 4E DD E8 DE E3 B9 BC E4, while I get 83 7D 26 8F 1C 93 8E 51 B2 A8 55 48 45 16 44 34 76 C0 AD A9 65 13 4C 52 68 C1 D6 70 2E 90 DD BE.
