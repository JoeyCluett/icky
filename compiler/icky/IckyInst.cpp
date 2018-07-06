#include "Icky.h"
#include "IckyRuntime.h"
#include <iostream>
#include <string>

// ==================================================================
// assembler functions
// ==================================================================

void IckyAsm::printString(IckyRuntimeData* ird, std::string name) { // <0>
	int str_index = IckyAsm::verify::_string(ird, name);
	ird->_asm_ops.push_back(IckyOpCode::printString);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((str_index >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::printDouble(IckyRuntimeData* ird, std::string name) { // <1>
	int doub_index = IckyAsm::verify::_double(ird, name); // may throw exception

	// the data is definitely in the runtime, ready to be printed
	ird->_asm_ops.push_back(IckyOpCode::printDouble);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((doub_index >> (8*i)) & 0xFF);

	ird->_instruction_size++;
}

void IckyAsm::printInteger(IckyRuntimeData* ird, std::string name) { // <2>
	int int_index = IckyAsm::verify::_integer(ird, name);

	ird->_asm_ops.push_back(IckyOpCode::printInteger);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((int_index >> (8*i)) & 0xFF);

	ird->_instruction_size++;
}

void IckyAsm::printStringLiteral(IckyRuntimeData* ird, std::string value) {
	int str_index = IckyAsm::put::_string(ird, value);

	ird->_asm_ops.push_back(IckyOpCode::printString);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((str_index >> (8*i)) & 0xFF);
	
	ird->_instruction_size++;
}

void IckyAsm::printCharacterLiteral(IckyRuntimeData* ird, char c) {
	ird->_asm_ops.push_back(IckyOpCode::printCharacter);
	ird->_asm_ops.push_back(c);
	ird->_instruction_size++;
}

void IckyAsm::unconditionalJump(IckyRuntimeData* ird, std::string dest) {
	int jump_index = IckyAsm::verify::_jumpDest(ird, dest);
	ird->_asm_ops.push_back(IckyOpCode::unconditionalJump);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((jump_index >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}


