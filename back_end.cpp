#include "back_end.h"
#include "capture.h"
#include "std_out_stream.h"
#include "str_appender.h"
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
    std::unique_lock<spin> mut(*spins_.at(index));
    if (stops_.at(index) == true) {
      return;
    }
    if (out_stream(index) == nullptr) {
      mut.unlock();
      abort("write non - opened file.");
    }
    else if (bufs_.at(index) == nullptr) {
      out_stream(index)->write(ptr, n);
      mut.unlock();
    }
    else {
      bufs_.at(index)->append(ptr, n);
      if (bufs_.at(index)->error() == true) {
        pool_.push_task([this, index]()->void {
          this->write_in_thread_pool(index);
        });
        cv_can_write_.at(index)->wait(*spins_.at(index), [this, index]()->bool {return bufs_.at(index)->getSize() == 0 || stops_.at(index) == true; });
        if (stops_.at(index) == true) {
          return;
        }
        bufs_.at(index)->append(ptr, n);
      }
    }
  }

  BackEnd::BackEnd(size_type size) :pool_(1),size_of_streams_and_bufs_(size), stop_(false) {
    assert(size > 0);
    out_streams_.resize(size);
    for (int i = 0; i < size; ++i) {
      bufs_.emplace_back(nullptr);
      stops_.push_back(false);
      cv_can_write_.emplace_back(new std::condition_variable_any());
      spins_.emplace_back(new spin());
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
    bufs_.at(index) = std::shared_ptr<CharArray>(new CharArray(4096));

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

  void BackEnd::all_flush() {
    for (size_type i = 0; i < size_of_streams_and_bufs_; ++i) {
      std::unique_lock<spin> mut(*(spins_.at(i).get()));
      if (bufs_.at(i) == nullptr) {
        continue;
      }
      stops_.at(i) = true;
      mut.unlock();
      cv_can_write_.at(i)->notify_all();
      pool_.push_task([this, i]()->void {
        this->write_in_thread_pool(i);
      });
    }
  }

  void BackEnd::close(size_type index) {
    bool range = rangecheck(index);
    if (range == false) {
      fprintf(stderr, "close file out of range : %d", static_cast<int>(index));
      abort();
    }
    std::unique_lock<spin> mut(*spins_.at(index));
    stops_.at(index) = true;
    mut.unlock();
    cv_can_write_.at(index)->notify_all();
    pool_.push_task([this, index]()->void {
      this->write_in_thread_pool(index);
      this->out_streams_.at(index)->close();
    });
  }

  void BackEnd::stop() {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      all_flush();
      pool_.stop();
    }
  }

  void BackEnd::abort(const char* error_message) {
    bool exp = false;
    if (stop_.compare_exchange_strong(exp, true)) {
      all_flush();
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

  void BackEnd::write_in_thread_pool(size_type index) {
    CharArray tmp(4096);
    std::unique_lock<spin> mut(*spins_.at(index));
    std::swap(*bufs_.at(index), tmp);
    mut.unlock();
    cv_can_write_.at(index)->notify_all();
    out_streams_.at(index)->write(tmp.getBuf(), tmp.getSize());
  }
}//namespace pnlog