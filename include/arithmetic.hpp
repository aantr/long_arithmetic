#ifndef ARITHMETIC_HPP
#define ARITHMETIC_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fft.hpp>
#include <assert.h>
#include <iomanip>
#include <exception>

using namespace std;
using namespace fft;

#define FREE_MEMORY
// #define PRECISION_ADD
// #define PRECISION_SUB
#define USE_SCIENTIFIC_OUTPUT 0
#define MAX_DIGIT_SCIENTIFIC_OUTPUT 16
#define MIN_PRECISION 1

/*
Short documentaion:

input string format:
[-+][0-9][.][0-9]

output format:
outputing all digits
big output -> print exponent if needed
small output -> print without exponent

how precision works
precison >= 2
always use floor in math
add/sub working only if defines
mul working for double precision 
for ex:
precision = 3
=> 
123 * 12345 = 151843e+1
division:
precision = 3
123 / 45 = 2.73

assymptotics:

sub / add O(n)
mul O(n * log n) (fft)
div O(n * log ^ 2 (n))
sqrt O(n * log ^ 3 (n))

*/

namespace arithmetic {    

    FFT fft;

    class DivisionByZeroException {
    public:
        const char* what () {
            return "Division by zero";
       }
    };

    class MemoryLimitException {
    public:
        const char* what () {
            return "Cannot allocate enough memory";
       }
    };

    class InitError {
    public:
        const char* what () {
            return "Inititalization error";
       }
    };

    class InitPrecisonError {
    public:
        const char* what () {
            return "Precision shoud be >= MIN_PRECISION";
       }
    };

    class InitStringError {
    public:
        const char* what () {
            return "Cannot init from string (wrong format)";
       }
    };

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

    void _error() {
        cerr << "Division by zero" << endl;
        exit(1);
    }



    class LongDouble {
    public:

        int sign = 1;
        digit* digits = nullptr; 
        int digits_size = 0;
        int base = 10;
        int precision = 64; // >= 2
        int exponent = 0;

        LongDouble(); 
        LongDouble(const LongDouble& x); 
        LongDouble(const LongDouble& x, int precision); 
        LongDouble(const char *value); 
        LongDouble(const char *value, int precision);
        LongDouble(const string &value); 
        LongDouble(const string &value, int precision);
        LongDouble(const int &value);
        LongDouble(const int &value, int precision); 
        LongDouble(const long long &value);
        LongDouble(const long long &value, int precision); 
        LongDouble(const double &value); 
        LongDouble(const double &value, int precision); 

        bool isInt() const; 
        bool isZero() const; 
        bool isPower() const; 
        void removeZeroes(); 
        void removeFirst(int value);
        void mulBase(int power);
        void divBase(int power);
        void round(int number);
        void round();
        void floor(int number);
        void floor();
        void sqrt();
        void sqrt_int();
        LongDouble abs() const;
        LongDouble operator+(const LongDouble& x) const;
        void operator+=(const LongDouble& x);
        LongDouble operator-(const LongDouble& x) const;
        void operator-=(const LongDouble& x);
        LongDouble operator*(const LongDouble& x) const;
        void operator*=(const LongDouble& x);
        LongDouble operator/(const LongDouble& x) const;
        void operator/=(const LongDouble& x);

        template<class T>
        LongDouble operator+(const T& x) const;
        template<class T>
        void operator+=(const T& x);
        template<class T>
        LongDouble operator-(const T& x) const;
        template<class T>
        void operator-=(const T& x);
        template<class T>
        LongDouble operator*(const T& x) const;
        template<class T>
        void operator*=(const T& x);
        template<class T>
        LongDouble operator/(const T& x) const;
        template<class T>
        void operator/=(const T& x);

        LongDouble operator-() const;
        bool operator<(const LongDouble& x) const;
        bool operator<=(const LongDouble& x) const;
        bool operator>(const LongDouble& x) const;
        bool operator>=(const LongDouble& x) const;
        bool operator==(const LongDouble& x) const;
        bool operator!=(const LongDouble& x) const;
        LongDouble& operator=(const LongDouble& x); // assignment operator

        template<class T>
        bool operator<(const T& x) const;
        template<class T>
        bool operator<=(const T& x) const;
        template<class T>
        bool operator>(const T& x) const;
        template<class T>
        bool operator>=(const T& x) const;
        template<class T>
        bool operator==(const T& x) const;
        template<class T>
        bool operator!=(const T& x) const;
        template<class T>
        LongDouble& operator=(const T& x);
    };

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
        #ifdef FREE_MEMORY
        delete digits;
        #endif
        digits = (digit*) calloc(other.digits_size, sizeof(digit));
        if (!digits) memory_error();
        memcpy(digits, other.digits, other.digits_size * sizeof(digit));
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

    LongDouble::LongDouble() {
        *this = 0;
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
        #ifdef FREE_MEMORY
        delete digits;
        #endif
        digits = (digit*) calloc(other.digits_size, sizeof(digit));
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
        #ifdef FREE_MEMORY
        delete res.digits;
        #endif
        res.digits = (digit*) calloc(res.digits_size, sizeof(digit));
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
        #ifdef FREE_MEMORY
        delete res.digits;
        #endif
        res.digits = (digit*) calloc(res.digits_size, sizeof(digit));
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
        ss << std::setprecision(14) << v;
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
            #ifdef FREE_MEMORY
            delete digits;
            #endif
            digits = (digit*) calloc(0, sizeof(digit*));
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
                digit* temp = (digit*) calloc(digits_size, sizeof(digit));
                if (!temp) memory_error();
                memcpy(temp, digits + left, digits_size * sizeof(digit));
                #ifdef FREE_MEMORY
                delete digits;
                #endif
                digits = temp;
            }
        }
            
    }

    void LongDouble::removeFirst(int value) {
        assert(digits_size >= value && value >= 0);
        digits_size = digits_size - value;
        digit* temp = (digit*) calloc(digits_size, sizeof(digit));
        if (!temp) memory_error();
        memcpy(temp, digits + value, digits_size * sizeof(digit));
        #ifdef FREE_MEMORY
        delete digits;
        #endif
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
        if (-exponent <= number) return;
        if (-exponent - number - 1 >= digits_size) {
            *this = 0;
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
            *this = 0;
            return;
        }
        removeFirst(-exponent - number);
        removeZeroes();
    }

    void LongDouble::floor() {
        floor(0);
    }

    void LongDouble::sqrt() { // use binary search
        assert(*this >= 0);
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

    void LongDouble::sqrt_int() { // work only for integers >= 1, use binary search

        LongDouble x(1, precision);
        x.mulBase(digits_size / 2 + 1);
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

        digit* res = (digit*) calloc(0, sizeof(digit));
        if (!res) memory_error();
        int res_size = 0;
        fft.multiply(digits, digits_size, x.digits, x.digits_size, res, res_size, base);
        digits = (digit*)calloc(0, sizeof(digit));
        if (!digits) memory_error();
        digits = res;
        digits_size = res_size;

        removeZeroes();
        if (digits_size > precision * 2) {
            removeFirst(digits_size - precision * 2);
        }
        removeZeroes();
    }

    LongDouble LongDouble::operator-() const {
        LongDouble res(*this);
        res.sign = -res.sign;
        return res;
    }

    bool LongDouble::operator<(const LongDouble& x) const {
        if (digits_size == 0) return x.sign == 1 && x.digits_size > 0;
        if (x.digits_size == 0) return sign == -1 && digits_size > 0;
        if (sign != x.sign) return sign < x.sign && (digits_size > 0 || x.digits_size > 0); 

        if (digits_size + exponent != x.digits_size + x.exponent)
            return (digits_size + exponent < x.digits_size + x.exponent) ^ (sign == -1);
        
        for (int i = 0; i < min(digits_size, x.digits_size); i++) {
            if (digits[digits_size - 1 - i] < x.digits[x.digits_size - 1 - i]) return sign == 1;
            if (digits[digits_size - 1 - i] > x.digits[x.digits_size - 1 - i]) return sign == -1;
        }
        return (digits_size < x.digits_size) ^ (sign == -1);
    }

    bool LongDouble::operator==(const LongDouble& x) const {
        if (sign != x.sign)
            return (digits_size == 0 && digits_size == 0); 
        if (digits_size != x.digits_size) return false;
        for (int i = 0; i < digits_size; i++) {
            if (digits[i] != x.digits[i]) return false;
        }
        return exponent == x.exponent;
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
            digit* resd = (digit*) calloc(res_size, sizeof(digit));
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
            // cout << *this << " " << x << endl;

        if (sign == 1 && x.sign == 1) {

            if (*this < x) {
                LongDouble res = x - *this;
                return -res;
            }

            LongDouble res;
            res.sign = 1;
            res.precision = precision;
            int res_size = max(digits_size + exponent, x.digits_size + x.exponent) - min(exponent, x.exponent);
            digit* resd = (digit*) calloc(res_size, sizeof(digit));
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

    LongDouble IntegerDivision(const LongDouble &x, const LongDouble &y) { // n * n * log^2(n)
        assert(x.isInt() && y.isInt());
        if (y == 0) throw exception();
        LongDouble b = y;
        LongDouble result = 0, current = 0;
        result.precision = x.precision;

        result.digits_size = x.digits_size;
        result.digits = (digit*) calloc(result.digits_size, sizeof(digit));
        if (!result.digits) memory_error();
        memset(result.digits, 0, result.digits_size * sizeof(digit));

        for (int i = x.digits_size - 1; i >= 0; i--) {
            current.mulBase(1);
            current += x.digits[i];
            int l = 0, r = LongDouble().base;
            while (r - l > 1) {
                int m = (l + r) / 2;
                LongDouble t = b * m;
                if (t <= current) l = m;
                else r = m;
            }
            result.digits[i] = l;
            current -= b * l;
        }
        result.sign = x.sign * y.sign;
        result.removeZeroes();

        return result;
    }

    void div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem);

    void div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res) { 
        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
        LongDouble x = a, y = b;
        x.precision = (int) 1e9;
        y.precision = (int) 1e9;

        LongDouble qwqw = x;
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
            first3m.digits = (digit*) calloc(first3m.digits_size, sizeof(digit));
            if (!first3m.digits) memory_error();
            memcpy(first3m.digits, x.digits + m, (first3m.digits_size) * sizeof(digit));
        } else {
            first3m.digits = (digit*) calloc(0, sizeof(digit));
            if (!first3m.digits) memory_error();
        }
        
        LongDouble res1(0, (int) 1e9), rem1(0, (int) 1e9);

        div32(first3m, y, m, res1, rem1);

        rem1.digits_size = rem1.digits_size + m;
        digit* temp = (digit*) calloc(rem1.digits_size, sizeof(digit));
        if (!temp) memory_error();
        memcpy(temp + m, rem1.digits, (rem1.digits_size - m) * sizeof(digit));
        #ifdef FREE_MEMORY
        delete rem1.digits;
        #endif
        rem1.digits = temp;
        memcpy(rem1.digits, x.digits, min(m, x.digits_size) * sizeof(digit));
        LongDouble res2(0, (int) 1e9), rem2(0, (int) 1e9);
        div32(rem1, y, m, res2, rem2);
        res1.digits_size = res1.digits_size + m;
        temp = (digit*) calloc(res1.digits_size, sizeof(digit));
        if (!temp) memory_error();
        memcpy(temp + m, res1.digits, (res1.digits_size - m) * sizeof(digit));

        #ifdef FREE_MEMORY
        delete res1.digits;
        #endif
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
            digit* temp = (digit*) calloc(rem.digits_size, sizeof(digit));
            if (!temp) memory_error();

            memcpy(temp + rem.exponent, rem.digits, (rem.digits_size - rem.exponent) * sizeof(digit));
            #ifdef FREE_MEMORY
            delete rem.digits;
            #endif
            rem.digits = temp;
            rem.exponent = 0;
            return;
        }

        if (y.digits_size <= n) {
            div21(x, y, n, res);
            rem = x - y * res;
            assert(rem >= 0);
            rem.digits_size += rem.exponent;
            digit* temp = (digit*) calloc(rem.digits_size, sizeof(digit));
            if (!temp) memory_error();

            memcpy(temp + rem.exponent, rem.digits, (rem.digits_size - rem.exponent) * sizeof(digit));
            #ifdef FREE_MEMORY
            delete rem.digits;
            #endif
            rem.digits = temp;
            rem.exponent = 0;
            assert(rem.sign == 1);
            return;
        }

        LongDouble x1(0, (int) 1e9); // first |x| - (|y| - n)
        x1.digits_size = max(0, x.digits_size - (y.digits_size - n));
        x1.digits = (digit*) calloc(x1.digits_size, sizeof(digit));
        if (!x1.digits) memory_error();

        memcpy(x1.digits, x.digits + (y.digits_size - n), (x1.digits_size) * sizeof(digit));

        LongDouble y1(0, (int) 1e9); // first n
        y1.digits_size = n;
        y1.digits = (digit*) calloc(y1.digits_size, sizeof(digit));
        if (!y1.digits) memory_error();

        memcpy(y1.digits, y.digits + (y.digits_size - n), (y1.digits_size) * sizeof(digit));

        // y *= 10 ^ n
        y1.digits_size += n;
        digit* temp = (digit*) calloc(y1.digits_size, sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp + n, y1.digits, (y1.digits_size - n) * sizeof(digit));
        #ifdef FREE_MEMORY
        delete y1.digits;
        #endif
        y1.digits = temp;

        bool maxres = x1 >= y1;

        // y /= 10 ^ n
        y1.digits_size -= n;
        temp = (digit*) calloc(y1.digits_size, sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp, y1.digits + n, y1.digits_size * sizeof(digit));
        #ifdef FREE_MEMORY
        delete y1.digits;
        #endif
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

        temp = (digit*) calloc(current_rem.digits_size + current_rem.exponent, sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp + current_rem.exponent, current_rem.digits, (current_rem.digits_size) * sizeof(digit));
        memset(temp, 0, (current_rem.exponent) * sizeof(digit));
        #ifdef FREE_MEMORY
        delete current_rem.digits;
        #endif
        current_rem.digits = temp;
        current_rem.digits_size += current_rem.exponent;
        current_rem.exponent = 0;

        temp = (digit*) calloc(res1.digits_size + res1.exponent, sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp + res1.exponent, res1.digits, (res1.digits_size) * sizeof(digit));
        memset(temp, 0, (res1.exponent) * sizeof(digit));
        #ifdef FREE_MEMORY
        delete res1.digits;
        #endif
        res1.digits = temp;
        res1.digits_size += res1.exponent;
        res1.exponent = 0;
 
        res = res1;
        rem = current_rem;
    }

    // LongDouble LongDouble::operator/(const LongDouble& other) const {
    //     LongDouble x (*this);
    //     x.sign = 1;
    //     LongDouble y (other);
    //     y.sign = 1;
    //     int res_exponent = exponent - other.exponent;
    //     x.exponent = 0;
    //     y.exponent = 0;

    //     int plus = precision - x.digits_size + y.digits_size + 2;

    //     int temp_size = plus + x.digits_size;
    //     digit* temp = (digit*) calloc(temp_size, sizeof(digit));
    //     if (!temp) memory_error();
    //     memset(temp, 0, plus * sizeof(digit));
    //     memcpy(temp + plus, x.digits, digits_size * sizeof(digit));

    //     x.digits = temp;
    //     x.digits_size = temp_size;

    //     res_exponent -= plus;

    //     LongDouble res = IntegerDivision(x, y);         
    //     res.precision = precision;
    //     res.exponent += res_exponent;
    //     res.sign = sign * other.sign;
    //     res.removeZeroes();
    //     if (res.digits_size > res.precision) {
    //         res.removeFirst(res.digits_size - res.precision);
    //     }
    //     res.removeZeroes();

    //     return res;
    // }

    LongDouble LongDouble::operator/(const LongDouble& other) const {
        LongDouble x (*this);
        x.sign = 1;
        LongDouble y (other);
        y.sign = 1;
        int res_exponent = exponent - other.exponent;
        x.exponent = 0;
        y.exponent = 0;

        int plus = precision - x.digits_size + y.digits_size + 2;

        int temp_size = plus + x.digits_size;
        digit* temp = (digit*) calloc(temp_size, sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, plus * sizeof(digit));
        memcpy(temp + plus, x.digits, digits_size * sizeof(digit));

        x.digits = temp;
        x.digits_size = temp_size;

        res_exponent -= plus;

        // LongDouble res = IntegerDivision(x, y);      
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

    template<class T>
    LongDouble LongDouble::operator+(const T& other) const {
        return *this + LongDouble(other);
    }

    template<class T>
    LongDouble LongDouble::operator-(const T& other) const { 
        return *this - LongDouble(other);
    }

    template<class T>
    void LongDouble::operator+=(const T& other) { 
        *this += LongDouble(other);
    }

    template<class T>
    void LongDouble::operator-=(const T& other) { 
        *this -= LongDouble(other);
    }

    template<class T>
    LongDouble LongDouble::operator*(const T& other) const { 
        return *this * LongDouble(other);
    }

    template<class T>
    LongDouble LongDouble::operator/(const T& other) const { 
        return *this / LongDouble(other);
    }

    template<class T>
    void LongDouble::operator*=(const T& other) { 
        *this *= LongDouble(other);
    }

    template<class T>
    void LongDouble::operator/=(const T& other) { 
        *this /= LongDouble(other);
    }

    template<class T>
    bool LongDouble::operator<(const T& x) const {
        return *this < LongDouble(x);
    }
    template<class T>
    bool LongDouble::operator<=(const T& x) const {
        return *this <= LongDouble(x);
    }
    template<class T>
    bool LongDouble::operator>(const T& x) const {
        return *this > LongDouble(x);
    }

    template<class T>
    bool LongDouble::operator>=(const T& x) const {
        return *this >= LongDouble(x);
    }

    template<class T>
    bool LongDouble::operator==(const T& x) const {
        return *this == LongDouble(x);
    }

    template<class T>
    bool LongDouble::operator!=(const T& x) const {
        return *this != LongDouble(x);
    }

    template<class T>
    LongDouble& LongDouble::operator=(const T& x) {
        return *this = LongDouble(x);
    }

};

#endif

