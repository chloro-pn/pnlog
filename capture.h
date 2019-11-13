#pragma once
#include "char_array.h"
#include "back_end.h"
#include "type.h"
#include "convert.h"
#include "spin_lock.h"
#include <array>
#include <iostream>
#include <functional>
#include <thread>
#include <list>
#include <condition_variable>
#include <mutex>
#include <array>

namespace pnlog {
  class CapTure {
  public:
    enum class Level { PN_TRACE, PN_DEBUG, PN_ERROR, PN_FATAL };

  private:
    BackEnd & back_;
    size_type index_;
    static Level default_level_;

  public:

    static constexpr size_type buf_size_ = 4096;

    static void setLevel(Level l);

    static Level getLevel();

    CapTure(BackEnd& b);

    void log(size_type index, Level level, size_type line, const char* file, const std::string& str);

    void log(size_type index, Level level, const std::string& str);

    void log_trace(size_type index, size_type line, const char* file, const std::string& str);

    void log_debug(size_type index, size_type line, const char* file, const std::string& str);

    void log_error(size_type index, size_type line, const char* file, const std::string& str);

    void log_fatal(size_type index, size_type line, const char* file, const std::string& str);

    void log_trace(size_type index, const std::string& str);

    void log_debug(size_type index, const std::string& str);

    void log_error(size_type index, const std::string& str);

    void log_fatal(size_type index, const std::string& str);

    ~CapTure();
  };
}//namespace pnlog