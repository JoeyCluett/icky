#ifndef __JJC__SIMPLE__DATABASE__EXCEPTION__HPP__
#define __JJC__SIMPLE__DATABASE__EXCEPTION__HPP__

#include <exception>

struct SD_Exception : public std::exception {

	std::string msg;

	SD_Exception(std::string msg) {
		this->msg = msg;
	}

	const char* what(void) const noexcept { // std::exception::what overridden
		return msg.c_str();
	}
};

struct SD_NotParsable : public SD_Exception {
	SD_NotParsable(std::string msg) 
			: SD_Exception(msg) {
		this->msg = "SD_NotParsable: ";
		this->msg += msg;
	}
};

struct SD_UnknownState : public SD_Exception {
	SD_UnknownState(std::string msg)
			: SD_Exception(msg) {
		this->msg = "SD_UnknownState: ";
		this->msg += msg;
	}
};

#endif // __JJC__SIMPLE__DATABASE__EXCEPTION__HPP__