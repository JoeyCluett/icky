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

void IckyAsm::loadDoubleFromLiteral(IckyRuntimeData* ird, std::string dest, int src) { // <5>
	int dest_index = IckyAsm::verify::_double(ird, dest);
	ird->_asm_ops.push_back(IckyOpCode::loadDoubleVar);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((dest_index >> (8*i)) & 0xFF);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((src >> (8*i)) & 0xFF);

	ird->_instruction_size++;
}

void IckyAsm::wsLoadDouble(IckyRuntimeData* ird, int src) {
	ird->_asm_ops.push_back(IckyOpCode::wsPushDouble);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((src >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::wsReverse(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsReverse);
	ird->_instruction_size++;
}

void IckyAsm::wsClear(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsClear);
	ird->_instruction_size++;
}

void IckyAsm::wsSaveDouble(IckyRuntimeData* ird, std::string dest) {
	int dest_index = IckyAsm::verify::_double(ird, dest);
	ird->_asm_ops.push_back(IckyOpCode::wsSaveDouble);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((dest_index >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::wsAdd(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsAdd);
	ird->_instruction_size++;
}

void IckyAsm::wsSubtract(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsSubtract);
	ird->_instruction_size++;
}

void IckyAsm::wsMultiply(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsMultiply);
	ird->_instruction_size++;
}

void IckyAsm::wsDivide(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsDivide);
	ird->_instruction_size++;
}

void IckyAsm::wsPower(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::wsPower);
	ird->_instruction_size++;
}

void IckyAsm::bGreaterThan(IckyRuntimeData* ird, int true_dest) {
	ird->_asm_ops.push_back(IckyOpCode::bGreaterThan);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((true_dest >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::bLessThan(IckyRuntimeData* ird, int true_dest) {
	ird->_asm_ops.push_back(IckyOpCode::bLessThan);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((true_dest >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::bGreaterThanEq(IckyRuntimeData* ird, int true_dest) {
	ird->_asm_ops.push_back(IckyOpCode::bGreaterThanEq);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((true_dest >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::bLessThanEq(IckyRuntimeData* ird, int true_dest) {
	ird->_asm_ops.push_back(IckyOpCode::bLessThanEq);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((true_dest >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::bEq(IckyRuntimeData* ird, int true_dest) {
	ird->_asm_ops.push_back(IckyOpCode::bEq);
	for(int i = 0; i < 4; i++)
		ird->_asm_ops.push_back((true_dest >> (8*i)) & 0xFF);
	ird->_instruction_size++;
}

void IckyAsm::sysRuntime(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::sysRuntime);
	ird->_instruction_size++;
}

void IckyAsm::sysWait(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::sysWait);
	ird->_instruction_size++;
}

void IckyAsm::sysQuit(IckyRuntimeData* ird) {
	ird->_asm_ops.push_back(IckyOpCode::sysQuit);
	ird->_instruction_size++;
}
