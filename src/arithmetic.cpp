#include <iostream>
#include <cstring>
#include <string>
#include <assert.h>
#include <iomanip>
#include <exception>
#include <arithmetic.hpp>
#include <fft.hpp>

namespace arithmetic {

    using namespace std;

    bool LongDouble::context_remove_left_zeroes = true;
    bool LongDouble::use_scientific_output = false;

    void division_error() {
        throw DivisionByZeroException();
    }   

    void memory_error() {
        throw MemoryLimitException();
    }

    void init_error() {
        throw InitError();
    }

    void init_precison_error() {
        throw InitPrecisonError();
    }

    void init_string_error() {
        throw InitStringError();
    }

    void sqrt_limit_error() {
        throw SqrtLimitError();
    }

    void sqrt_int_limit_error() {
        throw SqrtIntLimitError();
    }

    void negative_power_error() {
        throw NegativePowerError();
    }

    LongDouble::~LongDouble() {
        free(digits);
    }

    LongDouble::LongDouble() {
        *this = 0; // default precision
    }

    LongDouble::LongDouble(const LongDouble& x) {
        *this = x;
    }

    LongDouble::LongDouble(const LongDouble& other, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        sign = other.sign;
        digits_size = other.digits_size;
        free(digits);
        digits = (digit*) malloc(other.digits_size * sizeof(digit));
        if (!digits) memory_error();
        memcpy(digits, other.digits, other.digits_size * sizeof(digit));
        exponent = other.exponent;
    }

    template<class T>
    void init_from_string(LongDouble& res, const T& value) {
        int size = strlen(value);
        if (size == 0) {
            res = 0;
            return;
        }
        int index;
        if (value[0] == '-') {
            res.sign = -1; 
            index = 1; 
        } else if (value[0] == '+') {
            res.sign = 1;
            index = 1;
        } else {
            res.sign = 1;
            index = 0;
        }
        bool dot = false;
        for (int i = index; i < size; i++) {
            if (value[i] == '.') {
                dot = true;
                break;
            }
        }
        res.exponent = 0; 
        int digits_size_10 = size - index - dot;
        res.digits_size = (digits_size_10 - 1) / res.base_exp + 1;
        free(res.digits);
        res.digits = (digit*) malloc(res.digits_size * sizeof(digit));
        memset(res.digits, 0, res.digits_size * sizeof(digit));
        if (!res.digits) memory_error();
        int count = 0;
        bool was_dot = false;
        while (index < size) {
            if (value[index] == '.') {
                if (was_dot) {
                    init_string_error();
                }
                was_dot = true;
                res.exponent = -(size - index - 1); 
            } else if (value[index] <= '9' && value[index] >= '0') {
                int dig_index = digits_size_10 - 1 - count;
                res.digits[dig_index / res.base_exp] += (value[index] - '0') * res.pow_10[dig_index % res.base_exp];
                count++;
            } else {
                init_string_error();
            }
            index++;
        }
        res.removeZeroes();
    }

    template<class T>
    void init_from_int(LongDouble& res, T value) {
        T x = value;
        if (x < 0) res.sign = -1, x = -x;
        res.digits_size = 0;
        free(res.digits);
        res.digits = (digit*) malloc(res.digits_size * sizeof(digit));
        if (!res.digits) memory_error();
        while (x) {
            res.digits = (digit*) realloc(res.digits, (res.digits_size + 1) * sizeof(digit));
            if (!res.digits) memory_error();

            res.digits[res.digits_size] = x % res.base;
            res.digits_size++;
            x /= res.base;
        }
        res.exponent = 0;
        res.removeZeroes();
    }

    template<class T>
    void init_from_double(LongDouble& res, T& v) {
        stringstream ss;
        ss << fixed << v;
        init_from_string(res, ss.str().c_str());
    }

    LongDouble::LongDouble(const char* value) {
        init_from_string(*this, value);
    }

    LongDouble::LongDouble(const char* value, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        init_from_string(*this, value);
    }

    LongDouble::LongDouble(const string& value) {
        init_from_string(*this, value.c_str());
    }

    LongDouble::LongDouble(const string& value, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        init_from_string(*this, value.c_str());
    }

    LongDouble::LongDouble(const int &v) {
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const int &v, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const long long &v) {
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const long long &v, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const double &v) {
        init_from_double(*this, v);
    }

    LongDouble::LongDouble(const double &v, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        init_from_double(*this, v);
    }

    LongDouble::LongDouble(const long double &v) {
        init_from_double(*this, v);
    }

    LongDouble::LongDouble(const long double &v, int precision): precision(precision) {
        if (precision < MIN_PRECISION) {
            init_precison_error();
        }
        init_from_double(*this, v);
    }

    LongDouble LongDouble::abs() const {
        if (digits_size == 0) return 0;
        if (sign == -1) return -(*this);
        return *this;
    }

    bool LongDouble::isInt() const {
        return exponent >= 0;
    }

    bool LongDouble::isZero() const {
        return digits_size == 0;
    }

    bool LongDouble::isPower() const {
        return digits_size == 1 && digits[0] == 1;
    }

    void LongDouble::removeZeroes() {
        int left = 0, right = 0;
        for (int i = digits_size - 1; i >= 0; i--) {
            if (digits[i] == 0) {
                right++;
            } else {
                break;
            }
        }
        if (context_remove_left_zeroes) {
            for (int i = 0; i < digits_size; i++) {
                if (digits[i] == 0) {
                    exponent += base_exp;
                    left++;
                } else {
                    break;
                }
            }
        }
        if (right == digits_size) {
            free(digits);
            digits = (digit*) malloc(0 * sizeof(digit*));
            if (!digits) memory_error();
            digits_size = 0;
            sign = 1;
            exponent = 0;
        }
        else {
            if (left == 0) {
                digits_size = digits_size - right;
                digits = (digit*) realloc(digits, (digits_size) * sizeof(digit));
                if (!digits) memory_error();

            } else {
                digits_size = digits_size - left - right;
                digit* temp = (digit*) malloc(digits_size * sizeof(digit));
                if (!temp) memory_error();
                memcpy(temp, digits + left, digits_size * sizeof(digit));
                free(digits);
                digits = temp;
            }
        }
        
    }

    void LongDouble::removeFirst(int value) {
        assert(digits_size >= value && value >= 0);
        digits_size = digits_size - value;
        digit* temp = (digit*) malloc(digits_size * sizeof(digit));
        if (!temp) memory_error();
        memcpy(temp, digits + value, digits_size * sizeof(digit));
        free(digits);
        digits = temp;
        exponent += value * base_exp;
    }

    void LongDouble::mulBase(int power) { // base = 10
        exponent += power;
    }

    void LongDouble::divBase(int power) { // base = 10
        exponent -= power;
    }

    void LongDouble::round(int number) { // base = 10
        if (-exponent <= number || digits_size == 0) return;
        if (-exponent - number - 1 >= digits_size * base_exp) {
            *this = LongDouble(0, precision);
            return;
        }
        int d = (-exponent - number - 1) / base_exp;
        removeFirst((-exponent - number - 1) / base_exp);
        assert(digits_size >= 1);
        int rem = (-exponent - number - 1) % base_exp;
        digits[0] -= digits[0] % pow_10[rem];

        bool was = true;
        if (digits[0] / pow_10[rem] % 10 < 5) was = false;
        if (was) {
            LongDouble st(sign);
            st.divBase(number);
            *this += st;
        }

        d = (-exponent - number - 1) / base_exp;
        rem = (-exponent - number - 1) % base_exp;
        // delete rem-th digit
        digits[d] -= digits[d] % pow_10[rem + 1];
        removeZeroes();
    }

    void LongDouble::round() {
        round(0);
    }

    void LongDouble::floor(int number) {
        if (-exponent <= number) return;
        if (-exponent - number >= digits_size * base_exp) {
            *this = LongDouble(0, precision);
            return;
        }
        removeFirst((-exponent - number) / base_exp);
        int rem = (-exponent - number) % base_exp;
        if (rem > 0) {
            digits[0] -= digits[0] % pow_10[rem];
        }
        removeZeroes();
    }

    void LongDouble::floor() {
        floor(0);
    }

    void LongDouble::sqrt() { // use binary search
        if (*this < 0) {
            sqrt_limit_error();
        }
        LongDouble l, r = *this;
        l.precision = precision;
        r.precision = precision;
        LongDouble prev = -1;
        int less_one =  (*this < 1);
        if (less_one) {
            r = LongDouble(1, precision);
        }
        while (1) {
            LongDouble m = (l + r) * 0.5;
            if (m.digits_size > m.precision) {
                m.removeFirst(m.digits_size - m.precision); // floor m
            }
            if (m * m <= *this) {
                l = m;
            } else {                
                r = m;
            }
            if (prev == m) break;
            prev = m;
        }
        if (l.digits_size > l.precision) l.removeFirst(l.digits_size - l.precision);
        *this = l;
    }

    void LongDouble::sqrt_int() {
        if (!(isInt() && *this >= 1)) {
            sqrt_int_limit_error();
        }
        LongDouble x(1, precision);
        x.mulBase((digits_size - 1) / 2);
        LongDouble prev = -1;
        while (1) {

            x = (LongDouble(*this, precision + 1) / x + x) * 0.5;

            if (x.digits_size > precision) {
                x.removeFirst(x.digits_size - precision);
            }

            x.precision = precision;
            if (x == prev) {
                break;
            }
            prev = x;


        }
        *this = x;
    }

    void LongDouble::sqrt_fast() { // works only for integers >= 1
        if (!(isInt() && *this >= 1)) {
            sqrt_int_limit_error();
        }
        int plus = max(0, precision - (digits_size - exponent / base_exp) / 2 + 1);
        mulBase((plus * 2) * base_exp);
        LongDouble s, r;
        sqrt_rem(LongDouble(*this, digits_size + exponent / base_exp + 1), s, r);

        assert(*this == s * s + r);

        *this = LongDouble(s, precision);
        divBase(plus * base_exp);

        if (digits_size > precision) {
            removeFirst(digits_size - precision);
        }
    }

    void LongDouble::pow(int power) {
        if (power < 0) {
            negative_power_error();
        }
        if (power == 0) {
            *this = LongDouble(1, precision);
            return;
        }
        LongDouble res = *this;
        res.pow(power / 2);
        if (power & 1) {
            *this = res * res * (*this);
        } else {
            *this = res * res;
        }
    }

    void div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res) {

        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
        // should be assert on no right nulls digits
        LongDouble x = a, y = b;
        x.precision = (int) 1e9;
        y.precision = (int) 1e9;

        int m = n >> 1;
        if (n <= 1) {



            int A = 0, B = 0;
            for (int i = x.digits_size - 1; i >= 0; i--) {
                A = A * x.base + x.digits[i]; 
            }
            for (int i = y.digits_size - 1; i >= 0; i--) {
                B = B * y.base + y.digits[i];
            }
            if (B == 0) {
                division_error();
            }
            res = LongDouble(A / B, (int) 1e9);
            return;
        }


        LongDouble first3m(0, (int) 1e9);

        if (x.digits_size >= m) {
            first3m.digits_size = x.digits_size - m;
            first3m.digits = (digit*) malloc(first3m.digits_size * sizeof(digit));
            if (!first3m.digits) memory_error();
            memcpy(first3m.digits, x.digits + m, (first3m.digits_size) * sizeof(digit));
            LongDouble::context_remove_left_zeroes = false;
            first3m.removeZeroes();
            LongDouble::context_remove_left_zeroes = true;
        } else {
            first3m.digits = (digit*) malloc(0 * sizeof(digit));
            if (!first3m.digits) memory_error();
        }


        

        LongDouble res1(0, (int) 1e9), rem1(0, (int) 1e9);

        assert(first3m.isZero() || first3m.digits[first3m.digits_size - 1] != 0);
        div32(first3m, y, m, res1, rem1);


        assert(first3m == y * res1 + rem1);

        rem1.digits_size = rem1.digits_size + m;
        digit* temp = (digit*) malloc(rem1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, m * sizeof(digit));

        memcpy(temp + m, rem1.digits, (rem1.digits_size - m) * sizeof(digit));
        free(rem1.digits);
        rem1.digits = temp;
        memcpy(rem1.digits, x.digits, min(m, x.digits_size) * sizeof(digit));
        LongDouble res2(0, (int) 1e9), rem2(0, (int) 1e9);

        LongDouble::context_remove_left_zeroes = false;
        rem1.removeZeroes();
        LongDouble::context_remove_left_zeroes = true;


        div32(rem1, y, m, res2, rem2);

        assert(rem1 == y * res2 + rem2);
        res1.digits_size = res1.digits_size + m;
        temp = (digit*) malloc(res1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, m * sizeof(digit));

        memcpy(temp + m, res1.digits, (res1.digits_size - m) * sizeof(digit));

        free(res1.digits);
        res1.digits = temp;

        memcpy(res1.digits, res2.digits, res2.digits_size * sizeof(digit)); // add m digits
        LongDouble::context_remove_left_zeroes = false;
        res1.removeZeroes();
        LongDouble::context_remove_left_zeroes = true;
        res = res1;

    }

    void div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem) {


        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
        // should be assert on no right nulls digits

        LongDouble x = a, y = b;
        x.precision = (int) 1e9;
        y.precision = (int) 1e9;

        if (n <= 1) {

            __uint128_t A = 0, B = 0;
            for (int i = x.digits_size - 1; i >= 0; i--) {
                A = A * x.base + x.digits[i]; 
            }
            for (int i = y.digits_size - 1; i >= 0; i--) {
                B = B * y.base + y.digits[i];
            }
            if (B == 0) {
                division_error();
            }
            res = LongDouble((long long) (A / B), (int) 1e9);
            rem = LongDouble((long long) (A % B), (int) 1e9);

            assert(rem.exponent % rem.base_exp == 0);
            int d = rem.exponent / rem.base_exp;
            rem.digits_size += d;
            digit* temp = (digit*) malloc(rem.digits_size * sizeof(digit));
            if (!temp) memory_error();
            memset(temp, 0, d * sizeof(digit));

            memcpy(temp + d, rem.digits, (rem.digits_size - d) * sizeof(digit));
            free(rem.digits);
            rem.digits = temp;
            rem.exponent = 0;
            assert(a == b * res + rem);
            assert(rem >= 0);
            assert(rem < b);

            return;
        }


        if (y.digits_size <= n) {
            div21(x, y, n, res);
            LongDouble::context_remove_left_zeroes = false;
            rem = x - y * res;
            LongDouble::context_remove_left_zeroes = true;
            assert(rem >= 0);
            assert(rem.exponent == 0);
            // rem.digits_size += rem.exponent;
            // digit* temp = (digit*) malloc(rem.digits_size * sizeof(digit));
            // if (!temp) memory_error();
            // memset(temp, 0, rem.exponent * sizeof(digit));

            // memcpy(temp + rem.exponent, rem.digits, (rem.digits_size - rem.exponent) * sizeof(digit));
            // free(rem.digits);
            // rem.digits = temp;
            // rem.exponent = 0;
            assert(rem.sign == 1);
            assert(a == b * res + rem);
            assert(rem < b);
            return;
        }

        LongDouble x1(0, (int) 1e9); // first |x| - (|y| - n)
        x1.digits_size = max(0, x.digits_size - (y.digits_size - n));
        x1.digits = (digit*) malloc(x1.digits_size * sizeof(digit));
        if (!x1.digits) memory_error();

        memcpy(x1.digits, x.digits + (y.digits_size - n), (x1.digits_size) * sizeof(digit));

        LongDouble y1(0, (int) 1e9); // first n
        y1.digits_size = n;
        y1.digits = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!y1.digits) memory_error();

        memcpy(y1.digits, y.digits + (y.digits_size - n), (y1.digits_size) * sizeof(digit));

        // y *= base ^ n
        y1.digits_size += n;
        digit* temp = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, n * sizeof(digit));

        memcpy(temp + n, y1.digits, (y1.digits_size - n) * sizeof(digit));
        free(y1.digits);
        y1.digits = temp;

        bool maxres = x1 >= y1;

        // y /= base ^ n
        y1.digits_size -= n;
        temp = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp, y1.digits + n, y1.digits_size * sizeof(digit));
        free(y1.digits);
        y1.digits = temp;

        // if x1 >= y1 * base ^ n dont call div21

        // some statements
        // x1/y1 >= x/y
        // x1 / (y1 + 1) <= x/y
        // x1/y1 - base <= x1 / (y1 + 1)
        // x1 / (y1 + 1) <= x/y <= x1/y1
        // x1/y1-base <= x/y <= x1/y1



        LongDouble res1(0, (int) 1e9);
        if (maxres) {
            res1 = 1;
            res1.precision = (int) 1e9;
            res1.mulBase(n * x.base_exp);
            res1 -= 1;
        } else {
            div21(x1, y1, n, res1);            
        }

        LongDouble::context_remove_left_zeroes = false;

        LongDouble current_rem = LongDouble(x, (int) 1e9) - res1 * y; // may be exp != 0
        int count = 0;
        int q = 0;
        int leftq = -1, rightq = LongDouble::base - 1;
        while (rightq - leftq > 1) {
            int m = (leftq + rightq) / 2;
            (current_rem + y * m >= 0 ? rightq : leftq) = m; // n log**2(n)
        }
        q = rightq;
        current_rem += y * q;
        res1 -= q;  

        assert(count < LongDouble::base);

        LongDouble::context_remove_left_zeroes = true;
        assert(res1.exponent == 0);

        // temp = (digit*) malloc((current_rem.digits_size + current_rem.exponent) * sizeof(digit));
        // if (!temp) memory_error();

        // memcpy(temp + current_rem.exponent, current_rem.digits, (current_rem.digits_size) * sizeof(digit));
        // memset(temp, 0, (current_rem.exponent) * sizeof(digit));
        // free(current_rem.digits);
        // current_rem.digits = temp;
        // current_rem.digits_size += current_rem.exponent;
        // current_rem.exponent = 0;

        // temp = (digit*) malloc((res1.digits_size + res1.exponent) * sizeof(digit));
        // if (!temp) memory_error();

        // memcpy(temp + res1.exponent, res1.digits, (res1.digits_size) * sizeof(digit));
        // memset(temp, 0, (res1.exponent) * sizeof(digit));
        // free(res1.digits);
        // res1.digits = temp;
        // res1.digits_size += res1.exponent;
        // res1.exponent = 0;
        
        res = res1;
        rem = current_rem;
        assert(a == b * res + rem);
        assert(rem < b);
        assert(rem >= 0);


    }

    void sqrt_rem(const LongDouble n, LongDouble &s, LongDouble &r) {
        if (!(n.isInt() && n >= 1)) {
            sqrt_int_limit_error();
        }
        if (n <= 4) {
            LongDouble x(n, 2); 
            x.sqrt_int();            
            x.floor();
            s = x;
            r = n - s * s;
            assert(n == s * s + r);
            return;
        }

        int current_precison = n.digits_size + (n.exponent - 1) / n.base_exp + 3;
        LongDouble x(n, current_precison);

        // 10 ^ (d - 1) <= n < 10 ^ d
        // d - 1 <= log10 n < d

        // 2 ^ (d2 - 1) <= n < 2 ^ d2
        // d2 - 1 <= log2 n < d2

        // (d - 1) / d2 < log10(2)
        // d / (d2 - 1) > log10(2)

        // d2 > (d - 1) * log2(10)
        // d2 < d * log2(10) + 1

        // d * log2(10) - log(2, 10) < d2 < d * log2(10) + 1

        int digits_size_10 = n.digits_size * n.base_exp;
        while (digits_size_10 > 0 && n.digits[(digits_size_10 - 1) / n.base_exp] / n.pow_10[(digits_size_10 - 1) % n.base_exp] % 10 == 0) {
            digits_size_10--;
        }

        int power = floor((double) (digits_size_10 + n.exponent - 1) * log2(10)) + 1;
        // int power_max = floor((double) (n.digits_size + n.exponent) * log2(10));        
        LongDouble two(2, current_precison);
        two.pow(power);
        while (two <= x) { // max four times
            power++;
            two *= 2;
        }
        assert(two > x);
        assert(two <= x * 2);

        int was = 0;
        LongDouble st(2, current_precison);
        st.pow(power);
        int rem = power % 4;
        if (rem == 1 || rem == 2) {
            x *= 4;
            was = 1;
        }

        int power_b = (power - 1) / 4 + 1;
        LongDouble b(2, current_precison);
        b.pow(power_b);

        LongDouble temp_, a0, a1, a2, a3;
        temp_ = x;
        x.precision = x.digits_size + (x.exponent - 1) / LongDouble::base_exp - (b.digits_size + (b.exponent - 1) / LongDouble::base_exp) + 3;
        x /= b;
        x.floor();
        x.precision = (int) 1e9;
        a0 = temp_ - x * b;

        temp_ = x;
        x.precision = x.digits_size + (x.exponent - 1) / LongDouble::base_exp - (b.digits_size + (b.exponent - 1) / LongDouble::base_exp) + 3;
        x /= b;
        x.floor();
        x.precision = (int) 1e9;
        a1 = temp_ - x * b;

        temp_ = x;
        x.precision = x.digits_size + (x.exponent - 1) / LongDouble::base_exp - (b.digits_size + (b.exponent - 1) / LongDouble::base_exp) + 3;
        x /= b;
        x.floor();
        x.precision = (int) 1e9;
        a2 = temp_ - x * b;

        a3 = x;

        LongDouble s1, r1, q, u;

        sqrt_rem(a3 * b + a2, s1, r1);

        s1.precision = current_precison;
        r1.precision = current_precison;

        LongDouble A(r1 * b + a1, current_precison), B(s1 * 2, current_precison);
        A.precision = max(MIN_PRECISION, A.digits_size + (A.exponent - 1) / LongDouble::base_exp - (B.digits_size + (B.exponent - 1) / LongDouble::base_exp)) + 3;

        q = A / B;
        q.floor();
        q.precision = (int) 1e9;
        u = A - q * B;
        u.precision = (int) 1e9;
        s = s1 * b + q;
        r = u * b + a0 - q * q;

        if (r < 0) {
            r = r + s * 2 - 1;
            s -= 1;
        }

        if (was) { // if n was multipled by 4
            s *= 0.5;
            s.floor();
            r = n - s * s;
        }
        assert(n == s * s + r);

    }

};
