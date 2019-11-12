#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include "platform.h"
#include <iostream>

static_assert(BufContainer::buf_size_ >= CapTure::buf_size_, "container buf_size should >= capture buf_size!");

void BackEnd::write(size_type index, const char* ptr, size_type n) {
  if (stop_.load() == true) {
    return;
  }
  bool range = rangecheck(index);
  if (range == false) {
    stop();
    pf::fprintf(stderr, "index out of range : %d\n", index);
    abort();
  }
  spins_[index].lock();
  if (out_stream(index) == nullptr) {
    spins_[index].unlock();
    stop();
    pf::fprintf(stderr, "write non-opened file : %d\n", static_cast<int>(index));
    abort();
  }
  if (buf_container(index).inited() == false) {
    out_stream(index)->write(ptr, n);
    spins_[index].unlock();
  }
  else {
    bool succ = buf_container(index).write(ptr, n);
    if (succ == false) {
      spins_[index].unlock();
      stop();
      pf::fprintf(stderr, "buf_container write error:%d\n", static_cast<int>(index));
      abort();
    }
    spins_[index].unlock();
  }
}

BackEnd::BackEnd(size_type size) :pool_(size), stop_(false) {
  for (size_type i = 0; i < FILES; ++i) {
    out_streams_[i] = nullptr;
  }
  out_stream(0).reset(new StdOutStream(stdout));
  out_stream(1).reset(new StdOutStream(stderr));
  pool_.start();
}

void BackEnd::open(size_type index, out_stream_base* out, size_type log_container_size) {
  bool range = rangecheck(index);
  if (out_stream(index) != nullptr || range == false) {
    stop();
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
  bool exp = false;
  if (stop_.compare_exchange_strong(exp, true)) {
    for (size_type i = 0; i < FILES; ++i) {
      spins_[i].lock();
      buf_container(i).stop();
      spins_[i].unlock();
    }
    pool_.stop();
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