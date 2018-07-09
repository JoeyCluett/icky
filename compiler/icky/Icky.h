#ifndef __JJC__ICKY__H__
#define __JJC__ICKY__H__

#include "IckyRuntime.h"
#include <string>

/*
	Most of the functions here are responsible for assembling
	a given program into IcScript Bytecodes.
	The IckyVM then executes these byte codes much faster 
	than the previous version which was pretty fast for 
	a string-based interpreter
*/
namespace IckyAsm {

	/*
		Executes a precompiled bytecode program
	*/
	void execute(IckyRuntimeData* ird);

	/*
		Compiles the given program into a bytecode program
	*/
	void compile(IckyRuntimeData* ird, std::string filename);

	/*
		calls the two functions (compile(), execute()) above
	*/
	void run(IckyRuntimeData* ird, std::string filename);

	/*
		Disassemble the binary output in the IckyRuntimeData _asm_opcode vector
	*/
	void dasm(IckyRuntimeData* ird);

	/*
		Not a precise measurement but a relatively accurate
		measurement. Useful for runtime observation
	*/
	int runtimeSize(IckyRuntimeData* ird);

	/*
		Places various bits of info into the runtime, generates indices
		for doubles, strings, and integers
	*/
	namespace put {
		int _string(IckyRuntimeData* ird, std::string name, std::string value);
		int _integer(IckyRuntimeData* ird, std::string name, long value);
		int _double(IckyRuntimeData* ird, std::string name, double value);

		// sometimes it is necessary to store data representing different constants without names
		// no questions are asked about these variables
		int _string(IckyRuntimeData* ird, std::string value);
		int _integer(IckyRuntimeData* ird, long value);
		int _double(IckyRuntimeData* ird, double value);
	} // end of namespace put

	/*
		Verify that certain pieces of data exist in the runtime
		On success, they return the index into the proper
		data vector
	*/
	namespace verify {
		int _string(IckyRuntimeData* ird, std::string name);
		int _integer(IckyRuntimeData* ird, std::string name);
		int _double(IckyRuntimeData* ird, std::string name);
		int _jumpDest(IckyRuntimeData* ird, std::string name);
	} // end of namespace verify

	/*
		Print assembler functions, generate op codes (0, 1, 2) <integer index>
	*/
	void printString(IckyRuntimeData* ird,  std::string name);        // <0x00>
	void printDouble(IckyRuntimeData* ird,  std::string name);        // <0x01>
	void printInteger(IckyRuntimeData* ird, std::string name);        // <0x02>
	void printStringLiteral(IckyRuntimeData* ird, std::string value); // <0x00> string is put in the runtime when this function is called
	void printCharacterLiteral(IckyRuntimeData* ird, char c);         // <0x03>
	void unconditionalJump(IckyRuntimeData* ird, std::string dest);   // <0x04>
	void loadDoubleFromLiteral(IckyRuntimeData* ird, std::string dest, int src); // <5>

} // end of namespace Icky

#endif // __JJC__ICKY__H__