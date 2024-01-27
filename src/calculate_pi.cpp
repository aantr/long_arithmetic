#include <iostream>
#include <iomanip>
#include <math.h>
#include <arithmetic.hpp>
#include <testing.hpp>

using namespace std;

LongDouble calcpi(int digits) {
    int eps = 3;
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
    const int right_bound = 300;

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
    cout << "Число пи: " << calcpi(digits) << "\n";
    cerr << "TIME: " << (double) clock() / CLOCKS_PER_SEC << "\n";

    return 0;
}
