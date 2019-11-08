#pragma once
#include "back_end.h"
#include "capture.h"
#include "convert.h"
#include <string>

extern BackEnd backend;
extern CapTure capture;

template<class LastType>
std::string piece(const LastType& last) {
	return convert_to_string(last);
}

template<class  ThisType,class... Types>
std::string piece(const ThisType& tt, const Types... args) {
	std::string tmp = convert_to_string(tt);
	tmp += piece(args...);
	return tmp;
}
