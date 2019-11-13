#pragma once
#include <atomic>

namespace pnlog {
  class spin {
  public:
    spin() :lock_(false) {}

    void lock() {
      bool exp = false;
      while (lock_.compare_exchange_weak(exp, true) == false) {
        exp = false; // 这一步是必须的
      }
    }

    void unlock() {
      lock_.store(false);
    }

    bool try_lock() {
      bool exp = false;
      if (lock_.compare_exchange_weak(exp, true) == true) {
        return true;
      }
      return false;
    }

    ~spin() {
      unlock();
    }

  private:
    std::atomic<bool> lock_;
  };
}//namespace pnlog