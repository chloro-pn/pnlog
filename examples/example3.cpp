#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <cstdio>
#include <iostream>
/*
 * Use backend->open method to open a log file, and you can use pnlog::BackEnd::options
 * object to choose the different model.
 * each log file can be opened by sync model (which means call out_stream_base->
 * write() for each log)and async model (store a lot of logs in memory cache and
 * write to log file once).
 */

using pnlog::backend;
int main() {
  pnlog::BackEnd::options op;
  op.asyn = false; // op.asyn = true by default.
  backend->open(op, 1, new pnlog::FileOutStream("example3log.txt"));
  auto cap1 = backend->get_capture(1);
  cap1->enable_time();
  for(int i = 0; i < 5; ++i) {
    cap1->trace(piece("example3 ", i));
  }
  cap1->close();
  std::cout << "syn model \n";
  return 0;
}
