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
	const uint8_t printString    = 0; // <0> <4-byte index into string table>
	const uint8_t printDouble    = 1; // <1> <4-byte index into double table>
	const uint8_t printInteger   = 2; // <2> <4-byte index into integer table>
	const uint8_t printCharacter = 3; // <3> <single character to print out>
} // end of namespace IckyOpCode

/*
	This struct is responsible for holding all data 
	associated with a given instance of an IckyVM
*/
struct IckyRuntimeData {
	// variable raw data storage
	std::vector<double> _std_var_double;
	std::vector<long>   _std_var_integer;
	std::vector<char>   _std_var_string;

	std::map<std::string, int> _std_var_double_index;
	std::map<std::string, int> _std_var_string_index;
	std::map<std::string, int> _std_var_integer_index;

	// byte codes are placed here prior to being executed
	std::vector<uint8_t> _asm_ops;
	int _instruction_ptr = 0;
	int _instruction_size = 0; // incremented as individual instructions are added
};

#endif // __JJC__ICKY__RUNTIME__H__
