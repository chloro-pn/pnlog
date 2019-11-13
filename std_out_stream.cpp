#include "std_out_stream.h"
#include "platform.h"
#include <cstdio>

namespace pnlog {
  StdOutStream::StdOutStream(FILE* f) :file_(f) {

  }

  void StdOutStream::write(const char* ptr, size_type n) {
    //fprintf_s(file_, ptr);
    pf::fprintf(file_, ptr);
  }


  StdOutStream::~StdOutStream() {

  }
}//namespace pnlog