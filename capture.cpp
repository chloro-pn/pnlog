#include "capture.h"
#include "platform.h"
#include <Windows.h>

CapTure::CapTure(BackEnd& b) :back_(b), index_(0) {

}

/*
正常退出时默认不再有日志记录，
故fatal不必复用这个代码->因为很难,当fatal时可能其他线程正在往其他文件写东西，
如果要复用正常退出的代码，则为了保证fatal日志全部写入，不得不对每一次write都上锁。
提供一个刷盘功能：每个文件都有一个自旋锁，多个线程write一个文件时需要上自旋锁。
首先该自旋锁代价要够小，其次该自旋锁触发的概率要足够低。
自旋锁不要上在前端，要上在后端，因为后端退出的时候需要同步。
*/

void CapTure::log(size_type index, Level level,size_type line, const char* file, const std::string& str) {
  static thread_local CharArray tmp(buf_size_);
  tmp.append("file: ");
  tmp.append(file);
  tmp.append(" line: ");
  tmp.append(convert_to_string(line).c_str());
  tmp.append(" : ");
  tmp.append(str.c_str());
  tmp.append("\n");
  if (tmp.error() == true) {
    back_.stop();
    pf::fprintf(stderr, "log is too long!\n");
    back_.abort();
  }
  back_.write(index, tmp.getBuf(), tmp.getSize());
  if (level == Level::FATAL) {
    back_.stop();//只有一个线程会调用。
  }
  tmp.setZero();
}

void CapTure::log_trace(size_type index, size_type line, const char* file, const std::string& str) {
  if (default_level_ <= Level::TRACE) {
    log(index, Level::TRACE, line, file, str);
  }
}

void CapTure::log_debug(size_type index, size_type line, const char* file, const std::string& str) {
  if (default_level_ <= Level::DEBUG) {
    log(index, Level::DEBUG, line, file, str);
  }
}

void CapTure::log_error(size_type index, size_type line, const char* file, const std::string& str) {
  if (default_level_ <= Level::eERROR) {
    log(index, Level::eERROR, line, file, str);
  }
}

void CapTure::log_fatal(size_type index, size_type line, const char* file, const std::string& str) {
  if (default_level_ <= Level::FATAL) {
    log(index, Level::FATAL, line, file, str);
  }
}

void CapTure::log_trace(size_type index, const std::string& str) {
  if (default_level_ <= Level::TRACE) {
    log(index, Level::TRACE, -1, "", str);
  }
}

void CapTure::log_debug(size_type index, const std::string& str) {
  if (default_level_ <= Level::DEBUG) {
    log(index, Level::DEBUG, -1, "", str);
  }
}

void CapTure::log_error(size_type index, const std::string& str) {
  if (default_level_ <= Level::eERROR) {
    log(index, Level::eERROR, -1, "", str);
  }
}

void CapTure::log_fatal(size_type index, const std::string& str) {
  if (default_level_ <= Level::FATAL) {
    log(index, Level::FATAL, -1, "", str);
  }
}

void CapTure::setLevel(Level l) {
  default_level_ = l;
}

CapTure::Level CapTure::getLevel(){
  return default_level_;
}

CapTure::~CapTure() {

}

CapTure::Level CapTure::default_level_ = CapTure::Level::DEBUG;