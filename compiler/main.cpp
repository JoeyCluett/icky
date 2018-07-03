#include <iostream>

// Icky includes
#include "icky/Icky.h"
#include "icky/IckyRuntime.h"

using namespace std;

int main(int argc, char* argv[]) {
	IckyRuntimeData* ird = new IckyRuntimeData;
	try {

		IckyAsm::execute(ird, "sample.icky");
		int s = IckyAsm::runtimeSize(ird);
		cout << "Runtime size: " << s << endl;
	
	} catch(IckyException* ie) {
		cout << *ie << endl;
	}

	return 0;
}