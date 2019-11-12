#pragma once
#include <atomic>

class spin {
public:
  spin():lock_(false) {}

  void lock() {
    bool exp = false;
    while (lock_.compare_exchange_weak(exp, true) == false) {
      exp = false; // ��һ���Ǳ����
    }
  }

  void unlock() {
    lock_.store(false);
  }

  ~spin() {
    unlock();
  }

private:
  std::atomic<bool> lock_;
};