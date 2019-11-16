#include "buf_container.h"
#include "out_stream_base.h"

namespace pnlog {
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
    //other_buf_.resize(static_cast<unsigned int>(bufs));
    for (size_type i = 0; i < bufs; ++i) {
      other_buf_.emplace_back(buf_size_);
    }
    bufs_ = bufs;
    inited_ = true;
    return true;
  }

  void BufContainer::write(const char* ptr, size_type n) {
    std::unique_lock<lock_type> mut(mut_);
    buf_.append(ptr, n);
    if (buf_.error() == true) {
      while (the_first_clean_ > bufs_ && stop_ == false) {
        cv_can_write_.wait(mut);
      }
      if (stop_ == true) {
        return;
      }
      swapInBack();
      buf_.append(ptr, n);
    }
    return;
  }

  //��������Ҫ��֤ͬ��
  void BufContainer::swapInBack() {
    auto it = other_buf_.begin();
    for (size_type i = 1; i < the_first_clean_; ++i) {
      ++it;
    }
    using std::swap;
    swap(buf_, *it);
    ++the_first_clean_;
  }

  bool BufContainer::backEnd(std::shared_ptr<out_stream_base> out) {
    bool lock_ = mut_.try_lock();
    if (lock_ == false) {
      return true;
    }
    CharArrayType backbuf(buf_size_);
    if (the_first_clean_ <= 1 && stop_ == true) {
      mut_.unlock();
      return false;
    }
    if (the_first_clean_ <= 1 && stop_ == false) {
      mut_.unlock();
      return true;
    }

    CharArrayType tmp = std::move(other_buf_.front());
    other_buf_.erase(other_buf_.begin());
    other_buf_.push_back(std::move(tmp));
    std::swap(backbuf, other_buf_.back());
    --the_first_clean_;
    mut_.unlock();
    cv_can_write_.notify_all();
    out->write(backbuf.getBuf(), backbuf.getSize());
    return true;
  }

  //��Ҫ�����߱�֤ͬ��
  void BufContainer::stop() {
    if (inited() == true) {
      swapInBack();
      stop_ = true;
    }
  }

  BufContainer::~BufContainer() {

  }
}//namespace pnlog