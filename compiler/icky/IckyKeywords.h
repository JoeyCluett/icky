#ifndef __JJC__ICKY__KEYWORDS__H__
#define __JJC__ICKY__KEYWORDS__H__

namespace IckyKeyword {

	// ==============================================================
	// all of the same keywords as IcScript
	// ==============================================================

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
	const std::string IMPORT   = "IMPORT";   // import a file containing one or more C/C++ functions

	const std::string IF       = "IF";   // : self-explanatory
	const std::string QUIT     = "QUIT"; // -q ends execution immediately

	// math keywords
	const std::string ADD      = "ADD";      // +
	const std::string SUBTRACT = "SUBTRACT"; // -
	const std::string MULTIPLY = "MULTIPLY"; // *
	const std::string DIVIDE   = "DIVIDE";   // /
	const std::string EXPONENT = "EXPONENT"; // ^

	// comparison operators
	const std::string EQUALTO  = "-EQ";  // ==
	const std::string NEQUALTO = "-NEQ"; // != 
	const std::string GREATER  = "-GT";  // >
	const std::string LESSER   = "-LT";  // <
	const std::string GTEQUAL  = "-GTE"; // >=
	const std::string LTEQUAL  = "-LTE"; // <=

	// special runtime commands
	const std::string EXPOSE   = "EXPOSE";  // -e print runtime info
	const std::string NEWLINE  = "NEWLINE"; // \n
	const std::string SPC      = "SPC";     // \s
	const std::string RUNTIME  = "RUNTIME"; // -r how long has the program been running?
	const std::string WAIT     = "WAIT";    // -w wait for keyboard press

	// ==============================================================
	// additional keywords added in the new IcScript standard
	// ==============================================================

	const std::string STRING    = "STRING";    // make a string variable with specific name
	const std::string DOUBLE    = "DOUBLE";    // make a double precision floating point variable
	const std::string INTEGER   = "INTEGER";   // make an integer variable
	const std::string PROCEDURE = "PROCEDURE"; // self-contained function

} // end of namespace IckyKeyword

#endif // __JJC__ICKY__KEYWORDS__H__