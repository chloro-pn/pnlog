#pragma once
#include "out_stream_base.h"
#include "char_array.h"
#include "condition_variable_type.h"
#include "spin_lock.h"
#include <mutex>
#include <memory>
#include <atomic>

namespace pnlog {
  class BackEnd;

  class outer {
    friend class BackEnd;
  private:
    std::shared_ptr<out_stream_base> out_stream_;
    std::unique_ptr<CharArray> buf_;
    using lock_type = spin;
    mutable lock_type mut_;
    condition_variable_type<lock_type>::type reopen_or_close_cv_;
    BackEnd* back_;
    size_type index_;
    uint64_t written_bytes_;

    enum class state { closed, closing, writing };
    state state_;
    enum class syn { yes, no };
    syn syn_;

  public:
    outer(size_type index, BackEnd* back);

    void open(out_stream_base* stream);

    void open_syn(out_stream_base* stream);

    void reopen(out_stream_base* stream);

    void write(const char* buf, size_type length);

    void close();

    uint64_t written_bytes() const {
      uint64_t result;
      std::unique_lock<lock_type> mut(mut_);
      result = written_bytes_;
      return result;
    }
  };
}
