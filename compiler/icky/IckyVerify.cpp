#include "Icky.h"
#include "IckyRuntime.h"
#include <iostream>
#include <string>

// ==================================================================
// verify data in the runtime
// if verification fails, throw IckyException with 
// explanation
// on success, the index for the data is returned
// ==================================================================
int IckyAsm::verify::_string(IckyRuntimeData* ird, std::string name) {
	if(ird->_std_var_string_index.find(name) 
			== ird->_std_var_string_index.end()) {
		std::string except = "String [";
		except = except + name + "] does not exist in string variable index table";
		throw new IckyException(except);
	} else {
		return ird->_std_var_string_index[name];
	}
}

int IckyAsm::verify::_double(IckyRuntimeData* ird, std::string name) {
	if(ird->_std_var_double_index.find(name) 
			== ird->_std_var_double_index.end()) {
		std::string except = "Double [";
		except = except + name + "] does not exist in double variable index table";
		throw new IckyException(except);
	} else {
		return ird->_std_var_double_index[name];
	}
}
/*
int IckyAsm::verify::_integer(IckyRuntimeData* ird, std::string name) {
	if(ird->_std_var_integer_index.find(name) == ird->_std_var_integer_index.end()) {
		std::string except = "Integer [";
		except = except + name + "] does not exist in integer variable index table";
		throw new IckyException(except);
	} else {
		return ird->_std_var_integer_index[name];
	}
}
*/
int IckyAsm::verify::_jumpDest(IckyRuntimeData* ird, std::string name) {
	if(ird->_jump_table_index.find(name) == ird->_jump_table_index.end()) {
		std::string except = "Jump destination [";
		except = except + name + "] does not exist in jump index table";
		throw new IckyException(except);
	} else {
		return ird->_jump_table_index[name];
	}
}
