#pragma once
#include <cstdio>
#include <string>
#include "type.h"
//将缓冲块也做到输出流中么？这样貌似不太好，控制不了输出流的输出模式：例如立即输出还是缓存输出。
class OutStream {
private:
	FILE* file_;
	bool opened_;

public:
	OutStream();

	void open(std::string filename);

	void setSTDOUT();

	void setSTDERR();

	bool opened() const;

	void write(const char* ptr, size_type n);

	void close();

	~OutStream();
};