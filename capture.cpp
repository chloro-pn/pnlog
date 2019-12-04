#include "capture.h"
#include "back_end.h"
#include "convert.h"
#include <string>


namespace pnlog {

  constexpr size_type CapTure::buf_size_;

  std::shared_ptr<CapTure> CapTure::get_instance(std::shared_ptr<BackEnd> back) {
    static std::shared_ptr<CapTure> capture(new CapTure(back));
    return capture;
  }

  CapTure::CapTure(std::shared_ptr<BackEnd> b) :back_(b), index_(0) {

  }

  void CapTure::log(size_type index, Level level, size_type line, const char* file, const std::string& str) {
    static thread_local CharArray tmp(buf_size_);
    if (back_->rangecheck(index)) {
      back_->abort("index out of range !\n");
    }
    tmp.append("file: ");
    tmp.append(file);
    tmp.append(" line: ");
    tmp.append(convert_to_string(line).c_str());
    tmp.append(" : ");
    tmp.append(str.c_str());
    tmp.append("\n");
    if (tmp.error() == true) {
      back_->abort("log is too long!\n");
    }
    back_->write(index, tmp.getBuf(), tmp.getSize());
    if (level == Level::PN_FATAL) {
      back_->abort("log fatal!\n");
    }
    tmp.setZero();
  }

  void CapTure::log(size_type index, Level level, const std::string& str) {
    static thread_local CharArray tmp(buf_size_);
    if (back_->rangecheck(index) == false) {
      back_->abort("index out of range !\n");
    }
    tmp.append(str.c_str());
    tmp.append("\n");
    if (tmp.error() == true) {
      back_->abort("log is too long!\n");
    }
    back_->write(index, tmp.getBuf(), tmp.getSize());
    if (level == Level::PN_FATAL) {
      back_->abort("log fatal!\n");
    }
    tmp.setZero();
  }

  void CapTure::setLevel(Level l) {
    default_level_ = l;
  }

  CapTure::Level CapTure::getLevel() {
    return default_level_;
  }

  CapTure::~CapTure() {

  }

  CapTure::Level CapTure::default_level_ = CapTure::Level::PN_DEBUG;
}//namespace pnlog