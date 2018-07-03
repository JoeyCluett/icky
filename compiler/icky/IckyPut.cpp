#include "Icky.h"
#include "IckyRuntime.h"
#include <iostream>
#include <string>

// ==================================================================
// put data into runtime
// ==================================================================

int IckyAsm::put::_string(IckyRuntimeData* ird, std::string name, std::string value) {
	// dont put the same data in twice
	if(ird->_std_var_string_index.find(name) 
			== ird->_std_var_string_index.end())
		throw new IckyException("IckyAsm::put::_string() attempting to repeat data in runtime...");

	// this is important part
	int index = ird->_std_var_string.size();
	
	// assemble the string in memory
	for(auto c : value) { ird->_std_var_string.push_back(c); }
	ird->_std_var_string.push_back('\0'); // null-terminator

	// set the index so it can be found later
	ird->_std_var_string_index[name] = index;
	return index;
}

int IckyAsm::put::_double(IckyRuntimeData* ird, std::string name, double value) {
	if(ird->_std_var_double_index.find(name) 
			== ird->_std_var_double_index.end())
		throw new IckyException("IckyAsm::put::_double() attempting to repeat data in runtime...");

	int index = ird->_std_var_double.size();
	ird->_std_var_double.push_back(value);
	ird->_std_var_double_index[name] = index;
	return index;
}

int IckyAsm::put::_integer(IckyRuntimeData* ird, std::string name, long value) {
	if(ird->_std_var_integer_index.find(name) 
			== ird->_std_var_integer_index.end())
		throw new IckyException("IckyAsm::put::_integer() attempting to repeat data in runtime...");

	int index = ird->_std_var_integer.size();
	ird->_std_var_integer.push_back(value);
	ird->_std_var_integer_index[name] = index;
	return index;
}

int IckyAsm::put::_string(IckyRuntimeData* ird, std::string value) {
	// this is important part
	int index = ird->_std_var_string.size();
	
	// assemble the string in memory
	for(auto c : value) { ird->_std_var_string.push_back(c); }
	ird->_std_var_string.push_back('\0'); // null-terminator

	return index;
}

int IckyAsm::put::_double(IckyRuntimeData* ird, double value) {
	int index = ird->_std_var_double.size();
	ird->_std_var_double.push_back(value);
	return index;
}

int IckyAsm::put::_integer(IckyRuntimeData* ird, long value) {
	int index = ird->_std_var_integer.size();
	ird->_std_var_integer.push_back(value);
	return index;
}
