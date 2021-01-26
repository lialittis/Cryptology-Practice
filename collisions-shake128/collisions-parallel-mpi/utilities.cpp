#include "utilities.h"

half_byte hexa_to_binary(char c) {
  static std::unordered_map<char, half_byte> mp;
  mp['0'] = half_byte("0000");
  mp['1'] = half_byte("0001");
  mp['2'] = half_byte("0010");
  mp['3'] = half_byte("0011");
  mp['4'] = half_byte("0100");
  mp['5'] = half_byte("0101");
  mp['6'] = half_byte("0110");
  mp['7'] = half_byte("0111");
  mp['8'] = half_byte("1000");
  mp['9'] = half_byte("1001");
  mp['A'] = half_byte("1010");
  mp['B'] = half_byte("1011");
  mp['C'] = half_byte("1100");
  mp['D'] = half_byte("1101");
  mp['E'] = half_byte("1110");
  mp['F'] = half_byte("1111");
  return mp[c];
}

std::string binary_to_hexa(std::string b) {
  static std::unordered_map<std::string, std::string> mp;
  mp["0000"] = "0";
  mp["0001"] = "1";
  mp["0010"] = "2";
  mp["0011"] = "3";
  mp["0100"] = "4";
  mp["0101"] = "5";
  mp["0110"] = "6";
  mp["0111"] = "7";
  mp["1000"] = "8";
  mp["1001"] = "9";
  mp["1010"] = "A";
  mp["1011"] = "B";
  mp["1100"] = "C";
  mp["1101"] = "D";
  mp["1110"] = "E";
  mp["1111"] = "F";
  return mp[b];
}

state copy(state &A) {
  int w = A[0][0].size();
  state A_prime(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
  std::copy(A.begin(), A.end(), A_prime.begin());
  return A_prime;
}

std::vector<bool> string_vec_of_bool(std::string s) {
  s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
  int vec_size = s.size() * 4;
  std::vector<bool> vec(vec_size);
  int j = 0;
  for(auto&& c : s) {
    auto hb = hexa_to_binary(c);
    for(int i = 3; i >= 0; i--) {
      vec[j] = hb[i];
      j++;
    }
  }
  endian(vec);
  return vec;
}

void endian(std::vector<bool> &vec) {
  int w = vec.size()/25;
  if(w > 0)
    for(size_t i = 0; i < vec.size(); i += w) {
      std::reverse(vec.begin() + i, vec.begin() + i + w);
    }
}

std::vector<bool> concatinate(std::vector<bool> &N, std::vector<bool> &pad) {
    std::vector<bool> v;
    v.reserve(N.size() + pad.size());
    std::copy(N.begin(), N.end(), std::back_inserter(v));
    std::copy(pad.begin(), pad.end(), std::back_inserter(v));
    return v;
}

std::vector<bool> xor_vec(std::vector<bool> &u, std::vector<bool> &v) {
  std::vector<bool> res(u.size());
  for(size_t i = 0; i < u.size(); ++i) res[i] = u[i] xor v[i];
  return res;
}

std::string vec_of_bool_string(std::vector<bool> v) {
  int w = v.size() / 25;
  int n = v.size();
  while((n % 1600) != 0) {
    v.push_back(0);
    n++;
    w = v.size()/25;
  }
  if(w >= 16) {
    for(size_t i = 0; i < v.size(); i += w) {
      std::reverse(v.begin() + i, v.begin() + i + w);
    }
  }

  std::ostringstream o;
  std::ostringstream aux;
  std::bitset<4> hb;
  for(size_t i = 0; i < v.size(); i += 4) {
    aux << v[i] << v[i + 1] << v[i + 2] << v[i + 3];
    o << binary_to_hexa(aux.str());
    aux.str(std::string());
  }
  return o.str();
}

std::string reverse_string(std::string v) {
  int w = v.size() / 25;
  if(w > 0)
  for(size_t i = 0; i < v.size(); i += w) {
    for(int j = 0; j < w/2 ; ++j) {
      if(j % 2 == 0) std::swap(v[i + j], v[i + w - j - 2]);
      else           std::swap(v[i + j], v[i + w - j]);
    }
  }
  return v;
}
