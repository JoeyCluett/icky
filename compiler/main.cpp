#include <iostream>

// Icky includes
#include "icky/Icky.h"
#include "icky/IckyRuntime.h"

using namespace std;

int main(int argc, char* argv[]) {
	IckyRuntimeData* ird = new IckyRuntimeData;
	//ird->verbose = true;

	

	/*try {

		IckyAsm::compile(ird, "sample.icky");
		cout << "Instruction size: " << ird->_instruction_size << endl;
		cout << "Binary size:      " << ird->_asm_ops.size() << " bytes" << endl;

		cout << "Dasm: " << endl << "================================\n";
		IckyAsm::dasm(ird);
		IckyAsm::execute(ird);
		IckyAsm::saveRuntimeData(ird, "bin/newfile.ickyasm");

		// completely reset the runtime
		delete ird;
		ird = new IckyRuntimeData;

		// and read it back from the file
		IckyAsm::loadRuntimeData(ird, "bin/newfile.ickyasm");
		IckyAsm::dasm(ird);

		IckyAsm::execute(ird);
	
	} catch(IckyException* ie) {
		cout << "Caught icky exception...\n";
		cout << *ie << endl;
	} catch(exception& e) {
		cout << "Caught std::exception...\n";
		cout << e.what() << endl;
	}*/

	delete ird;

	return 0;
}