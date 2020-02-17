#include <iostream>
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"

using pnlog::backend;

/*
 * 1. Global object backend's type is std::shared_ptr<BackEnd>, and you can only
 * have one object of the class BackEnd, which means the constructor of class BackEnd
 * is privated.
 * 2. Global object backend is in charge of opening, closing, and reopening various
 * log files, and each of log files managed by backend is identified by an index.
 * 3. stdcout is opened by backend by default and it's index is 0.
 * 4. You can call backend's get_capture method to create a std::shared<CapTure> obejct from backend.
 * The parameter is the index of log file, each log submitd by CapTure object will be appended to
 * log file identified by index.
 */

int main() {
  auto capture = backend->get_capture(0);

  for(int i = 0; i < 10; ++i) {
    if(i >= 5) {
      capture->enable_time();
    }
    if(i >= 8) {
      capture->disable_time();
    }
    if(i % 2 == 0) {
      capture->trace(piece("hello world ! ", " ", i));
    }
    else {
      capture->trace(piece("hello world ! ", " ", i), __FILE__, __LINE__);
    }
  }
  return 0;
}
