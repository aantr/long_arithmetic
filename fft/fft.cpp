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
 
void square(int*& a, int &a_size, int64_t*& res, int &res_size) {
  if (a_size == 0) {
    res_size = 0;
    res = (int64_t*) malloc(0);
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
  res = (int64_t*) malloc(res_size * sizeof(int64_t));
  for (int i = 0; i < need; i++) {
    res[i] = llround(i % 2 == 0 ? fa[i >> 1].x : fa[i >> 1].y);
  }
}
 
void multiply(int*& a, int &a_size, int*& b, int &b_size, int64_t*& res, int &res_size) {
  if (a_size == 0 || b_size == 0) {
    res_size = 0;
    res = (int64_t*) malloc(0);
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
  res = (int64_t*) malloc(res_size * sizeof(int64_t));
  memset(res, 0, res_size * sizeof(int64_t));
  // vector<int64_t> res(need);
  for (int i = 0; i < need; i++) {
    res[i] = llround(i % 2 == 0 ? fa[i >> 1].x : fa[i >> 1].y);
  }

}
 
}  // namespace fft


namespace fft {

    void FFT::multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, int base) {
        if (size_a == 0 || size_b == 0) {
            res_size = 0;
            res = (digit*) malloc(0 * sizeof(digit));
            return;
        }


        int mult_size = 0;
        int64_t* mult = (int64_t*) malloc(0);
        fft_tourist::multiply(a, size_a, b, size_b, mult, mult_size);

        // vector<int> A{a, a + size_a}, B{b, b + size_b};
        // vector<int64_t> result = fft_vector::multiply(A, B);
        // int mult_size = (int) result.size();
        // int64_t* mult = (int64_t*) malloc(mult_size * sizeof(int64_t));
        // memcpy(mult, result.data(), mult_size * sizeof(int64_t));

        res_size = (int) mult_size + 1;
        res = (digit*) malloc(res_size * sizeof(digit));
        int carry = 0;
        for (int i = 0; i < res_size - 1; i++) {
            uint64_t d = mult[i] + carry;
            carry = d / base;
            res[i] = (digit) (d % base);
        }
        res[res_size - 1] = carry;
        for (int i = 0; i < res_size; i++) assert(res[i] >= 0);
        free(mult);
    }

}