#include "Icky.h"
#include "IckyRuntime.h"
#include <SimpleDatabase.hpp>
#include <iostream>
#include <string>

void IckyAsm::execute(IckyRuntimeData* ird, std::string filename) {
	std::cout << "Filename: " << filename << std::endl;
	auto* str_vec = SimpleDatabase::alignStringVector(filename);

	int str_vec_size = str_vec->size();
	for(int k = 0; k < str_vec_size; k++) {
		std::string& str = str_vec->at(k); // bounds checking with at()
		
		// lets see the string
		std::cout << str << std::endl;

		// the main compilation loop

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
