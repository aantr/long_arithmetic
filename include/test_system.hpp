#pragma once

#include <iostream>
#include <functional>
#include <string>

using namespace std;

#define check(x) if (!(x)) {throw #x;}
#define test_(x) TEST_SYSTEM.add_test(Test(string(#x), []() -> bool {
#define _test return 0;}));
#define TEST_SYSTEM test_system

// # Use ANSI color codes:
const string BRED    = "\033[1;31m";
const string BGREEN  = "\033[1;32m";
const string BYELLOW = "\033[1;33m";
const string RED    = "\033[38;5;202m";
const string GREEN   = "\033[38;5;192m";
const string YELLOW = "\033[38;5;230m";
const string BCYAN   = "\033[1;36m";
const string RESET   = "\033[0m";

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
					os << YELLOW + "Test" + RESET + " \"" << test.name << "\" ... ";
					os.flush();
					test.test();
					os << GREEN + "passed" + RESET << std::endl;
				} catch (const char *str) {
					os << RED + "failed" + RESET + ", expression: (" << str << ")\n";
					os.flush();
					failed++;
				}
			}
			if (failed) {
				if (failed == 1) {
					os << failed << " test " + RED + "failed\n" + RESET;
					os.flush();
				} else {
					os << failed << " tests " + RED + "failed\n" + RESET;
					os.flush();
				}
			} else {
				os << GREEN + "All tests passed\n" + RESET;
				os.flush();
			}
		}
	};


}

