#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <list>
#include <condition_variable>
#include "type.h"
#include "spin_lock.h"

template<class T>
struct cv_type {
  using type = std::condition_variable_any;
};

template<>
struct cv_type<std::mutex> {
  using type = std::condition_variable;
};

namespace pnlog {
  class ThreadPool {
  private:
    using lock_type = spin;
    std::vector<std::thread> threads_;
    using task_type = std::function<void()>;
    std::list<task_type> tasks_;
    size_type th_counts_;
    cv_type<lock_type>::type cv_;
    lock_type mut_;
    bool stop_;

    using exception_callback = std::function<void(std::exception&)>;
    exception_callback ec_;

  public:
    ThreadPool(size_type count);

    void each_thread();

    void start();

    void stop();

    template<class F>
    void push_task(F&& func) {
      mut_.lock();
      tasks_.push_back(std::forward<F>(func));
      mut_.unlock();
      cv_.notify_one();
    }

    template<class T>
    void set_exception_callback(T&& ec) {
      ec_ = std::forward<T>(ec);
    }

    ~ThreadPool();
  };
}//namespace pnlog