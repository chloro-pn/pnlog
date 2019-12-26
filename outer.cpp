#include "outer.h"
#include "back_end.h"
#include <cassert>

namespace pnlog {
  outer::outer(size_type index, BackEnd* back) :state_(state::closed), index_(index), back_(back), syn_(syn::no) {

  }

  void outer::open(out_stream_base* stream) {
    std::unique_lock<std::mutex> mut(mut_);
    assert(out_stream_ == nullptr);
    out_stream_.reset(stream);
    assert(buf_ == nullptr);
    buf_.reset(new CharArray(4096, index_));
    assert(state_ == state::closed);
    state_ = state::writing;
  }

  void outer::open_syn(out_stream_base* stream) {
    std::unique_lock<std::mutex> mut(mut_);
    assert(out_stream_ == nullptr);
    out_stream_.reset(stream);
    assert(buf_ == nullptr);
    syn_ = syn::yes;
    assert(state_ == state::closed);
    state_ = state::writing;
  }

  void outer::write(const char* buf, size_type length) {
    std::unique_lock<std::mutex> mut(mut_);
    if (state_ == state::closed || state_ == state::closing) {
      return;
    }
    if (syn_ == syn::yes) {
      out_stream_->write(buf, length);
      return;
    }
    buf_->append(buf, length);
    if (buf_->error() == true) {
      back_->push_buf(std::move(*buf_));
      buf_.reset(new CharArray(4096, index_));
      buf_->append(buf, length);
      assert(buf_->error() == false);
    }
  }

  void outer::close() {
    std::unique_lock<std::mutex> mut(mut_);
    if (state_ == state::closed || state_ == state::closing) {
      return;
    }
    if (syn_ == syn::yes) {
      state_ = state::closed;
      return;
    }
    std::future<void> w = back_->push_buf(std::move(*buf_));
    buf_.reset(); // closing 状态下buf_不可用，但有数据在后台队列中未写入。
    state_ = state::closing;
    mut.unlock();
    w.get();//blocking...
            //阻塞到后台线程将此块buf_全部写入，然后在这里将out_stream_关闭，并将状态修改为closed。
    mut.lock();
    assert(state_ == state::closing);
    out_stream_.reset();
    state_ = state::closed;
  }
}