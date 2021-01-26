#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <bitset>
#include <cmath>
#include <unordered_map>
#include <sstream>

typedef std::bitset<4> half_byte;
typedef std::vector<std::vector<std::vector<bool>>> state;

/**
* this function converts the string s in hexa decimal to
* a vector of bool.
*/
std::vector<bool> string_vec_of_bool(std::string s);

/**
* This function converts a hexadecimal digit to its binary representation.
*/
half_byte hexa_to_binary(char c);

/**
* This function concatinates to vector of bool, result = N || pad
*/
std::vector<bool> concatinate(std::vector<bool> &N, std::vector<bool> &pad);

/**
* this function computes the xor of u and v, result[i] = u[i] xor v[i] for
* i between 0 and length(u) or length(v).
*/
std::vector<bool> xor_vec(std::vector<bool> &u, std::vector<bool> &v);

/**
* This functions converts a string of bits
* represented as a vector of bool to its hexadecimal representation.
* Note: this function may take time if v is big, because v is not passed by
* reference.
*/
std::string vec_of_bool_string(std::vector<bool> v);

/**
* This functions is used to convert the OUTPUT of vec_of_bool_string to
* the correct order as it is in:
* https://github.com/gvanas/KeccakCodePackage/tree/master/TestVectors
*/
std::string reverse_string(std::string v);

/**
* this functions just returns a copy of the state
*/
state copy(state& A);

/**
* this function converts the vector from littel endian to big endian
*/
void endian(std::vector<bool> &vec);
