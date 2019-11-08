#pragma once
#include "type.h"

class out_stream_base {
public:
	virtual void write(const char* ptr, size_type n) = 0;
	virtual void close() = 0;
};