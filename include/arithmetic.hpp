#pragma once

#include <iostream>

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
precison >= MIN_PRECISION
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
sqrt O(n * log ^ 2(n)) Karatsuba square root algorithm

*/

namespace arithmetic {  

    using namespace std;
    using digit = int32_t;

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

    class SqrtLimitError {
    public:
        const char* what () {
            return "Sqrt limit error (value must be >= 0)";
        }
    };

    class SqrtIntLimitError {
    public:
        const char* what () {
            return "Sqrt integer limit error (value must be >= 0 and value must be an integer)";
        }
    };

    class NegativePowerError {
    public:
        const char* what () {
            return "Power must be not negative integer";
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

    void sqrt_limir_error() {
        throw SqrtLimitError();
    }

    void sqrt_int_limir_error() {
        throw SqrtIntLimitError();
    }

    void negative_power_error() {
        throw NegativePowerError();
    }

    class LongDouble {
    public:

        int sign = 1;
        digit* digits = nullptr; 
        int digits_size = 0;
        int base = 10;
        int precision = 64; // >= MIN_PRECISION
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
        LongDouble(const long double &value); 
        LongDouble(const long double &value, int precision); 

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
        void sqrt_fast();
        void pow(int power);
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
        ~LongDouble();
    };

    istream& operator>>(istream& os, LongDouble& value);

    ostream& operator<<(ostream& os, const LongDouble& value);

    void div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem);

    void div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res);

    void sqrt_rem(const LongDouble n, LongDouble &s, LongDouble &r);

    LongDouble operator""_ld (const char* x, unsigned long size);

    LongDouble operator""_ld (long double x);

    LongDouble operator""_ld (unsigned long long x);
};
