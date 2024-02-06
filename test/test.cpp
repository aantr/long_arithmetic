#include <defines.hpp>

#include <arithmetic.hpp>
#include <test_system.hpp>
#include <random>
#include <iomanip>

using namespace std;
using namespace arithmetic;
std::mt19937 rnd(7327158);

using namespace test_system;

// test_system define
#undef TEST_SYSTEM
#define TEST_SYSTEM ts

void add_tests(TestSystem &ts) {
	
	TEST_(Init)
	ASSERT(LongDouble("0.013459359706757822") < "0.0134594");
	ASSERT("+22.0"_ld == "22.");
	ASSERT((LongDouble((long long)1e18) == 1e18));
	ASSERT(LongDouble((int)1e9 - 1) == 1e9 - 1);
	ASSERT(LongDouble((double)1e18) == LongDouble((long double)1e18));
	LongDouble a(123), b = -0.00_ld, c = 0_ld, d = -123_ld, e("+0123.00"), k = "-0.0000";
	ASSERT(a == e);
	ASSERT(b == k);
	ASSERT(a != d);
	ASSERT(c == k);
	ASSERT(d == -a);
	ASSERT(d != a);
	ASSERT(LongDouble("1.0") == "1");
	_TEST


	TEST_(Inequality)

	LongDouble a(123), b, c, d = -123, e("0123"), k = string("0");
	ASSERT(1_ld < 2);
	ASSERT((a >= d) == true);
	ASSERT((b <= k) == true);
	ASSERT((d > a) == false);
	ASSERT((e < d) == false);
	ASSERT((d > c) == false);
	ASSERT((k < b) == false);
	ASSERT(("1"_ld < 123) == true);
	ASSERT((d < d + 1) == true);
	_TEST


	TEST_(Add)
	int count = 100;
	for (int i = 0; i < count; i++) {
		long long x = rnd(), y = rnd();
		ASSERT(LongDouble(x + y) == LongDouble(x) + LongDouble(y));
	}
	_TEST

	TEST_(Sub)
	
	int count = 100;
	for (int i = 0; i < count; i++) {
		long long x = (long long) rnd() - rnd(), y = (long long) rnd() - rnd();
		ASSERT(LongDouble(x - y) == LongDouble(x) - LongDouble(y));
	}
	_TEST

	TEST_(Round)
	LongDouble a = 1.5, b = 1.4, c = -1.4, d = -1.5, e = -1.6, n = 123.45678;
	a.round();
	b.round();
	c.round();
	d.round();
	e.round();
	ASSERT(a == 2);
	ASSERT(b == 1);
	ASSERT(c == -1);
	ASSERT(d == -2);
	ASSERT(e == -2);
	n.round(5);
	ASSERT(n == 123.45678);
	n.round(4);
	ASSERT(n == 123.4568);
	n.round(3);
	ASSERT(n == 123.457);
	n.round(2);
	ASSERT(n == 123.46);
	n.round(-1);
	ASSERT(n == 120);
	n.round(-2);
	ASSERT(n == 100);

	n = 3453345.44563636;
	n.floor(5);
	ASSERT(n == 3453345.44563);
	n.floor(-2);
	ASSERT(n == 3453300);

	n = LongDouble((double)123 / 13);
	n.round(1);
	ASSERT(n == 9.5);
	n = LongDouble((double)123 /  13);
	n.round(2);
	ASSERT(n == 9.46);

	for (int i = 1000; i < 1100; i++) {
		LongDouble x = i;
		x.divBase(2);
		x.round();
		ASSERT(x == (double) round((double) i / 100));
	}	
	_TEST

	TEST_(Mul)
	int count = 100;
	for (int i = 0; i < count; i++) {
		cout << setprecision(200);
		int precision = 18 / LongDouble::base_exp + 1;
		long long x = (long long) (rnd() - rnd()) % 1000000000, y = (long long) (rnd() - rnd()) % 1000000000;
		if (!(LongDouble(x * y) == LongDouble(x, precision) * LongDouble(y))) {
			cout << x << " " << y <<  " " << LongDouble(x, precision) * LongDouble(y)<< endl;	
			exit(0);
		}

		ASSERT(LongDouble(x * y) == LongDouble(x, precision) * LongDouble(y));

		LongDouble X(x, precision);
		X *= X;
		LongDouble Y(y, precision);
		Y *= x;

		ASSERT(X == LongDouble(x, precision) * LongDouble(x) && X == x * x);
		ASSERT(Y == LongDouble(y, precision) * LongDouble(x) && Y == x * y);
	}
	_TEST

	TEST_(Div)
	int count = 1000;
	for (int i = 0; i < count; i++) {
		int x = (long long) (rnd() - rnd()) % 1000000, y = (long long) (rnd()) % 1000000 + 1;

		LongDouble res = LongDouble(x) / LongDouble(y);
		res.floor();
		ASSERT(LongDouble(x / y) == res);

		LongDouble res_round = LongDouble(x) / LongDouble(y);
		res_round.round(5);
		ASSERT(res_round == (double)round((double)x / y * pow(10, 5)) / pow(10, 5));
	}
	_TEST

	TEST_(Sqrt)
	int count = 200;
	for (int i = 1; i < count; i++) {
		string str = to_string(i);
		LongDouble x(str, 16 / LongDouble::base_exp + 1);
		LongDouble xx(str, 16 / LongDouble::base_exp + 1);
		LongDouble s, r;
		sqrt_rem(x, s, r);
		ASSERT(s * s + r == x);
		ASSERT(r >= 0);
		ASSERT((s + 1) * (s + 1) > x);
		x.sqrt_fast();
		xx.sqrt_int();
		x.floor(10);
		xx.floor(10);
		ASSERT(x == xx);

	}
	count = 100;
	for (int i = 0; i < count; i++) {
		double x = rnd() % 100000000;
		x /= pow(10, rnd() % 8);
		LongDouble X ((double)x, 16 / LongDouble::base_exp + 1);
		X.sqrt();
		x = sqrtl(x);
		LongDouble st = 1;
		int ASSERT = 6;
		st.divBase(ASSERT);
		ASSERT((X - (double)x).abs() < st);
	}	
	_TEST

	TEST_(Precision)
	int count = 50;
	for (int i = 0; i < count; i++) {
		int len = 10; // тестируем числа с len цифрами
		LongDouble x(0, len * 2 / LongDouble::base_exp + 1), y(0, len * 2 / LongDouble::base_exp + 1);

		for (int j = 0; j < len; j++) {
			x = x * LongDouble(10) + LongDouble((int) (rnd() % 10));
			y = y * LongDouble(10) + LongDouble((int) (rnd() % 10));
		}

		int expx = rnd() % len + 1;
		x.divBase(expx);
		y.divBase(rnd() % len + 1);
		LongDouble power = 1; power.divBase(expx - 1); // допускаем последнюю цифру

		ASSERT((x - x / y * y).abs() < power);
		ASSERT((x - x * y / y).abs() < power);
		ASSERT((x - x / y / y * y * y).abs() < power * 10); // допуск 2 цифры
		ASSERT((x - x / y * y / y * y).abs() < power * 10);
	}

	_TEST
}


int main() {
	TestSystem ts;
	add_tests(ts);
	cout << setprecision(32);
	ts.run_testing(cout);
	cout << setprecision(6);

	std::cout << "Finished in " << (double) clock() / CLOCKS_PER_SEC << " sec\n";
	return 0;
};