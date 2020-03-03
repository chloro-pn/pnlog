#pragma once
#include <memory> // std::shared_ptr
#include <vector> // std::vector
#include "char_array.h"
#include "spin_lock.h"
#include "condition_variable_type.h"
#include <mutex>
#include <cassert>

namespace pnlog {
class CharArrayAllocator {
private:
  std::vector<std::shared_ptr<CharArray>> bufs_; //unused bufs.
  int size_; // total bufs size, including unused and using bufs.
  static constexpr int max_size_ = 16; // max size_.

  using lock_type = spin;
  lock_type m_;
  typename condition_variable_type<lock_type>::type cv_;

public:
  CharArrayAllocator():size_(0) {

  }

  std::shared_ptr<CharArray> apply(int index) {
    std::lock_guard<lock_type> mut(m_);
    if(bufs_.empty() == true) {
      if (size_ < max_size_) {
        bufs_.emplace_back(new CharArray(4096, index));
        ++size_;
      }
      else {
        cv_.wait(mut, [this]()->bool {
                   return !bufs_.empty();
                 });
      }
    }
    std::shared_ptr<CharArray> result = bufs_.back();
    bufs_.pop_back();
    result->index_ = index;
    return result;
  }

  void give_back(std::shared_ptr<CharArray> item) {
    std::lock_guard<lock_type> mut(m_);
    item->index_ = -1;
    bufs_.push_back(item);
  }

  ~CharArrayAllocator() {
    assert(size_ == static_cast<int>(bufs_.size()));
  }
};
}
