#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <list>
#include <condition_variable>
#include <mutex>
#include "type.h"

class ThreadPool {
private:
  std::vector<std::thread> threads_;
  using task_type = std::function<bool()>;
  std::list<task_type> tasks_;
  size_type th_counts_;
  std::condition_variable cv_;
  std::mutex mut_;
  bool stop_;

public:
  ThreadPool(size_type count);

  void each_thread();

  void start();

  void stop();

  template<class F>
  void push_task(const F& func) {
	  mut_.lock();
	  tasks_.push_back(func);
	  mut_.unlock();
	  cv_.notify_one();
  }


  ~ThreadPool();
};