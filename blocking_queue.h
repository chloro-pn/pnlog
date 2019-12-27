#pragma once
#include <vector>
#include <mutex>
#include <future>
#include "condition_variable_type.h"
#include "spin_lock.h"

namespace pnlog {
  template<class T>
  class BlockingQueue {
  private:
    using element_type = std::pair<T, std::promise<void>>;
    std::vector<element_type> queue_;
    using lock_type = spin;
    lock_type mut_;
    condition_variable_type<lock_type>::type cv_;

  public:
    BlockingQueue() {

    }

    std::future<void> push(T&& t) {
      std::unique_lock<lock_type> mut(mut_);
      cv_.wait(mut, [this]()->bool {
        return this->queue_.size() <= 10;
      });
      std::promise<void> pro;
      std::future<void> fu = pro.get_future();
      queue_.push_back(std::make_pair<T, std::promise<void>>(std::move(t), std::move(pro)));
      mut.unlock();
      return std::move(fu);
    }

    std::vector<element_type> get_all() {
      std::vector<element_type> tmp;
      std::unique_lock<lock_type> mut(mut_);
      tmp.swap(queue_);
      mut.unlock();
      cv_.notify_all();
      return tmp;
    }

    inline 
    size_type size() const {
      return queue_.size();
    }
  };
}