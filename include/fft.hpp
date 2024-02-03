#pragma once 

#include <math.h>
#include <vector>
#include <complex.h>
#include <algorithm>
#include <assert.h>

namespace fft {

    using namespace std;
    using digit = int64_t;
    typedef complex<double> ftype;

    class FFT { 
    private:
        const double pi = acos(-1);
        const int maxn = 1 << 23;
        ftype *w;
    public:
        FFT();

        ~FFT();

        void init();

        template<typename T>
        void fft(T *in, ftype *out, int n, int k = 1);

        void evaluate(digit*& p, int &size, ftype*& res, int &res_size);

        void interpolate(ftype* p, int &size, digit*& res, int &res_size);

        void align(digit*& a, int &size_a, digit*& b, int &size_b);

        void poly_multiply(digit*& a, int &size_a, digit*& b, int &size_b, digit*& res, int &res_size);

        void normalize(digit*& c, int &size, int base);

        void multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, int base=10);
    };

}
