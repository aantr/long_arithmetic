#include <defines.hpp>

#include <gtest/gtest.h>
#include <arithmetic.hpp>
#include <arithmetic_32.hpp>
#include <random>
#include <iomanip>

using namespace std;
using namespace arithmetic;
std::mt19937 rnd(7327158);

using ld = arithmetic_32::LongDouble;

	
TEST(LongDouble, Init) {
	ASSERT_TRUE(LongDouble("0.013459359706757822") < "0.0134594");
	ASSERT_TRUE("+22.0"_ld == "22.");
	ASSERT_TRUE((LongDouble((long long)1e18) == 1e18));
	ASSERT_TRUE(LongDouble((int)1e9 - 1) == 1e9 - 1);
	ASSERT_TRUE(LongDouble((double)1e18) == LongDouble((long double)1e18));
	LongDouble a(123), b = -0.00_ld, c = 0_ld, d = -123_ld, e("+0123.00"), k = "-0.0000";
	ASSERT_TRUE(a == e);
	ASSERT_TRUE(b == k);
	ASSERT_TRUE(a != d);
	ASSERT_TRUE(c == k);
	ASSERT_TRUE(d == -a);
	ASSERT_TRUE(d != a);
	ASSERT_TRUE(LongDouble("1.0") == "1");
}


TEST(LongDouble, Inequality) {
	LongDouble a(123), b, c, d = -123, e("0123"), k = string("0");
	ASSERT_TRUE(1_ld < 2);
	ASSERT_TRUE((a >= d) == true);
	ASSERT_TRUE((b <= k) == true);
	ASSERT_TRUE((d > a) == false);
	ASSERT_TRUE((e < d) == false);
	ASSERT_TRUE((d > c) == false);
	ASSERT_TRUE((k < b) == false);
	ASSERT_TRUE(("1"_ld < 123) == true);
	ASSERT_TRUE((d < d + 1) == true);
}


TEST(LongDouble, Add) {
	int count = 100;
	for (int i = 0; i < count; i++) {
		long long x = rnd(), y = rnd();
		ASSERT_TRUE(LongDouble(x + y) == LongDouble(x) + LongDouble(y));
	}
}

TEST(LongDouble, Sub) {
	int count = 100;
	for (int i = 0; i < count; i++) {
		long long x = (long long) rnd() - rnd(), y = (long long) rnd() - rnd();
		ASSERT_TRUE(LongDouble(x - y) == LongDouble(x) - LongDouble(y));
	}
}

TEST(LongDouble, Round) {
	LongDouble a = 1.5, b = 1.4, c = -1.4, d = -1.5, e = -1.6, n = 123.45678;
	a.round();
	b.round();
	c.round();
	d.round();
	e.round();
	ASSERT_TRUE(a == 2);
	ASSERT_TRUE(b == 1);
	ASSERT_TRUE(c == -1);
	ASSERT_TRUE(d == -2);
	ASSERT_TRUE(e == -2);
	n.round(5);
	ASSERT_TRUE(n == 123.45678);
	n.round(4);
	ASSERT_TRUE(n == 123.4568);
	n.round(3);
	ASSERT_TRUE(n == 123.457);
	n.round(2);
	ASSERT_TRUE(n == 123.46);
	n.round(-1);
	ASSERT_TRUE(n == 120);
	n.round(-2);
	ASSERT_TRUE(n == 100);

	n = 3453345.44563636;
	n.floor(5);
	ASSERT_TRUE(n == 3453345.44563);
	n.floor(-2);
	ASSERT_TRUE(n == 3453300);

	n = LongDouble((double)123 / 13);
	n.round(1);
	ASSERT_TRUE(n == 9.5);
	n = LongDouble((double)123 /  13);
	n.round(2);
	ASSERT_TRUE(n == 9.46);

	for (int i = 1000; i < 1100; i++) {
		LongDouble x = i;
		x.divBase(2);
		x.round();
		ASSERT_TRUE(x == (double) round((double) i / 100));
	}	
}

TEST(LongDouble, Mul) {
	int count = 100;
	for (int i = 0; i < count; i++) {
		cout << setprecision(200);
		int precision = 18 / LongDouble::base_exp + 1;
		long long x = (long long) (rnd() - rnd()) % 1000000000, y = (long long) (rnd() - rnd()) % 1000000000;
		if (!(LongDouble(x * y) == LongDouble(x, precision) * LongDouble(y))) {
			cout << x << " " << y <<  " " << LongDouble(x, precision) * LongDouble(y)<< endl;	
			exit(0);
		}

		ASSERT_TRUE(LongDouble(x * y) == LongDouble(x, precision) * LongDouble(y));

		LongDouble X(x, precision);
		X *= X;
		LongDouble Y(y, precision);
		Y *= x;

		ASSERT_TRUE(X == LongDouble(x, precision) * LongDouble(x) && X == x * x);
		ASSERT_TRUE(Y == LongDouble(y, precision) * LongDouble(x) && Y == x * y);
	}
}

TEST(LongDouble, Div) {
	int count = 1000;
	for (int i = 0; i < count; i++) {
		int x = (long long) (rnd() - rnd()) % 1000000, y = (long long) (rnd()) % 1000000 + 1;

		LongDouble res = LongDouble(x) / LongDouble(y);
		res.floor();
		ASSERT_TRUE(LongDouble(x / y) == res);

		LongDouble res_round = LongDouble(x) / LongDouble(y);
		res_round.round(5);
		ASSERT_TRUE(res_round == (double)round((double)x / y * pow(10, 5)) / pow(10, 5));
	}
}

TEST(LongDouble, Sqrt) {
	int count = 200;
	for (int i = 1; i < count; i++) {
		string str = to_string(i);
		LongDouble x(str, 16 / LongDouble::base_exp + 1);
		LongDouble xx(str, 16 / LongDouble::base_exp + 1);
		LongDouble s, r;
		sqrt_rem(x, s, r);
		ASSERT_TRUE(s * s + r == x);
		ASSERT_TRUE(r >= 0);
		ASSERT_TRUE((s + 1) * (s + 1) > x);
		x.sqrt_fast();
		xx.sqrt_int();
		x.floor(10);
		xx.floor(10);
		ASSERT_TRUE(x == xx);

	}
	count = 100;
	for (int i = 0; i < count; i++) {
		double x = rnd() % 100000000;
		x /= pow(10, rnd() % 8);
		LongDouble X ((double)x, 16 / LongDouble::base_exp + 1);
		X.sqrt();
		x = sqrtl(x);
		LongDouble st = 1;
		int ASSERT_TRUE = 6;
		st.divBase(ASSERT_TRUE);
		ASSERT_TRUE((X - (double)x).abs() < st);
	}		
}

TEST(LongDouble, Precision) {
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

		ASSERT_TRUE((x - x / y * y).abs() < power);
		ASSERT_TRUE((x - x * y / y).abs() < power);
		ASSERT_TRUE((x - x / y / y * y * y).abs() < power * 10); // допуск 2 цифры
		ASSERT_TRUE((x - x / y * y / y * y).abs() < power * 10);
	}
}

TEST(LongDouble32, Inequality) {
	ld a(123.1), b, c, d = -123.1, e = 123.0, k = 0.0;
	ASSERT_TRUE(1_ld < 2);
	ASSERT_TRUE((a >= d) == true) << a << " " << a.sign << "  " << d << " " << d.sign;
	ASSERT_TRUE((b <= k) == true);
	ASSERT_TRUE((d > a) == false);
	ASSERT_TRUE((e < d) == false);
	ASSERT_TRUE((d > c) == false);
	ASSERT_TRUE((k < b) == false);
	ASSERT_TRUE((d < d + 1) == true) << "d: " << d << "d + 1: " << d + 1;
}


TEST(LongDouble32, Add) {
	int count = 1000;
	for (int i = 0; i < count; i++) {
		long long x = rnd(), y = rnd();
		ASSERT_TRUE(ld(x + y) == ld(x) + ld(y)) << "test: " << x << " " << y << " "<< ld(x) + ld(y);
	}
}

TEST(LongDouble32, Sub) {
	int count = 1000;
	for (int i = 0; i < count; i++) {
		long long x = (long long) rnd() - rnd(), y = (long long) rnd() - rnd();
		cout << "test: " << x << " " << y << " "<< ld(x) - ld(y) << endl;
		ASSERT_TRUE(ld(x - y) == ld(x) - ld(y)) << "test: " << x << " " << y << " "<< ld(x) - ld(y);;
	}
}


TEST(LongDouble32, Mul) {
	int count = 100;
	for (int i = 0; i < count; i++) {
		cout << setprecision(200);
		int precision = 10;
		long long x = (long long) (rnd() - rnd()) % 1000000000, y = (long long) (rnd() - rnd()) % 1000000000;
		if (!(ld(x * y) == ld(x, precision) * ld(y))) {
			cout << x << " " << y <<  " " << ld(x, precision) * ld(y) << endl;	
			exit(0);
		}

		ASSERT_TRUE(ld(x * y) == ld(x, precision) * ld(y));

		ld X(x, precision);
		X *= X;
		ld Y(y, precision);
		Y *= x;

		ASSERT_TRUE(X == ld(x, precision) * ld(x) && X == x * x);
		ASSERT_TRUE(Y == ld(y, precision) * ld(x) && Y == x * y);
	}
}

TEST(LongDouble32, Div) {
	int count = 1000;
	for (int i = 0; i < count; i++) {
		int x = (long long) (rnd() - rnd()) % 1000000, y = (long long) (rnd()) % 1000000 + 1;

		ld res = ld(x) / ld(y);
		res.floor();
		ASSERT_TRUE(ld(x / y) == res) << "val: " << ld(x / y) << " " << res;

		ld res_round = ld(x) / ld(y);
		res_round.round(5);
		ASSERT_TRUE(res_round == (double)round((double)x / y * pow(2, 5)) / pow(2, 5)) << "test: " << x << " " << y << " " << ld((double)round((double)x / y * pow(2, 5)) / pow(2, 5)) << " "<<res_round;
	}
}

TEST(LongDouble32, Sqrt) {

	int count = 200;
	for (int i = 1; i < count; i++) {
		ld x(i, 4);
		ld xx(i, 4);
		ld xxx(i, 4);
		ld s, r;
		arithmetic_32::sqrt_rem(x, s, r);

		ASSERT_TRUE(s * s + r == x);
		ASSERT_TRUE(r >= 0);
		ASSERT_TRUE((s + 1) * (s + 1) > x);
		
		x.sqrt_fast();
		xx.sqrt();
		// xxx.sqrt();

		x.floor(20);
		xx.floor(20);
		xxx.floor(20);

		EXPECT_TRUE(x == xx) << "test: \n" << x << "\n" << xx << endl;
		// EXPECT_TRUE(xxx == xx) << "test: \n" << x << "\n" << xx << endl;
		ASSERT_TRUE(x == xx) << "test: \n" << x << "\n" << xx << endl;


	}
}


// TEST(LongDouble32, Precision) {
// 	int count = 50;
// 	for (int i = 0; i < count; i++) {
// 		int len = 10; // тестируем числа с len цифрами
// 		LongDouble x(0, len * 2 / LongDouble::base_exp + 1), y(0, len * 2 / LongDouble::base_exp + 1);

// 		for (int j = 0; j < len; j++) {
// 			x = x * LongDouble(10) + LongDouble((int) (rnd() % 10));
// 			y = y * LongDouble(10) + LongDouble((int) (rnd() % 10));
// 		}

// 		int expx = rnd() % len + 1;
// 		x.divBase(expx);
// 		y.divBase(rnd() % len + 1);
// 		LongDouble power = 1; power.divBase(expx - 1); // допускаем последнюю цифру

// 		ASSERT_TRUE((x - x / y * y).abs() < power);
// 		ASSERT_TRUE((x - x * y / y).abs() < power);
// 		ASSERT_TRUE((x - x / y / y * y * y).abs() < power * 10); // допуск 2 цифры
// 		ASSERT_TRUE((x - x / y * y / y * y).abs() < power * 10);
// 	}
// }



