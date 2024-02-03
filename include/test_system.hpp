#include <iostream>
#include <functional>

using namespace std;

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

	void run_testing() {
		int failed = 0;
		for (Test test: tests) {
			try {
				cout << "Test \"" << test.name << "\" ... "; cout.flush();
				test.test();
				cout << "passed" << endl;
			} catch (const char *str) {
				cout << "failed, expression: (" << str << ")" << endl;
				failed++;
			}
		}
		if (failed) {
			if (failed == 1) cout << failed << " test failed" << endl;
			else cout << failed << " tests failed" << endl;
		} else {
			cout << "All tests passed" << endl;
		}
	}
};
