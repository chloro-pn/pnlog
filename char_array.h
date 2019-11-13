#pragma once
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "type.h"

class CharArray {
private:
  char* buf_;
  size_type size_;
  size_type end_;//第一个还未被写的位置。
  bool error_;

public:
  explicit CharArray(size_type size = 1024):buf_(nullptr),size_(size),end_(0),error_(false) {
    try {
      buf_ = new char[static_cast<unsigned int>(size)](); //值初始化
    }
    catch (const std::exception& e) {
      printf_s(e.what());
    }
  }

  ~CharArray() {
    delete[] buf_;
  }

  CharArray(const CharArray& other) = delete;

  CharArray& operator=(const CharArray&) = delete;

  CharArray(CharArray&& other) noexcept {
	  buf_ = other.buf_;
	  size_ = other.size_;
	  end_ = other.end_;
	  error_ = other.error_;
	  other.buf_ = nullptr;
  }

  CharArray& operator=(CharArray&& other) noexcept {
	  delete[] buf_;
	  buf_ = other.buf_;
	  size_ = other.size_;
	  end_ = other.end_;
	  error_ = other.error_;
	  other.buf_ = nullptr;

	  return *this;
  }

  void append(const char* ptr, size_type nbyte) {
	  size_type capacity = size_ - 1 - end_;
	  if (capacity < nbyte || error_ == true) {
		  error_ = true;
		  return;
	  }
	  memcpy_s(&buf_[end_], static_cast<rsize_t>(capacity), ptr, static_cast<rsize_t>(nbyte));
	  end_ += nbyte;
	  buf_[end_] = '\0';
    return;
  }

  void append(const char* ptr) {
	  append(ptr, strlen(ptr));
  }

  const char* getBuf() const {
	  return buf_;
  }

  size_type getSize() const {
	  return end_;
  }

    bool error() const {
      return error_;
    }

  void setZero() {
	  end_ = 0;
	  buf_[end_] = '\0';
	  error_ = false;
  }
};