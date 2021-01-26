#include "keccak.h"

int to_index(int slice_index) { return (slice_index + 2)  % 5;}

std::vector<std::vector<bool>> theta_C(const state& A, int w) {
  std::vector<std::vector<bool>> C(5, std::vector<bool>(w, false));
  for(int x = 0; x < 5; ++x) {
    int x_slice = to_index(x);
    for(int z = 0; z < w; ++z) {
      C[x_slice][z] = A[x_slice][0][z] ^ A[x_slice][1][z] ^ A[x_slice][2][z] ^ A[x_slice][3][z] ^ A[x_slice][4][z];
    }
  }
  return C;
}

std::vector<std::vector<bool>> theta_D(std::vector<std::vector<bool>> &C, int w) {
  std::vector<std::vector<bool>> D(5, std::vector<bool>(w, false));
  for(int x = 0; x < 5; ++x) {
    int x_slice = to_index(x);
    for(int z = 0; z < w; ++z) {
      int cx1 = (x - 1 + 5) % 5;
      int cx2 = (x + 1) % 5;
      int cz  = (z - 1 + w) % w;
      D[x_slice][z] = C[to_index(cx1)][z] ^ C[to_index(cx2)][cz];
    }
  }
  return D;
}

state theta(state &A) {
  int w = A[0][0].size();
  auto C = theta_C(A, w);
  auto D = theta_D(C, w);

  state A_prime(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
  for(int x = 0; x < 5; ++x) {
    int x_slice = to_index(x);
    for(int y = 0; y < 5; ++y) {
      int y_slice = to_index(y);
      for(int z = 0; z < w; ++z) {
        A_prime[x_slice][y_slice][z] = A[x_slice][y_slice][z] ^ D[x_slice][z];
      }
    }
  }
  return A_prime;
}

state get_A_prime(state& A) {
  int w = A[0][0].size();
  state A_prime(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
  int zero = to_index(0);
  for(int z = 0; z < w; ++z)
      A_prime[zero][zero][z] = A[zero][zero][z];
  return A_prime;
}

state rho(state& A) {
  int w = A[0][0].size();
  auto A_prime = get_A_prime(A);
  int x = 1;
  int y = 0;
  int x_slice = to_index(x);
  int y_slice = to_index(y);
  for(int t = 0; t <= 23; ++t) {
    for(int z = 0; z < w; ++z) {
      int z_index = (((z - ((t + 1) * (t + 2))/ 2) + w) % w + w) % w;
      A_prime[x_slice][y_slice][z] = A[x_slice][y_slice][z_index];
    }
    int tmp = y;
    y = (2 * x + 3 * y + 5) % 5;
    x = tmp;
    x_slice = to_index(x);
    y_slice = to_index(y);
  }
  return A_prime;
}

state pi(state& A) {
  int w = A[0][0].size();
  state A_prime(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
  for(int x = 0; x < 5; ++x) {
    int x_slice = to_index(x);
    for(int y = 0; y < 5; ++y) {
      int y_slice = to_index(y);
      for(int z = 0; z < w; ++z) {
        A_prime[x_slice][y_slice][z] = A[to_index((x + 3 * y) % 5)][x_slice][z];
      }
    }
  }
  return A_prime;
}

state chi(state& A) {
  int w = A[0][0].size();
  state A_prime(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w, false)));
  for(int x = 0; x < 5; ++x) {
    int x_slice = to_index(x);
    for(int y = 0; y < 5; ++y) {
      int y_slice = to_index(y);
      for(int z = 0; z < w; ++z) {
        A_prime[x_slice][y_slice][z] = A[x_slice][y_slice][z] xor ((A[to_index((x + 1) % 5)][y_slice][z] xor 1) and A[to_index((x + 2) % 5)][y_slice][z]);
      }
    }
  }
  return A_prime;
}

bool rc(int t) {
  if((t % 255) == 0) return 1;
  std::bitset<9> R("010000000");
  int limit = t % 255;
  for(int i = 1; i < limit; ++i) {
    R[8] = R[8] xor R[0];
    R[4] = R[4] xor R[0];
    R[3] = R[3] xor R[0];
    R[2] = R[2] xor R[0];
    std::bitset<9> tmp;
    R >>= 1;
  }
  return R[0];
}

std::vector<bool> RC_array(int w, int ir) {
  int l = (int)log2(w);
  std::vector<bool> RC(w, false);

  for(int j = 0; j <= l; ++j) {
    int index =  1 << j;
    index = index - 1;
    RC[index] = rc(j + 7 * ir);
  }
  return RC;
}

state iota(state &A, int ir) {
  int w = A[0][0].size();
  auto A_prime = copy(A);
  auto RC = RC_array(w, ir);

  int zero = to_index(0);
  for(int z = 0; z < w; ++z)
    A_prime[zero][zero][z] = A_prime[zero][zero][z] xor RC[z];
  return A_prime;
}

state Rnd(state& A, int ir) {
  auto theta_A = theta(A);
  auto rho_A   = rho(theta_A);
  auto pi_A    = pi(rho_A);
  auto chi_A   = chi(pi_A);
  return iota(chi_A, ir);
}

std::vector<bool> keccak_p(int nr, std::vector<bool> S) {
  int w = S.size() / 25;
  auto A = convert_string_to_state(S, w);
//  int l = (int)log2(w);
//  int ir_init  = 12 + 2 * l - nr;
//  int ir_limit = 12 + 2 * l - 1;
  for(int ir = 0; ir < 24; ir++) A = Rnd(A, ir);
  return convert_state_to_string(A);
}

state convert_string_to_state(std::vector<bool> s, int w) {
  state A(5, std::vector<std::vector<bool>>(5, std::vector<bool>(w)));
  for(int y = 0; y < 5; ++y) {
    int y_slice = to_index(y);
    for(int x = 0; x < 5; ++x) {
      int x_slice = to_index(x);
      for(int z = 0; z < w; ++z) {
        A[x_slice][y_slice][z] = s[w * (5 * y + x) + z];
      }
    }
  }
  return A;
}

std::vector<bool> convert_state_to_string(state& A) {
  int w = A[0][0].size();
  int b = slize_size * w;
  std::vector<bool> S(b);
  int index = 0;
  for(int y = 0; y < 5; ++y) {
    int y_slice = to_index(y);
    for(int x = 0; x < 5; ++x) {
      int x_slice = to_index(x);
      for(int z = 0; z < w; z++) {
        S[index++] = A[x_slice][y_slice][z];
      }
    }
  }
  return S;
}

// x and m is positive integer
std::vector<bool> pad101(const int x, const int m) {
  int j = ((((-1 * m - 2) + x) % x) + x) % x;
  int len = 2 + j;
  std::vector<bool> P(len, false);
  P[0] = true;
  P[P.size() - 1] = true;
  return P;
}

std::vector<bool> sponge(std::vector<bool>& N, int d, int r, int b, int nr) {
  int len_N = N.size();
  auto pad = pad101(r, len_N);
  auto P = concatinate(N, pad);

  int n = P.size()/r;
  int c = b - r;
  // p0, p1, ... P_n-1
  std::vector<bool> S(b);
  std::vector<bool> zero_c(c);

  for(int i = 0; i < n; ++i) {
    auto pi = get_Pi(P, i, r);
    auto p_zeros = concatinate(pi, zero_c);
    auto s_xor_p = xor_vec(S, p_zeros);
    S = keccak_p(nr, s_xor_p);
  }
  std::vector<bool> Z;
  while(true) {
    std::vector<bool> trunc_r(r);
    std::copy(S.begin(), S.begin() + r, trunc_r.begin());

    Z = concatinate(Z, trunc_r);
    if(d <= Z.size()) {
      std::vector<bool> trunc_d(d);
      std::copy(Z.begin(), Z.begin() + d, trunc_d.begin());
      return trunc_d;
    }
    S = keccak_p(nr, S);

  }
  return Z;
}

std::vector<bool> keccak_c(std::vector<bool> &N, int d, int c) {
  return sponge(N, d, 1600 - c, 1600 , 24);
}

std::vector<bool> shake128(std::vector<bool> &M, int d) {
  M.push_back(1); M.push_back(1); M.push_back(1); M.push_back(1);
  return keccak_c(M, d, 256);
}

std::vector<bool> get_Pi(const std::vector<bool> &source, const int i, const int r) {
  std::vector<bool> P(r);
  std::copy(source.begin() + i * r, source.begin() + i * r + r, P.begin());
  return P;
}
