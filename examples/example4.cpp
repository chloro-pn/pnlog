#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <iostream>
#include <cstdio>

//capture would not close log file identified by index,
//you can call close method to do it.

using pnlog::backend;
int main() {
  pnlog::BackEnd::options op;
  backend->open(op, 1, new pnlog::FileOutStream("example4"));
  auto cap1 = backend->get_capture(1);
  cap1->enable_time();
  for(int i = 0; i < 5; ++i) {
    cap1->trace(piece("example cap1 ", i));
  }
  cap1->close();
  std::cout << "the program is running now, but log file has been closed, try to "
               "check out it.\n";
  getchar();
  return 0;
}
