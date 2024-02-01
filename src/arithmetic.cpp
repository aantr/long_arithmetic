#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <exception>
#include <arithmetic.hpp>
#include <fft.hpp>

namespace arithmetic {

    using namespace std;
    using namespace fft;

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

    void sqrt_limir_error() {
        throw SqrtLimitError();
    }

    void sqrt_int_limir_error() {
        throw SqrtIntLimitError();
    }

    void negative_power_error() {
        throw NegativePowerError();
    }

    FFT fft;

    istream& operator>>(istream& os, LongDouble& value) {
        string s;
        os >> s;
        value = s;
        return os;
    }

    ostream& operator<<(ostream& os, const LongDouble& value) { // выведет все ненулевые цифры
        if (value.sign == -1)
            os << '-';
        if (value.digits_size == 0) {
            os << '0';
            return os;
        }
        if (USE_SCIENTIFIC_OUTPUT && (value.digits_size > MAX_DIGIT_SCIENTIFIC_OUTPUT || (value.exponent > 0 && value.digits_size + value.exponent > MAX_DIGIT_SCIENTIFIC_OUTPUT) || 
            (value.exponent <= -value.digits_size && -value.exponent + 1 > MAX_DIGIT_SCIENTIFIC_OUTPUT))) {
            for (int i = value.digits_size - 1; i >= 0; i--) {
                os << value.digits[i];
                if (value.digits_size > 1 && i == value.digits_size - 1) os << '.';
            }
            int val = value.exponent + value.digits_size - 1;
            if (val > 0) os << 'e' << '+' << val;
            else if (val < 0) os << 'e' << val;
        } else {
            for (int i = -value.exponent; i > value.digits_size - 1; i--) {
                os << '0';
                if (i == -value.exponent) os << '.';
            }
            for (int i = value.digits_size - 1; i >= 0; i--) {
                os << value.digits[i];
                if (i > 0 && i == -value.exponent) os << '.';
            }
            for (int i = 0; i < value.exponent; i++) {
                os << '0';
            }
        }
        return os;
    }

    LongDouble& LongDouble::operator=(const LongDouble& other) // C5267
    {
        sign = other.sign;
        digits_size = other.digits_size;
        free(digits);
        digits = (digit*) malloc(other.digits_size * sizeof(digit));
        if (!digits) memory_error();
        memcpy(digits, other.digits, other.digits_size * sizeof(digit));
        assert(digits != other.digits);
        base = other.base;
        precision = other.precision;
        exponent = other.exponent;
        return *this;
    }

    LongDouble operator""_ld (const char* x, unsigned long size) {
        LongDouble res(x);
        res.precision = max((unsigned long) res.precision, size);
        return res;
    }

    LongDouble operator""_ld (long double x) {
        LongDouble res = x;
        return res;
    }

    LongDouble operator""_ld (unsigned long long x) {
        LongDouble res = (long long) x;
        return res;
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
        base = other.base;
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
        res.exponent = 0; 
        res.digits_size = size - index;
        free(res.digits);
        res.digits = (digit*) malloc(res.digits_size * sizeof(digit));
        if (!res.digits) memory_error();
        int count = 0;
        bool was_dot = false;
        while (index < size) {
            if (value[index] == '.') {
                if (was_dot) {
                    init_string_error();
                }
                was_dot = true;
                res.exponent = -(size - 1 - index); 
            } else if (value[index] <= '9' && value[index] >= '0') {
                res.digits[count] = value[index] - '0';
                count++;
            } else {
                init_string_error();
            }
            index++;
        }
        res.digits = (digit*) realloc(res.digits, count * sizeof(digit));
        if (!res.digits) memory_error();

        res.digits_size = count;
        reverse(res.digits, res.digits + res.digits_size);
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
        for (int i = 0; i < digits_size; i++) {
            if (digits[i] == 0) {
                exponent++;
                left++;
            } else {
                break;
            }
        }
        if (left == digits_size) {
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

    bool check_non_negative(LongDouble v) {
        for (int i = 0; i < v.digits_size; i++) {
            if (v.digits[i] < 0) return false;
        }
        return true;
    }

    void LongDouble::removeFirst(int value) {
        assert(digits_size >= value && value >= 0);
        digits_size = digits_size - value;
        digit* temp = (digit*) malloc(digits_size * sizeof(digit));
        if (!temp) memory_error();
        memcpy(temp, digits + value, digits_size * sizeof(digit));
        free(digits);
        digits = temp;
        exponent += value;
    }

    void LongDouble::mulBase(int power) {
        exponent += power;
    }

    void LongDouble::divBase(int power) {
        exponent -= power;
    }

    void LongDouble::round(int number) {
        if (-exponent <= number || digits_size == 0) return;
        if (-exponent - number - 1 >= digits_size) {
            *this = LongDouble(0, precision);
            return;
        }
        removeFirst(-exponent - number - 1);
        bool was = true;
        assert(digits_size >= 1);
        if (digits[0] < 5) was = false;
        if (was) {
            LongDouble st(sign);
            st.divBase(number);
            *this += st;
        }
        removeFirst(1);
        removeZeroes();
    }

    void LongDouble::round() {
        round(0);
    }

    void LongDouble::floor(int number) {
        if (-exponent <= number) return;
        if (-exponent - number >= digits_size) {
            *this = LongDouble(0, precision);
            return;
        }
        removeFirst(-exponent - number);
        removeZeroes();
    }

    void LongDouble::floor() {
        floor(0);
    }

    void LongDouble::sqrt() { // use binary search
        if (*this < 0) {
            sqrt_limir_error();
        }
        LongDouble l, r = *this;
        l.precision = precision;
        r.precision = precision;
        LongDouble prev = -1;
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

    void LongDouble::sqrt_int() { // works only for integers >= 1
        if (!(isInt() && *this >= 1)) {
            sqrt_int_limir_error();
        }
        LongDouble x(1, precision);
        x.mulBase((digits_size - 1) / 2);
        LongDouble prev = -1;
        while (1) {
            x = (LongDouble(*this, precision) / x + x) * 0.5;
            if (x.digits_size > x.precision) {
                x.removeFirst(x.digits_size - x.precision); // floor x
            }

            if (x == prev) {
                break;
            }
            prev = x;
        }
        *this = x;
    }

    void LongDouble::sqrt_fast() { // works only for integers >= 1
        if (!(isInt() && *this >= 1)) {
            sqrt_int_limir_error();
        }
        int plus = max(0, precision - (digits_size + exponent) / 2 + 1);
        mulBase(plus * 2);
        LongDouble s, r;
        sqrt_rem(*this, s, r);
        *this = LongDouble(s, precision);
        divBase(plus);

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

    // operators

    LongDouble LongDouble::operator*(const LongDouble& x) const {
        LongDouble res = *this;
        res *= x;
        return res;
    }

    void LongDouble::operator*=(const LongDouble& other) {

        LongDouble x = other;

        sign = sign * x.sign;
        exponent = exponent + x.exponent;

        digit* res = (digit*) malloc(0 * sizeof(digit));
        if (!res) memory_error();

        int res_size = 0;
        fft.multiply(digits, digits_size, x.digits, x.digits_size, res, res_size, base);
        
        free(digits);

        digits = res;
        digits_size = res_size;
        removeZeroes();
        if (digits_size > precision * 2) {
            removeFirst(digits_size - precision * 2);
        }
        removeZeroes();

    }

    LongDouble LongDouble::operator-() const {
        LongDouble res(*this, precision);
        res.sign = -res.sign;
        return res;
    }

    bool LongDouble::operator<(const LongDouble& x) const {
        if (isZero()) return x.sign == 1 && !x.isZero();
        if (x.isZero()) return sign == -1 && !isZero();
        if (sign != x.sign) return sign < x.sign && (!isZero() || !x.isZero()); 

        if (digits_size + exponent != x.digits_size + x.exponent)
            return (digits_size + exponent < x.digits_size + x.exponent) ^ (sign == -1);
        
        for (int i = 0; i < min(digits_size, x.digits_size); i++) {
            if (digits[digits_size - 1 - i] < x.digits[x.digits_size - 1 - i]) return sign == 1;
            if (digits[digits_size - 1 - i] > x.digits[x.digits_size - 1 - i]) return sign == -1;
        }
        bool all_zeroes = true;
        for (int i = min(digits_size, x.digits_size); i < digits_size; i++) {
            if (all_zeroes == false || digits[digits_size - 1 - i] != 0) {
                all_zeroes = false;
                break;
            }
        }
        for (int i = min(digits_size, x.digits_size); i < x.digits_size; i++) {
            if (all_zeroes == false || x.digits[x.digits_size - 1 - i] != 0) {
                all_zeroes = false;
                break;
            }
        }
        return (digits_size < x.digits_size && !all_zeroes) ^ (sign == -1);
    }

    bool LongDouble::operator==(const LongDouble& x) const {
        if (sign != x.sign)
            return (isZero() && x.isZero()); 
        if (digits_size + exponent != x.digits_size + x.exponent) return false;
        for (int i = 0; i < min(digits_size, x.digits_size); i++) {
            if (digits[digits_size - 1 - i] != x.digits[x.digits_size - 1 - i]) return false;
        }
        bool all_zeroes = true;
        for (int i = min(digits_size, x.digits_size); i < digits_size; i++) {
            if (all_zeroes == false || digits[digits_size - 1 - i] != 0) {
                all_zeroes = false;
                break;
            }
        }
        for (int i = min(digits_size, x.digits_size); i < x.digits_size; i++) {
            if (all_zeroes == false || x.digits[x.digits_size - 1 - i] != 0) {
                all_zeroes = false;
                break;
            }
        }
        return all_zeroes;
    }

    bool LongDouble::operator!=(const LongDouble& x) const {
        return !(*this == x);
    }

    bool LongDouble::operator>(const LongDouble& x) const {
        return x < *this;
    }

    bool LongDouble::operator<=(const LongDouble& x) const {
        return *this < x || *this == x;
    }

    bool LongDouble::operator>=(const LongDouble& x) const {
        return *this > x || *this == x;
    }

    LongDouble LongDouble::operator+(const LongDouble& x) const {
        if (sign == x.sign) {
            LongDouble res;
            res.precision = precision;
            res.sign = sign;

            int res_size = max(digits_size + exponent, x.digits_size + x.exponent) - min(exponent, x.exponent);
            digit* resd = (digit*) malloc(res_size * sizeof(digit));
            if (!resd) memory_error();
            memset(resd, 0, res_size * sizeof(digit));

            if (exponent <= x.exponent) {
                res.exponent = exponent;
                for (int i = 0; i < digits_size; i++) {
                    resd[i] += digits[i];
                }
                for (int i = 0; i < x.digits_size; i++) {
                    resd[i + x.exponent - exponent] += x.digits[i];
                }
            } else {
                res.exponent = x.exponent;
                for (int i = 0; i < x.digits_size; i++) {
                    resd[i] += x.digits[i];
                }
                for (int i = 0; i < digits_size; i++) {
                    resd[i + exponent - x.exponent] += digits[i];
                }
            }
            int carry = 0;
            for (int i = 0; i < res_size; i++) {
                resd[i] += carry;
                carry = 0;
                if (resd[i] >= base) resd[i] -= base, carry = 1;
            }
            if (carry) {
                resd = (digit*) realloc(resd, (res_size + 1) * sizeof(digit));
                if (!resd) memory_error();

                resd[res_size] = 1;
                res_size++;
            }
            free(res.digits);
            res.digits = resd;
            res.digits_size = res_size;
            res.removeZeroes();
            #ifdef PRECISION_ADD
            if (res.digits_size > res.precision) {
                res.removeFirst(res.digits_size - res.precision);
            }
            res.removeZeroes();
            #endif
            return res;
        }

        if (sign == -1)
            return x - (-(*this));

        return *this - (-x); 
    }

    void LongDouble::operator+=(const LongDouble& x) {
        *this = *this + x;
    }  

    LongDouble LongDouble::operator-(const LongDouble& x) const {
        if (sign == 1 && x.sign == 1) {

            if (*this < x) {
                LongDouble res = x - *this;
                return -res;
            }

            LongDouble res;
            res.sign = 1;
            res.precision = precision;
            int res_size = max(digits_size + exponent, x.digits_size + x.exponent) - min(exponent, x.exponent);
            digit* resd = (digit*) malloc(res_size * sizeof(digit));
            if (!resd) memory_error();
            memset(resd, 0, res_size * sizeof(digit));

            if (exponent <= x.exponent) {
                res.exponent = exponent;
                for (int i = 0; i < digits_size; i++) {
                    resd[i] += digits[i];
                }
                for (int i = 0; i < x.digits_size; i++) {
                    resd[i + x.exponent - exponent] -= x.digits[i];
                }
            } else {
                res.exponent = x.exponent;
                for (int i = 0; i < x.digits_size; i++) {
                    resd[i] -= x.digits[i];
                }
                for (int i = 0; i < digits_size; i++) {
                    resd[i + exponent - x.exponent] += digits[i];
                }
            }

            int carry = 0;
            for (int i = 0; i < res_size; i++) {
                resd[i] -= carry;
                carry = 0;
                if (resd[i] < 0) resd[i] += base, carry = 1;
            }
            assert(carry == 0);

            free(res.digits);

            res.digits = resd;
            res.digits_size = res_size;

            res.removeZeroes();
        #ifdef PRECISION_SUB
            if (res.digits_size > res.precision) {
                res.removeFirst(res.digits_size - res.precision);
            }
            res.removeZeroes();
        #endif
            return res;
        }

        if (sign == -1 && x.sign == -1)
            return (-x) - (-(*this));

        return *this + (-x);
    }

    void LongDouble::operator-=(const LongDouble& x) {
        *this = *this - x;
    }

    LongDouble LongDouble::operator/(const LongDouble& other) const {
        LongDouble x (*this);
        x.sign = 1;
        LongDouble y (other);
        y.sign = 1;
        int res_exponent = x.exponent - y.exponent;
        x.exponent = 0;
        y.exponent = 0;
        int plus = max(0, precision - x.digits_size + y.digits_size + 2);

        int temp_size = plus + x.digits_size;
        digit* temp = (digit*) malloc(temp_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, plus * sizeof(digit));
        memcpy(temp + plus, x.digits, digits_size * sizeof(digit));
        free(x.digits);
        x.digits = temp;
        x.digits_size = temp_size;

        res_exponent -= plus;
        LongDouble res;
        int n = 1;
        while (x.digits_size > 2 * n || y.digits_size > n || x.digits_size - y.digits_size + 1 > n) n <<= 1;
        div21(x, y, n, res);  
        res.precision = precision;
        res.exponent += res_exponent;
        res.sign = sign * other.sign;
        res.removeZeroes();
        if (res.digits_size > res.precision) {
            res.removeFirst(res.digits_size - res.precision);
        }
        res.removeZeroes();

        return res;
    }

    void LongDouble::operator/=(const LongDouble& other) { 
        *this = *this / LongDouble(other);
    }

    void div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res) { 
        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
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
        } else {
            first3m.digits = (digit*) malloc(0 * sizeof(digit));
            if (!first3m.digits) memory_error();
        }

        LongDouble res1(0, (int) 1e9), rem1(0, (int) 1e9);

        div32(first3m, y, m, res1, rem1);

        rem1.digits_size = rem1.digits_size + m;
        digit* temp = (digit*) malloc(rem1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, m * sizeof(digit));

        memcpy(temp + m, rem1.digits, (rem1.digits_size - m) * sizeof(digit));
        free(rem1.digits);
        rem1.digits = temp;
        memcpy(rem1.digits, x.digits, min(m, x.digits_size) * sizeof(digit));
        LongDouble res2(0, (int) 1e9), rem2(0, (int) 1e9);
        div32(rem1, y, m, res2, rem2);
        res1.digits_size = res1.digits_size + m;
        temp = (digit*) malloc(res1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, m * sizeof(digit));

        memcpy(temp + m, res1.digits, (res1.digits_size - m) * sizeof(digit));

        free(res1.digits);
        res1.digits = temp;

        memcpy(res1.digits, res2.digits, res2.digits_size * sizeof(digit)); // add m digits
        res = res1;
    }

    void div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem) {
        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
        LongDouble x = a, y = b;
        x.precision = (int) 1e9;
        y.precision = (int) 1e9;

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
            rem = LongDouble(A % B, (int) 1e9);

            rem.digits_size += rem.exponent;
            digit* temp = (digit*) malloc(rem.digits_size * sizeof(digit));
            if (!temp) memory_error();
            memset(temp, 0, rem.exponent * sizeof(digit));

            memcpy(temp + rem.exponent, rem.digits, (rem.digits_size - rem.exponent) * sizeof(digit));
            free(rem.digits);
            rem.digits = temp;
            rem.exponent = 0;
            return;
        }

        if (y.digits_size <= n) {
            div21(x, y, n, res);
            rem = x - y * res;
            assert(rem >= 0);
            rem.digits_size += rem.exponent;
            digit* temp = (digit*) malloc(rem.digits_size * sizeof(digit));
            if (!temp) memory_error();
            memset(temp, 0, rem.exponent * sizeof(digit));

            memcpy(temp + rem.exponent, rem.digits, (rem.digits_size - rem.exponent) * sizeof(digit));
            free(rem.digits);
            rem.digits = temp;
            rem.exponent = 0;
            assert(rem.sign == 1);
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

        // y *= 10 ^ n
        y1.digits_size += n;
        digit* temp = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, n * sizeof(digit));

        memcpy(temp + n, y1.digits, (y1.digits_size - n) * sizeof(digit));
        free(y1.digits);
        y1.digits = temp;

        bool maxres = x1 >= y1;

        // y /= 10 ^ n
        y1.digits_size -= n;
        temp = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp, y1.digits + n, y1.digits_size * sizeof(digit));
        free(y1.digits);
        y1.digits = temp;

        // if x1 >= y1 * 10 ^ n dont call div21

        // some statements
        // x1/y1 >= x/y
        // x1 / (y1 + 1) <= x/y
        // x1/y1 - 10 <= x1 / (y1 + 1)
        // x1 / (y1 + 1) <= x/y <= x1/y1
        // x1/y1-10 <= x/y <= x1/y1

        LongDouble res1(0, (int) 1e9);
        if (maxres) {
            res1 = 1;
            res1.precision = (int) 1e9;
            res1.mulBase(n);
            res1 -= 1;
        } else {
            div21(x1, y1, n, res1);
        }

        LongDouble current_rem = LongDouble(x, (int) 1e9) - res1 * y;
        while (current_rem < 0) {
            current_rem += y;            
            res1 -= 1;
        }

        temp = (digit*) malloc((current_rem.digits_size + current_rem.exponent) * sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp + current_rem.exponent, current_rem.digits, (current_rem.digits_size) * sizeof(digit));
        memset(temp, 0, (current_rem.exponent) * sizeof(digit));
        free(current_rem.digits);
        current_rem.digits = temp;
        current_rem.digits_size += current_rem.exponent;
        current_rem.exponent = 0;

        temp = (digit*) malloc((res1.digits_size + res1.exponent) * sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp + res1.exponent, res1.digits, (res1.digits_size) * sizeof(digit));
        memset(temp, 0, (res1.exponent) * sizeof(digit));
        free(res1.digits);
        res1.digits = temp;
        res1.digits_size += res1.exponent;
        res1.exponent = 0;
        
        res = res1;
        rem = current_rem;
    }

    void sqrt_rem(const LongDouble n, LongDouble &s, LongDouble &r) {
        if (!(n.isInt() && n >= 1)) {
            sqrt_int_limir_error();
        }
        if (n < n.base * n.base) {
            LongDouble x(n, n.digits_size + n.exponent + 2);
            x.sqrt_int();
            x.floor();
            s = x;
            r = n - s * s;
            return;
        }
        int current_precison = n.digits_size + n.exponent + 1;
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

        int power = floor((double) (n.digits_size + n.exponent - 1) * log2(10)) + 1;
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
        x.precision = x.digits_size + x.exponent + 1 - (b.digits_size + b.exponent) + 1;
        x /= b;
        x.floor();
        a0 = temp_ - x * b;

        temp_ = x;
        x.precision = x.digits_size + x.exponent + 1 - (b.digits_size + b.exponent) + 1;
        x /= b;
        x.floor();
        a1 = temp_ - x * b;

        temp_ = x;
        x.precision = x.digits_size + x.exponent + 1 - (b.digits_size + b.exponent) + 1;
        x /= b;
        x.floor();
        a2 = temp_ - x * b;

        a3 = x;

        LongDouble s1, r1, q, u;

        sqrt_rem(a3 * b + a2, s1, r1);

        s1.precision = current_precison;
        r1.precision = current_precison;

        LongDouble A(r1 * b + a1, current_precison), B(s1 * 2, current_precison);
        A.precision = max(MIN_PRECISION, A.digits_size + A.exponent + 1 - (B.digits_size + B.exponent) + 2);

        q = A / B;
        q.floor(); 
        u = A - q * B;

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

    }

};
