#ifndef __JJC__ICKY__RUNTIME__H__
#define __JJC__ICKY__RUNTIME__H__

#include <string>
#include <iostream>
#include <map>
#include <exception>
#include <vector>

class IckyException : public std::exception {
public:
	std::string msg;
	IckyException(std::string msg) { this->msg = msg; }
	const char* what(void) const noexcept { return msg.c_str(); }

	friend std::ostream& operator<<(std::ostream& os, IckyException& ie) {
		os << "IckyException: " << ie.msg;
		return os;
	}
};

namespace IckyOpCode {
	const uint8_t printString       = 0; // <0> <4-byte index into string table>
	const uint8_t printDouble       = 1; // <1> <4-byte index into double table>
	const uint8_t printInteger      = 2; // <2> <4-byte index into integer table>
	const uint8_t printCharacter    = 3; // <3> <single character to print out>
	const uint8_t unconditionalJump = 4; // <4> <4-byte index into jump table>
	const uint8_t loadDoubleVar     = 5; // <5> <4-byte src> <4-byte dest>

} // end of namespace IckyOpCode

union LargeRegister {
	long i;
	double f;
};

/*
	This struct is responsible for holding all data 
	associated with a given instance of an IckyVM
*/
struct IckyRuntimeData {
	// useful for the compiler. nothing more
	bool verbose = false;

	// variable raw data storage
	std::vector<double> _std_var_double;
	std::vector<long>   _std_var_integer;
	std::vector<char>   _std_var_string;

	std::map<std::string, int> _std_var_double_index;
	std::map<std::string, int> _std_var_string_index;
	std::map<std::string, int> _std_var_integer_index;

	// index tables for GOTOs, BRANCHes, CALLs, LABELs
	std::vector<int> _jump_table; // this is checked after compilation run, to verify all entries have been filled
	std::map<std::string, int> _jump_table_index;

	// storage for immediate operands
	LargeRegister aReg;
	LargeRegister bReg;

	// byte codes are placed here prior to being executed
	std::vector<uint8_t> _asm_ops;
	int _instruction_ptr = 0;
	int _instruction_size = 0; // incremented as individual instructions are added
};

#endif // __JJC__ICKY__RUNTIME__H__
