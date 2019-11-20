#pragma once
#include "char_array.h"
#include "type.h"
#include "out_stream_base.h"
#include "spin_lock.h"
#include <list>
#include <condition_variable>

namespace pnlog {
  class BufContainer {
    friend class BackEnd;
  private:
    using CharArrayType = CharArray;
    using lock_type = spin;
    CharArrayType buf_;
    std::list<CharArrayType> other_buf_;
    size_type bufs_;
    size_type the_first_clean_;
    std::condition_variable_any cv_can_write_;
    lock_type mut_;
    enum class state { running, stop };
    state state_;

  public:
    static constexpr size_type buf_size_ = 4096;

    BufContainer();

    bool init(size_type bufs);

    inline
      bool running() const {
      return state_ == state::running;
    }

    void write(const char* ptr, size_type n);

    void swapInBack();

    bool backEnd(std::shared_ptr<out_stream_base> out);

    void stop();

    //should be called after out_stream()->close() called.
    void clear();

    ~BufContainer();
  };
}//namespace pnlog