#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include "str_appender.h"
#include "blocking_queue.h"
#include "outer.h"
#include <cstring>
#include <locale>
#include <cassert>
#include <mutex>

namespace pnlog {
  std::shared_ptr<BackEnd> BackEnd::get_instance() {
    static std::shared_ptr<BackEnd> backend( new BackEnd(128));
    return backend;
  }

  void BackEnd::write(size_type index, const char* ptr, size_type n) {
    outers_.at(static_cast<unsigned int>(index))->write(ptr, n);
  }

  BackEnd::BackEnd(size_type size) :pool_(1),size_of_streams_and_bufs_(size), stop_(false),event_pool_(new event_pool()) {
    assert(size > 0);
    for (int i = 0; i < size; ++i) {
      outers_.emplace_back(new outer(i,this));
    }
    pool_.push_task([this]()->void {this->event_pool_->run();});

    open_syn(0, new StdOutStream(stdout));
    pool_.start();
  }

  void BackEnd::open(size_type index, out_stream_base* out) {
    outers_.at(static_cast<unsigned int>(index))->open(out);
  }

  void BackEnd::open_syn(size_type index, out_stream_base* out) {
    outers_.at(static_cast<unsigned int>(index))->open_syn(out);
  }

  void BackEnd::reopen(size_type index, out_stream_base* out) {
    outers_.at(static_cast<unsigned int>(index))->reopen(out);
  }

  void BackEnd::close(size_type index) {
    outers_.at(static_cast<unsigned int>(index))->close();
  }

  void BackEnd::stop() {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      for (auto& each : outers_) {
        each->close();
      }
      event_pool_->stop();
      pool_.stop();
    }
  }

  void BackEnd::abort(const char* error_message) {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      for (auto& each : outers_) {
        each->close();
      }
      event_pool_->stop();
      pool_.stop();
      if (error_message != nullptr) {
        fprintf(stderr, error_message);
      }
      std::abort();
    }
  }

  BackEnd::~BackEnd() {
    stop();
  }

  bool BackEnd::rangecheck(size_type index) const {
    if (index < 0 || index >= size_of_streams_and_bufs_) {
      return false;
    }
    return true;
  }

  std::future<void> BackEnd::push_buf(CharArray&& buf) {
      std::shared_ptr<event_handle> e(new event_handle());
      e->args_.reset(new CharArray(std::move(buf)));
      e->func_ = [this](std::shared_ptr<event_handle> self)->void {
          auto ptr_buf = static_cast<CharArray*>(self->args_.get());
          size_type index = ptr_buf->getIndex();
          this->outers_.at(index)->out_stream_->write(ptr_buf->getBuf(),ptr_buf->getSize());
      };
      e->type_ = event_handle::type::once;
      auto result = event_pool_->push_event(e);
      e->wake_up();
      return result;
  }
}//namespace pnlog
