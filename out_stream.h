#pragma once
#include <cstdio>
#include <string>
#include "type.h"
//�������Ҳ�����������ô������ò�Ʋ�̫�ã����Ʋ�������������ģʽ����������������ǻ��������
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