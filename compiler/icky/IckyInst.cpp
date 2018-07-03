#include "Icky.h"
#include "IckyRuntime.h"
#include <iostream>
#include <string>

// ==================================================================
// assembler functions
// ==================================================================

void IckyAsm::printString(IckyRuntimeData* ird, std::string name) { // <0>
	throw new IckyException("Instruction (printString) not defined");
}

void IckyAsm::printDouble(IckyRuntimeData* ird, std::string name) { // <1>
	int str_index = IckyAsm::verify::_string(ird, name);

	// the string is definitely in the runtime, ready to be printed
	ird->_asm_ops.push_back(IckyOpCode::printDouble);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((str_index >> (8*i)) & 0xFF);

	ird->_instruction_size++;
}

void IckyAsm::printInteger(IckyRuntimeData* ird, std::string name) { // <2>
	int int_index = IckyAsm::verify::_integer(ird, name);
}