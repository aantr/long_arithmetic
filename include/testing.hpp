#include <arithmetic.hpp>
#include <assert.h>
#include <bits/stdc++.h>

using namespace std;

std::mt19937 rnd(7327158);

namespace testing {
	class Test {
	public:
		string name = "Empty test";
		void test() {};
		void _test() {cout << "Testing " + name + "...\n";};
	};

	class TestInit : public Test {
	public:
		TestInit() {
			name = "TestInit";
		}
		void test() {
			_test();
			LongDouble a(123), b(0), c, d(-123), e("0123"), k("0");
			assert(a == e);
			assert(b == k);
			assert(a != d);
			assert(c == k);
			assert(d == -a);
			assert(d != a);
		}
	};

	class TestAddition : public Test {
	public:
		TestAddition() {
			name = "TestAddition";
		}
		void test() {
			_test();
			int count = 100;
			for (int i = 0; i < count; i++) {
				long long x = rnd(), y = rnd();
				assert(LongDouble(x + y) == LongDouble(x) + LongDouble(y));
			}
		}
	};

	class TestSubstracion : public Test {
	public:
		TestSubstracion() {
			name = "TestSubstracion";
		}
		void test() {
			_test();
			int count = 100;
			for (int i = 0; i < count; i++) {
				long long x = (long long) rnd() - rnd(), y = (long long) rnd() - rnd();
				assert(LongDouble(x - y) == LongDouble(x) - LongDouble(y));
			}
		}
	};

	class TestMult : public Test {
	public:
		TestMult() {
			name = "TestMult";
		}
		void test() {
			_test();
			int count = 100;
			for (int i = 0; i < count; i++) {
				int x = (long long) (rnd() - rnd()) % 10000, y = (long long) (rnd() - rnd()) % 10000;
				assert(LongDouble(x * y) == LongDouble(x) * LongDouble(y));
			}
		}
	};

	class TestDiv : public Test {
	public:
		TestDiv() {
			name = "TestDiv";
		}
		void test() {
			_test();
			int count = 100;
			for (int i = 0; i < count; i++) {
				int x = (long long) rnd() - rnd(), y = (long long) rnd() - rnd();
				assert(LongDouble(round((double)x / y)) == LongDouble(x, 0) / LongDouble(y));
			}
		}
	};

	class TestPrecision : public Test {
	public:
		TestPrecision() {
			name = "TestPrecision";
		}
		void test() {
			_test();
			int count = 100;
			for (int i = 0; i < count; i++) {
				int len = 30; // тестируем числа с len цифрами
				LongDouble x(0, len + 1), y;
				for (int j = 0; j < len; j++) {
					x = x * LongDouble(10) + LongDouble((int) (rnd() % 9) + 1);
					y = y * LongDouble(10) + LongDouble((int) (rnd() % 9) + 1);
				}
				x.mantissa = rnd() % (len / 2);
				y.mantissa = rnd() % (len / 2);
				LongDouble power = string("1"); power.divBase(x.mantissa);
				assert(x - x / y * y < LongDouble(1, x.mantissa) / power);
			}
		}
	};

	class TestRound : public Test {
	public:
		TestRound() {
			name = "TestRound";
		}
		void test() {
			_test();
			LongDouble a = 1.5, b = 1.4, c = -1.4, d = -1.5, e = -1.6, n = 123.45678;
			a.round();
			b.round();
			c.round();
			d.round();
			e.round();
			assert(a == 2);
			assert(b == 1);
			assert(c == -1);
			assert(d == -2);
			assert(e == -2);
			n.round(5);
			assert(n == 123.45678);
			n.round(4);
			assert(n == 123.4568);
			n.round(3);
			assert(n == 123.457);
			n.round(2);
			assert(n == 123.46);
			n = LongDouble(123, 1) / 13;
			assert(n == 9.5);
			assert(LongDouble(123, 2) / 13 == 9.46);	
		}
	};

	void test() {
 		TestInit().test();
		TestAddition().test();
		TestSubstracion().test();
		TestMult().test();
		TestDiv().test();
		TestPrecision().test();
		TestRound().test();
		cout << "Finished in " << (double) clock() / CLOCKS_PER_SEC << " sec\n";
	};
}