#include "outer.h"
#include "back_end.h"
#include <cassert>

namespace pnlog {
  outer::outer(size_type index, BackEnd* back) : back_(back),
                                                 index_(index),
                                                 state_(state::closed),
                                                 syn_(syn::no),
                                                 written_bytes_(0){

  }

  void outer::open(out_stream_base* stream) {
    std::unique_lock<lock_type> mut(mut_);
    assert(out_stream_ == nullptr);
    out_stream_.reset(stream);
    assert(buf_ == nullptr);
    buf_.reset(new CharArray(4096, index_));
    assert(state_ == state::closed);
    state_ = state::writing;
    syn_ = syn::no;
    written_bytes_ = 0;
  }

  void outer::open_syn(out_stream_base* stream) {
    std::unique_lock<lock_type> mut(mut_);
    assert(out_stream_ == nullptr);
    out_stream_.reset(stream);
    assert(buf_ == nullptr);
    syn_ = syn::yes;
    assert(state_ == state::closed);
    state_ = state::writing;
    written_bytes_ = 0;
  }

  void outer::reopen(out_stream_base* stream) {
    std::unique_lock<lock_type> mut(mut_);
    reopen_or_close_cv_.wait(mut,[this]()->bool{
        return this->state_ != state::closing;
    });
    if (state_ == state::closed) {
      mut.unlock();
      open(stream);
      return;
    }
    else {
      if (syn_ == syn::yes) {
        out_stream_.reset(stream);
        buf_.reset(new CharArray(4096, index_));
        written_bytes_ = 0;
        syn_ = syn::no;
        //state_ == state::writing.
        return;
      }
      std::future<void> w = back_->push_buf(std::move(*buf_));
      written_bytes_ += buf_->getSize();
      buf_.reset();
      state_ = state::closing;
      mut.unlock();
      w.get();//blocking until back thread write all data.

      mut.lock();
      assert(state_ == state::closing);
      out_stream_.reset(stream);
      written_bytes_ = 0;
      buf_.reset(new CharArray(4096, index_));
      syn_ = syn::no;
      state_ = state::writing;
      reopen_or_close_cv_.notify_all();
    }
  }

  void outer::write(const char* buf, size_type length) {
    std::unique_lock<lock_type> mut(mut_);
      //等待closing的最终状态。
    reopen_or_close_cv_.wait(mut,[this]()->bool{
        return this->state_ != state::closing;
    });
    if (state_ == state::closed) {
      return;
    }
    //state_ == state::writing.
    if (syn_ == syn::yes) {
      out_stream_->write(buf, length);
      written_bytes_ += length;
      return;
    }
    buf_->append(buf, length);
    if (buf_->error() == true) {
      back_->push_buf(std::move(*buf_));
      written_bytes_ += length;
      buf_.reset(new CharArray(4096, index_));
      buf_->append(buf, length);
      assert(buf_->error() == false);
    }
  }

  void outer::close() {
    std::unique_lock<lock_type> mut(mut_);
    if (state_ == state::closed || state_ == state::closing) {
      return;
    }
    if (syn_ == syn::yes) {
      out_stream_.reset();
      state_ = state::closed;
      written_bytes_ = 0;
      return;
    }
    std::future<void> w = back_->push_buf(std::move(*buf_));
    written_bytes_ += buf_->getSize();
    buf_.reset();
    //under the closing state,buf_ is unavailable,but there are some data waiting to be written.
    state_ = state::closing;
    mut.unlock();
    w.get();//blocking until back thread write all data.

    mut.lock();
    assert(state_ == state::closing);
    out_stream_.reset();
    //under the closed state,buf_ is unabailable and no data need to be written in out_stream_.
    state_ = state::closed;
    written_bytes_ = 0;
    reopen_or_close_cv_.notify_all();
  }
}
