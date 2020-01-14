#include <iostream>
#include <thread>
#include "pnlog.h"
#include "file_out_stream.h"

using pnlog::CapTure;
using pnlog::capture;
using pnlog::backend;

int main()
{
  capture->setLevel(CapTure::Level::PN_TRACE);
  for (int i = 2; i < 10; ++i) {
    backend->open(i, new pnlog::FileOutStream(piece("test", i, ".txt")));
    capture->log_trace(i, capture->get_current_time());
  }
  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";
  std::vector<std::thread> ths;
  capture->time_stamp(0, piece("begin."));
  for (int i = 2; i < 3; ++i) {
    ths.emplace_back([&,i]()->void {
      for (int k = 0; k < 1000000; ++k) {
        capture->log_trace(i,buf);
      }
    });
  }
  //backend->reopen(2, new pnlog::FileOutStream("reopenfile.txt"));
  for (auto& each : ths) {
    each.join();
  }
  capture->time_record(piece("over."));
  return 0;
}
