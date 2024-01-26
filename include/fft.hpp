#include <math.h>
#include <vector>
#include <complex.h>
#include <algorithm>

using namespace std;

const double pi = acos(-1);
const int maxn = 1 << 17;
typedef complex<double> ftype;
ftype w[maxn];

using digit = int16_t;

class FFT {

public:
    FFT () {
        init();
    }

    void init() {
        for(int i = 0; i < maxn; i++) {
            w[i] = polar(1., 2 * pi / maxn * i);
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

    vector<ftype> evaluate(vector<digit> p) {
        while(__builtin_popcount(p.size()) != 1) {
            p.push_back(0);
        }
        vector<ftype> res(p.size());
        fft(p.data(), res.data(), p.size());
        return res;
    }

    vector<digit> interpolate(vector<ftype> p) {
        int n = p.size();
        vector<ftype> inv(n);
        fft(p.data(), inv.data(), n);
        vector<digit> res(n);
        for(int i = 0; i < n; i++) {
            res[i] = round(real(inv[i]) / n);
        }
        reverse(begin(res) + 1, end(res));
        return res;
    }

    void align(vector<digit> &a, vector<digit> &b) {
        int n = a.size() + b.size() - 1;
        while((int)a.size() < n) {
            a.push_back(0);
        }
        while((int)b.size() < n) {
            b.push_back(0);
        }
    }

    vector<digit> poly_multiply(vector<digit> a, vector<digit> b) {
        align(a, b);
        auto A = evaluate(a);
        auto B = evaluate(b);
        for(int i = 0; i <(int) A.size(); i++) {
            A[i] *= B[i];
        }
        return interpolate(A);
    }

    vector<digit> normalize(vector<digit> c, int base=10) {
        int carry = 0;
        for(auto &it: c) {
            it += carry;
            carry = it / base;
            it %= base;
        }
        while (carry) {
            c.push_back(carry % base);
            carry /= base;
        }
        return c;
    }

    vector<digit> multiply(vector<digit> a, vector<digit> b, int base=10) {
        return normalize(poly_multiply(a, b), base);
    }

};
