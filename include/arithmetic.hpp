#ifndef ARITHMETIC_HPP
#define ARITHMETIC_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fft.hpp>
#include <assert.h>
using namespace std;

namespace arithmetic {

    FFT fft;

    class LongDouble {
    public:

        int sign = 1;
        vector<digit> digits; 
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
        void removeZeroes(); 
        void removeFirst(int value);
        void mulBase(int power);
        void divBase(int power);
        void round(int number);
        void round();
        void floor(int number);
        void floor();
        void sqrt();
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
        digits = other.digits;
        base = other.base;
        precision = other.precision;
        exponent = other.exponent;
        return *this;
    }

    LongDouble::LongDouble() {
        *this = 0;
    }

    LongDouble::LongDouble(const LongDouble& x) {
        sign = x.sign; 
        exponent = x.exponent; 
        precision = x.precision;
        digits = vector<digit>(x.digits);
    }

    LongDouble::LongDouble(const LongDouble& x, int precision): precision(precision) {
        sign = x.sign; 
        exponent = x.exponent; 
        precision = x.precision;
        digits = vector<digit>(x.digits);
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
        while (index < (int)value.length()) {
            if (value[index] == '.') 
                res.exponent = -((int)value.length() - 1 - index); 
            else {
                res.digits.push_back(value[index] - '0');
            }
            index++;
        }
        reverse(res.digits.begin(), res.digits.end());
        res.removeZeroes();
        if ((int) res.digits.size() == 0) res.sign = 1;
    }

    template<class T>
    void init_from_int(LongDouble& res, T value) {
        T x = value;
        if (x < 0) res.sign = -1, x = -x;
        while (x) {
            res.digits.push_back(x % res.base);
            x /= res.base;
        }
        res.exponent = 0;
        res.removeZeroes();
    }

    template<class T>
    void init_from_double(LongDouble& res, T& v) {
        stringstream ss;
        ss << setprecision(14) << v;
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

    void LongDouble::removeZeroes() {
        int left = 0, right = 0;
         for (int i = (int)digits.size() - 1; i >= 0; i--) {
            if (digits[i] == 0) {
                right++;
            } else {
                break;
            }
        }
        for (int i = 0; i < (int)digits.size(); i++) {
            if (digits[i] == 0) {
                exponent++;
                left++;
            } else {
                break;
            }
        }
        if (left == (int) digits.size()) {
            digits.clear();
            sign = 1;
            exponent = 0;
        }
        else {
            if (left == 0) {
                for (int i = 0; i < right; i++) {
                    digits.pop_back();
                }
            } else {
                vector<digit> temp {digits.begin() + left, digits.end() - right};
                digits = temp;
            }
        }
            
    }

    void LongDouble::removeFirst(int value) {
        assert((int) digits.size() >= value && value >= 0);
        vector<digit> temp {digits.begin() + value, digits.end()};
        digits = temp;
        exponent += value;
    }

    istream& operator>>(istream& os, LongDouble& value) {
        string s;
        os >> s;
        value = LongDouble(s);
        return os;
    }

    ostream& operator<<(ostream& os, const LongDouble& value) {
        if (value.sign == -1)
            os << '-';
        if ((int)value.digits.size() == 0) {
            os << '0';
            return os;
        }
        if ((int)value.digits.size() > 16 || (value.exponent > 0 && (int)value.digits.size() + value.exponent > 16)|| 
                    (value.exponent <= -(int)value.digits.size() && -value.exponent + 1 > 16)) {
            for (int i = (int)value.digits.size() - 1; i >= 0; i--) {
                os << value.digits[i];
                if ((int) value.digits.size() > 1 && i == (int)value.digits.size() - 1) os << '.';
            }
            int val = value.exponent + (int)value.digits.size() - 1;
            if (val > 0) os << 'e' << '+' << val;
            else if (val < 0) os << 'e' << val;
        } else {
            for (int i = -value.exponent; i > (int)value.digits.size() - 1; i--) {
                os << '0';
                if (i == -value.exponent) os << '.';
            }
            for (int i = (int)value.digits.size() - 1; i >= 0; i--) {
                os << value.digits[i];
                if (i > 0 && i == -value.exponent) os << '.';
            }
            for (int i = 0; i < value.exponent; i++) {
                os << '0';
            }
        }
        return os;
    }

    void LongDouble::mulBase(int power) {
        exponent += power;
    }

    void LongDouble::divBase(int power) {
        exponent -= power;
    }

    void LongDouble::round(int number) {
        if (-exponent <= number) return;
        if (-exponent - number - 1 >= (int)digits.size()) {
            *this = 0;
            return;
        }
        removeFirst(-exponent - number - 1);
        bool was = true;
        assert((int) digits.size() >= 1);
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
        if (-exponent - number >= (int)digits.size()) {
            *this = 0;
            return;
        }
        removeFirst(-exponent - number);
        removeZeroes();
    }

    void LongDouble::floor() {
        floor(0);
    }

    void LongDouble::sqrt() { // bs
        assert(sign == 1);
        LongDouble l, r = *this;
        l.precision = precision;
        r.precision = precision;
        LongDouble prev = -1;
        while (1) {
            LongDouble m = (l + r) * 0.5;
            if ((int) m.digits.size() > m.precision) m.removeFirst((int) m.digits.size() - m.precision);
            if (m * m <= *this) {
                l = m;
            } else {
                r = m;
            }
            if (prev == m) break;
            prev = m;
        }
        if ((int) l.digits.size() > l.precision) l.removeFirst((int) l.digits.size() - l.precision);
        *this = l;
    }

    LongDouble LongDouble::abs() const {
        if ((int) digits.size() == 0) return 0;
        if (sign == -1) return -(*this);
        return *this;
    }

    LongDouble LongDouble::operator*(const LongDouble& x) const {
        LongDouble res = (*this);
        res *= x;
        return res;
    }

    void LongDouble::operator*=(const LongDouble& x) {
        sign = sign * x.sign;
        exponent = exponent + x.exponent;
        digits = fft.multiply(digits, x.digits, base);
        removeZeroes();
        if ((int)digits.size() > precision * 2) {
            removeFirst((int)digits.size() - precision * 2);
        }
        removeZeroes();
    }

    LongDouble LongDouble::operator-() const {
        LongDouble res(*this);
        res.sign = -res.sign;
        return res;
    }

    bool LongDouble::operator<(const LongDouble& x) const {
        if ((int)digits.size() == 0) return x.sign == 1 && (int) x.digits.size() > 0;
        if ((int)x.digits.size() == 0) return x.sign == -1 && (int) digits.size() > 0;
        if (sign != x.sign) return sign < x.sign && ((int) digits.size() > 0 || (int) x.digits.size() > 0); 

        if ((int) digits.size() + exponent != (int)x.digits.size() + x.exponent)
            return ((int) digits.size() + exponent < (int)x.digits.size() + x.exponent) ^ (sign == -1);
        
        for (int i = 0; i < min((int) digits.size(), (int) x.digits.size()); i++) {
            if (digits[digits.size() - 1 - i] < x.digits[x.digits.size() - 1 - i]) return true;
            if (digits[digits.size() - 1 - i] > x.digits[x.digits.size() - 1 - i]) return false;
        }
        return digits.size() < x.digits.size();
    }

    bool LongDouble::operator==(const LongDouble& x) const {
        if (sign != x.sign)
            return ((int) digits.size() == 0 && (int) x.digits.size() == 0); 
        return exponent == x.exponent && digits == x.digits;
    }

    bool LongDouble::operator!=(const LongDouble& x) const {
        if (sign != x.sign)
            return ((int) digits.size() > 0 || (int) x.digits.size() > 0); 
        return exponent != x.exponent || digits != x.digits;
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
            vector<digit> resd(max((int)digits.size() + exponent, (int)x.digits.size() + x.exponent) - min(exponent, x.exponent));
            if (exponent <= x.exponent) {
                res.exponent = exponent;
                for (int i = 0; i < (int) digits.size(); i++) {
                    resd[i] += digits[i];
                }
                for (int i = 0; i < (int) x.digits.size(); i++) {
                    resd[i + x.exponent - exponent] += x.digits[i];
                }
            } else {
                res.exponent = x.exponent;
                for (int i = 0; i < (int) x.digits.size(); i++) {
                    resd[i] += x.digits[i];
                }
                for (int i = 0; i < (int) digits.size(); i++) {
                    resd[i + exponent - x.exponent] += digits[i];
                }
            }
            int carry = 0;
            for (int i = 0; i < (int) resd.size(); i++) {
                resd[i] += carry;
                carry = 0;
                if (resd[i] >= base) resd[i] -= base, carry = 1;
            }
            if (carry) resd.push_back(1);
            res.digits = resd;
            res.removeZeroes();
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
            // cout << *this << " " << x << endl;

            LongDouble res;
            res.sign = 1;
            res.precision = precision;
            vector<digit> resd(max((int)digits.size() + exponent, (int)x.digits.size() + x.exponent) - min(exponent, x.exponent));
            if (exponent <= x.exponent) {
                res.exponent = exponent;
                for (int i = 0; i < (int) digits.size(); i++) {
                    resd[i] += digits[i];
                }
                for (int i = 0; i < (int) x.digits.size(); i++) {
                    resd[i + x.exponent - exponent] -= x.digits[i];
                }
            } else {
                res.exponent = x.exponent;
                for (int i = 0; i < (int) x.digits.size(); i++) {
                    resd[i] -= x.digits[i];
                }
                for (int i = 0; i < (int) digits.size(); i++) {
                    resd[i + exponent - x.exponent] += digits[i];
                }
            }

            int carry = 0;
            for (int i = 0; i < (int) resd.size(); i++) {
                resd[i] -= carry;
                carry = 0;
                if (resd[i] < 0) resd[i] += base, carry = 1;
            }
            assert(carry == 0);
            res.digits = resd;
            res.removeZeroes();
            if ((int) res.digits.size() == 0) res.sign = 1;
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
        assert(x.exponent == 0 && y.exponent == 0);
        if (y == LongDouble(0)) throw exception();
        LongDouble b = y;
        LongDouble result(0), current(0);
        result.precision = x.precision;
        result.digits.resize((int) x.digits.size());
        for (int i = (int) (x.digits.size()) - 1; i >= 0; i--) {
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

    LongDouble LongDouble::operator/(const LongDouble& other) const {
        LongDouble x (*this);
        x.sign = 1;
        LongDouble y (other);
        y.sign = 1;
        int res_exponent = exponent - other.exponent;
        x.exponent = 0;
        y.exponent = 0;

        int plus = precision - (int)digits.size() + (int)x.digits.size() + 2;
        vector<digit> temp(plus + (int)x.digits.size(), 0);
        for (int i = 0; i < (int) x.digits.size(); i++) {
            temp[plus + i] = x.digits[i];
        }
        x.digits = temp;
        res_exponent -= plus;
        LongDouble res = IntegerDivision(x, y);

        res.precision = precision;
        res.exponent += res_exponent;
        res.sign = sign * other.sign;
        res.removeZeroes();
        if ((int)res.digits.size() - res.precision > 0) res.removeFirst((int)res.digits.size() - res.precision);

        return res;
    }

    void LongDouble::operator/=(const LongDouble& other) { 
        *this = *this / other;
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

