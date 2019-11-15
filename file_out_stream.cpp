#include "file_out_stream.h"
#include "platform.h"
#include <stdlib.h>

namespace pnlog {
  FileOutStream::FileOutStream(std::string filename) :file_(nullptr) {
    if (fopen_s(&file_, filename.c_str(), "w") != 0) {
      pf::fprintf(stderr, "file open error!");
      ::abort();
    }
  }

  void FileOutStream::write(const char* ptr, size_type n) {
    if (file_ == nullptr) {
      pf::fprintf(stderr, "write before open!");
      ::abort();
    }
    pf::fwrite(ptr, static_cast<size_t>(n), static_cast<size_t>(1), file_);
    pf::fflush(file_);
  }

  FileOutStream::~FileOutStream() {

  }
}//namesapce pnlog