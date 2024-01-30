#include <iostream>
#include <iomanip>
#include <math.h>
#include <testing.hpp>
#include <arithmetic.hpp>

#define TIME (double) clock() / CLOCKS_PER_SEC

using namespace arithmetic;
using namespace std;

array<LongDouble, 3> binary_split(int l, int r, int digits) {
    LongDouble Pab, Qab, Rab;
    if (r == l + 1) {
        Pab = LongDouble(-(6 * l - 5), digits) * 
                LongDouble((2 * l - 1), digits) * LongDouble((6 * l - 1), digits);
        Qab = LongDouble(l, digits) * LongDouble(l, digits) * LongDouble(l, digits) * LongDouble(10939058860032000ll, digits);
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
    int n_eps = 5; // for precision
    int p_eps = 5; // for precision
    LongDouble sq10005(10005, digits + 5 + p_eps);
    sq10005.sqrt_int();
    int n = digits / 10 + n_eps;
    auto [P1n, Q1n, R1n] = binary_split(1, n, (int) 1e9);  

    Q1n.precision = digits + p_eps;
    LongDouble res = (Q1n * LongDouble(426880) * sq10005);
    LongDouble res2 = (Q1n * LongDouble(13591409) + R1n);

    res /= res2;
    if (res.digits_size - 1 - digits > 0) {
        res.removeFirst(res.digits_size - 1 - digits);
    }
    return res;
}

LongDouble Leibnica(int digits) {
    int eps = 30;
    digits += eps;
    LongDouble sum (1, digits);
    LongDouble sm (1, digits);
    LongDouble st(1);
    st.divBase(digits);
    for (int i = 1; ; i++) {
        sm *= LongDouble(i, digits) / (LongDouble(2 * i + 1, digits));
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

int main(int argc, char* argv[]) {
    int digits;
    const int right_bound = 1000;

    if (argc > 1) {
        if (string(argv[1]) == "--test") {
            testing::test();
            return 0;
        }
        digits = atoi(argv[1]);
    } else {
        cout << "Количество знаков после запятой (0 - " << right_bound << "): ";
        cin >> digits;
    }
    if (cin.fail() || digits < 0 || digits > right_bound) {
        cerr << "Ошибка ввода\n";
        return 1;
    }

    double start = (double) clock() / CLOCKS_PER_SEC;
    cout << "PI:\n" << Chudnovsky(digits) << "\n";
    cerr << "TIME: " << TIME - start<< " sec (total " << TIME << " sec)\n";

    return 0;
}
