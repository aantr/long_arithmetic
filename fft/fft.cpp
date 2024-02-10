#include <defines.hpp>

#include <math.h>
#include <complex.h>
#include <algorithm>
#include <assert.h>
#include <fft.hpp>
#include <iostream>

using namespace std;

namespace fft_tourist { // by tourist https://codeforces.com/contest/1054/submission/48071254
 
typedef double dbl;
 
struct num {
  dbl x, y;
  num() { x = y = 0; }
  num(dbl x_, dbl y_) : x(x_), y(y_) {}
};
 
inline num operator+(num a, num b) { return num(a.x + b.x, a.y + b.y); }
inline num operator-(num a, num b) { return num(a.x - b.x, a.y - b.y); }
inline num operator*(num a, num b) { return num(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x); }
inline num conj(num a) { return num(a.x, -a.y); }
 
int base = 1;
num* roots = new num[2]{{0, 0}, {1, 0}};
int roots_size = 2;
int* rev = new int[2]{0, 1};
int rev_size = 2;
 
const dbl PI = static_cast<dbl>(acosl(-1.0));
 
void ensure_base(int nbase) {
  if (nbase <= base) {
    return;
  }
  // rev.resize(1 << nbase);
  rev_size = 1 << nbase;
  rev = (int*) realloc(rev, rev_size * sizeof(int));
  for (int i = 0; i < (1 << nbase); i++) {
    rev[i] = (rev[i >> 1] >> 1) + ((i & 1) << (nbase - 1));
  }
  roots_size = 1 << nbase;
  roots = (num*) realloc(roots, roots_size * sizeof(num));
  while (base < nbase) {
    dbl angle = 2 * PI / (1 << (base + 1));
//      num z(cos(angle), sin(angle));
    for (int i = 1 << (base - 1); i < (1 << base); i++) {
      roots[i << 1] = roots[i];
//        roots[(i << 1) + 1] = roots[i] * z;
      dbl angle_i = angle * (2 * i + 1 - (1 << base));
      roots[(i << 1) + 1] = num(cos(angle_i), sin(angle_i));
    }
    base++;
  }
}
 
void fft(num*& a, int &a_size, int n = -1) {
  if (n == -1) {
    n = (int) a_size;
  }
  assert((n & (n - 1)) == 0);
  int zeros = __builtin_ctz(n);
  ensure_base(zeros);
  int shift = base - zeros;
  for (int i = 0; i < n; i++) {
    if (i < (rev[i] >> shift)) {
      swap(a[i], a[rev[i] >> shift]);
    }
  }
  for (int k = 1; k < n; k <<= 1) {
    for (int i = 0; i < n; i += 2 * k) {
      for (int j = 0; j < k; j++) {
        num z = a[i + j + k] * roots[j + k];
        a[i + j + k] = a[i + j] - z;
        a[i + j] = a[i + j] + z;
      }
    }
  }
}
 
num* fa = new num[0], *fb = new num[0];
int fa_size = 0, fb_size = 0;
 
void square(uint32_t *& a, int &a_size, uint64_t *& res, int &res_size) {
  if (a_size == 0) {
    res_size = 0;
    res = (uint64_t*) malloc(0);
    return;
  }
  int need = (int) a_size + (int) a_size - 1;
  int nbase = 1;
  while ((1 << nbase) < need) nbase++;
  ensure_base(nbase);
  int sz = 1 << nbase;
  if ((sz >> 1) > (int) fa_size) {
    // fa.resize(sz >> 1);
    fa_size = sz >> 1;
    fa = (num*) realloc(fa, fa_size * sizeof(num));
  }
  for (int i = 0; i < (sz >> 1); i++) {
    int x = (2 * i < (int) a_size ? a[2 * i] : 0);
    int y = (2 * i + 1 < (int) a_size ? a[2 * i + 1] : 0);
    fa[i] = num(x, y);
  }
  fft(fa, fa_size, sz >> 1);
  num r(1.0 / (sz >> 1), 0.0);
  for (int i = 0; i <= (sz >> 2); i++) {
    int j = ((sz >> 1) - i) & ((sz >> 1) - 1);
    num fe = (fa[i] + conj(fa[j])) * num(0.5, 0);
    num fo = (fa[i] - conj(fa[j])) * num(0, -0.5);
    num aux = fe * fe + fo * fo * roots[(sz >> 1) + i] * roots[(sz >> 1) + i];
    num tmp = fe * fo;
    fa[i] = r * (conj(aux) + num(0, 2) * conj(tmp));
    fa[j] = r * (aux + num(0, 2) * tmp);
  }
  fft(fa, fa_size, sz >> 1);
  res_size = need;
  res = (uint64_t*) malloc(res_size * sizeof(uint64_t));
  for (int i = 0; i < need; i++) {
    res[i] = llround(i % 2 == 0 ? fa[i >> 1].x : fa[i >> 1].y);
  }
}
 
void multiply(uint32_t *& a, int &a_size, uint32_t*& b, int &b_size, uint64_t*& res, int &res_size) {
  if (a_size == 0 || b_size == 0) {
    res_size = 0;
    res = (uint64_t*) malloc(0);
    return;
  }
  int eq = 1;
  if (a_size == b_size) {
    for (int i = 0; i < a_size; i++) {
      eq &= a[i] == b[i];
    }
  } else {
    eq = 0;
  }
  if (a_size == b_size && eq) {
    square(a, a_size, res, res_size);
    return;
  }
  int need = (int) a_size + (int) b_size - 1;
  int nbase = 1;
  while ((1 << nbase) < need) nbase++;
  ensure_base(nbase);
  int sz = 1 << nbase;
  if (sz > (int) fa_size) {
    // fa.resize(sz);
    fa_size = sz;
    fa = (num*) realloc(fa, fa_size * sizeof(num));
  }

  for (int i = 0; i < sz; i++) {
    int x = (i < (int) a_size ? a[i] : 0);
    int y = (i < (int) b_size ? b[i] : 0);
    fa[i] = num(x, y);
  }
  fft(fa, fa_size, sz);
  num r(0, -0.25 / (sz >> 1));
  for (int i = 0; i <= (sz >> 1); i++) {
    int j = (sz - i) & (sz - 1);
    num z = (fa[j] * fa[j] - conj(fa[i] * fa[i])) * r;
    fa[j] = (fa[i] * fa[i] - conj(fa[j] * fa[j])) * r;
    fa[i] = z;
  }
  for (int i = 0; i < (sz >> 1); i++) {
    num A0 = (fa[i] + fa[i + (sz >> 1)]) * num(0.5, 0);
    num A1 = (fa[i] - fa[i + (sz >> 1)]) * num(0.5, 0) * roots[(sz >> 1) + i];
    fa[i] = A0 + A1 * num(0, 1);
  }

  fft(fa, fa_size, sz >> 1);

  res_size = need;
  res = (uint64_t*) malloc(res_size * sizeof(uint64_t));
  memset(res, 0, res_size * sizeof(int64_t));
  // vector<int64_t> res(need);
  for (int i = 0; i < need; i++) {
    res[i] = llround(i % 2 == 0 ? fa[i >> 1].x : fa[i >> 1].y);
  }

}
 
}  // namespace fft




void karatsuba_mul(int n, uint64_t* a, uint64_t* b, uint64_t*& res, int &res_size) {
    auto k = n >> 1;
    
    if (n == 1) {
        res = (uint64_t*) malloc(2 * sizeof(uint64_t));
        res[0] = a[0] * b[0];
        res[1] = 0;
        res_size = 2;
        return;
    }

    uint64_t* P1;
    int P1_size;
    uint64_t* P2;
    int P2_size;
    uint64_t* P3;
    int P3_size;
    karatsuba_mul(k, a + k, b + k, P1, P1_size);
    karatsuba_mul(k, a, b, P2, P2_size);

    uint64_t* Xlr = (uint64_t*) malloc(k * sizeof(uint64_t));
    uint64_t* Ylr = (uint64_t*) malloc(k * sizeof(uint64_t));
    
    for (int i = 0; i < k; ++i) {
        Xlr[i] = a[i + k] + a[i];
        Ylr[i] = b[i + k] + b[i];
    }
    
    karatsuba_mul(k, Xlr, Ylr, P3, P3_size);

    res = (uint64_t*) malloc(2 * n * sizeof(uint64_t));
    memset(res, 0, 2 * n * sizeof(uint64_t));
    res_size = 2 * n;
    
    for (auto i = 0; i < n; ++i) {
        P3[i] -= P2[i] + P1[i];
    }
    
    for (auto i = 0; i < n; ++i) {
        res[i] = P2[i];
    }

    for (auto i = n; i < 2 * n; ++i) {
        res[i] = P1[i - n];
    }

    for (auto i = k; i < n + k; ++i) {
        res[i] += P3[i - k];
    }

}


namespace fft {

    void FFT::multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, uint32_t base) {
        if (size_a == 0 || size_b == 0) {
            res_size = 0;
            res = (digit*) malloc(0 * sizeof(digit));
            return;
        }

        int n = 1;
        while (n < size_a || n < size_b) n <<= 1;
        int prev_size_a = size_a, prev_size_b = size_b;
        uint64_t* A = (uint64_t*) malloc(n * sizeof(uint64_t));
        uint64_t* B = (uint64_t*) malloc(n * sizeof(uint64_t));
        memset(A + size_a, 0, (n - size_a) * sizeof(uint64_t));
        memset(B + size_b, 0, (n - size_b) * sizeof(uint64_t));
        for (int i = 0; i < size_a; i++) {
            A[i] = a[i];
        }
        for (int i = 0; i < size_b; i++) {
            B[i] = b[i];
        }

        int mult_size = 0;
        uint64_t* mult = (uint64_t*) malloc(0);
        // karatsuba_mul(n, A, B, mult, mult_size);
        fft_tourist::multiply(a, size_a, b, size_b, mult, mult_size);
        res_size = (int) mult_size + 1;
        res = (digit*) malloc(res_size * sizeof(digit));
        if (__builtin_popcount(base) == 1) {
            uint64_t pow = 0;
            uint64_t x = 1;
            while (x < base) {
                x <<= 1;
                pow++;
            }
            digit carry = 0;
            for (int i = 0; i < res_size - 1; i++) {
                uint64_t d = mult[i] + carry;
                carry = d >> pow;
                res[i] = (d & ((1 << pow) - 1));
            }
            res[res_size - 1] = carry;
        } else {
            digit carry = 0;
            for (int i = 0; i < res_size - 1; i++) {
                uint64_t d = mult[i] + carry;
                carry = d / base;
                res[i] = (digit) (d % base);
            }
            res[res_size - 1] = carry;
        }
    
        for (int i = 0; i < res_size; i++) assert(res[i] >= 0);
        free(mult);

        a = (digit*) realloc(a, prev_size_a * sizeof(digit));
        b = (digit*) realloc(b, prev_size_b * sizeof(digit));
    }

}
