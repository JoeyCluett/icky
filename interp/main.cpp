#include <iostream>
#include <string>
#include <SimpleDatabase.hpp>

#include "IcRuntime.hpp"
#include "conio.h"

using namespace std;

bool verbose = false;

int main(int argc, char* argv[]) {
	//auto* sl = SimpleDatabase::parseString("sc.ics");
	//cout << *sl << endl;

	//auto* str_vec = SimpleDatabase::alignStringVector("count.ics");
	//auto* str_vec = SimpleDatabase::alignStringVector("cryptic.ics");
	
	auto* str_vec = SimpleDatabase::alignStringVector(argv[1]);
	if(*argv[2] == 'T' || *argv[2] == 't')
		verbose = true;

	vector<double> tmp_push_stack;

	IcRuntime* rt = IcRuntime_new();
	IcRuntime_addVariable(rt, "__pi", 3.14159265);
	IcRuntime_addVariable(rt, "__root2", 1.4142135623);
	IcRuntime_addVariable(rt, "__true", 1);
	IcRuntime_addVariable(rt, "__false", 0);

	IcRuntime_findBranchTargets(rt, str_vec);

	const int STATE_default  = 0;
	const int STATE_variable = 1;
	const int STATE_expose   = 2;
	const int STATE_print    = 3;
	const int STATE_math     = 4;
	const int STATE_label    = 5;
	const int STATE_goto     = 6;
	const int STATE_branch   = 7;
	const int STATE_wait     = 8;
	const int STATE_quit     = 9;
	const int STATE_runtime  = 10; 
	const int STATE_call     = 11;
	const int STATE_return   = 12;
	const int STATE_push     = 13;
	const int STATE_pop      = 14;
	const int STATE_clear    = 15;
	const int STATE_import   = 16;
	int current_state = STATE_default;

	// the stack machine used for math operations needs
	// to know these things	
	string math_result = "";
	const int MATH_result    = 0;
	const int MATH_equ       = 1;
	const int MATH_operation = 2;
	const int MATH_operand   = 3;
	int MATH_state = MATH_result;

	// states needed for branch logic
	const int BRANCH_target = 0;
	const int BRANCH_if     = 1;
	const int BRANCH_lhs    = 2;
	const int BRANCH_op     = 3;
	const int BRANCH_rhs    = 4;
	const int BRANCH_end    = 5;
	int BRANCH_state = BRANCH_target;

	// import needs a few bits of information before
	// acting on it
	string import_filename = "";
	string import_alias    = "";

	const int IMPORT_name  = 0;
	const int IMPORT_alias = 1;
	const int IMPORT_end   = 2;
	int IMPORT_state = IMPORT_name;

	// for adding new variables to the runtime
	string var_name = "";
	double var_value = 0.0;

	// really need to just make this into another small state machine
	bool var_name_ = false;
	bool var_equ   = false;
	bool var_val   = false;
	bool var_end   = false;

	unsigned long start_time = getUsecTimestamp();

	//for(auto& sv : *sl) {
		//for(auto& str : sv) {
	for(int k = 0; k < str_vec->size(); k++) {
		string& str = str_vec->at(k);

		if(verbose) 
			cout << "\nToken(" << str << ")\n";
		switch(current_state) {
			case STATE_default:
				if(verbose)
					cout << "  STATE_default\n";
				
				if(str == VARIABLE || str == ">>") {
					current_state = STATE_variable;

				} else if(str == EXPOSE || str == "-e") {
					current_state = STATE_expose;

				} else if(str == PRINT || str == "<") {
					current_state = STATE_print;

				} else if(str == MATH || str == ":=") {
					current_state = STATE_math;

				} else if(str == LABEL || str == "::") {
					current_state = STATE_label;

				} else if(str == GOTO || str == "..") {
					current_state = STATE_goto;

				} else if(str == BRANCH || str == ".") {
					current_state = STATE_branch;

				} else if(str == WAIT || str == "-w") {
					current_state = STATE_wait;

				} else if(str == QUIT || str == "-q") {
					current_state = STATE_quit;

				} else if(str == RUNTIME || str == "-r") {
					current_state = STATE_runtime;

				} else if(str == CALL || str == ">c") {
					current_state = STATE_call;

				} else if(str == RETURN || str == ">r") {
					current_state = STATE_return;

				} else if(str == PUSH || str == "<p") {
					tmp_push_stack.clear();
					current_state = STATE_push;

				} else if(str == POP || str == ">p") {
					current_state = STATE_pop;

				} else if(str == IMPORT) {
					current_state = STATE_import;

				} else {
					cout << "    Unknown token: " << str << endl;
					return 1;
				}
				break;
			case STATE_variable:
				if(verbose)
					cout << "  STATE_variable\n";
				
				if(!var_name_) {
					var_name = str;
					var_name_ = true;
				} else if(!var_equ) {
					if(str != EQU && str != "=") {
						cout << "    UNKNOWN TOKEN (expecting 'EQU'): " << str << endl;
						return 1;
					} else {
						var_equ = true;
					}
				} else if(!var_val) {

					if(str[0] == '$') {
						if(IcRuntime_checkVariable(rt, str.c_str()+1))
							var_value = IcRuntime_getVariable(rt, str.c_str()+1);
						else {
							cout << "    Unknown operand: " << str.c_str()+1 << endl;
							return 1;
						}

					} else if(str[0] == '@') {
						//cout << "    Prepping stack value..." << endl;
						var_value = IcRuntime_getStackValue(rt, stoi(str.c_str()+1));

					} else {
						if(IcRuntime_checkVariable(rt, str)) {
							var_value = IcRuntime_getVariable(rt, str);
						} else {
							var_value = stod(str);
						}
					}

					var_val = true;
					if(verbose)
						cout << "    STATE_variable :: " << var_name << " = " << var_value << endl;
				} else if(!var_end) {
					if(str != END && str != ";") {
						cout << "    UNKNOWN TOKEN (expecting 'END' or ';'): " << str << endl;
						return 1;
					}

					var_name_ = false;
					var_equ = false;
					var_val = false;
					var_end = false;

					IcRuntime_addVariable(rt, var_name, var_value);

					current_state = STATE_default;

				} else {
					cout << "    UNKNOWN SUB-STATE (in STATE_variable): ALL FLAGS SET" << endl;
					return 1;
				}
				break;
			case STATE_expose:
				if(verbose)
					cout << "  STATE_expose\n";

				IcRuntime_printRuntimeData(rt);
				current_state = STATE_default;
				break;
			case STATE_print:
				if(verbose)
					cout << "  STATE_print\n";

				if(str != END && str != ";") {
					if(str == NEWLINE || str == "\\n") {
						cout << endl;
					} else if(str == SPC || str == "\\s") {
						cout << ' ';
					} else if(str == "\\$") {
						cout << '$';
					} else if(str[0] == '$') { // special case of printing a variable
						cout << IcRuntime_getVariable(rt, str.c_str()+1);
					} else if(str[0] == '@') {
						cout << IcRuntime_getStackValue(rt, stoi(str.c_str()+1));
					} else {
						cout << str << ' ';
					}
				} else {
					//cout << endl;
					current_state = STATE_default;
				}
				break;
			case STATE_math:
				if(verbose)
					cout << "  STATE_math\n";

				if(str == END || str == ";") {
					current_state = STATE_default;
					MATH_state = MATH_result;
					IcRuntime_evaluateOperation(rt);
					break;
				}

				switch(MATH_state) {
					case MATH_result:
						if(str[0] != '$') {
							cout << "    Expecting '$' in variable reference: " << str << endl;
							return 1;
						}
						rt->result = str.c_str()+1;
						MATH_state = MATH_equ;
						break;
					case MATH_equ:
						if(str != "=") {
							cout << "    Expecting '=': " << str << endl;
							return 1;
						}
						rt->operations.clear();
						rt->operands.clear();
						MATH_state = MATH_operand;
						break;
					case MATH_operation:
						if(str == MULTIPLY || str == "*") {
							rt->operations.push_back(IcOps::Mult);

						} else if(str == DIVIDE || str == "/") {
							rt->operations.push_back(IcOps::Divide);

						} else if(str == ADD || str == "+") {
							rt->operations.push_back(IcOps::Add);

						} else if(str == SUBTRACT || str == "-") {
							rt->operations.push_back(IcOps::Subtract);

						} else if(str == EXPONENT || str == "^") {
							rt->operations.push_back(IcOps::Exp);

						} else {
							cout << "    Unrecognized math operator: " << str << endl;
							return 1;
						}
						MATH_state = MATH_operand;
						break;
					case MATH_operand:
						if(str[0] == '$') {
							if(IcRuntime_checkVariable(rt, str.c_str()+1))
								rt->operands.push_back(IcRuntime_getVariable(rt, str.c_str()+1));
							else {
								cout << "    Unknown operand: " << str.c_str()+1 << endl;
								return 1;
							}
						} else if(str[0] == '@') {
							rt->operands.push_back(IcRuntime_getStackValue(rt, stoi(str.c_str()+1)));
						} else {
							rt->operands.push_back(stod(str));
						}
						MATH_state = MATH_operation;
						break;
					default:
						cout << "    UNKNOWN SUB-STATE (STATE_math)\n";
						return 1;
				}
				break;
			case STATE_label:
				if(verbose)
					cout << "  STATE_label\n";

				rt->jump_table[str] = k;
				current_state = STATE_default;
				break;
			case STATE_goto:
				if(verbose)
					cout << "  STATE_goto\n";

				if(IcRuntime_checkJumpTarget(rt, str)) {
					//cout << "    GOOD JUMP TARGET: " << str << endl;
					k = IcRuntime_getJumpTarget(rt, str);
				} else {
					cout << "    Unknown jump target: " << str << endl;
					return 1;
				}
				current_state = STATE_default;
				break;
			case STATE_branch:
				if(verbose)
					cout << "  STATE_branch\n";

				switch(BRANCH_state) {
					case BRANCH_target:
						if(IcRuntime_checkJumpTarget(rt, str))
							rt->branch_target = IcRuntime_getJumpTarget(rt, str);
						else {
							cout << "    Unknown branch target: " << str << endl;
							return 1;
						}
						BRANCH_state = BRANCH_if;
						break;
					case BRANCH_if:
						if(str != IF && str != ":") {
							cout << "    Expecting 'IF': " << str << endl;
							return 1;
						}
						BRANCH_state = BRANCH_lhs;
						break;
					case BRANCH_lhs:
						if(str[0] == '$') {
							if(IcRuntime_checkVariable(rt, str.c_str()+1))
								rt->lhs = IcRuntime_getVariable(rt, str.c_str()+1);
						} else {
							rt->lhs = stod(str);
						}
						BRANCH_state = BRANCH_op;
						break;
					case BRANCH_op:
						if(str == EQUALTO || str == "==") {
							rt->operator__ = IcComp::Equ;
						} else if(str == NEQUALTO || str == "!=") {
							rt->operator__ = IcComp::NotEqu;
						} else if(str == GREATER || str == ">") {
							rt->operator__ = IcComp::GT;
						} else if(str == LESSER || str == "<") {
							rt->operator__ = IcComp::LT;
						} else if(str == GTEQUAL || str == ">=") {
							rt->operator__ = IcComp::GTE;
						} else if(str == LTEQUAL || str == "<=") {
							rt->operator__ = IcComp::LTE;
						} else {
							cout << "    Unknown comparison operator: " << str << endl;
							return 1;
						}
						BRANCH_state = BRANCH_rhs;
						break;
					case BRANCH_rhs:
						if(str[0] == '$') {
							if(IcRuntime_checkVariable(rt, str.c_str()+1)) {
								rt->rhs = IcRuntime_getVariable(rt, str.c_str()+1);
							} else {
								cout << "    Unknown variable in branch comparison: " << str.c_str()+1 << endl;
							} 
						} else {
							rt->rhs = stod(str);
						}
						BRANCH_state = BRANCH_end;
						break;
					case BRANCH_end:
						if(str != ";" && str != END) {
							cout << "    (BRANCH_end) Expecting ';' : " << str << endl;
							return 1;
						}

						if(IcRuntime_evaluateBranch(rt)) {
							if(verbose)
								cout << "    Branch taken: " << rt->branch_target << endl;
							k = rt->branch_target;
						}
						current_state = STATE_default;
						BRANCH_state = BRANCH_target;
						break;
					default:
						cout << "    UNKNOWN SUB-STATE: " << BRANCH_state << endl;
						return 1;
				}

				break;
			case STATE_wait:
				if(verbose)
					cout << "  STATE_wait\n";

				if(str != ";" && str != END) {
					cout << "    Expecting ';': " << str << endl;
					return 1;
				}

				getch(); // pause program for input
				current_state = STATE_default;
				break;
			case STATE_quit:
				if(verbose)
					cout << "  STATE_quit\n";

				cout << "    Quitting...\n";
				return 1;

				break;
			case STATE_runtime:
				if(verbose)
					cout << "  STATE_runtime\n";

				if(str != ";" && str != END) {
					cout << "    Expecting ';': " << str << endl;
					return 1;
				}

				cout << "    Current runtime: " << double(getUsecTimestamp() - start_time)/1000000.0 << " seconds..." << endl;
				current_state = STATE_default;
				break;
			case STATE_call:
				if(verbose)
					cout << "  STATE_call\n";

				if(str[0] == '{') {
					IcRuntime_callImportedFunction(rt, str);
					current_state = STATE_default;
				} else {
					if(IcRuntime_checkJumpTarget(rt, str)) {
						rt->return_address.push_back(k);
						k = IcRuntime_getJumpTarget(rt, str);
						//cout << "    Jump target: " << k << endl;
						current_state = STATE_default;
					} else {
						cout << "    Unknown branch target: " << str << endl;
						return 1;
					}
				}

				break;
			case STATE_return:
				if(verbose)
					cout << "  STATE_return\n";

				k = rt->return_address.back();
				//cout << "    Return target: " << k << endl;
				rt->return_address.pop_back();
				current_state = STATE_default;

				break;
			case STATE_push:
				if(verbose)
					cout << "  STATE_push\n";

				if(str == "END" || str == ";") {
					for(auto d : tmp_push_stack)
						rt->stack_data.push_back(d);
					current_state = STATE_default;
				} else if(str[0] == '$') { // variable
					if(IcRuntime_checkVariable(rt, str.c_str()+1)) {
						tmp_push_stack.push_back(IcRuntime_getVariable(rt, str.c_str()+1));
					} else {
						cout << "    Unknown variable in STATE_push: " << str.c_str()+1 << endl;
					} 
				} else if(str[0] == '@') { // existing stack value
					tmp_push_stack.push_back(IcRuntime_getStackValue(rt, stoi(str.c_str()+1)));
				} else { // push raw data
					tmp_push_stack.push_back(stod(str));
				}

				break;
			case STATE_pop:
				if(verbose)
					cout << "  STATE_pop\n";

				static bool __exit__ = false;

				if(__exit__) {
					__exit__ = false;
					current_state = STATE_default;
				} else {
					int iter = stoi(str);
					for(int i = 0; i < iter; i++)
						rt->stack_data.pop_back();
					__exit__ = true;
				}

				break;
			case STATE_clear:
				if(verbose)
					cout << "    STATE_clear\n";

				break;
			case STATE_import:
				if(verbose)
					cout << "    STATE_import\n";

				switch(IMPORT_state) {
					case IMPORT_name:
						cout << "      IMPORT_name: " << str << endl;
						import_filename = str;
						IMPORT_state = IMPORT_alias;
						break;
					case IMPORT_alias:
						cout << "      IMPORT_alias: " << str << endl;
						import_alias = str;
						IMPORT_state = IMPORT_end;
						break;
					case IMPORT_end:
						cout << "      IMPORT_end: " << str << endl;
						if(str != END && str != ";") {
							cout << "      UNKNOWN TOKEN IN STATE_import: " << str << endl;
							exit(1);
						}

						// valid end token
						IcRuntime_importFile(rt, import_filename, import_alias);

						IMPORT_state = IMPORT_name;
						current_state = STATE_default;
						break;
					default:
						cout << "UNKNOWN SUBSTATE: " << IMPORT_state << endl;
						break;
				}

				break;
			default:
				cout << "  default\n";
				break;
		}
	}
}