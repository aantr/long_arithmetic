
#include <bits/stdc++.h>
#include "fft_pointer.hpp"

using namespace std;

signed main() {
	FFT fft;
	digit *a;
	digit *b;
	digit *c;
	int c_size;
	int k = 5;
	a = (digit*) calloc(k, sizeof(digit));
	b = nullptr;

	for (int i = 0; i < k; i++) {
		a[i]=i + 1;
	}

	fft.multiply(a, k, b, 0, c, c_size);
	for (int i = 0; i < c_size; i++) {
		cout << c[i] << " ";
	}
	cout << endl;

}