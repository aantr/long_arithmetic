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
		int failed = false;
		for (Test test: tests) {
			try {
				test.test();
				cout << "Test \"" << test.name << "\" passed" << endl;
			} catch (const char *str) {
				cout << "Test \"" << test.name << "\" failed, expression: (" << str << ")" << endl;
				failed = true;
			}
		}
		if (failed) {
			cout << failed << " tests failed" << endl;
		} else {
			cout << "All tests passed" << endl;
		}
	}
};
