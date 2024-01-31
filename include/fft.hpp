#ifndef FFT_HPP
#define FFT_HPP

#include <math.h>
#include <vector>
#include <complex.h>
#include <algorithm>
#include <assert.h>

using namespace std;

using digit = int32_t;

namespace fft {

    class FFT {
    private:
        const double pi = acos(-1);
        static const int maxn = 1 << 20;
        typedef complex<double> ftype;
        ftype w[maxn];

    public:
        FFT () {
            init();
        }

        void init() {
            for(int i = 0; i < (*this).maxn; i++) {
                w[i] = polar(1., 2 * pi / (*this).maxn * i);
            }
        }

        template<typename T>
        void fft(T *in, ftype *out, int n, int k = 1) {
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

        void evaluate(digit*& p, int &size, ftype*& res, int &res_size) {
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
            res = (ftype*) calloc(size, sizeof(ftype));
            res_size = size;
            for (int i = 0; i < size; i++) {
                res[i] = ftype();
            }
            fft(p, res, size);
        }

        void interpolate(ftype* p, int &size, digit*& res, int &res_size) {
            ftype* inv = (ftype*) calloc(size, sizeof(ftype));
            fft(p, inv, size);
            res = (digit*)calloc(size, sizeof(digit));
            res_size = size;
            for (int i = 0; i < size; i++) {
                res[i] = round(real(inv[i]) / size);
            }
            reverse(res + 1, res + size);
        }

        void align(digit*& a, int &size_a, digit*& b, int &size_b) {
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

        void poly_multiply(digit*& a, int &size_a, digit*& b, int &size_b, digit*& res, int &res_size) {

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

        void normalize(digit*& c, int &size, int base=10) {
            int carry = 0;
            for (int i = 0; i < size; i++) {
                c[i] += carry;
                carry = c[i] / base;
                c[i] %= base;
            }
            int temp = carry;
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

        void multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, int base=10) {
            if (size_a == 0 && size_b == 0) {
                res_size = 0;
                res = (digit*) calloc(0, sizeof(digit));
                return;
            }
            poly_multiply(a, size_a, b, size_b, res, res_size);
            normalize(res, res_size, base);
        }

    };

}

#endif