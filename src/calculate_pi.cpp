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
#define CHECK_FROM_FILE

using namespace arithmetic;
using namespace std;

// https://en.wikipedia.org/wiki/Chudnovsky_algorithm
array<LongDouble, 3> binary_split(int l, int r, int digits) {
    LongDouble Pab, Qab, Rab;
    if (r == l + 1) {
        Pab = -LongDouble((6 * l - 5), digits) * LongDouble((2 * l - 1)) * LongDouble((6 * l - 1));
        Qab = LongDouble(l, digits) * l * l * 10939058860032000ll;
        Rab = Pab * LongDouble(545140134ll * l + 13591409, digits);
    } else {
        int m = (l + r) / 2;
        auto [Pam, Qam, Ram] = binary_split(l, m, digits);
        auto [Pmb, Qmb, Rmb] = binary_split(m, r, digits);
        Pab = Pam * Pmb;
        Qab = Qam * Qmb;
        Rab = Qmb * Ram + Pam * Rmb;
    }
    return {Pab, Qab, Rab};
}

LongDouble Chudnovsky(int digits) {

    // calc eps more so the last digit rounded down is correct
    LongDouble sq10005(10005, digits / LongDouble::base_exp + 2);
    sq10005.sqrt_fast();
    auto [P1n, Q1n, R1n] = binary_split(1, digits / LongDouble::base_exp + 2, (int) 1e9);  
    Q1n.precision = digits / LongDouble::base_exp + 2;
    LongDouble res = (Q1n * LongDouble(426880) * sq10005);
    LongDouble res2 = (Q1n * LongDouble(13591409) + R1n);
    res /= res2;
    return res;
}

LongDouble Leibnica(int digits) {
    int eps = 5;
    digits += eps;
    LongDouble sum (1, digits / LongDouble::base_exp + 2);
    LongDouble sm (1, digits / LongDouble::base_exp + 2);
    LongDouble st(1);
    st.divBase(digits);
    for (int i = 1; ; i++) {
        sm *= (LongDouble(i, digits / LongDouble::base_exp + 2) / (LongDouble(2 * i + 1)));
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
    const int right_bound = 1000000;

    cout << "Количество знаков после запятой (0 - " << right_bound << "): ";
    cin >> digits;
    
    if (cin.fail() || digits < 0 || digits > right_bound) {
        cerr << "Ошибка ввода\n";
        return 1;
    }

    double start = (double) clock() / CLOCKS_PER_SEC;
    LongDouble result = Leibnica(digits);
    LongDouble::output_insignificant_zeroes = true;

    cout << "PI:\n" << setprecision(digits + 1) << result << "\n";
    cout << setprecision(6);
    cerr << "TIME: " << TIME - start<< " sec (total " << TIME << " sec)\n";

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
