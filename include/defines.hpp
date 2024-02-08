// #undef assert
// #define assert(x) 
#pragma once

// #define debug(x) cout << "[debug] " << #x << ": " << x << endl;
// #define debug(x, y) cout << "[debug] " << #x << ", " << #y << ": " << x << " " << y << endl;
// #define debug(x, y, z) cout << "[debug] " << #x << ", " << #y << ", " << z << ": " << x << " " << y << " " << z << endl;
#include <iostream>
using namespace std;

void _print();

template <class T, class... V>
void _print(T t, V... v);

#define debug(...) {\
	cerr<<"["<<#__VA_ARGS__<<"] = [";\
	_print(__VA_ARGS__);\
}