#ifndef __JJC__IC__RUNTIME__HPP__
#define __JJC__IC__RUNTIME__HPP__

#include <iostream>
#include <string>
#include <map> // map<string, double>
#include <cmath>
#include <SimpleDatabase.hpp>

#include <dlfcn.h> // dlopen, dlsym. required to extend IcScript with C/C++

// KEYWORDS
const std::string VARIABLE = "VARIABLE"; // >>
const std::string EQU      = "EQU";      // =
const std::string PRINT    = "PRINT";    // <
const std::string MATH     = "MATH";     // :=
const std::string LABEL    = "LABEL";    // ::
const std::string END      = "END";      // ;
const std::string GOTO     = "GOTO";     // .. unconditional jump
const std::string BRANCH   = "BRANCH";   // . conditional jump
const std::string CALL     = "CALL";     // call with return value
const std::string RETURN   = "RETURN";   // return to saved address
const std::string PUSH     = "PUSH";     // push onto data stack
const std::string POP      = "POP";      // pop from data stack
const std::string CLEAR    = "CLEAR";    // delete a variable from the runtime
const std::string IMPORT   = "IMPORT";   // import a file containing one or more 

const std::string IF       = "IF";   // : self-explanatory
const std::string QUIT     = "QUIT"; // -q ends execution immediately

// math keywords
const std::string ADD      = "ADD";      // +
const std::string SUBTRACT = "SUBTRACT"; // -
const std::string MULTIPLY = "MULTIPLY"; // *
const std::string DIVIDE   = "DIVIDE";   // /
const std::string EXPONENT = "EXPONENT"; // ^

// comparison operators
const std::string EQUALTO  = "EQ";  // ==
const std::string NEQUALTO = "NEQ"; // != 
const std::string GREATER  = "GT";  // >
const std::string LESSER   = "LT";  // <
const std::string GTEQUAL  = "GTE"; // >=
const std::string LTEQUAL  = "LTE"; // <=

// special runtime commands
const std::string EXPOSE   = "EXPOSE";  // -e print runtime info
const std::string NEWLINE  = "NEWLINE"; // \n
const std::string SPC      = "SPC";     // \s
const std::string RUNTIME  = "RUNTIME"; // -r how long has the program been running?
const std::string WAIT     = "WAIT";    // -w wait for keyboard press

enum class IcOps : int {
	Mult = 0, Divide = 1, 
	Add = 2, Subtract = 3,
	Exp = 4,
};

enum class IcComp : int {
	Equ = 0, NotEqu = 1,
	GT = 2, LT = 3,
	GTE = 4, LTE = 5
};

std::ostream& operator<<(std::ostream& os, IcOps op) {
	switch(op) {
		case IcOps::Mult:
			os << '*'; break;
		case IcOps::Divide:
			os << '/'; break;
		case IcOps::Add:
			os << '+'; break;
		case IcOps::Subtract:
			os << '-'; break;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, IcComp op) {
	switch(op) {
		case IcComp::Equ:
			os << "=="; break;
		case IcComp::NotEqu:
			os << "!="; break;
		case IcComp::GT:
			os << ">"; break;
		case IcComp::LT:
			os << "<"; break;
		case IcComp::GTE:
			os << ">="; break;
		case IcComp::LTE:
			os << "<="; break;
	}
	return os;
}

// forward declaration
struct IcRuntime;
typedef void(*IC_callback)(IcRuntime*);

struct IcRuntime {
	std::map<std::string, double> vars;

	// math operations need this info
	std::string result;
	std::vector<double> operands;
	std::vector<IcOps> operations;

	// jumps
	std::map<std::string, int> jump_table;

	// branch info
	double lhs = 0.0;
	double rhs = 0.0;
	IcComp operator__;
	int branch_target;

	// call/return data
	std::vector<int> return_address;
	std::vector<double> stack_data;

	// import function info
	std::map<std::string, void*> dl_ptrs;
	std::map<std::string, IC_callback> import_functions;
};

struct IcImportFunction {
	const char* __tag;
	IC_callback __cb;

	friend std::ostream& operator<<(std::ostream& os, IcImportFunction& ic) {
		os << "Tag: " << ic.__tag << "\nCallback: " << (void*)ic.__cb << "\n";
		return os;
	}
};

typedef IcImportFunction*(*get_ic_function)(void);

void IcRuntime_importFile(IcRuntime* ic, std::string filename, std::string alias) {
	{
		std::string b = "{";
		alias = b + alias + "}";
	}

	//std::cout << "      Alias token: " << alias << std::endl;

	std::string prefix = "./";

	void* handle = dlopen((prefix + filename + ".so").c_str(), RTLD_NOW);
	if(!handle) {
		std::cout << "      ERROR opening .so file\n        " << dlerror() << std::endl;
		return;
	}

	ic->dl_ptrs[filename] = handle;

	IcImportFunction* ic_ptr = NULL;
	{
		get_ic_function f_ptr = (get_ic_function)dlsym(handle, "get_ic_functions");
		ic_ptr = f_ptr();
	}

	// iterate through and extract the function ptrs
	for(int i = 0; ic_ptr[i].__tag != NULL; i++) {
		//std::cout << ic_ptr << std::endl; // for debug purposes

		std::string new_tag = alias + ic_ptr[i].__tag;
		ic->import_functions[new_tag] = ic_ptr[i].__cb;
	}
}

void IcRuntime_callImportedFunction(IcRuntime* ic, std::string function_name) {
	ic->import_functions[function_name](ic);
}

double IcRuntime_getStackValue(IcRuntime* ic, int stack_index) {
	double* last = &ic->stack_data.back();
	return *(last - stack_index);
}

void IcRuntime_findBranchTargets(IcRuntime* ic, StringVector* sv) {

	const int FIND_default = 0;
	const int FIND_print   = 1;
	const int FIND_label   = 2;

	int FIND_state = FIND_default;

	for(int i = 0; i < sv->size(); i++) {
		std::string str = sv->at(i);
		switch(FIND_state) {
			case FIND_default:
				if(str == PRINT) {
					FIND_state = FIND_print;
				} else if(str == LABEL || str == "::") {
					FIND_state = FIND_label;
				}
				break;
			case FIND_print:
				if(str == ";") {
					FIND_state = FIND_default;
				}
				break;
			case FIND_label:
				ic->jump_table[str] = i;
				FIND_state = FIND_default;
				break;
			default:
				std::cout << "    (IcRuntime_findBranchTargets): Unknown state: " << FIND_state << std::endl;
				exit(1);
		}
	}
}

bool IcRuntime_evaluateBranch(IcRuntime* ic) {

	//std::cout << "Evaluating: " << ic->lhs << ic->operator__ << ic->rhs << std::endl;

	switch(ic->operator__) {
		case IcComp::Equ:
			return ic->lhs == ic->rhs; break;
		case IcComp::NotEqu:
			return ic->lhs != ic->rhs; break;
		case IcComp::GT:
			return ic->lhs > ic->rhs; break;
		case IcComp::LT:
			return ic->lhs < ic->rhs; break;
		case IcComp::GTE:
			return ic->lhs >= ic->rhs; break;
		case IcComp::LTE:
			return ic->lhs <= ic->rhs; break;
		default:
			std::cout << "    Unknown operator: " << ic->operator__ << std::endl;
			exit(1);
	}
}

void IcRuntime_evaluateOperation(IcRuntime* ic) {
	double start = ic->operands[0];

	for(int i = 1; i < ic->operands.size(); i++) {
		switch(ic->operations[i-1]) {
			case IcOps::Mult:
				start = start * ic->operands[i];
				break;
			case IcOps::Divide:
				start = start / ic->operands[i];
				break;
			case IcOps::Add:
				start = start + ic->operands[i];
				break;
			case IcOps::Subtract:
				start = start - ic->operands[i];
				break;

			case IcOps::Exp:
				start = pow(start, ic->operands[i]);
				break;

			default:
				std::cout << "Unknown operation: " << ic->operations[i-1] << std::endl;
				exit(1);
		}
	}

	ic->vars[ic->result] = start;

	// this is performed by the main state machine instead
	//ic->operands.clear();
	//ic->operations.clear();
}

void IcRuntime_printRuntimeData(IcRuntime* ic) {
	std::cout << "\n===== Runtime Data =====\n\n";
	
	std::cout << "  Variables:\n";
	for(auto& v : ic->vars)
		std::cout << "    " << v.first << " : " << v.second << std::endl;
	std::cout << std::endl;

	std::cout << "  Most recent operation:\n";
	std::cout << "    Result: " << ic->result << std::endl;
	std::cout << "    Operands: ";
	for(auto c : ic->operands)
		std::cout << c << ' ';
	std::cout << std::endl;
	std::cout << "    Operations: ";
	for(auto c : ic->operations)
		std::cout << c << ' ';
	std::cout << std::endl;

	std::cout << "\n  Jump table entries:\n";
	for(auto& v : ic->jump_table) {
		std::cout << "    " << v.first << " : " << v.second << std::endl;
	}
	std::cout << std::endl;

	std::cout << "  Call stack:\n";
	for(auto i : ic->return_address)
		std::cout << "    " << i << std::endl;
	std::cout << std::endl;

	std::cout << "  Data stack (" << ic->stack_data.size() << "):\n";
	for(auto d : ic->stack_data)
		std::cout << "    " << d << std::endl;
	std::cout << std::endl;

	std::cout << "  Imported function table:\n";
	for(auto f : ic->import_functions)
		std::cout << "    " << f.first << ", ADDR: " << (void*)f.second << std::endl;
	std::cout << std::endl;

	std::cout << "========================\n" << std::endl;
}

IcRuntime* IcRuntime_new(void) {
	return new IcRuntime;
}

void IcRuntime_addVariable(IcRuntime* ic, std::string var_name, double value) {
	ic->vars[var_name] = value;
}

void IcRuntime_addVariable(IcRuntime* ic, std::string var_name) {
	IcRuntime_addVariable(ic, var_name, 0.0);
}

bool IcRuntime_checkVariable(IcRuntime* ic, std::string var_name) {
	if(ic->vars.find(var_name) == ic->vars.end())
		return false;
	return true;
}

bool IcRuntime_checkJumpTarget(IcRuntime* ic, std::string target) {
	if(ic->jump_table.find(target) == ic->jump_table.end())
		return false;
	return true;
}

int IcRuntime_getJumpTarget(IcRuntime* ic, std::string target) {
	return ic->jump_table[target];
}

double IcRuntime_getVariable(IcRuntime* ic, std::string var_name) {
	return ic->vars[var_name];
}

#endif // __JJC__IC__RUNTIME__HPP__