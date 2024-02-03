#pragma once

#include <iostream>
#include <functional>

using namespace std;

#define check(x) if (!(x)) {throw #x;}
#define test_(x) TEST_SYSTEM.add_test(Test(string(#x), []() -> bool {
#define _test return 0;}));
#define TEST_SYSTEM test_system

namespace test_system {

	class Test {
	public:
		string name;
		function<bool()> test;
		Test(string name, function<bool()> test): name(name), test(test) {

		}
	};

	class TestSystem {
	public:
		std::vector<Test> tests;
		void add_test(Test test) {
			tests.push_back(test);
		}

		void run_testing(ostream& os) {
			int failed = 0;
			for (Test test: tests) {
				try {
					os << "Test \"" << test.name << "\" ... ";
					os.flush();
					test.test();
					os << "passed" << std::endl;
				} catch (const char *str) {
					os << "failed, expression: (" << str << ")\n";
					os.flush();
					failed++;
				}
			}
			if (failed) {
				if (failed == 1) {
					os << failed << " test failed\n";
					os.flush();
				} else {
					os << failed << " tests failed\n";
					os.flush();
				}
			} else {
				os << "All tests passed\n";
				os.flush();
			}
		}
	};


}

