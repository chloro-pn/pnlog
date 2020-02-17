#include "../include/std_out_stream.h"
#include <cstdio>
#include <cassert>

namespace pnlog {
  StdOutStream::StdOutStream(FILE* f) :file_(f) {
    assert(f != nullptr);
  }

  void StdOutStream::write(const char* ptr, size_type n) {
    auto code = fprintf(file_, ptr);
    assert(code >= 0);
  }

  void StdOutStream::close() {
    //do nothing.pnlog would not close stdout.
  }

  StdOutStream::~StdOutStream() {

  }
}//namespace pnlog
