#undef assert
#define assert(x) 
#pragma once

#include <iostream>
using namespace std;

void _print();

template <typename T, typename... V>
void _print(T t, V... v);

#define debug(...) {\
	cerr<<"["<<#__VA_ARGS__<<"] = [";\
	_print(__VA_ARGS__);\
}


