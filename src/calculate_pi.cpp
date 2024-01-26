#include <iostream>
#include <iomanip>
#include <math.h>
#include <arithmetic.hpp>

using namespace std;

LongDouble calcpi(int digits) {
    int eps = 3;
    digits += eps;
    LongDouble sum (1, digits);
    LongDouble sm (1, digits);
    LongDouble st(1);
    st.mulBase(digits);
    st = LongDouble(1, digits) / st;
    for (int i = 1; ; i++) {
        sm = sm * (LongDouble(i, digits) / (LongDouble(2 * i + 1, digits)));
        sum = sum + sm;
        if (sm < st) {
            break;
        }
    }
    digits -= eps;
    sum = sum * LongDouble(2);
    sum.removeFirst(sum.mantissa - digits);
    return sum;
}

int main() {

    const int right_bound = 1000;
    cout << "Количество знаков после запятой (0 - " << right_bound << "): ";
    int digits;
    cin >> digits;

    if (cin.fail() || digits < 0 || digits > right_bound) {
        cerr << "Ошибка ввода\n";
        return 1;
    }
    cout << "Число пи: " << calcpi(digits) << "\n";
    cout << "TIME: " << (double) clock() / CLOCKS_PER_SEC << "\n";

    return 0;
}
