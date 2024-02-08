#include <iostream>
#include <defines.hpp>
#include <arithmetic.hpp>
#include <arithmetic_32.hpp>

using namespace std;

void __print(int x)
{
	cerr << x;
}
void __print(long x)
{
	cerr << x;
}
void __print(long long x) {
	cerr << x;
}
void __print(unsigned x) {
	cerr << x;
}
void __print(unsigned long x) {
	cerr << x;
}
void __print(unsigned long long x) {
	cerr << x;
}
void __print(float x) {
	cerr << x;
}
void __print(double x) {
	cerr << x;
}
void __print(long double x) {
	cerr << x;
}
void __print(char x) {
	cerr << '\'' << x << '\'';
}
void __print(const char* x) {
	cerr << '\"' << x << '\"';
}
void __print(const string& x) {
	cerr << '\"' << x << '\"';
}
void __print(bool x) {
	cerr << (x ? "true" : "false");
}

void __print(arithmetic::LongDouble x) {
	cerr << x;
}
void __print(arithmetic_32::LongDouble x) {
	cerr << x;
}

template<class T, class V>
void __print(const pair<T, V>& x) {
	cerr << '{';
	__print(x.first);
	cerr << ',';
	__print(x.second); cerr << '}';
}
template<class T>
void __print(const T& x) {
	int f = 0;
	cerr << '{';
	for (auto& i : x)
	{
		cerr << (f++ ? "," : "");
		__print(i);
	}
	cerr << "}";
}
void _print() {
	cerr << "]" << endl;
}
template <class T, class... V>
void _print(T t, V... v) {
	__print(t);
	if (sizeof...(v))
		cerr << ", ";
	_print(v...);
}

void _print(arithmetic_32::LongDouble t) {
	__print(t);
	// if (sizeof...(v))
	// 	cerr << ", ";
	// _print(v...);
}

