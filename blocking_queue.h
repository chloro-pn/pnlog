#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>

template<class T>
class BlockingQueue {
private:
  std::vector<std::pair<T,std::promise<void>>> queue_;
  std::mutex mut_;
  std::condition_variable cv_;

public:

  using value_type = std::vector<std::pair<T, std::promise<void>>>;
  BlockingQueue() {

  }

  std::future<void> push(T&& t) {
    std::unique_lock<std::mutex> mut(mut_);
    cv_.wait(mut,[this]()->bool {
      return this->queue_.size() <= 10;
    });
    std::promise<void> pro;
    std::future<void> fu = pro.get_future();
    queue_.push_back(std::make_pair<T,std::promise<void>>(std::move(t),std::move(pro)));
    mut.unlock();
    return std::move(fu);
  }

  value_type get_all() {
    value_type tmp;
    std::unique_lock<std::mutex> mut(mut_);
    tmp.swap(queue_);
    mut.unlock();
    cv_.notify_all();
    return tmp;
  }
};