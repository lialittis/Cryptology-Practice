#include <iostream>
#include <fstream>
#include <iterator>
#include "keccak.h"
#include "utilities.h"
#include <vector>
#include <bitset>
#include <algorithm>

/**
* Note: binary_write and binary_read are taken from
* https://stackoverflow.com/questions/29623605/how-to-dump-stdvectorbool-in-a-binary-file
* with small edit
* author: https://stackoverflow.com/users/3378179/caduchon
*/
void binary_write(std::ofstream& fout, const std::vector<bool>& x) {
    std::vector<bool>::size_type n = x.size();
    for(std::vector<bool>::size_type i = 0; i < n;) {
        unsigned char aggr = 0;
        for(unsigned char mask = 1; mask > 0 && i < n; ++i, mask <<= 1)
            if(x.at(i))
                aggr |= mask;
        fout.write((const char*)&aggr, sizeof(unsigned char));
    }
}

void binary_read(std::ifstream& fin, std::vector<bool>& x, std::vector<bool>::size_type n) {
    x.resize(n);
    for(std::vector<bool>::size_type i = 0; i < n;)
    {
        unsigned char aggr;
        fin.read((char*)&aggr, sizeof(unsigned char));
        for(unsigned char mask = 1; mask > 0 && i < n; ++i, mask <<= 1)
            x.at(i) = aggr & mask;
    }
}

// this function return the file size
int get_file_size_in_byte(const std::string file_name) {
  int size_f = 0;
  std::ifstream inf(file_name);

  inf.seekg(0, std::ios::end);
  int file_size = inf.tellg();
  inf.seekg(0, std::ios::beg);
  std::cout << "file size = " << file_size << std::endl;
  return file_size * 8;
}

#ifdef SHAKE_

int main(int argc, char *argv[]) {

  if(argc != 3) {
    std::cout << "incorrect number of arguments" << '\n';
    return 0;
  }

  std::cout << argv[0] << '\n';
  int N = std::stoi(argv[1]);
  N *= 8;
  std::string file_name = std::string(argv[2]);

  auto file_size = get_file_size_in_byte(file_name);
  std::vector<bool> input_vector(file_size);


  std::ifstream in;
  in.open(file_name, std::ios::binary);
  std::vector<bool>::size_type n = input_vector.size();
  std::cout << "reading the file" << std::endl;
  binary_read(in, input_vector, n);
  in.close();

  std::cout << "applying shake128" << std::endl;
  auto res = shake128(input_vector, N);


  std::ofstream out;
  out.open("digest.bin");

  std::cout << "writing the result in digest.bin" << std::endl;
  binary_write(out, res);
  out.close();


  in.open("digest.bin", std::ios::binary);
  auto vsize = get_file_size_in_byte("digest.bin");
  std::vector<bool> ver(vsize);
  binary_read(in, ver, vsize);

  std::cout << "uncomment the last lines to display the result" << std::endl;
  auto str = vec_of_bool_string(ver);
  auto ss = reverse_string(str);
//  std::cout << vsize << std::endl;
  std::string display = ss; //std::string(ss.begin(), ss.begin() + N/4);
  std::transform(display.begin(), display.end(), display.begin(), ::tolower);
  std::cout << display << std::endl;
  return 0;

}
#endif
