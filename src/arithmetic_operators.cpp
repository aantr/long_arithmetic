#include <defines.hpp>

#include <iostream>
#include <cstring>
#include <string>
#include <assert.h>
#include <iomanip>
#include <arithmetic.hpp>
#include <fft.hpp>

namespace arithmetic {

    using namespace std;

    fft::FFT fft;

    istream& operator>>(istream& os, LongDouble& value) {
        string s;
        os >> s;
        value = s;
        return os;
    }

    ostream& operator<<(ostream& os, const LongDouble& value) {
        if (value.sign == -1)
            os << '-';
        if (value.digits_size == 0) {
            os << '0';
            return os;
        }

        int digits_size_10 = value.digits_size * value.base_exp;
        while (digits_size_10 > 0 && value.digits[(digits_size_10 - 1) / value.base_exp] / value.pow_10[(digits_size_10 - 1) % value.base_exp] % 10 == 0) {
            digits_size_10--;
        }
        int left = max(0, digits_size_10 - (int)os.precision());
        while (LongDouble::output_insignificant_zeroes == false && left < digits_size_10 && value.digits[left / value.base_exp] / value.pow_10[left % value.base_exp] % 10 == 0) {
            left++;
        }

        if (value.use_scientific_output) {
            for (int i = digits_size_10 - 1; i >= left; i--) {
                os << value.digits[i / value.base_exp] / value.pow_10[i % value.base_exp] % 10;
                if (digits_size_10 - left > 1 && i == digits_size_10 - 1) os << '.'; // output first digit with . if >= 2
            }
            int val = value.exponent + digits_size_10 - 1; // than we can print exponent
            if (val > 0) os << 'e' << '+' << val;
            else if (val < 0) os << 'e' << val;
        } else {
            for (int i = -value.exponent; i > digits_size_10 - 1; i--) {
                os << '0';
                if (i == -value.exponent) os << '.';
            }
            for (int i = digits_size_10 - 1; i >= left; i--) {
                os << value.digits[i / value.base_exp] / value.pow_10[i % value.base_exp] % 10;
                if (i > left && i == -value.exponent) os << '.';
            }
            for (int i = 0; i < value.exponent + left; i++) {
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
        precision = other.precision;
        exponent = other.exponent;
        return *this;
    }

    LongDouble operator""_ld (const char* x, unsigned long size) {
        LongDouble res(x);
        res.precision = max((unsigned long) res.precision, (size - 1) / LongDouble::base_exp + 1);
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

    LongDouble LongDouble::operator*(const LongDouble& x) const {
        LongDouble res = *this;
        res *= x;
        return res;
    }

    void LongDouble::operator*=(const LongDouble& other) {

        LongDouble x = other;

        sign = sign * x.sign;
        exponent = exponent + x.exponent;

        fft::digit* res = (fft::digit*) malloc(0 * sizeof(fft::digit));
        if (!res) memory_error();

        int res_size = 0;
        // cut on a half blocks
        auto cut = [](digit* arr, int size) -> pair<fft::digit*, int> {
            if (base_exp == 1 || size == 0) {
                fft::digit* res = (fft::digit*) malloc(size * sizeof(fft::digit));
                memcpy(res, arr, size * sizeof(fft::digit));
                return {res, size};
            }
            assert(base_exp % 2 == 0);
            int newsize = size * 2;
            int k1 = base_exp / 2;
            fft::digit* res = (fft::digit*) malloc(newsize * sizeof(fft::digit));
            for (int i = 0; i < size; i++) {
                res[i * 2] = arr[i] % pow_10[k1];
                res[i * 2 + 1] = arr[i] / pow_10[k1];
            }
            return {res, newsize};
        };
        auto merge = [](fft::digit* arr, int size) -> pair<digit*, int> {
            if (base_exp == 1 || size == 0) {
                digit* res = (digit*) malloc(size * sizeof(digit));
                for (int i = 0; i < size; i++) {
                    res[i] = arr[i];
                }
                return {res, size};
            }
            assert(base_exp % 2 == 0);
            int newsize = (size - 1) / 2 + 1;
            int k1 = base_exp / 2;
            digit* res = (digit*) malloc(newsize * sizeof(digit));
            for (int i = 0; i < size; i++) {
                if (i % 2 == 0) res[i / 2] = arr[i];
                else res[i / 2] += (digit) arr[i] * pow_10[k1];
            }
            return {res, newsize};
        };
        auto [first, first_size] = cut(digits, digits_size);
        free(digits);

        auto [second, second_size] = cut(x.digits, x.digits_size);
        fft.multiply(first, first_size, second, second_size, res, res_size, base / pow_10[base_exp / 2]);
        free(first);
        free(second);
        auto [newres, newressize] = merge(res, res_size);
        free(res);

        digits = newres;
        digits_size = newressize;
        removeZeroes();
        if (digits_size > precision) {
            removeFirst(digits_size - precision);
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
        if (sign != x.sign) {
            return sign < x.sign && (!isZero() || !x.isZero());
        }


        assert(digits[digits_size - 1] != 0 && x.digits[x.digits_size - 1] != 0);

        int digits_size_10 = digits_size * base_exp;
        while (digits_size_10 > 0 && digits[(digits_size_10 - 1) / base_exp] / pow_10[(digits_size_10 - 1) % base_exp] % 10 == 0) {
            digits_size_10--;
        }

        int x_digits_size_10 = x.digits_size * x.base_exp;
        while (digits_size_10 > 0 && x.digits[(x_digits_size_10 - 1) / x.base_exp] / x.pow_10[(x_digits_size_10 - 1) % x.base_exp] % 10 == 0) {
            x_digits_size_10--;
        }

        if (digits_size_10 + exponent != x_digits_size_10 + x.exponent) {
            return (digits_size_10 + exponent < x_digits_size_10 + x.exponent) ^ (sign == -1);
        }

        int exp_mod = digits_size * base_exp - digits_size_10 - (x.digits_size * x.base_exp - x_digits_size_10);

        if (exp_mod >= 0) {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                int next_d = (digits_size - 1 - i - 1 >= 0 ? digits[digits_size - 1 - i - 1] / pow_10[base_exp - (exp_mod)] : 0);
                if (digits[digits_size - 1 - i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d > x.digits[x.digits_size - 1 - i]) return false ^ (sign == -1);                
                if (digits[digits_size - 1 - i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d < x.digits[x.digits_size - 1 - i]) return true ^ (sign == -1);                
            }
        } else {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                int next_d = (x.digits_size - 1 - i - 1 >= 0 ? x.digits[x.digits_size - 1 - i - 1] / pow_10[base_exp - (-exp_mod)] : 0);
                if (digits[digits_size - 1 - i] > x.digits[x.digits_size - 1 - i] % pow_10[base_exp - (-exp_mod)] * pow_10[-exp_mod] + next_d) return false ^ (sign == -1);
                if (digits[digits_size - 1 - i] < x.digits[x.digits_size - 1 - i] % pow_10[base_exp - (-exp_mod)] * pow_10[-exp_mod] + next_d) return true ^ (sign == -1);
            }   
        }
        for (int i = min(digits_size, x.digits_size); i < digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && exp_mod > 0) {
                if (digits[digits_size - 1 - i] % pow_10[base_exp - exp_mod] != 0) {
                    return false ^ (sign == -1);
                }
            } else {
                if (digits[digits_size - 1 - i] != 0) {
                    return false ^ (sign == -1);
                }
            }
        }
        for (int i = min(digits_size, x.digits_size); i < x.digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && -(exp_mod) > 0) {
                if (x.digits[x.digits_size - 1 - i] % pow_10[base_exp - (-exp_mod)] != 0) {
                    return true ^ (sign == -1);
                }
            } else {
                if (x.digits[x.digits_size - 1 - i] != 0) {
                    return true ^ (sign == -1);
                }
            }
        }
        return false;
    }

    bool LongDouble::operator==(const LongDouble& x) const {
        if (sign != x.sign) {
            return (isZero() && x.isZero()); 
        }
        if (isZero() || x.isZero()) {
            return isZero() && x.isZero();
        }

        assert(digits[digits_size - 1] != 0 && x.digits[x.digits_size - 1] != 0);
        int digits_size_10 = digits_size * base_exp;
        while (digits_size_10 > 0 && digits[(digits_size_10 - 1) / base_exp] / pow_10[(digits_size_10 - 1) % base_exp] % 10 == 0) {
            digits_size_10--;
        }

        int x_digits_size_10 = x.digits_size * x.base_exp;
        while (digits_size_10 > 0 && x.digits[(x_digits_size_10 - 1) / x.base_exp] / x.pow_10[(x_digits_size_10 - 1) % x.base_exp] % 10 == 0) {
            x_digits_size_10--;
        }

        if (digits_size_10 + exponent != x_digits_size_10 + x.exponent) {
            return false;
        }

        int exp_mod = digits_size * base_exp - digits_size_10 - (x.digits_size * x.base_exp - x_digits_size_10);

        if (exp_mod >= 0) {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                int next_d = (digits_size - 1 - i - 1 >= 0 ? digits[digits_size - 1 - i - 1] / pow_10[base_exp - (exp_mod)] : 0);
                if (digits[digits_size - 1 - i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d != x.digits[x.digits_size - 1 - i]) return false;                
            }
        } else {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                int next_d = (x.digits_size - 1 - i - 1 >= 0 ? x.digits[x.digits_size - 1 - i - 1] / pow_10[base_exp - (-exp_mod)] : 0);
                if (digits[digits_size - 1 - i] != x.digits[x.digits_size - 1 - i] % pow_10[base_exp - (-exp_mod)] * pow_10[-exp_mod] + next_d) return false;
            }   
        }
        for (int i = min(digits_size, x.digits_size); i < digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && exp_mod > 0) {
                if (digits[digits_size - 1 - i] % pow_10[base_exp - exp_mod] != 0) {
                    return false;
                }
            } else {
                if (digits[digits_size - 1 - i] != 0) {
                    return false;
                }
            }
        }
        for (int i = min(digits_size, x.digits_size); i < x.digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && -(exp_mod) > 0) {
                if (x.digits[x.digits_size - 1 - i] % pow_10[base_exp - (-exp_mod)] != 0) {
                    return false;
                }
            } else {
                if (x.digits[x.digits_size - 1 - i] != 0) {
                    return false;
                }
            }
        }
        return true;
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

            int exp_div = exponent / base_exp, exp_mod = exponent % base_exp;
            if (exp_mod < 0) exp_mod += base_exp, exp_div--;
            int x_exp_div = x.exponent / base_exp, x_exp_mod = x.exponent % base_exp;
            if (x_exp_mod < 0) x_exp_mod += base_exp, x_exp_div--;


            int res_size = max(digits_size + exp_div, x.digits_size + x_exp_div) - min(exp_div, x_exp_div) + 1;
            digit* resd = (digit*) malloc(res_size * sizeof(digit));
            if (!resd) memory_error();
            memset(resd, 0, res_size * sizeof(digit));

            if (exp_div <= x_exp_div) {
                res.exponent = exp_div * base_exp;
                for (int i = 0; i < digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? digits[i - 1] / pow_10[base_exp - (exp_mod)] : 0);
                    resd[i] += digits[i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d; // + exp_mod
                }
                int next_d = (digits_size - 1 >= 0 ? digits[digits_size - 1] / pow_10[base_exp - (exp_mod)] : 0);
                resd[digits_size] += next_d;
                for (int i = 0; i < x.digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? x.digits[i - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                    resd[i + x_exp_div - exp_div] += x.digits[i] % pow_10[base_exp - x_exp_mod] * pow_10[x_exp_mod] + next_d;
                }
                next_d = (x.digits_size - 1 >= 0 ? x.digits[x.digits_size - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                resd[x.digits_size + x_exp_div - exp_div] += next_d;


            } else {
                res.exponent = x_exp_div * base_exp;
                for (int i = 0; i < x.digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? x.digits[i - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                    resd[i] += x.digits[i] % pow_10[base_exp - x_exp_mod] * pow_10[x_exp_mod] + next_d;
                }
                int next_d = (x.digits_size - 1 >= 0 ? x.digits[x.digits_size - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                resd[x.digits_size] += next_d;
                for (int i = 0; i < digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? digits[i - 1] / pow_10[base_exp - (exp_mod)] : 0);
                    resd[i + exp_div - x_exp_div] += digits[i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d;
                }
                next_d = (digits_size - 1 >= 0 ? digits[digits_size - 1] / pow_10[base_exp - (exp_mod)] : 0);
                resd[digits_size + exp_div - x_exp_div] += next_d;
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

            int exp_div = exponent / base_exp, exp_mod = exponent % base_exp;
            if (exp_mod < 0) exp_mod += base_exp, exp_div--;
            int x_exp_div = x.exponent / base_exp, x_exp_mod = x.exponent % base_exp;
            if (x_exp_mod < 0) x_exp_mod += base_exp, x_exp_div--;

            int res_size = max(digits_size + exp_div, x.digits_size + x_exp_div) - min(exp_div, x_exp_div) + 1;
            digit* resd = (digit*) malloc(res_size * sizeof(digit));
            if (!resd) memory_error();
            memset(resd, 0, res_size * sizeof(digit));

            if (exp_div <= x_exp_div) {
                res.exponent = exp_div * base_exp;
                for (int i = 0; i < digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? digits[i - 1] / pow_10[base_exp - (exp_mod)] : 0);
                    resd[i] += digits[i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d; // + exp_mod
                }
                int next_d = (digits_size - 1 >= 0 ? digits[digits_size - 1] / pow_10[base_exp - (exp_mod)] : 0);
                resd[digits_size] += next_d;

                for (int i = 0; i < x.digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? x.digits[i - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                    resd[i + x_exp_div - exp_div] -= x.digits[i] % pow_10[base_exp - x_exp_mod] * pow_10[x_exp_mod] + next_d;
                }
                next_d = (x.digits_size - 1 >= 0 ? x.digits[x.digits_size - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                resd[x.digits_size + x_exp_div - exp_div] -= next_d;


            } else {
                res.exponent = x_exp_div * base_exp;
                for (int i = 0; i < x.digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? x.digits[i - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                    resd[i] -= x.digits[i] % pow_10[base_exp - x_exp_mod] * pow_10[x_exp_mod] + next_d;
                }
                int next_d = (x.digits_size - 1 >= 0 ? x.digits[x.digits_size - 1] / pow_10[base_exp - (x_exp_mod)] : 0);
                resd[x.digits_size] -= next_d;
                for (int i = 0; i < digits_size; i++) {
                    int next_d = (i - 1 >= 0 ? digits[i - 1] / pow_10[base_exp - (exp_mod)] : 0);
                    resd[i + exp_div - x_exp_div] += digits[i] % pow_10[base_exp - exp_mod] * pow_10[exp_mod] + next_d;
                }
                next_d = (digits_size - 1 >= 0 ? digits[digits_size - 1] / pow_10[base_exp - (exp_mod)] : 0);
                resd[digits_size + exp_div - x_exp_div] += next_d;
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
        assert(x.isZero() || x.digits[x.digits_size - 1] != 0);
        assert(other.isZero() || other.digits[other.digits_size - 1] != 0);

        x.sign = 1;
        LongDouble y (other);
        y.sign = 1;
        int res_exponent = x.exponent - y.exponent;
        x.exponent = 0;
        y.exponent = 0;
        int plus = max(0, precision - x.digits_size + y.digits_size + 2);


        if (!x.isZero()) {
            int temp_size = plus + x.digits_size;
            digit* temp = (digit*) malloc(temp_size * sizeof(digit));
            if (!temp) memory_error();
            memset(temp, 0, plus * sizeof(digit));
            memcpy(temp + plus, x.digits, x.digits_size * sizeof(digit));
            free(x.digits);
            x.digits = temp;
            x.digits_size = temp_size;
        }


        res_exponent -= plus * base_exp;
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

};