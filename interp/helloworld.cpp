#include <iostream>
#include "IcRuntime.hpp"

using namespace std;

void sayhello(IcRuntime* icr) {
	std::cout << "Hello World" << std::endl;
}

void saybye(IcRuntime* icr) {
	std::cout << "Good bye" << std::endl;
}

void print_from_c(IcRuntime* icr) {
	std::cout << "PRINTING WITHIN C/CPP FUNCTION" << std::endl;
	IcRuntime_printRuntimeData(icr);
	std::cout << "RETURNING FROM C/CPP FUNCTION" << std::endl;
}

static IcImportFunction arr[] {
	{"sayhello", sayhello},
	{"saybye", saybye},
	{"printc", print_from_c},
	{NULL, NULL}
};

extern "C" IcImportFunction* get_ic_functions(void) {
	return arr;
}
