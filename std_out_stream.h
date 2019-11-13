#pragma once
#include "out_stream_base.h"
#include "type.h"
#include <cstdio>

namespace pnlog {
  class StdOutStream : public out_stream_base {
  public:
    StdOutStream(FILE* f);

    void write(const char* ptr, size_type n) override;

    ~StdOutStream();

  private:
    FILE * file_;
  };
}//namespace pnlog