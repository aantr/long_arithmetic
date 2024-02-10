#pragma once 

#include <math.h>
#include <vector>
#include <complex.h>
#include <algorithm>
#include <assert.h>

namespace fft {

    using namespace std;
    using digit = uint32_t;
    class FFT { 
    public:
        void multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, int base=10);
    };

}
