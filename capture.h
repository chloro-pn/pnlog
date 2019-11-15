#pragma once
#include "char_array.h"
#include "back_end.h"
#include "type.h"
#include "convert.h"
#include "spin_lock.h"
#include "str_appender.h"
#include <list>
#include <ctime>
#include <memory>

namespace pnlog {
  class CapTure {
  public:
    enum class Level { PN_TRACE, PN_DEBUG, PN_ERROR, PN_FATAL };

  private:
    std::shared_ptr<BackEnd> back_;
    size_type index_;
    static Level default_level_;

  public:

    static constexpr size_type buf_size_ = 4096;

    static std::shared_ptr<CapTure> get_instance(std::shared_ptr<BackEnd> back);

    CapTure(const CapTure&) = delete;
    CapTure(CapTure&&) = delete;
    CapTure& operator=(const CapTure&) = delete;
    CapTure& operator=(CapTure&&) = delete;

    static void setLevel(Level l);

    static Level getLevel();

    void log_trace(size_type index, size_type line, const char* file, const std::string& str);

    void log_debug(size_type index, size_type line, const char* file, const std::string& str);

    void log_error(size_type index, size_type line, const char* file, const std::string& str);

    void log_fatal(size_type index, size_type line, const char* file, const std::string& str);

    void log_trace(size_type index, const std::string& str);

    void log_debug(size_type index, const std::string& str);

    void log_error(size_type index, const std::string& str);

    void log_fatal(size_type index, const std::string& str);

    ~CapTure();

    void time_stamp(size_type index,const std::string& str) {
      time_t& start_ = get_thread_local_time_start();
      start_ = clock();
      size_type& index_ = get_thrad_local_file_index();
      index_ = index;
      log_debug(index, piece(str," time start : ", start_));
    }

    void time_record(const std::string& str) {
      const time_t& start_ = get_thread_local_time_start();
      time_t end_ = clock();
      size_type index_ = get_thrad_local_file_index();
      double dt = static_cast<double>(end_ - start_) / CLOCKS_PER_SEC;
      log_debug(index_, piece(str, " use ", dt, " s."));
    }

  private:

    CapTure(std::shared_ptr<BackEnd> b);

    void log(size_type index, Level level, size_type line, const char* file, const std::string& str);

    void log(size_type index, Level level, const std::string& str);

    time_t& get_thread_local_time_start() {
      thread_local time_t start;
      return start;
    }

    size_type& get_thrad_local_file_index() {
      thread_local size_type index;
      return index;
    }
  };
}//namespace pnlog