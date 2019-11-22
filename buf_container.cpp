#include "buf_container.h"
#include <cassert>

namespace pnlog {

  BufContainer::BufContainer(size_type max) :bufs_(), max_size_(max) {

  }

  bool BufContainer::empty() const {
    return bufs_.empty();
  }

  bool BufContainer::full() const {
    return bufs_.size() >= max_size_;
  }

  CharArray BufContainer::get_first_buf() {
    assert(empty() == false);
    CharArray result = std::move(bufs_.front());
    bufs_.pop_front();
    return result;
  }

  //CharArray can only be moved.
  void BufContainer::push_buf(CharArray&& buf) {
    assert(full() == false);
    bufs_.push_back(std::move(buf));
  }

  BufContainer::~BufContainer() {

  }
}//namespace pnlog