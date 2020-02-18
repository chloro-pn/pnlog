#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <unistd.h>
#include <iostream>

/*
 * log rotate can be supported by set op object.
 * pnlog support duration-based rotate models.
 */

using pnlog::backend;
int main() {
  pnlog::BackEnd::options op;
  //Rotate by duration, and each 3 seconds rotate the log file.
  op.duration_rotating = true;
  op.duration = std::chrono::seconds(3);
  //set rotate file path and name.
  op.path = "example5";
  backend->open(op, 1, new pnlog::FileOutStream("example5.txt"));
  auto cap1 = backend->get_capture(1);
  cap1->enable_time();
  for(int i = 0; i < 10; ++i) {
    cap1->trace(piece("example cap1 ", i));
    sleep(1);
    std::cout << "wait for 9 : "<< i << std::endl;
  }
  return 0;
}
