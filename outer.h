#pragma once
#include "out_stream_base.h"
#include "char_array.h"
#include <mutex>
#include <condition_variable>
#include <memory>

namespace pnlog {
  class BackEnd;

  class outer {
    friend class BackEnd;
  private:
    std::shared_ptr<out_stream_base> out_stream_;
    std::unique_ptr<CharArray> buf_;
    std::mutex mut_;
    std::condition_variable cv_;
    BackEnd* back_;
    size_type index_;

    enum class state { closed, closing, writing };
    state state_;
    enum class syn { yes, no };
    syn syn_;

  public:
    outer(size_type index, BackEnd* back);

    void open(out_stream_base* stream);

    void open_syn(out_stream_base* stream);

    void write(const char* buf, size_type length);

    void close();
  };
}