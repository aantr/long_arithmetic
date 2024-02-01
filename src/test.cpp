#include <arithmetic.hpp>
#include <test_system.hpp>
#include <random>

using namespace std;
using namespace arithmetic;
std::mt19937 rnd(7327158);


#define check(x) if (!(x)) {throw #x;}
#define TEST_(x) ts.add_test(Test(string(#x), []() -> bool {
#define _TEST return 0;}));

void add_tests(TestSystem &ts) {
	
	TEST_(Init)
	check((LongDouble((long long)1e18) == 1e18));
	check(LongDouble((int)1e9 - 1) == 1e9 - 1);
	check(LongDouble((double)1e18) == LongDouble((long double)1e18));
	LongDouble a(123), b = -0.00_ld, c = 0_ld, d = -123_ld, e("+0123.00"), k = "-0.0000";
	check("+22.0"_ld == "22.");
	check(a == e);
	check(b == k);
	check(a != d);
	check(c == k);
	check(d == -a);
	check(d != a);
	_TEST


	TEST_(Inequality)
	LongDouble a(123), b, c, d = -123, e("0123"), k = string("0");
	check((a >= d) == true);
	check((b <= k) == true);
	check((d > a) == false);
	check((e < d) == false);
	check((d > c) == false);
	check((k < b) == false);
	check((d < d + 1) == true);
	_TEST


	TEST_(Add)
	int count = 100;
	for (int i = 0; i < count; i++) {
		long long x = rnd(), y = rnd();
		check(LongDouble(x + y) == LongDouble(x) + LongDouble(y));
	}
	return 0;
	_TEST


	TEST_(Sub)
	int count = 100;
	for (int i = 0; i < count; i++) {
		long long x = (long long) rnd() - rnd(), y = (long long) rnd() - rnd();
		check(LongDouble(x - y) == LongDouble(x) - LongDouble(y));
	}
	_TEST

	TEST_(Round)
	LongDouble a = 1.5, b = 1.4, c = -1.4, d = -1.5, e = -1.6, n = 123.45678;
	a.round();
	b.round();
	c.round();
	d.round();
	e.round();
	check(a == 2);
	check(b == 1);
	check(c == -1);
	check(d == -2);
	check(e == -2);
	n.round(5);
	check(n == 123.45678);
	n.round(4);
	check(n == 123.4568);
	n.round(3);
	check(n == 123.457);
	n.round(2);
	check(n == 123.46);
	n.round(-1);
	check(n == 120);
	n.round(-2);
	check(n == 100);

	n = 3453345.44563636;
	n.floor(5);
	check(n == 3453345.44563);
	n.floor(-2);
	check(n == 3453300);

	n = LongDouble((double)123 / 13);
	n.round(1);
	check(n == 9.5);
	n = LongDouble((double)123 /  13);
	n.round(2);
	check(n == 9.46);	
	_TEST


	TEST_(Mul)
	int count = 100;
	for (int i = 0; i < count; i++) {
		int x = (long long) (rnd() - rnd()) % 10000, y = (long long) (rnd() - rnd()) % 10000;
		check(LongDouble(x * y) == LongDouble(x) * LongDouble(y));

		LongDouble X = x;
		X *= X;
		LongDouble Y = y;
		Y *= x;
		check(X == LongDouble(x) * LongDouble(x) && X == x * x);
		check(Y == LongDouble(y) * LongDouble(x) && Y == x * y);
	}
	_TEST

	TEST_(Div)
	int count = 100;
	for (int i = 0; i < count; i++) {
		int x = (long long) rnd() - rnd(), y = ((long long) rnd() - rnd()) / 100;

		LongDouble res = LongDouble(x) / LongDouble(y);
		res.floor();
		check(LongDouble(x / y) == res);

		LongDouble res_round = LongDouble(x) / LongDouble(y);
		res_round.round(5);
		check(res_round == (double)round((double)x / y * pow(10, 5)) / pow(10, 5));
	}
	_TEST

	TEST_(Sqrt)
	int count = 200;
	for (int i = 1; i < count; i++) {
		string str = to_string(i);

		LongDouble x(str, 16);
		LongDouble xx(str, 16);

		LongDouble s, r;
		sqrt_rem(x, s, r);
		check(s * s + r == x);
		check(r >= 0);
		check((s + 1) * (s + 1) > x);
		x.sqrt_fast();
		xx.sqrt_int();
		check(x == xx);

	}

	count = 100;
	for (int i = 0; i < count; i++) {
		double x = rnd() % 1000000000;
		x /= pow(10, rnd() % 5);
		LongDouble X ((double)x, 16);
		X.sqrt();

		x = sqrt(x);
		LongDouble st = 1;
		int check = 6;
		st.divBase(check);
		check((X - (double)x).abs() < st);
	}
	_TEST

	TEST_(Precision)

	check(LongDouble(123, 3) / 45 == 2.73);
	check(LongDouble(123, 3) * 12345 == 1518430);
	int count = 50;
	for (int i = 0; i < count; i++) {
		int len = 50; // тестируем числа с len цифрами
		LongDouble x(0, len * 2), y(0, len * 2);

		for (int j = 0; j < len; j++) {
			x = x * LongDouble(10) + LongDouble((int) (rnd() % 10));
			y = y * LongDouble(10) + LongDouble((int) (rnd() % 10));
		}

		int expx = rnd() % len + 1;
		x.divBase(expx);
		y.divBase(rnd() % len + 1);
		LongDouble power = 1; power.divBase(expx - 1); // допускаем последнюю цифру

		check((x - x / y * y).abs() < power);
		check((x - x * y / y).abs() < power);
		check((x - x / y / y * y * y).abs() < power * 10); // допуск 2 цифры
		check((x - x / y * y / y * y).abs() < power * 10);
	}

	_TEST


}


int main() {
	TestSystem ts;
	add_tests(ts);
	ts.run_testing();

	std::cout << "Finished in " << (double) clock() / CLOCKS_PER_SEC << " sec\n";
	return 0;
};