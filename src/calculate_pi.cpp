#include <iostream>
#include <iomanip>
#include <math.h>
#include "arithmetic.hpp"

using namespace std;

int main() {
    std::cout << "pi" << std::endl;
}
// using ls = LongDouble;

// LongDouble calcpi(int digits) {
//     int eps = 10;
//     digits += eps;
//     LongDouble sum (1, digits);
//     LongDouble sm (1, digits);
//     LongDouble st(1);
//     st.mulBase(digits);
//     st = LongDouble(1, digits) / st;
//     for (int i = 1; ; i++) {
//         sm = sm * (LongDouble(i, digits) / (LongDouble(2 * i + 1, digits)));
//         sum = sum + sm;
//         if (sm < st) {
//             break;
//         }
//     }
//     digits -= eps;
//     sum = sum * LongDouble(2);
//     sum.removeFirst(sum.mantissa - digits);
//     return sum;
// }

// int main() {

//     cout << calcpi(100) << "\n";
//     cout << pi << "\n";
//     cout << fixed << M_PI << "\n";
//     cout << "TIME: " << (double) clock() / CLOCKS_PER_SEC << "\n";

//     return 0;
// }
