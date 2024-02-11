#include <defines.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <arithmetic_32.hpp>
#include <assert.h>
#include <sstream>
#include <fft.hpp>

#define TIME (double) clock() / CLOCKS_PER_SEC

using namespace arithmetic_32;
using namespace std;

// https://en.wikipedia.org/wiki/Chudnovsky_algorithm
array<LongDouble, 3> binary_split(int l, int r) {
    LongDouble Pab, Qab, Rab;
    if (r == l + 1) {
        Pab = -(6_ld * l - 5) * (2_ld * l - 1) * (6_ld * l - 1);
        Qab = 10939058860032000_ld * l * l * l;
        Rab = Pab * (545140134_ld * l + 13591409);
    } else {
        int m = (l + r) / 2;
        auto [Pam, Qam, Ram] = binary_split(l, m);
        auto [Pmb, Qmb, Rmb] = binary_split(m, r);
        Pab = Pam * Pmb;
        Qab = Qam * Qmb;
        Rab = Qmb * Ram + Pam * Rmb;
    }
    return {Pab, Qab, Rab};
}

LongDouble Chudnovsky(int digits) {
    int precision = (digits / 8 + 2) * 2;
    int default_precision = LongDouble::default_precision;
    LongDouble::default_precision = precision;
    LongDouble sq10005 = 10005;
    sq10005.sqrt_fast();

    LongDouble::default_precision = INT_MAX;
    auto [P1n, Q1n, R1n] = binary_split(1, digits / 10 + 2);
    Q1n.precision = precision;
    LongDouble::default_precision = default_precision;

    LongDouble res = Q1n * 426880 * sq10005;
    LongDouble res2 = Q1n * 13591409 + R1n;
    res /= res2;

    return res;
}

int main() {

    int digits;
    const int right_bound = 10000000;

    cout << "Количество знаков после запятой в hex (0 - " << right_bound << "): ";
    cin >> digits;
    
    if (cin.fail() || digits < 0 || digits > right_bound) {
        cerr << "Ошибка ввода\n";
        return 1;
    }

    double start = (double) clock() / CLOCKS_PER_SEC;
    LongDouble result = Chudnovsky(digits);
    LongDouble::output_insignificant_zeroes = true;

    cout << "PI:\n" << setprecision(digits + 1) << result << endl;
    cout << setprecision(6);
    cerr << "TIME: " << TIME - start<< " sec (total " << TIME << " sec)" << endl;
    return 0;
}
