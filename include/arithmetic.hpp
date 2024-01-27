#ifndef ARITHMETIC_HPP
#define ARITHMETIC_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fft.hpp>
#include <assert.h>
using namespace std;

FFT fft;

class LongDouble {
public:
    int sign = 1;
    vector<digit> digits; 
    int base = 10;
    int precision = -1;
    int mantissa = 0;

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
    mantissa = other.mantissa;
    return *this;
}

LongDouble::LongDouble() {
    sign = 1; 
    digits = vector<digit>(0, 0);
    precision = -1;
    mantissa = 0;
}

LongDouble::LongDouble(const LongDouble& x) {
    sign = x.sign; 
    mantissa = x.mantissa; 
    precision = x.precision;
    digits = vector<digit>(x.digits);
}

LongDouble::LongDouble(const LongDouble& x, int precision): precision(precision) {
    sign = x.sign; 
    mantissa = x.mantissa; 
    precision = x.precision;
    digits = vector<digit>(x.digits);
}

LongDouble::LongDouble(const string& value) {
    int index;
    if (value[0] == '-') {
        sign = -1; 
        index = 1; 
    } else {
        sign = 1;
        index = 0;
    }
    mantissa = 0; 
    while (index < (int)value.length()) {
        if (value[index] == '.') 
            mantissa = value.length() - 1 - index; 
        else {
            digits.push_back(value[index] - '0');
        }

        index++;
    }
    reverse(digits.begin(), digits.end());
    removeZeroes();
}

LongDouble::LongDouble(const string& value, int precision): precision(precision) {
    int index;
    if (value[0] == '-') {
        sign = -1; 
        index = 1; 
    } else {
        sign = 1;
        index = 0;
    }
    mantissa = 0; 
    while (index < (int)value.length()) {
        if (value[index] == '.') 
            mantissa = value.length() - 1 - index; 
        else {
            digits.push_back(value[index] - '0');
        }
        index++;
    }
    reverse(digits.begin(), digits.end());
    removeZeroes();
}

LongDouble::LongDouble(const int &v) {
    int x = v;
    if (x < 0) sign = -1, x = -x;
    while (x) {
        digits.push_back(x % base);
        x /= base;
    }
    mantissa = 0;
}

LongDouble::LongDouble(const int &v, int precision): precision(precision) {
    int x = v;
    if (x < 0) sign = -1, x = -x;
    while (x) {
        digits.push_back(x % base);
        x /= base;
    }
    mantissa = 0;
}

LongDouble::LongDouble(const long long &v) {
    long long x = v;
    if (x < 0) sign = -1, x = -x;
    while (x) {
        digits.push_back(x % base);
        x /= base;
    }
    mantissa = 0;
}

LongDouble::LongDouble(const long long &v, int precision): precision(precision) {
    long long x = v;
    if (x < 0) sign = -1, x = -x;
    while (x) {
        digits.push_back(x % base);
        x /= base;
    }
    mantissa = 0;
}

LongDouble::LongDouble(const double &v) {
    string value = to_string(v);
    int index;
    if (value[0] == '-') {
        sign = -1; 
        index = 1; 
    } else {
        sign = 1;
        index = 0;
    }
    mantissa = 0; 
    while (index < (int)value.length()) {
        if (value[index] == '.') 
            mantissa = value.length() - 1 - index; 
        else {
            digits.push_back(value[index] - '0');
        }
        index++;
    }
    reverse(digits.begin(), digits.end());
    removeZeroes();
}

LongDouble::LongDouble(const double &v, int precision): precision(precision) {
    string value = to_string(v);
    int index;
    if (value[0] == '-') {
        sign = -1; 
        index = 1; 
    } else {
        sign = 1;
        index = 0;
    }
    mantissa = 0; 
    while (index < (int)value.length()) {
        if (value[index] == '.') 
            mantissa = value.length() - 1 - index; 
        else {
            digits.push_back(value[index] - '0');
        }
        index++;
    }
    reverse(digits.begin(), digits.end());
    removeZeroes();
}

void LongDouble::removeZeroes() {
    int left = 0, right = 0;
     for (int i = (int)digits.size() - 1; i >= 0; i--) {
        if (digits[i] == 0 && mantissa < (int) digits.size() - right) {
            right++;
        } else {
            break;
        }
    }
    for (int i = 0; i < (int)digits.size(); i++) {
        if (digits[i] == 0 && mantissa > 0) {
            mantissa--;
            left++;
        } else {
            break;
        }
    }
    vector<digit> temp {digits.begin() + left, digits.end() - right};

    digits = temp;
}

void LongDouble::removeFirst(int value) {
    assert((int) digits.size() >= value);
    vector<digit> temp {digits.begin() + value, digits.end()};
    digits = temp;
    mantissa -= value;
    if (mantissa < 0) mantissa = 0;
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
    if (value.mantissa == (int) value.digits.size()) {
        os << '0';
    }
    for (int i = 0; i < (int)value.digits.size(); i++) {
        if (i == (int) value.digits.size() - value.mantissa) os << '.';
        os << value.digits[(int) value.digits.size() - 1 - i];
    }
    return os;
}

void LongDouble::mulBase(int power) {
    int m = min(mantissa, power);
    mantissa -= m;
    power -= m;
    vector<digit> dig(power, 0);
    for (digit i : digits) dig.push_back(i);
    digits = dig;
    removeZeroes();
}

void LongDouble::divBase(int power) {
    int m = min((int) digits.size() - mantissa, power);
    mantissa += m;
    power -= m;
    for (int i = 0; i < power; i++) {
        mantissa++;
        digits.push_back(0);
    }
    removeZeroes();
}

LongDouble LongDouble::operator*(const LongDouble& x) const {
    LongDouble res = (*this);
    res *= x;
    return res;
}


void LongDouble::operator*=(const LongDouble& x) {
    sign = sign * x.sign;
    mantissa = mantissa + x.mantissa;
    digits = fft.multiply(digits, x.digits, base);
    if (precision != -1 && mantissa > precision) {
        removeFirst(mantissa - precision);
    }
    while ((int) digits.size() < mantissa) {
        digits.push_back(0);
    }
    removeZeroes();
    if (digits.empty()) sign = 1;
}

LongDouble LongDouble::operator-() const {
    LongDouble res;
    res.precision = precision;
    res.sign = -sign; 
    res.mantissa = mantissa; 
    res.digits = vector<digit>(digits);
    return res;
}

bool LongDouble::operator<(const LongDouble& x) const {
    if (sign != x.sign)
        return sign < x.sign && ((int) digits.size() > 0 || (int) x.digits.size() > 0); 

    if ((int) digits.size() - mantissa != (int)x.digits.size() - x.mantissa)
        return ((int) digits.size() - mantissa < (int)x.digits.size() - x.mantissa) ^ (sign == -1);
    
    for (int i = 0; i < min((int) digits.size(), (int) x.digits.size()); i++) {
        if (digits[digits.size() - 1 - i] < x.digits[x.digits.size() - 1 - i]) return true;
        if (digits[digits.size() - 1 - i] > x.digits[x.digits.size() - 1 - i]) return false;
    }
    if (x.digits.size() > digits.size()) return true;
    return false;
}

bool LongDouble::operator==(const LongDouble& x) const {
    if (sign != x.sign)
        return ((int) digits.size() == 0 && (int) x.digits.size() == 0); 
    return mantissa == x.mantissa && digits == x.digits;
}

bool LongDouble::operator!=(const LongDouble& x) const {
    if (sign != x.sign)
        return ((int) digits.size() > 0 || (int) x.digits.size() > 0); 
    return mantissa != x.mantissa || digits != x.digits;
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
        res.mantissa = max(mantissa, x.mantissa);
        vector<digit> resd(max(mantissa, x.mantissa) + max((int)digits.size() - mantissa, (int)x.digits.size() - x.mantissa));
        for (int i = 0; i < (int) digits.size(); i++) {
            resd[res.mantissa + i - mantissa] += digits[i];
        }
        for (int i = 0; i < (int) x.digits.size(); i++) {
            resd[res.mantissa + i - x.mantissa] += x.digits[i];
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
            res.sign = -res.sign;
            return res;
        }


        LongDouble res;
        res.sign = 1;
        res.precision = precision;
        res.mantissa = max(mantissa, x.mantissa);
        vector<digit> resd(max(mantissa, x.mantissa) + max((int)digits.size() - mantissa, (int)x.digits.size() - x.mantissa));
        for (int i = 0; i < (int) digits.size(); i++) {
            resd[res.mantissa + i - mantissa] += digits[i];
        }
        for (int i = 0; i < (int) x.digits.size(); i++) {
            resd[res.mantissa + i - x.mantissa] -= x.digits[i];
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
    assert(x.mantissa == 0 && y.mantissa == 0);
    if (y == LongDouble(0)) throw exception();
    LongDouble b = y;
    LongDouble result(0), current(0);
    result.precision = x.precision;
    result.digits.resize((int) x.digits.size());
    for (int i = (int) (x.digits.size()) - 1; i >= 0; i--) {
        current.mulBase(1);
        if ((int) current.digits.size() == 0) {
            current.digits.push_back(x.digits[i]);
        } else {
            current.digits[0] = x.digits[i];
        }
        current.removeZeroes();
        int l = 0, r = LongDouble().base;
        while (r - l > 1) {
            int m = (l + r) / 2;
            LongDouble t = b * LongDouble(m);
            if (t < current || t == current) {
                l = m;
            }
            else r = m;
        }
        result.digits[i] = l;
        current = current - LongDouble(l) * b;
    }
    result.sign = x.sign * y.sign;
    result.removeZeroes();
    return result;
}

LongDouble LongDouble::operator/(const LongDouble& other) const { // окргуление последнего знака вниз
    LongDouble x (*this);
    x.sign = 1;
    LongDouble y (other);
    y.sign = 1;

    if (y.mantissa > x.mantissa) {
        int temp = x.mantissa;
        x.mantissa = 0;
        y.mantissa -= temp;
        x.mulBase(y.mantissa);
        y.mantissa = 0; 
    } else {
        x.mantissa -= y.mantissa;
        y.mantissa = 0;
        y.mulBase(x.mantissa);
        x.mantissa = 0;
    }
    int plus = max(16, mantissa); // минимальная точность при делении 16 зн после запятой
    if (precision != -1) {
        plus = precision;
    }
    x.mulBase(plus);
    LongDouble res = IntegerDivision(x, y);
    res.precision = precision;
    res.mantissa = plus;
    while (res.mantissa > (int) res.digits.size()) res.digits.push_back(0);
    res.removeZeroes();
    res.sign = sign * other.sign;
    return res;
}

void LongDouble::operator/=(const LongDouble& other) { // окргуление последнего знака вниз
    *this = *this / other;
}

template<class T>
LongDouble LongDouble::operator+(const T& other) const { // окргуление последнего знака вниз
    return *this + LongDouble(other);
}

template<class T>
LongDouble LongDouble::operator-(const T& other) const { // окргуление последнего знака вниз
    return *this - LongDouble(other);
}

template<class T>
void LongDouble::operator+=(const T& other) { // окргуление последнего знака вниз
    *this += LongDouble(other);
}

template<class T>
void LongDouble::operator-=(const T& other) { // окргуление последнего знака вниз
    *this -= LongDouble(other);
}

template<class T>
LongDouble LongDouble::operator*(const T& other) const { // окргуление последнего знака вниз
    return *this * LongDouble(other);
}

template<class T>
LongDouble LongDouble::operator/(const T& other) const { // окргуление последнего знака вниз
    return *this / LongDouble(other);
}

template<class T>
void LongDouble::operator*=(const T& other) { // окргуление последнего знака вниз
    *this *= LongDouble(other);
}

template<class T>
void LongDouble::operator/=(const T& other) { // окргуление последнего знака вниз
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


#endif
