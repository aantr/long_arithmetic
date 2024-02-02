#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <arithmetic.hpp>
#include <assert.h>

#define TIME (double) clock() / CLOCKS_PER_SEC
#define CHECK_FROM_FILE

using namespace arithmetic;
using namespace std;

#define DEBUG 0

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

int f(int n) {
    auto [P1n, Q1n, R1n] = binary_split(1, 1 + n, (int) 1e9);  
    return min({P1n.digits_size, Q1n.digits_size, R1n.digits_size});
}

LongDouble Chudnovsky(int digits) {

    // calc eps more so the last digit rounded down is correct
    int p_eps = 2;

    LongDouble sq10005(10005, digits + p_eps);
    if (DEBUG) {
        cout << "[Calculating sqrt: "; cout.flush();
    }
    auto start = TIME;
    sq10005.sqrt_fast();
    if (DEBUG) {
        cout << TIME - start << "]" << endl;
    }
    if (DEBUG) {
        cout << "[Calculating binary_split: "; cout.flush(); start = TIME;
    }
    auto [P1n, Q1n, R1n] = binary_split(1, 1 + digits, (int) 1e9);  
    if (DEBUG) {
        cout << TIME - start << "]" << endl;
    }
    if (DEBUG) {
        cout << "[Multiplying binary_split and sqrt: "; cout.flush(); start = TIME;
    }
    Q1n.precision = digits + p_eps;
    LongDouble res = (Q1n * LongDouble(426880) * sq10005);
    LongDouble res2 = (Q1n * LongDouble(13591409) + R1n);

    if (DEBUG) {
        cout << TIME - start << "]" << endl;
    }
    if (DEBUG) {
        cout << "[Calculating result: "; cout.flush(); start = TIME;
    }
    res /= res2;

    if (DEBUG) {
        cout << TIME - start << "]" << endl;
    }
    if (res.digits_size - 1 - digits > 0) {
        res.removeFirst(res.digits_size - 1 - digits);
    }
    return res;
}

LongDouble Leibnica(int digits) {
    int eps = 1;
    digits += eps;
    LongDouble sum (1, digits);
    LongDouble sm (1, digits);
    LongDouble st(1);
    st.divBase(digits);
    for (int i = 1; ; i++) {
        sm *= (LongDouble(i, digits) / (LongDouble(2 * i + 1, digits)));
        sum += sm;
        if (sm < st) {
            break;
        }
    }
    digits -= eps;
    sum *= 2;
    sum.floor(digits);
    return sum;
}
int main() {
    int digits;
    const int right_bound = 1000;

    cout << "Количество знаков после запятой (0 - " << right_bound << "): ";
    cin >> digits;
    
    if (cin.fail() || digits < 0 || digits > right_bound) {
        cerr << "Ошибка ввода\n";
        return 1;
    }

    double start = (double) clock() / CLOCKS_PER_SEC;
    LongDouble result = Chudnovsky(digits);
    cout << "PI:\n" << result << "\n";
    cerr << "TIME: " << TIME - start<< " sec (total " << TIME << " sec)\n";

    #ifdef CHECK_FROM_FILE
    const string pifilename = "src/pi-1000000.txt";
    ifstream pifile (pifilename);
    if (pifile.is_open()) {
        string correct;
        pifile >> correct;
        if (result != LongDouble(correct.substr(0, digits + 2))) {
            cout << "Incorrect output, correct:\n" << correct.substr(0, digits + 2) << "\n";
        } else {
            cout << "Check from file: " << digits << " digits is correct\n";
        }
        pifile.close();
    }
    #endif

    return 0;
}
