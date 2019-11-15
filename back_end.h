#pragma once
#include "file_out_stream.h"
#include "type.h"
#include "buf_container.h"
#include "thread_pool.h"
#include "out_stream_base.h"
#include "spin_lock.h"
#include <memory>
#include <atomic>

namespace pnlog {
  class BackEnd {
    friend class CapTure;
  public:
    static constexpr size_t FILES = 128;

    static std::shared_ptr<BackEnd> get_instance();

    BackEnd(const BackEnd&) = delete;
    BackEnd(BackEnd&&) = delete;
    BackEnd& operator=(const BackEnd&) = delete;
    BackEnd& operator=(BackEnd&&) = delete;

    void open(size_type index, out_stream_base* out, size_type log_container_size = 6);

    void stop();

    void abort(const char* = nullptr);

    ~BackEnd();

  private:
    ThreadPool pool_;
    std::shared_ptr<out_stream_base> out_streams_[FILES];
    BufContainer buf_containers_[FILES];
    std::atomic<bool> stop_;

    explicit BackEnd(size_type size);

    bool rangecheck(size_type index) const;

    void write(size_type index, const char* ptr, size_type n);

    std::shared_ptr<out_stream_base>& out_stream(size_type index);

    BufContainer& buf_container(size_type index);

    void all_flush();
  };
}//namespace pnlog