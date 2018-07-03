#ifndef __JJC__SIMPLE__DATABASE__HPP__
#define __JJC__SIMPLE__DATABASE__HPP__

#include <vector>
#include <string>
#include <iostream> // cout and friends
#include <fstream>
#include <list>
#include <sys/stat.h> // fstat()

#include "SimpleDatabaseException.hpp"

typedef std::vector<std::string> StringVector;
typedef std::list<StringVector> StringList;

std::ostream& operator<<(std::ostream& os, StringVector& sv) {
	for(auto& s : sv)
		os << s << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, StringList& sl) {
	for(auto& sv : sl) {
		for(auto& str : sv) {
			os << str << ' ';
		}
		os << std::endl;
	}
	return os;
}

namespace SimpleDatabase {
	// limit to file scope
	static bool isWhitespace(char c) {
		switch(c) {
			case ' ':
			case '\n':
			case '\t':
				return true;
		}
		return false;
	}

	// array of strings
	StringVector* parseLine(std::string filename) {
		std::ifstream is(filename, std::ifstream::in | std::ifstream::binary);
		int length = 0;

		if(is) {
			// get length of file:
		    is.seekg (0, is.end);
		    length = is.tellg();
		    is.seekg (0, is.beg); // hop back to beginning
		}

		std::vector<char> v;
		v.resize(length, '\0');
		is.read(&v[0], length);

		//v.push_back('\0');
		//std::cout << &v[0] << std::endl;

		StringVector* sv = new StringVector();

		const int STATE_default = 0;
		const int STATE_second_comment_slash = 1;
		const int STATE_in_comment           = 2;

		std::string str = "";
		int current_state = STATE_default;

		// guided by state machine
		for(auto c : v) {
			switch(current_state) {
				case STATE_default:
					if(c == '/') {
						current_state = STATE_second_comment_slash;
					} else if(c == '\n') {
						sv->push_back(str);
						str = "";
					} else {
						str.push_back(c);
					}
					break;
				case STATE_second_comment_slash:
					if(c == '/') {
						sv->push_back(str);
						str = "";
						current_state = STATE_in_comment;
					} else { // not a comment, just a lone forward slash
						str += '/';
						str += c;
						current_state = STATE_default;
					}
					break;
				case STATE_in_comment:
					if(c == '\n')
						current_state = STATE_default;
					break;
				default:
					throw new SD_UnknownState(std::to_string(current_state));
					break;
			}
		}

		is.close();
		return sv;
	}

	StringList* parseString(std::string filename) {
		StringVector* sv = parseLine(filename);
		StringList* sl = new StringList();

		std::string tmp_str = "";

		for(std::string& str : *sv) {
			sl->push_back(StringVector());
			
			const int STATE_default    = 0;
			const int STATE_whitespace = 1;
			const int STATE_in_string  = 2;

			int current_state = STATE_default;

			for(char c : str) {
				switch(current_state) {
					case STATE_default:
						if(isWhitespace(c)) {
							current_state = STATE_whitespace;
						} else {
							tmp_str += c;
							current_state = STATE_in_string;
						}
						break;
					case STATE_whitespace:
						if(!isWhitespace(c)) {
							tmp_str = "";
							tmp_str += c;
							current_state = STATE_in_string;
						}
						break;
					case STATE_in_string:
						if(isWhitespace(c)) {
							(*(--sl->end())).push_back(tmp_str);
							tmp_str = "";
							current_state = STATE_whitespace;
						} else {
							tmp_str += c;
						}
						break;
					default:
						throw new SD_UnknownState(std::to_string(current_state));
						break;
				}
			}

			if(current_state == STATE_in_string && tmp_str.length() > 0) {
				(*(--sl->end())).push_back(tmp_str);
			}
			tmp_str = "";

		}

		delete sv;
		return sl;
	}

	StringVector* alignStringVector(std::string filename) {
		StringList* sl = parseString(filename);
		StringVector* sv = new StringVector;

		for(auto v : *sl) {
			for(auto s : v) {
				sv->push_back(s);
			}
		}

		delete sl;
		return sv;
	}

}; // namespace SimpleDatabase

#endif // __JJC__SIMPLE__DATABASE__HPP__