#include "capture.h"
#include "back_end.h"
#include "convert.h"
#include "char_array.h"
#include <sys/time.h>
#include <string>


namespace pnlog {
  constexpr size_type CapTure::buf_size_;

  thread_local time_t tlast_seconds_ = 0;
  thread_local char tbuf_[64] = {0};

  std::shared_ptr<CapTure> CapTure::get_instance(std::shared_ptr<BackEnd> back) {
    static std::shared_ptr<CapTure> capture(new CapTure(back));
    return capture;
  }

  CapTure::CapTure(std::shared_ptr<BackEnd> b) :back_(b) {

  }

  void CapTure::log(size_type index, const char* level, const std::string& str) {
    static thread_local CharArray tmp(buf_size_, -1);
    if (back_->rangecheck(index) == false) {
      back_->abort("index out of range !\n");
    }
    std::string time = get_current_time(); //too slowly.
    tmp.append("[");
    tmp.append(time.c_str());
    tmp.append("] ");
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
