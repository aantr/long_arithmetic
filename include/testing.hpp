#include <arithmetic.hpp>
#include <assert.h>

#if __cplusplus >= 201103L
#include <random>
#endif

using namespace std;
using namespace arithmetic;

std::mt19937 rnd(7327158);

namespace testing {

	class Test {
public:
		string name = "Empty test";
		void test() {};
		void _test() {cout << "Testing " + name + "..." << endl;};
	};

	class TestInit : public Test {
	public:
		TestInit() {
			name = "TestInit";
		}
		void test() {
			_test();
			LongDouble a(123), b, c, d = -123, e("0123"), k = string("0");
			assert(a == e);
			assert(b == k);
			assert(a != d);
			assert(c == k);
			assert(d == -a);
			assert(d != a);
		}
	};

	class TestInequlity : public Test {
	public:
		TestInequlity() {
			name = "TestInequlity";
		}
		void test() {
			_test();
			LongDouble a(123), b, c, d = -123, e("0123"), k = string("0");
			assert((a >= d) == true);
			assert((b <= k) == true);
			assert((d > a) == false);
			assert((e < d) == false);
			assert((d > c) == false);
			assert((k < b) == false);
			assert((d < d + 1) == true);

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
			n.round(-1);
			assert(n == 120);
			n.round(-2);
			assert(n == 100);

			n = 3453345.44563636;
			n.floor(5);
			assert(n == 3453345.44563);
			n.floor(-2);
			assert(n == 3453300);

			n = LongDouble((double)123 / 13);
			n.round(1);
			assert(n == 9.5);
			n = LongDouble((double)123 /  13);
			n.round(2);
			assert(n == 9.46);	
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
				int x = (long long) rnd() - rnd(), y = ((long long) rnd() - rnd()) / 100;
				LongDouble res = LongDouble(x) / LongDouble(y);
				res.floor();
				assert(LongDouble(x / y) == res);

				LongDouble res_round = LongDouble(x) / LongDouble(y);
				res_round.round(5);
				assert(res_round == (double)round((double)x / y * pow(10, 5)) / pow(10, 5));
			}
		}
	};

	class TestSqrt : public Test {
	public:
		TestSqrt() {
			name = "TestSqrt";
		}
		void test() {
			_test();
			int count = 100;
			for (int i = 0; i < count; i++) {
				double x = rnd() % 1000000000;
				x /= pow(10, rnd() % 5);
				LongDouble X ((double)x, 16);
				X.sqrt();

				x = sqrt(x);
				LongDouble st = 1;
				int check = 6;
				st.divBase(check);
				assert((X - (double)x).abs() < st);
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
				int len = 40; // тестируем числа с len цифрами
				LongDouble x(0, len * 3 / 2), y(0, 10);
				for (int j = 0; j < len; j++) {
					x = x * LongDouble(10) + LongDouble((int) (rnd() % 10));
					y = y * LongDouble(10) + LongDouble((int) (rnd() % 10));
				}
				int expx = rnd() % len;
				x.divBase(expx);
				y.divBase(rnd() % len);
				LongDouble power = 1; power.divBase(expx);
				assert(x - x / y * y < power);
			}
		}
	};

	void test() {

 		TestInit().test();
 		TestInequlity().test();
		TestAddition().test();
		TestSubstracion().test();
		TestRound().test();
		TestMult().test();
		TestDiv().test();
		TestSqrt().test();
		TestPrecision().test();
		cout << "Finished in " << (double) clock() / CLOCKS_PER_SEC << " sec\n";
	};
}