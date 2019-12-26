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
    buf_.reset(); // closing ״̬��buf_�����ã����������ں�̨������δд�롣
    state_ = state::closing;
    mut.unlock();
    w.get();//blocking...
            //��������̨�߳̽��˿�buf_ȫ��д�룬Ȼ�������ｫout_stream_�رգ�����״̬�޸�Ϊclosed��
    mut.lock();
    assert(state_ == state::closing);
    out_stream_.reset();
    state_ = state::closed;
  }
}