#include "buf_container.h"
#include "out_stream_base.h"
#include "platform.h"

BufContainer::BufContainer() :the_first_clean_(1), 
                              inited_(false), 
                              bufs_(-1), 
                              stop_(false), 
                              buf_(buf_size_) {

}

bool BufContainer::init(size_type bufs) {
  if (bufs < 1) {
    return false;
  }
  other_buf_.resize(static_cast<unsigned int>(bufs));
  bufs_ = bufs;
  inited_ = true;
  return true;
}

bool BufContainer::write(const char* ptr, size_type n) {
  buf_.append(ptr, n);
  if (buf_.error() == true) {
    swapInBack();
    buf_.append(ptr, n); //ptr不能装入一个空的buf
    if (buf_.error() == true) {
      return false;
    }
  }
  return true;
}

void BufContainer::swapInBack() { 
  std::unique_lock<std::mutex> mut(mut_);
  if (buf_.getSize() == 0) {
    return;
  }
  while (the_first_clean_ > bufs_) {
    cv_can_write_.wait(mut);
  }
  auto it = other_buf_.begin();
  for (size_type i = 1; i < the_first_clean_; ++i) {
    ++it;
  }

  using std::swap;
  swap(buf_, *it);
  ++the_first_clean_;
  mut.unlock();
}

bool BufContainer::backEnd(std::shared_ptr<out_stream_base> out) {
  static CharArrayType backbuf(buf_size_); 
  std::unique_lock<std::mutex> mut(mut_);
  if (the_first_clean_ <= 1 && stop_ == true) { 
    return false;
  }
  if (the_first_clean_ <= 1 && stop_ == false) {
    return true;
  }

  CharArrayType tmp = std::move(other_buf_.front());
  other_buf_.erase(other_buf_.begin());
  other_buf_.push_back(std::move(tmp));
  std::swap(backbuf, other_buf_.back());
  --the_first_clean_;
  mut.unlock();
  cv_can_write_.notify_all();
  out->write(backbuf.getBuf(), backbuf.getSize());
  backbuf.setZero();
  return true;
}

void BufContainer::stop() {
  if (inited() == true) {
    swapInBack();
    mut_.lock();
    stop_ = true;
    mut_.unlock();
  }
}

BufContainer::~BufContainer() {

}