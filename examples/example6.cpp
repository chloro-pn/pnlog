#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <thread>
#include <iostream>
#include <unistd.h>

/*
 * thread-safe reopen method and close method.
 */
using namespace pnlog;

int main() {
  BackEnd::options op;
  backend->open(op, 1, new FileOutStream("example6log.txt"));
  backend->open(op, 2, new FileOutStream("example6_closedlog.txt"));
  auto cap = backend->get_capture(1);
  auto cap2 = backend->get_capture(2);
  std::thread th([&]()->void {
                   for(int i = 0; i < 10000; ++i) {
                     cap->trace(piece("writing in back thread. ", i));
                     cap2->trace(piece("waiting for close.", i));
                   }
                 });
  backend->close(2);
  usleep(1e4);
  backend->reopen(1, new FileOutStream("example6_reopenlog.txt"));
  th.join();
  std::cout << "please check example6*.txt file \n";
  return 0;
}
