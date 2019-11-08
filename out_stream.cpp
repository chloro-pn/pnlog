#include "out_stream.h"
#include <iostream>

OutStream::OutStream():file_(nullptr),opened_(false) {

}

void OutStream::open(std::string filename) {
    if (file_ != nullptr) {
        fprintf_s(stderr, "open opened file!");
        exit(-1);
    }
	if (fopen_s(&file_, filename.c_str(), "w") != 0) {
		fprintf_s(stderr, "file open error!");
		exit(-1);
	}
}

void OutStream::setSTDOUT() {
	if (file_ != nullptr) {
    fprintf_s(stderr, "open opened file!");
    exit(-1);
	}
	file_ = stdout;
}

void OutStream::setSTDERR() {
  if (file_ != nullptr) {
    fprintf_s(stderr, "open opened file!");
    exit(-1);
  }
	file_ = stderr;
}

void OutStream::write(const char* ptr, size_type n) {
	if (file_ == nullptr) {
		fprintf_s(stderr, "write before open!");
		exit(-1);
	}
	fwrite(ptr, static_cast<size_t>(n), static_cast<size_t>(1), file_);
	fflush(file_);
}

bool OutStream::opened() const {
	return file_ != nullptr;
}

void OutStream::close() {
	if (file_ != nullptr) {
		fclose(file_);
		file_ = nullptr;
	}
}

OutStream::~OutStream() {
	close();
}