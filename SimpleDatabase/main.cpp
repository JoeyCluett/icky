#include <iostream>
#include "SimpleDatabase.hpp"
#include "SimpleDatabaseException.hpp"

#include <list>

using namespace std;

int main(int argc, char* argv[]) {

	StringVector* sv = 0;
	
	try {
		sv = SimpleDatabase::parseLine("mytext.txt");
	} catch(SD_Exception* up) {
		cout << up->what() << endl;
		throw up; // lol
	}

	StringVector& sv_ref = *sv;

	// print each string on its own line
	for(auto& str : sv_ref)
		cout << str << endl;

	// print dividing line
	cout << "===============================================\n";

	StringList* sl = NULL;

	try {
		sl = SimpleDatabase::parseString("mytext.txt");
	} catch(SD_Exception* up) {
		cout << up->what() << endl;
		throw up;
	}

	for(auto& v : *sl) {
		for(auto& str : v) {
			cout << str << ' ';
		}
		cout << endl;
	}

	return 0;
}