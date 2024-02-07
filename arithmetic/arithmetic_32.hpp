#pragma once

#include <iostream>

#define PRECISION_ADD
#define PRECISION_SUB
#define MIN_PRECISION 1

namespace arithmetic_32 {  

    using namespace std;
    using digit = uint32_t;

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

        
        static bool use_scientific_output;
        static bool output_insignificant_zeroes;
        static bool context_remove_left_zeroes;
        static int default_precision;
        static const uint64_t base;
        static digit ones[33];

        int sign;
        digit* digits; 
        int digits_size;
        int precision; // >= MIN_PRECISION
        int exponent; // base 10

        LongDouble(); 
        LongDouble(const LongDouble& x); 
        LongDouble(const LongDouble& x, int precision); 
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

        LongDouble operator<<(unsigned int val) const;
        void operator<<=(unsigned int val);
        LongDouble operator>>(unsigned int val) const;
        void operator>>=(unsigned int val);
        LongDouble getFirstBits(unsigned int val) const;


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

    ostream& operator<<(ostream& os, const LongDouble& value);

    void div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem);

    void div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res);

    void sqrt_rem(const LongDouble n, LongDouble &s, LongDouble &r);

    LongDouble operator""_ld (long double x);

    LongDouble operator""_ld (unsigned long long x);
};
