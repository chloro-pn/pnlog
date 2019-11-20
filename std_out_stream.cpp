#include "std_out_stream.h"
#include <cstdio>

namespace pnlog {
  StdOutStream::StdOutStream(FILE* f) :file_(f) {

  }

  void StdOutStream::write(const char* ptr, size_type n) {
    fprintf(file_, ptr);
  }

  void StdOutStream::close() {

  }

  StdOutStream::~StdOutStream() {

  }
}//namespace pnlog