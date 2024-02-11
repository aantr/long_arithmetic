#include <defines.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <arithmetic.hpp>
#include <assert.h>
#include <sstream>

#define TIME (double) clock() / CLOCKS_PER_SEC
// #define CHECK_FROM_FILE

using namespace arithmetic;
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
    int precision = (digits / LongDouble::base_exp + 2);
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

LongDouble Leibnica(int digits) {
    int eps = 5;
    digits += eps;
    LongDouble::default_precision = digits / LongDouble::base_exp + 2;
    LongDouble sum = 1, sm = 1, st = 1;
    st.divBase(digits);
    for (int i = 1; ; i++) {
        sm *= (LongDouble(i) / (LongDouble(2 * i + 1)));
        sum += sm;
        if (sm < st) {
            break;
        }
    }
    digits -= eps;
    sum *= 2;
    return sum;
}

int main() {

    int digits;
    const int right_bound = 10000000;
    // 1e3 - 201989
    // 1e4 - 5678
    // 1e5 - 4646 // 2 sec
    // 1e6 - 8151 // 11 sec
    // 5e6 - 4619715 ~ 1 min

    cout << "Количество знаков после запятой (0 - " << right_bound << "): ";
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

    #ifdef CHECK_FROM_FILE
    const string pifilename = "src/pi-1000000.txt";
    ifstream pifile (pifilename);
    if (pifile.is_open()) {
        string correct;
        pifile >> correct;
        stringstream ss;
        ss << setprecision(digits + 1) << result;
        string result_string = ss.str();
        if (result_string != correct.substr(0, digits + 2) && !(digits == 0 && result_string == "3")) {
            cout << "Incorrect output, correct:\n" << correct.substr(0, digits + 2) << "\n";
        } else {
            cout << "Check from file: " << digits << " digits is correct\n";
        }
        pifile.close();
    }
    #endif

    return 0;
}
