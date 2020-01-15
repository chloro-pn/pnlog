#pragma once
#include "type.h"
#include "thread_pool.h"
#include "out_stream_base.h"
#include "blocking_queue.h"
#include "char_array.h"
#include "event_pool.h"
#include <memory>
#include <string>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <chrono>

namespace pnlog {
  class outer;

  class BackEnd {
    friend class CapTure;
    using lock_type = std::mutex;

  public:
    struct options {
      bool asyn = true;
      bool size_rotating = false;
      uint64_t rotate_size;
      bool duration_rotating = false;
      std::chrono::milliseconds duration;
      std::string path;
    };

    static std::shared_ptr<BackEnd> get_instance();

    BackEnd(const BackEnd&) = delete;
    BackEnd(BackEnd&&) = delete;
    BackEnd& operator=(const BackEnd&) = delete;
    BackEnd& operator=(BackEnd&&) = delete;

    void open(options option, size_type index, out_stream_base* out);

    void reopen(size_type index, out_stream_base* out);

    void close(size_type index);

    void stop();

    void abort(const char* = nullptr);

    ~BackEnd();

    bool rangecheck(size_type index) const;

    std::future<void> push_buf(CharArray&& buf);

  private:
    ThreadPool pool_;

    std::shared_ptr<event_pool> event_pool_;

    size_type size_of_streams_and_bufs_;

    BlockingQueue<CharArray> bufs_;

    std::vector<std::unique_ptr<outer>> outers_;

    std::atomic<bool> stop_;

    explicit BackEnd(size_type size);

    void write(size_type index, const char* ptr, size_type n);

    void run_in_back();
  };
}//namespace pnlog
