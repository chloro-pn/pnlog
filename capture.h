#pragma once
#include "type.h"
#include "back_end.h"
#include "str_appender.h"
#include <ctime>
#include <string>
#include <memory>
#include <sys/time.h>

namespace pnlog {
  using namespace std::chrono;

  class CapTure {
  public:
    enum class Level { PN_TRACE, PN_DEBUG, PN_INFO, PN_WARNING, PN_ERROR, PN_FATAL };

  private:
    std::shared_ptr<BackEnd> back_;
    static Level default_level_;

  public:
    static constexpr size_type buf_size_ = 4096;

    static std::shared_ptr<CapTure> get_instance(std::shared_ptr<BackEnd> back);

    static void setLevel(Level l);

    static Level getLevel();

    CapTure(const CapTure&) = delete;
    CapTure(CapTure&&) = delete;
    CapTure& operator=(const CapTure&) = delete;
    CapTure& operator=(CapTure&&) = delete;

    inline
    void trace(size_type index, const std::string& str) {
      if (default_level_ <= Level::PN_TRACE) {
        log(index, "TRACE", str);
      }
    }

    inline
    void debug(size_type index, const std::string& str) {
      if (default_level_ <= Level::PN_DEBUG) {
        log(index, "DEBUG", str);
      }
    }

    inline
    void info(size_type index, const std::string& str) {
      if (default_level_ <= Level::PN_INFO) {
        log(index, "INFO", str);
      }
    }

    inline
    void warning(size_type index, const std::string& str) {
      if (default_level_ <= Level::PN_WARNING) {
        log(index, "WARNING", str);
      }
    }

    inline
    void error(size_type index, const std::string& str) {
      if (default_level_ <= Level::PN_ERROR) {
        log(index, "ERROR", str);
      }
    }

    inline
    void fatal(size_type index, const std::string& str) {
      if (default_level_ <= Level::PN_FATAL) {
        log(index, "FATAL", str);
      }
    }

    ~CapTure();

    void time_stamp(size_type index,const std::string& str) {
      time_point<system_clock,milliseconds>& start_ = get_thread_local_time_start();
      start_ = time_point_cast<milliseconds>(system_clock::now());
      size_type& index_ = get_thrad_local_file_index();
      index_ = index;
      trace(index, piece(str," time start!"));
    }

    void time_record(const std::string& str) {
      time_point<system_clock,milliseconds>& start_ = get_thread_local_time_start();
      time_point<system_clock,milliseconds> end_ = time_point_cast<milliseconds>(system_clock::now());
      size_type index_ = get_thrad_local_file_index();
      uint64_t dt = (end_ - start_).count();
      trace(index_, piece(str, " use ", dt, " ms."));
    }

    std::string get_current_time() const {
      /*
      std::time_t current_time = std::time(nullptr);
      char buf[128];
      std::strftime(buf, sizeof(buf), "%c", std::localtime(&current_time));
      std::string result(buf, sizeof(buf));
      */
      timeval time;
      gettimeofday(&time,nullptr);
      char buf[64] = {0};
      time_t seconds = time.tv_sec;
      struct tm tm_time;
      gmtime_r(&seconds, &tm_time);

      int microseconds = static_cast<int>(time.tv_usec);
      snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
               microseconds);

      return buf;
    }

  private:
    CapTure(std::shared_ptr<BackEnd> b);

    void log(size_type index, const char* level, const std::string& str);

    time_point<system_clock,milliseconds>& get_thread_local_time_start() {
      thread_local time_point<system_clock,milliseconds> start;
      return start;
    }

    size_type& get_thrad_local_file_index() {
      thread_local size_type index;
      return index;
    }
  };
}//namespace pnlog
