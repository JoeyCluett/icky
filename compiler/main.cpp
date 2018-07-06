#include <iostream>

// Icky includes
#include "icky/Icky.h"
#include "icky/IckyRuntime.h"

using namespace std;

int main(int argc, char* argv[]) {
	IckyRuntimeData* ird = new IckyRuntimeData;
	try {

		IckyAsm::compile(ird, "sample.icky");
		//int s = IckyAsm::runtimeSize(ird);
		//cout << "Runtime size: " << s << endl;
		//cout << "Running compiled program\n";

		IckyAsm::execute(ird);
	
	} catch(IckyException* ie) {
		cout << *ie << endl;
	}

	return 0;
}