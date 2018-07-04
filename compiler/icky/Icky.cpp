#include "Icky.h"
#include "IckyRuntime.h"
#include "IckyKeywords.h"
#include <SimpleDatabase.hpp>
#include <iostream>
#include <string>

void IckyAsm::execute(IckyRuntimeData* ird, std::string filename) {

	// some default values go into the runtime always
	IckyAsm::put::_double(ird, "__pi",    3.14159265);
	IckyAsm::put::_double(ird, "__root2", 1.4142135623);
	IckyAsm::put::_double(ird, "__true",  1);
	IckyAsm::put::_double(ird, "__false", 0);

	std::cout << "Filename: " << filename << std::endl;
	auto* str_vec = SimpleDatabase::alignStringVector(filename);

	const int STATE_default = 0;
	const int STATE_print   = 1;
	const int STATE_label   = 2;
	const int STATE_goto    = 3;
	int current_state = STATE_default;

	int str_vec_size = str_vec->size();
	for(int k = 0; k < str_vec_size; k++) {
		std::string str = str_vec->at(k); // bounds checking with at()
		
		// lets see the string
		//std::cout << str << std::endl;

		// the main compilation loop
		switch(current_state) {
			case STATE_default:
				//std::cout << "STATE_default\n";
				if(str == IckyKeyword::PRINT || str == "<") {
					current_state = STATE_print;

				} else if(str == IckyKeyword::LABEL || str == "::") { // no END/;
					current_state = STATE_label;

				} else if(str == IckyKeyword::GOTO || str == "..") { // no END/;
					current_state = STATE_goto;

				}
				break;

			case STATE_print:
				std::cout << "STATE_print\n";
				if(str == IckyKeyword::END || str == ";") {
					current_state = STATE_default;
				} else {
					if(str == "\\n") {
						IckyAsm::printCharacterLiteral(ird, '\n');
					} else if(str == "\\s") {
						IckyAsm::printCharacterLiteral(ird, ' ');
					} else {
						if(str[0] == '$') {
							IckyAsm::printDouble(ird, &str[1]);
						} else {
							IckyAsm::printStringLiteral(ird, str);
						}
					}
				}
				break;

			case STATE_label:
				std::cout << "STATE_label\n";
				if(ird->_jump_table_index.find(str) == ird->_jump_table_index.end()) {
					// entry doesnt exist. create it
					int asm_index = ird->_asm_ops.size();
					ird->_jump_table_index[str] = ird->_jump_table.size();
					ird->_jump_table.push_back(asm_index);
				} else {
					// entry exists but it doesnt have a valid jump destination
					ird->_jump_table.at(ird->_jump_table_index[str]) = ird->_asm_ops.size();
				}
				current_state = STATE_default;
				break;

			case STATE_goto:
				std::cout << "STATE_goto\n";
				if(ird->_jump_table_index.find(str) == ird->_jump_table_index.end()) {
					// entry doesnt exist. create a placeholder until a LABEL is encoutered
					ird->_jump_table_index[str] = ird->_jump_table.size();
					ird->_jump_table.push_back(-1);
					IckyAsm::unconditionalJump(ird, str);
				} else {
					// entry already exists. congrats
					IckyAsm::unconditionalJump(ird, str);
				}
				break;

			default:
				throw new IckyException("Unknown STATE_ in compilation stage of IckyAsm::execute");
				break;
		}
	}
}

int IckyAsm::runtimeSize(IckyRuntimeData* ird) {
	int s = sizeof(IckyRuntimeData);

	s += ird->_std_var_string.size();      // vector<char>
	s += ird->_std_var_double.size() * 8;  // vector<double>
	s += ird->_std_var_integer.size() * 8; // vector<long>

	//const int str_size = sizeof(std::string);

	return s; // not finished yet, but who cares
}

void IckyAsm::run(IckyRuntimeData* ird) {
	int& IP = ird->_instruction_ptr; // just a shorter way to use

	for(;;) {
		//uint8_t opcode = ird->_asm_ops[IP]; // avoid the bounds check for a lil speed boost
		uint8_t opcode = ird->_asm_ops.at(IP);

		switch(opcode) {
			case IckyOpCode::printString:
				std::cout << &ird->_std_var_string[*(int*)&ird->_asm_ops[++IP]] << ' ';
				IP += 4;
				break;
			case IckyOpCode::printDouble:
				std::cout << ird->_std_var_double[*(int*)&ird->_asm_ops[++IP]] << ' ';
				IP += 4;
				break;
			case IckyOpCode::printInteger:
				std::cout << ird->_std_var_integer[*(int*)&ird->_asm_ops[++IP]] << ' ';
				IP += 4;
				break;
			case IckyOpCode::printCharacter:
				std::cout << (char)ird->_asm_ops[++IP];
				IP++;
				break;
			default:
				throw new IckyException(std::string("Unknown opcode: ") + std::to_string((int)opcode));
				break;
		}
	}
}

/*
void IckyAsm::dasm(IckyRuntimeData* ird) {
	for(uint8_t ui : ird->_asm_ops) {
		
	}
}
*/