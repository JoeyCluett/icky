#include <iostream>
#include "conio.h"

using namespace std;

volatile double j = 0;

int main(int argc, char* argv[]) {

	unsigned long start_time = getUsecTimestamp();

	for(int i = 0; i < 100000000; i++) {
		j = j + 1.0;//cout << i << endl;
	}

	cout << "Runtime: " << double(getUsecTimestamp() - start_time) / 1000000.0 << " seconds\n";

	return 0;
}
