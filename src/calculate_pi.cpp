#include <iostream>
#include <iomanip>
#include <math.h>
#include <arithmetic.hpp>
#include <testing.hpp>

using namespace std;

LongDouble Leibnica(int iter, int digits) {
    int eps = 5;
    LongDouble pi;
    for (int i = 0; i < iter; i++) {
        if (i & 1) pi -= LongDouble(1, digits + eps) / LongDouble(i * 2 + 1);
        else       pi += LongDouble(1, digits + eps) / LongDouble(i * 2 + 1);
    }
    pi *= 4;
    pi.removeFirst(pi.mantissa - digits);
    return pi;
}

array<LongDouble, 3> binary_split(int l, int r) {
    LongDouble Pab, Qab, Rab;
    if (r == l + 1) {
        Pab = LongDouble(-(6 * l - 5) * (2 * l - 1) * (6 * l - 1));
        Qab = LongDouble(10939058860032000ll) * LongDouble(l) * LongDouble(l) * LongDouble(l);
        Rab = Pab * LongDouble(545140134ll * l + 13591409);
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
    int eps = 5;
    LongDouble sq10005(10005, digits + eps);
    sq10005.sqrt();
    auto [P1n, Q1n, R1n] = binary_split(1, digits / 10);  
    assert(Q1n * LongDouble(426880, digits + eps) * sq10005 == LongDouble(426880, digits + eps) * sq10005 * Q1n); 
    Q1n.precision = digits + eps;
    LongDouble res = (Q1n * LongDouble(426880, digits + eps) * sq10005);
    LongDouble res2 = (LongDouble(13591409) * Q1n + R1n);
    res /= res2;
    res.removeFirst(res.mantissa - digits);
    return res;
}

LongDouble calcpi(int digits) {
    int eps = 5;
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
    sum.removeFirst(sum.mantissa - digits);
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
    cout << "Chudnovsky:\n" << Chudnovsky(digits) << "\n";
    cerr << "TIME: " << (double) clock() / CLOCKS_PER_SEC - start<< " sec (total " << (double) clock() / CLOCKS_PER_SEC << " sec)\n";
    
    start = (double) clock() / CLOCKS_PER_SEC;
    cout << "Leibnica:\n" << calcpi(digits) << "\n";
    cerr << "TIME: " << (double) clock() / CLOCKS_PER_SEC - start << " sec (total " << (double) clock() / CLOCKS_PER_SEC << " sec)\n";

    return 0;
}
