#include "../include/capture.h"
#include "../include/back_end.h"
#include "../include/convert.h"
#include "../include/char_array.h"
#include <string>


namespace pnlog {
  constexpr size_type CapTure::buf_size_;

  std::string get_file_name(const char* path) {
    const char* ptr = path;
    size_t index = 0;
    size_t begin = index;
    while(*(ptr + begin) != '\0') {
      if(*(ptr + begin) == '/') {
        index = begin;
      }
      ++begin;
    }
    return std::string(static_cast<const char*>(ptr + index + 1));
  }

  CapTure::CapTure(std::shared_ptr<BackEnd> b, size_type ind)
      :back_(b), default_level_(Level::PN_TRACE), index_(ind), tflag_(time_record::off) {

  }

  void CapTure::close() {
    back_->close(index_);
  }

  void CapTure::log(size_type index, const char* level, const std::string& str) {
    CharArray tmp(buf_size_, -1);
    if (back_->rangecheck(index) == false) {
      fprintf(stderr,"index out of range !");
      std::abort();
    }
    if(tflag_ == time_record::on) {
      tmp.append("[");
      tmp.append(timer::instance().now());
      tmp.append("] ");
    }
    tmp.append("[");
    tmp.append(level);
    tmp.append("] ");
    tmp.append(str.c_str());
    tmp.append("\n");
    if (tmp.error() == true) {
      back_->abort("log is too long!\n");
    }
    back_->write(index, tmp.getBuf(), tmp.getSize());
    if (*level == 'F') { // fatal.
      back_->abort(piece("log fatal from file : ",__FILE__, ", line : ", __LINE__));
    }
  }

  void CapTure::setLevel(Level l) {
    default_level_ = l;
  }

  CapTure::Level CapTure::getLevel() {
    return default_level_;
  }

  CapTure::~CapTure() {

  }
}//namespace pnlog
