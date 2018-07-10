#include "Icky.h"
#include "IckyRuntime.h"
#include "IckyKeywords.h"
#include <SimpleDatabase.hpp>
#include <iostream>
#include <string>
#include <algorithm>

void IckyAsm::compile(IckyRuntimeData* ird, std::string filename) {

	// some default values go into the runtime always
	IckyAsm::put::_double(ird, "__pi",    3.14159265);
	IckyAsm::put::_double(ird, "__root2", 1.4142135623);
	IckyAsm::put::_double(ird, "__true",  1);
	IckyAsm::put::_double(ird, "__false", 0);

	std::cout << "Filename: " << filename << std::endl;
	auto* str_vec = SimpleDatabase::alignStringVector(filename);

	const int STATE_default  = 0;
	const int STATE_print    = 1;
	const int STATE_label    = 2;
	const int STATE_goto     = 3;
	const int STATE_variable = 4;
	const int STATE_math     = 5;
	const int STATE_branch   = 6;
	int current_state = STATE_default;

	const int VARIABLE_var_name = 0;
	const int VARIABLE_equ      = 1;
	const int VARIABLE_value    = 2;
	const int VARIABLE_end      = 3;
	int current_variable_state = VARIABLE_var_name;

	const int MATH_dest     = 0;
	const int MATH_equ      = 1;
	const int MATH_operand  = 2;
	const int MATH_operator = 3;
	int current_math_state = MATH_dest;

	const int BRANCH_dest = 0;
	const int BRANCH_if   = 1;
	const int BRANCH_lhs  = 2;
	const int BRANCH_comp = 3;
	const int BRANCH_rhs  = 4;
	const int BRANCH_end  = 5;
	int current_branch_state = BRANCH_dest;

	int str_vec_size = str_vec->size();
	for(int k = 0; k < str_vec_size; k++) {
		std::string str = str_vec->at(k); // bounds checking with at()
		
		// lets see the string
		if(ird->verbose) std::cout << str << " : ";

		// the main compilation loop
		switch(current_state) {
			case STATE_default:
				if(ird->verbose) std::cout << "STATE_default\n";
				if(str == IckyKeyword::PRINT || str == "<") {
					current_state = STATE_print;

				} else if(str == IckyKeyword::LABEL || str == "::") { // no END/;
					current_state = STATE_label;

				} else if(str == IckyKeyword::GOTO || str == "..") { // no END/;
					current_state = STATE_goto;

				} else if(str == IckyKeyword::VARIABLE || str == ">>") { // >> varname = value ;
					current_state = STATE_variable;

				} else if(str == IckyKeyword::MATH || str == ":=") {
					current_state = STATE_math;

				} else if(str == IckyKeyword::BRANCH || str == ".") {
					current_state = STATE_branch;

				} else {
					throw new IckyException(std::string("Unknown token: ") + str);
				}
				break;

			case STATE_print:
				if(ird->verbose) std::cout << "STATE_print\n";
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
				if(ird->verbose) std::cout << "STATE_label\n";
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
				if(ird->verbose) std::cout << "STATE_goto\n";
				if(ird->_jump_table_index.find(str) == ird->_jump_table_index.end()) {
					// entry doesnt exist. create a placeholder until a LABEL is encoutered
					ird->_jump_table_index[str] = ird->_jump_table.size();
					ird->_jump_table.push_back(-1);
					IckyAsm::unconditionalJump(ird, str);
				} else {
					// entry already exists. congrats
					IckyAsm::unconditionalJump(ird, str);
				}
				current_state = STATE_default;
				break;

			case STATE_variable:
				if(ird->verbose) std::cout << "STATE_variable\n";
				static std::string var_name  = "";
				static std::string var_value = "";

				// go through all the steps of acquiring variable initialization data
				switch(current_variable_state) {
					case VARIABLE_var_name:
						var_name = str;
						current_variable_state = VARIABLE_equ;
						break;
					case VARIABLE_equ:
						if(str != IckyKeyword::EQU && str != "=")
							throw new IckyException("Unexpected token, expecting 'EQU' or '='");
						else
							current_variable_state = VARIABLE_value;
						break;
					case VARIABLE_value:
						var_value = str;
						current_variable_state = VARIABLE_end;
						break;
					case VARIABLE_end:
						if(str != IckyKeyword::END && str != ";")
							throw new IckyException("Unexpected token, expecting 'END' or ';'");
						else {
							int data_index = -1;
							if(var_value[0] == '$') {
								// source is another variable
								data_index = IckyAsm::verify::_double(ird, &var_value[1]);
							} else {
								// assume source is double literal
								data_index = IckyAsm::put::_double(ird, std::stod(var_value));
							}

							if(ird->_std_var_double_index.find(var_name) 
									!= ird->_std_var_double_index.end()) {
								// variable has already been declared, throw error
								throw new IckyException("Variable has already been declared!");
							} else {
								// variable doesnt exist, create one
								ird->_std_var_double_index[var_name] = ird->_std_var_double.size();
								ird->_std_var_double.push_back(0.0);

								// generate appropiate instruction
								IckyAsm::loadDoubleFromLiteral(ird, var_name, data_index);
							}
						}
						current_state = STATE_default;
						current_variable_state = VARIABLE_var_name;
						break;
					default:
						throw new IckyException("Unknown substate in STATE_variable");
						return;
				}
				break;

			case STATE_math:
				if(ird->verbose) std::cout << "STATE_math\n";
				static std::vector<std::string> _operation_vec;
				static std::string _op_dest;

				switch(current_math_state) {
					case MATH_dest:
						_op_dest = str;
						current_math_state = MATH_equ;
						break;
					case MATH_equ:
						if(str != IckyKeyword::EQU && str != "=") {
							throw new IckyException("STATE_math:MATH_dest expecting EQU/=");
							return;
						} else {
							current_math_state = MATH_operand;
						}
						break;
					case MATH_operand:
						if(str == IckyKeyword::END || str == ";") {
							throw new IckyException("STATE_math:MATH_operand expecting operand, got END/;");
						} else {
							int data_index = -1;
							if(str[0] == '$') {
								data_index = IckyAsm::verify::_double(ird, &str[1]);
							} else {
								data_index = IckyAsm::put::_double(ird, std::stod(str));
							}
							IckyAsm::wsLoadDouble(ird, data_index);
							current_math_state = MATH_operator;
						}
						break;
					case MATH_operator:
						if(str == IckyKeyword::END || str == ";") {
							// end of math ops, generate asm
							IckyAsm::wsReverse(ird);
							for(auto& _op : _operation_vec) {
								if(_op == "+") {
									IckyAsm::wsAdd(ird);
								} else if(_op == "-") {
									IckyAsm::wsSubtract(ird);
								} else if(_op == "*") {
									IckyAsm::wsMultiply(ird);
								} else if(_op == "/") {
									IckyAsm::wsDivide(ird);
								} else if(_op == "^") {
									IckyAsm::wsPower(ird);
								} else {
									throw new IckyException("STATE_math:MATH_operator - WTF! How did this happen!?");
								}
							}
							IckyAsm::wsSaveDouble(ird, &_op_dest[1]);
							IckyAsm::wsClear(ird);

							// quick cleanup
							_operation_vec.clear();
							current_state = STATE_default;
							current_math_state = MATH_dest; // BACK TO NORMAL
						} else {
							if(str == "+" || str == "-" || str == "*" || str == "/" || str == "^") {
								_operation_vec.push_back(str);
							} else {
								throw new IckyException(std::string(
										"Unexpected operator in STATE_math:MATH_operator: ") + str);
								return;
							}
							current_math_state = MATH_operand;
						}
						break;
					default:
						throw new IckyException("Unknown substate in STATE_math");
						return;
				}
				break;

			case STATE_branch:
				static std::string branch_dest = "";
				static std::string branch_lhs  = ""; //
				static std::string branch_comp = "";
				static std::string branch_rhs  = ""; //

				switch(current_branch_state) {
					case BRANCH_dest:
						branch_dest = str;
						current_branch_state = BRANCH_if;
						break;
					case BRANCH_if:
						if(str != IckyKeyword::IF && str != ":") {
							throw new IckyException("STATE_branch:BRANCH_if expecting ('IF'|':')");
							return;
						} else {
							current_branch_state = BRANCH_lhs;
							break;
						}
					case BRANCH_lhs:
						branch_lhs = str;
						current_branch_state = BRANCH_comp;
						break;
					case BRANCH_comp:
						if(str == "<" || str == ">" || str == "<=" || str == ">=" || str == "==") {
							branch_comp = str;
							current_branch_state = BRANCH_rhs;
							break;
						} else {
							throw new IckyException("STATE_branch:BRANCH_comp expecting ('<'|'>'|'<='|'>='|'==')");
							return;
						}
					case BRANCH_rhs:
						branch_rhs = str;
						current_branch_state = BRANCH_end;
						break;
					case BRANCH_end:
						{
							int data_index = -1;

							// generate code for left hand operand
							if(branch_lhs[0] == '$') {
								data_index = IckyAsm::verify::_double(ird, &branch_lhs[1]);
							} else { // double literal
								data_index = IckyAsm::put::_double(ird, std::stod(branch_lhs));
							}
							IckyAsm::wsLoadDouble(ird, data_index);

							// generate code for right hand operand
							if(branch_rhs[0] == '$') {
								data_index = IckyAsm::verify::_double(ird, &branch_rhs[1]);
							} else { // double literal
								data_index = IckyAsm::put::_double(ird, std::stod(branch_rhs));
							}
							IckyAsm::wsLoadDouble(ird, data_index);

							// index_table index for jump destination
							data_index = IckyAsm::verify::_jumpDest(ird, branch_dest);

							// generate branch code
							if(branch_comp == "<") {
								IckyAsm::bLessThan(ird, data_index);
							} else if(branch_comp == ">") {
								IckyAsm::bGreaterThan(ird, data_index);
							} else if(branch_comp == "<=") {
								IckyAsm::bLessThanEq(ird, data_index);
							} else if(branch_comp == ">=") {
								IckyAsm::bGreaterThanEq(ird, data_index);
							} else if(branch_comp == "==") {
								IckyAsm::bEq(ird, data_index);
							} else {
								throw new IckyException("STATE_branch:BRANCH_end. This shouldn't have happened");
								return;
							}

							// cleanup
							current_state = STATE_default;
							current_branch_state = BRANCH_dest;
						}
						break;
					default:
						throw new IckyException("STATE_branch unknown substate...");
						return;
				}
				break;

			default:
				throw new IckyException("Unknown STATE_ in compilation stage of IckyAsm::execute");
				break;
		}
	}

	// verify IckyRuntimeData to verify all variables have been initialized

}

int IckyAsm::runtimeSize(IckyRuntimeData* ird) {
	int s = sizeof(IckyRuntimeData);

	s += ird->_std_var_string.size();      // vector<char>
	s += ird->_std_var_double.size() * 8;  // vector<double>
	s += ird->_std_var_integer.size() * 8; // vector<long>

	//const int str_size = sizeof(std::string);

	return s; // not finished yet, but who cares
}

void IckyAsm::execute(IckyRuntimeData* ird) {
	int& IP = ird->_instruction_ptr; // just a shorter way to use

	for(;;) {
		////uint8_t opcode = ird->_asm_ops[IP]; // avoid the bounds check for a lil speed boost
		uint8_t opcode = ird->_asm_ops.at(IP); // however, bounds checking ensures safer execution

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
			case IckyOpCode::unconditionalJump:
				IP = ird->_jump_table[*(int*)&ird->_asm_ops[++IP]];
				break;
			case IckyOpCode::loadDoubleVar:
				ird->_std_var_double[*(int*)&ird->_asm_ops[IP + 1]] 
						= ird->_std_var_double[*(int*)&ird->_asm_ops[IP + 5]];
				IP += 9;
				break;
			case IckyOpCode::wsPushDouble:
				ird->_working_stack.push_back(ird->_std_var_double[*(int*)&ird->_asm_ops[IP+1]]);
				IP += 5;
				break;
			case IckyOpCode::wsReverse:
				std::reverse(std::begin(ird->_working_stack), std::end(ird->_working_stack));
				IP++;
				break;
			case IckyOpCode::wsClear:
				ird->_working_stack.clear();
				IP++;
				break;
			case IckyOpCode::wsSaveDouble:
				ird->_std_var_double[*(int*)&ird->_asm_ops[IP+1]] = ird->_working_stack[0];
				IP += 5;
				break;
			case IckyOpCode::wsAdd:
				{
					double tmp = ird->_working_stack.back() + *(ird->_working_stack.end()-2);
					ird->_working_stack.pop_back();
					ird->_working_stack.back() = tmp;
					IP++;
				}
				break;
			case IckyOpCode::wsSubtract:
				IP++;
				break;
			case IckyOpCode::wsMultiply:
				IP++;
				break;
			case IckyOpCode::wsDivide:
				IP++;
				break;
			case IckyOpCode::wsPower:
				IP++;
				break;
			case IckyOpCode::bGreaterThan:
				if(ird->_working_stack[0] > ird->_working_stack[1]) {
					IP = ird->_jump_table[*(int*)&ird->_asm_ops[IP+1]];
				} else {
					IP += 5;
				}
				ird->_working_stack.clear();
				break;
			case IckyOpCode::bLessThan:
				if(ird->_working_stack[0] < ird->_working_stack[1]) {
					IP = ird->_jump_table[*(int*)&ird->_asm_ops[IP+1]];
				} else {
					IP += 5;
				}
				ird->_working_stack.clear();
				break;
			case IckyOpCode::bGreaterThanEq:
				if(ird->_working_stack[0] >= ird->_working_stack[1]) {
					IP = ird->_jump_table[*(int*)&ird->_asm_ops[IP+1]];
				} else {
					IP += 5;
				}
				ird->_working_stack.clear();
				break;
			case IckyOpCode::bLessThanEq:
				if(ird->_working_stack[0] <= ird->_working_stack[1]) {
					IP = ird->_jump_table[*(int*)&ird->_asm_ops[IP+1]];
				} else {
					IP += 5;
				}
				ird->_working_stack.clear();
				break;
			case IckyOpCode::bEq:
				if(ird->_working_stack[0] == ird->_working_stack[1]) {
					IP = ird->_jump_table[*(int*)&ird->_asm_ops[IP+1]];
				} else {
					IP += 5;
				}
				ird->_working_stack.clear();
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