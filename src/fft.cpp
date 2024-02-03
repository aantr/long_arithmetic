#include <math.h>
#include <vector>
#include <complex.h>
#include <algorithm>
#include <assert.h>
#include <fft.hpp>

namespace fft {

    FFT::FFT () {
        init();
    }

    FFT::~FFT() {
        free(w);
    }

    void FFT::init() {
        w = (ftype*)malloc(maxn * sizeof(ftype));
        for(int i = 0; i < (*this).maxn; i++) {
            w[i] = polar(1., 2 * pi / (*this).maxn * i);
        }
    }

    template<typename T>
    void FFT::fft(T *in, ftype *out, int n, int k) {
        if(n == 1) {
            *out = *in;
            return;
        }
        int t = maxn / n;
        n >>= 1;
        fft(in, out, n, 2 * k);
        fft(in + k, out + n, n, 2 * k);
        for(int i = 0, j = 0; i < n; i++, j += t) {
            ftype t = w[j] * out[i + n];
            out[i + n] = out[i] - t;
            out[i] += t;
        }

    }

    void FFT::evaluate(digit*& p, int &size, ftype*& res, int &res_size) {
        int prev_size = size;
        int s = 1;
        while (s < size) {
            s <<= 1;
        }
        p = (digit*) realloc(p, s * sizeof(digit));
        for (int i = prev_size; i < s; i++) {
            p[i] = 0;
        }
        size = s;
        res = (ftype*) malloc(size * sizeof(ftype));
        res_size = size;
        for (int i = 0; i < size; i++) {
            res[i] = ftype();
        }
        fft(p, res, size);
    }

    void FFT::interpolate(ftype* p, int &size, digit*& res, int &res_size) {
        ftype* inv = (ftype*) malloc(size * sizeof(ftype));
        fft(p, inv, size);
        res = (digit*)malloc(size * sizeof(digit));
        res_size = size;
        for (int i = 0; i < size; i++) {
            res[i] = round(real(inv[i]) / size);
        }
        reverse(res + 1, res + size);
        free(inv);
    }

    void FFT::align(digit*& a, int &size_a, digit*& b, int &size_b) {
        int n = size_a + size_b - 1;
        a = (digit*) realloc(a, n * sizeof(digit));
        for (int i = size_a; i < n; i++) {
            a[i] = 0;
        }
        size_a = n;

        b = (digit*) realloc(b, n * sizeof(digit));
        for (int i = size_b; i < n; i++) {
            b[i] = 0;
        }
        size_b = n;

    }

    void FFT::poly_multiply(digit*& a, int &size_a, digit*& b, int &size_b, digit*& res, int &res_size) {

        align(a, size_a, b, size_b);

        ftype* A, *B;

        int size_A, size_B;
        evaluate(a, size_a, A, size_A);        
        evaluate(b, size_b, B, size_B);

        for (int i = 0; i < size_A; i++) {
            A[i] *= B[i];
        }
        interpolate(A, size_A, res, res_size);
        free(A);
        free(B);
    }

    void FFT::normalize(digit*& c, int &size, int base) {
        digit carry = 0;
        for (int i = 0; i < size; i++) {
            c[i] += carry;
            carry = c[i] / base;
            c[i] %= base;
        }
        digit temp = carry;
        int count = 0;
        while (carry) {
            count++;
            carry /= base;
        }
        carry = temp;
        c = (digit*) realloc(c, (size + count) * sizeof(digit));
        int i = size;
        size += count;
        while (carry) {
            c[i++] = (carry % base);
            carry /= base;
        }
    }

    void FFT::multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, int base) {
        if (size_a == 0 && size_b == 0) {
            res_size = 0;
            res = (digit*) malloc(0 * sizeof(digit));
            return;
        }
        poly_multiply(a, size_a, b, size_b, res, res_size);
        normalize(res, res_size, base);
        for (int i = 0; i < res_size; i++) assert(res[i]>=0);
    }

}
