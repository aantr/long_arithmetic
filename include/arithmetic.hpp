#ifndef ARITHMETIC_HPP
#define ARITHMETIC_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fft.hpp>
#include <assert.h>
#include <iomanip>

using namespace std;
using namespace fft;

#define FREE_MEMORY
// #define PRECISION_ADD
// #define PRECISION_SUB

namespace arithmetic {    

    FFT fft;

    void memory_error() {
        cerr << "Cannot allocate enough memory" << endl;
        exit(1);
    }

    void division_error() {
        cerr << "Division by zero" << endl;
        exit(1);
    }

    class LongDouble {
    public:

        int sign = 1;
        digit* digits = nullptr; 
        int digits_size = 0;
        int base = 10;
        int precision = 16; // >= 2
        int exponent = 0;

        LongDouble(); 
        LongDouble(const LongDouble& x); 
        LongDouble(const LongDouble& x, int precision); 
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
        void sqrt_int(int digits);
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

    LongDouble& LongDouble::operator=(const LongDouble& other) // C5267
    {
        sign = other.sign;
        digits_size = other.digits_size;
        #ifdef FREE_MEMORY
        delete digits;
        #endif
        digits = (digit*) calloc(other.digits_size, sizeof(digit));
        memcpy(digits, other.digits, other.digits_size * sizeof(digit));
        base = other.base;
        precision = other.precision;
        exponent = other.exponent;
        return *this;
    }

    LongDouble::LongDouble() {
        *this = 0;
    }

    LongDouble::LongDouble(const LongDouble& x) {
        *this = x;
    }

    LongDouble::LongDouble(const LongDouble& other, int precision): precision(precision) {
        sign = other.sign;
        digits_size = other.digits_size;
        #ifdef FREE_MEMORY
        delete digits;
        #endif
        digits = (digit*) calloc(other.digits_size, sizeof(digit));
        memcpy(digits, other.digits, other.digits_size * sizeof(digit));
        base = other.base;
        exponent = other.exponent;
    }

    template<class T>
    void init_from_string(LongDouble& res, const T& value) {
        int index;
        if (value[0] == '-') {
            res.sign = -1; 
            index = 1; 
        } else {
            res.sign = 1;
            index = 0;
        }
        res.exponent = 0; 
        res.digits_size = (int)value.size() - index;
        #ifdef FREE_MEMORY
        delete res.digits;
        #endif
        res.digits = (digit*) calloc(res.digits_size, sizeof(digit));
        int count = 0;
        while (index < (int)value.length()) {
            if (value[index] == '.') 
                res.exponent = -((int)value.length() - 1 - index); 
            else {
                res.digits[count] = value[index] - '0';
                count++;
            }
            index++;
        }
        res.digits = (digit*) realloc(res.digits, count * sizeof(digit));
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
        while (x) {
            res.digits = (digit*) realloc(res.digits, (res.digits_size + 1) * sizeof(digit));
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
        init_from_string(res, ss.str());
    }

    LongDouble::LongDouble(const string& value) {
        init_from_string(*this, value);
    }

    LongDouble::LongDouble(const string& value, int precision): precision(precision) {
        init_from_string(*this, value);
    }

    LongDouble::LongDouble(const int &v) {
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const int &v, int precision): precision(precision) {
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const long long &v) {
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const long long &v, int precision): precision(precision) {
        init_from_int(*this, v);
    }

    LongDouble::LongDouble(const double &v) {
        init_from_double(*this, v);
    }

    LongDouble::LongDouble(const double &v, int precision): precision(precision) {
        init_from_double(*this, v);
    }

    LongDouble LongDouble::abs() const {
        if (digits_size == 0) return 0;
        if (sign == -1) return -(*this);
        return *this;
    }

    bool LongDouble::isInt() const {
        return exponent == 0;
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
            digits_size = 0;
            sign = 1;
            exponent = 0;
        }
        else {
            if (left == 0) {
                digits_size = digits_size - right;
                digits = (digit*) realloc(digits, (digits_size) * sizeof(digit));
            } else {
                digits_size = digits_size - left - right;
                digit* temp = (digit*) calloc(digits_size, sizeof(digit));
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

    void LongDouble::sqrt_int(int digits) { // work only for integers >= 1, use binary search
        assert(isInt() && !isZero());
        LongDouble x = *this;
        x.mulBase(digits * 2);
        LongDouble l(0, (int) 1e9), r(x, (int) 1e9);
        int remove = (r.digits_size - 1) / 2;
        r.removeFirst(remove);
        r.divBase(remove);
        while (r - l > 1) {
            LongDouble m = (l + r) * 0.5;
            m.floor();
            if (m * m <= x) {
                l = m;
            } else {
                r = m;
            }
        }
        l.divBase(digits);
        *this = l;
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
        int res_size = 0;
        fft.multiply(digits, digits_size, x.digits, x.digits_size, res, res_size, base);
        digits = (digit*)calloc(0, sizeof(digit));
        digits = res;
        digits_size = res_size;

        removeZeroes();
        if (digits_size > precision) {
            removeFirst(digits_size - precision);
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

    void div32(const LongDouble &a, const LongDouble &b, LongDouble &res, LongDouble& rem);

    void div21(const LongDouble &a, const LongDouble &b, LongDouble &res) { // return not normalized
        assert(a.isInt() && b.isInt() && a.sign == 1 && b.sign == 1);
        if (b.isZero()) division_error();
        LongDouble x = a, y = b;
        x.removeZeroes();
        y.removeZeroes();
        int n = 1;
        while (x.digits_size > 2 * n || y.digits_size > n || x.digits_size - (y.digits_size - 1) > n) {
            n <<= 1;
        }
        if (n == 1) {
            if (b.digits[0] == 0) division_error();
            res = LongDouble(a.digits[0] / b.digits[0]);
            if (res.digits_size < 1) {
                res.digits_size = 1;
                res.digits = (digit*) calloc(1, sizeof(digit));
                memset(y.digits, 0, 1 * sizeof(digit));
            }
            return;
        }
        int m = n >> 1;
        int prev_size = x.digits_size;
        x.digits_size = 2 * n;
        x.digits = (digit*) realloc(x.digits, (x.digits_size) * sizeof(digit));
        memset(x.digits + prev_size, 0, (x.digits_size - prev_size) * sizeof(digit));
        prev_size = y.digits_size;
        y.digits_size = n;
        y.digits = (digit*) realloc(y.digits, (y.digits_size) * sizeof(digit));
        memset(y.digits + prev_size, 0, (y.digits_size - prev_size) * sizeof(digit));

        LongDouble first3m;
        first3m.digits_size = 3 * m;
        first3m.digits = (digit*) calloc(first3m.digits_size, sizeof(digit));
        memcpy(first3m.digits, x.digits + m, (first3m.digits_size) * sizeof(digit));

        LongDouble res1, rem1;
        div32(first3m, b, res1, rem1);

        rem1.digits_size = 3 * m;
        digit* temp = (digit*) calloc(3 * m, sizeof(digit));
        memcpy(temp + m, rem1.digits + m, 2 * m * sizeof(digit));
        #ifdef FREE_MEMORY
        delete rem1.digits;
        #endif
        rem1.digits = temp;

        memcpy(rem1.digits, x.digits, m * sizeof(digit));

        LongDouble res2, rem2;
        div32(rem1, b, res2, rem2);

        // shift res1 + m
        res1.digits_size = res1.digits_size + m;
        temp = (digit*) calloc(res1.digits_size, sizeof(digit));
        memcpy(temp + m, res1.digits, (res1.digits_size - m) * sizeof(digit));

        #ifdef FREE_MEMORY
        delete res1.digits;
        #endif
        res1.digits = temp;

        memcpy(res1.digits, res2.digits, res.digits_size * sizeof(digit)); // add m digits

        res = res1;
    }

    void div32(const LongDouble &a, const LongDouble &b, LongDouble &res, LongDouble& rem) { // return normalized
        assert(a.isInt() && b.isInt() && a.sign == 1 && b.sign == 1);
        if (b.isZero()) division_error();
        LongDouble x = a, y = b;
        x.removeZeroes();
        y.removeZeroes();
        int n = 1;
        while (x.digits_size > 3 * n || y.digits_size > 2 * n || x.digits_size - (y.digits_size - 1) > n) {
            n <<= 1;
        }
        if (n == 1) {
            int A = 0, B = 0;
            for (int i = x.digits_size - 1; i >= 0; i++) {
                A = A * x.base + x.digits[i]; 
            }
            for (int i = y.digits_size - 1; i >= 0; i--) {
                B = B * y.base + y.digits[i];
            }
            if (B == 0) division_error();
            res = LongDouble(A / B);
            rem = LongDouble(A % B);
            return;
        }

        if (y.digits_size <= n) {
            x.digits_size = 2 * n;
            x.digits = (digit*) realloc(x.digits, (x.digits_size) * sizeof(digit));
            y.digits_size = n;
            y.digits = (digit*) realloc(y.digits, (y.digits_size) * sizeof(digit));
            div21(x, y, res);
            res.removeZeroes();
            rem = res - res * b;
            return;
        }

        // cut on blocks
        int prev_size = x.digits_size;
        x.digits_size = 3 * n;
        x.digits = (digit*) realloc(x.digits, (x.digits_size) * sizeof(digit));
        memset(x.digits + prev_size, 0, (x.digits_size - prev_size) * sizeof(digit));

        prev_size = y.digits_size;
        y.digits_size = 2 * n;
        y.digits = (digit*) realloc(y.digits, (y.digits_size) * sizeof(digit));
        memset(y.digits + prev_size, 0, (y.digits_size - prev_size) * sizeof(digit));

        LongDouble y1; // first n
        y1.digits_size = n;
        y1.digits = (digit*) calloc(y1.digits_size, sizeof(digit));
        memcpy(y1.digits, y.digits + 2 * n, (y1.digits_size) * sizeof(digit));

        LongDouble x1; // first |x| - (|y| - n)
        x1.digits_size = 2 * n;
        x1.digits = (digit*) calloc(x1.digits_size, sizeof(digit));
        memcpy(x1.digits, x.digits + n, (x1.digits_size) * sizeof(digit));

        // some statementsr
        // x1/y1 >= x/y
        // x1 / (y1 + 1) <= x/y
        // x1/y1 - 10 <= x1 / (y1 + 1)
        // x1 / (y1 + 1) <= x/y <= x1/y1
        // x1/y1-10 <= x/y <= x1/y1

        LongDouble res1;
        div21(x1, y1, res1);
        res1.removeZeroes();
        LongDouble current_rem = a - res1 * b;
        while (current_rem < 0) {
            current_rem += b;
            res1 -= 1;
        }

        res = res1;
        rem = current_rem;

    }

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

        LongDouble res = IntegerDivision(x, y);         
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
        const int max_out_exponent = 16;
        if (value.digits_size > max_out_exponent || (value.exponent > 0 && value.digits_size + value.exponent > max_out_exponent)|| 
                    (value.exponent <= -value.digits_size && -value.exponent + 1 > max_out_exponent)) {
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

};

#endif

