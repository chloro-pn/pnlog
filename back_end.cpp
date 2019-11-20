#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include <ctime>
#include <cstring>
#include <locale>

namespace pnlog {

  static_assert(BufContainer::buf_size_ >= CapTure::buf_size_, "container buf_size should >= capture buf_size!");

  std::shared_ptr<BackEnd> BackEnd::get_instance() {
    static std::shared_ptr<BackEnd> backend( new BackEnd(1));
    return backend;
  }

  void BackEnd::write(size_type index, const char* ptr, size_type n) {
    std::unique_lock<typename BufContainer::lock_type> mut(buf_container(index).mut_);
    if (out_stream(index) == nullptr) {
      mut.unlock();
      abort("write non - opened file.");
    }
    else if (buf_container(index).running() == false) {
      out_stream(index)->write(ptr, n);
      mut.unlock();
    }
    else {
      mut.unlock();
      buf_container(index).write(ptr, n);
    }
  }

  BackEnd::BackEnd(size_type size) :pool_(size), stop_(false) {
    for (size_type i = 0; i < FILES; ++i) {
      out_streams_[i] = nullptr;
    }
    open_syn(0, new StdOutStream(stdout));
    open_syn(1, new StdOutStream(stderr));
    pool_.start();
  }

  void BackEnd::open(size_type index, out_stream_base* out, size_type log_container_size) {
    bool range = rangecheck(index);
    if (range == false) {
      fprintf(stderr, "open file out of range : %d", static_cast<int>(index));
      abort();
    }
    if (out_stream(index) != nullptr) {
      abort("register opened file");
    }
    out_stream(index) = std::shared_ptr<out_stream_base>(out);
    bool init_ = buf_container(index).init(log_container_size);
    if (init_ == false) {
      fprintf(stderr, "buf_container %d init error, container size : %d", static_cast<int>(index), static_cast<int>(log_container_size));
      abort();
    }
    pool_.push_task(
      std::make_pair(
        [this, index]()->bool {
          return buf_container(index).backEnd(out_stream(index));
        }, 
        [this, index]()->void {
          out_stream(index)->write(buf_container(index).buf_.getBuf(), buf_container(index).buf_.getSize());
          out_stream(index)->close();

          out_stream(index).reset();
          buf_container(index).clear();
        }
      ));

    //每个日志打开后第一条日志是当前时刻的日期。
    std::time_t current_time = std::time(nullptr);
    char buf[128];
    std::strftime(buf, sizeof(buf), "%c", std::localtime(&current_time));
    std::string result = piece("index : ", index, ", ", buf, "\n");
    write(index, result.c_str(), result.size());
  }

  void BackEnd::open_syn(size_type index, out_stream_base* out) {
    bool range = rangecheck(index);
    if (out_stream(index) != nullptr || range == false) {
      fprintf(stderr, "register opened file or out of range : %d", static_cast<int>(index));
      abort();
    }

    out_stream(index) = std::shared_ptr<out_stream_base>(out);

    //每个日志打开后第一条日志是当前时刻的日期。
    std::time_t current_time = std::time(nullptr);
    char buf[128];
    std::strftime(buf, sizeof(buf), "%c", std::localtime(&current_time));
    std::string result = piece("index : ", index, ", ", buf, "\n");
    write(index, result.c_str(), result.size());
  }

  inline
  std::shared_ptr<out_stream_base>& BackEnd::out_stream(size_type index) {
    return out_streams_[index];
  }

  inline
  BufContainer& BackEnd::buf_container(size_type index) {
    return buf_containers_[index];
  }

  void BackEnd::all_stop() {
    for (size_type i = 0; i < FILES; ++i) {
      if (buf_containers_[i].running() == false) {
        continue;
      }
      std::unique_lock<typename BufContainer::lock_type> tmp(buf_containers_[i].mut_);
      buf_containers_[i].stop();
      tmp.unlock();
      buf_containers_[i].cv_can_write_.notify_all();
    }
  }

  //非线程安全，且只能被调用一次。
  void BackEnd::close(size_type index) {
    bool range = rangecheck(index);
    if (range == false) {
      fprintf(stderr, "close file out of range : %d", static_cast<int>(index));
      abort();
    }
    if (out_streams_[index] == nullptr) {
      return;
    }
    if (buf_containers_[index].running() == false) {
      //非缓冲文件，直接调用close。
      out_streams_[index]->close();
      out_streams_[index].reset();
      return;
    }
    else {
      //首先等待在可写上
      std::unique_lock<typename BufContainer::lock_type> lock(buf_containers_[index].mut_);
      //由于可写被唤醒，将当前块swap进后台并设置stop_为true。
      buf_containers_[index].stop();
      lock.unlock();
      //唤醒由于关闭文件而等待的write线程，让其被唤醒。
      buf_containers_[index].cv_can_write_.notify_all();
    }
  }

  void BackEnd::stop() {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      all_stop();
      pool_.stop();
    }
  }

  void BackEnd::abort(const char* error_message) {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      all_stop();
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
    if (index < 0 || index >= FILES) {
      return false;
    }
    return true;
  }
}//namespace pnlog