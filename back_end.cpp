#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include "str_appender.h"
#include "blocking_queue.h"
#include "outer.h"
#include <ctime>
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

  void BackEnd::run_in_back() {
    auto bufs = bufs_.get_all();
    if (bufs.empty() == true) {
      return;
    }
    for (auto& each : bufs) {
      CharArray& buf = each.first;
      size_type index = buf.getIndex();
      outers_.at(static_cast<unsigned int>(index))->out_stream_->write(buf.getBuf(), buf.getSize());
      each.second.set_value();
    }
  }

  BackEnd::BackEnd(size_type size) :pool_(1),size_of_streams_and_bufs_(size), stop_(false) {
    assert(size > 0);
    for (int i = 0; i < size; ++i) {
      outers_.emplace_back(new outer(i,this));
    }
    open_syn(0, new StdOutStream(stdout));
    pool_.start();
  }

  void BackEnd::open(size_type index, out_stream_base* out) {
    //每个日志打开后第一条日志是当前时刻的日期。
    outers_.at(static_cast<unsigned int>(index))->open(out);
    std::time_t current_time = std::time(nullptr);
    char buf[128];
    std::strftime(buf, sizeof(buf), "%c", std::localtime(&current_time));
    std::string result = piece("index : ", index, ", ", buf, "\n");
    write(index, result.c_str(), result.size());
  }

  void BackEnd::open_syn(size_type index, out_stream_base* out) {
    outers_.at(static_cast<unsigned int>(index))->open_syn(out);
    //每个日志打开后第一条日志是当前时刻的日期。
    std::time_t current_time = std::time(nullptr);
    char buf[128];
    std::strftime(buf, sizeof(buf), "%c", std::localtime(&current_time));
    std::string result = piece("index : ", index, ", ", buf, "\n");
    write(index, result.c_str(), result.size());
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
      pool_.stop();
    }
  }

  void BackEnd::abort(const char* error_message) {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      for (auto& each : outers_) {
        each->close();
      }
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
    auto result = bufs_.push(std::move(buf));
    pool_.push_task([this]()->void {
      this->run_in_back();
    });
    return result;
  }
}//namespace pnlog