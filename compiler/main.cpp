#include <iostream>

// Icky includes
#include "icky/Icky.h"
#include "icky/IckyRuntime.h"

using namespace std;

int main(int argc, char* argv[]) {
	IckyRuntimeData* ird = new IckyRuntimeData;
	//ird->verbose = true;

	try {

		IckyAsm::compile(ird, "sample.icky");
		cout << "Instruction size: " << ird->_instruction_size << endl;
		//int s = IckyAsm::runtimeSize(ird);
		//cout << "Runtime size: " << s << endl;
		//cout << "Running compiled program\n";

		IckyAsm::execute(ird);
	
	} catch(IckyException* ie) {
		cout << "Caught icky exception...\n";
		cout << *ie << endl;
	}

	return 0;
}