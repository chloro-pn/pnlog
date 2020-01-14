#include "convert.h"

namespace pnlog {
  template<>
  std::string convert_to_string(const std::string& str) {
    return str;
  }

  std::string convert_to_string(const char* ptr) {
    return std::string(ptr);
  }
/*
  template<>
  std::string convert_to_string(const int& i) {
      std::string buf;
      buf.resize(32);
      snprintf(const_cast<char*>(buf.data()),32,"%d",i);
      return buf;
  }*/
}//namespace pnlog
