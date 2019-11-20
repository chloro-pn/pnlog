#include "file_out_stream.h"
#include <cstdlib>

namespace pnlog {
  FileOutStream::FileOutStream(std::string filename) :file_(nullptr),closed_(false) {
    file_ = fopen( filename.c_str(), "w");
    if (file_ == nullptr) {
      fprintf(stderr, "file open error!");
      ::abort();
    }
  }

  void FileOutStream::write(const char* ptr, size_type n) {
    if (file_ == nullptr) {
      fprintf(stderr, "write before open!");
      ::abort();
    }
    fwrite(ptr, static_cast<size_t>(n), static_cast<size_t>(1), file_);
    fflush(file_);
  }

  void FileOutStream::close() {
    if (closed_ == false) {
      fclose(file_);
      closed_ = true;
    }
  }

  FileOutStream::~FileOutStream() {
    close();
  }
}//namesapce pnlog