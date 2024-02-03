#pragma once

#include <iostream>

#define PRECISION_ADD
#define PRECISION_SUB
#define MIN_PRECISION 1

/*

base_exp - how mush digits in 10 base we save in one digit

input string format:
[-+][0-9][.][0-9]

output format:
use_scientific_output - scientific / not
outputing max presicion digits in given stream

how precision works
precison >= MIN_PRECISION
always use floor in math
add/sub works only if defines
for ex:
cout.precision() == 3
=> 
123 * 12345 = 1518430
division:
cout.precision() == 3
123 / 45 = 2.73

assymptotics:

sub / add O(n)
mul O(n * log n) (fft)
div O(n * log ^ 2 (n)) ????????????
sqrt O(n * log ^ 2(n)) Karatsuba square root algorithm ??????????????????

*/

namespace arithmetic {  

    using namespace std;
    using digit = int64_t;

    class DivisionByZeroException {

    };

    class MemoryLimitException {

    };

    class InitError {

    };

    class InitPrecisonError {

    };

    class InitStringError {

    };

    class SqrtLimitError {

    };

    class SqrtIntLimitError {

    };

    class NegativePowerError {

    };

    void division_error();

    void memory_error();

    void init_error();

    void init_precison_error();

    void init_string_error();

    void sqrt_limir_error();

    void sqrt_int_limir_error();

    void negative_power_error();

    class LongDouble {
    public:

        int sign = 1;
        digit* digits = nullptr; 
        int digits_size = 0;
        static bool use_scientific_output;
        static bool output_insignificant_zeroes;
        static const int base = 1000;
        static const int base_exp = 3;
        static constexpr int pow_10[10] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 
                                            10000000, 100000000, 1000000000 };
        int precision = 32; // >= MIN_PRECISION
        int exponent = 0; // base 10
        static bool context_remove_left_zeroes;

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

        LongDouble operator-() const;
        bool operator<(const LongDouble& x) const;
        bool operator<=(const LongDouble& x) const;
        bool operator>(const LongDouble& x) const;
        bool operator>=(const LongDouble& x) const;
        bool operator==(const LongDouble& x) const;
        bool operator!=(const LongDouble& x) const;
        LongDouble& operator=(const LongDouble& x);
        
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
