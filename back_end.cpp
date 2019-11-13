#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include "platform.h"
#include <iostream>

static_assert(BufContainer::buf_size_ >= CapTure::buf_size_, "container buf_size should >= capture buf_size!");

void BackEnd::write(size_type index, const char* ptr, size_type n) {
  if (out_stream(index) == nullptr) {
    pf::fprintf(stderr, "write non-opened file : %d\n", static_cast<int>(index));
    abort();
  }
  else {
    buf_container(index).write(ptr, n);
  }
}

BackEnd::BackEnd(size_type size) :pool_(size), stop_(false) {
  for (size_type i = 0; i < FILES; ++i) {
    out_streams_[i] = nullptr;
  }
  out_stream(0).reset(new StdOutStream(stdout));
  buf_container(0).init(1);
  out_stream(1).reset(new StdOutStream(stderr));
  buf_container(0).init(1);
  pool_.start();
}

void BackEnd::open(size_type index, out_stream_base* out, size_type log_container_size) {
  bool range = rangecheck(index);
  if (out_stream(index) != nullptr || range == false) {
    pf::fprintf(stderr, "register opened file or out of range : %d",index);
    abort();
  }

  out_stream(index) = std::shared_ptr<out_stream_base>(out);
  buf_container(index).init(log_container_size);

  pool_.push_task([this, index]()->bool {
    return buf_container(index).backEnd(out_stream(index));
  });
}

inline
std::shared_ptr<out_stream_base>& BackEnd::out_stream(size_type index) {
  return out_streams_[index];
}

inline
BufContainer& BackEnd::buf_container(size_type index) {
  return buf_containers_[index];
}

void BackEnd::stop() {
  //这里有bug：先全部上锁，那么线程池中的backend任务会被阻塞。
  //这里又调用wait，会导致死锁。
  bool exp = false;
  if (stop_.compare_exchange_strong(exp, true)) {
    std::unique_lock<std::mutex> locks[FILES];
    std::cout << "stop : " << std::endl;
    for (size_type i = 0; i < FILES; ++i) {
      std::unique_lock<std::mutex> tmp(buf_containers_[i].mut_);
      tmp.swap(locks[i]);
      std::cout << "lock " << i << std::endl;
    }
    for (size_type i = 2; i < FILES; ++i) {
      if (buf_containers_[i].inited() == false) {
        continue;
      }//虽然locks[i]会被unlock，但也许线程池被其他backend阻塞了，因为这里锁住了所有的lock。
      std::cout << "wait : " << i << std::endl;
      while (buf_containers_[i].the_first_clean_ > buf_containers_[i].bufs_) {
        buf_containers_[i].cv_can_write_.wait(locks[i]);
      }
    }
    for (size_type i = 0; i < FILES; ++i) {
      buf_containers_[i].stop();
      locks[i].unlock();
      //buf_containers_[i].cv_can_write_.notify_all(); // 不再唤醒了
    }

    //如果这里已经析构了
    pool_.stop();
    abort();
  }
}

void BackEnd::abort() {
  bool exp = false;
  if (abort_.compare_exchange_strong(exp, true) == true) {
    std::abort();
    //std::terminate();
  }
}

BackEnd::~BackEnd() {
  stop();
}

bool BackEnd::rangecheck(size_type index) const {
  if (index < 0 || index >= FILES) {
    return false;
  }
  return true;
}