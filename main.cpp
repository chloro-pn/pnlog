#include <iostream>
#include <thread>
#include "pnlog.h"
#include "file_out_stream.h"
#include "back_end.h"
#include <ctime>

using pnlog::CapTure;
using pnlog::capture;
using pnlog::backend;

/*
upadte 20200115.
日志回核心功能已经完成，考虑路径解析。
*/

int main()
{
  capture->setLevel(CapTure::Level::PN_TRACE);
  pnlog::BackEnd::options option;
  
  option.asyn = true;
  option.duration_rotating = true;
  option.duration = std::chrono::seconds(1);
  option.path = std::string("pnlog");
  backend->open(option, 2, new pnlog::FileOutStream("pnlog"));
  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";

  std::vector<std::thread> ths;

  time_t start, end;
  start = clock();
  for (int i = 2; i < 3; ++i) {
    ths.emplace_back([&,i]()->void {
      for (int k = 0; k < 1000000; ++k) {
        capture->log_trace(i, piece(buf, " ", k));
      }
    });
  }
  //backend->reopen(2, new pnlog::FileOutStream("reopenfile.txt"));
  for (auto& each : ths) {
    each.join();
  }
  backend->stop();
  end = clock();
  std::cout << "use : " << (double)(end - start) / CLOCKS_PER_SEC << " seconds." << std::endl;
  system("pause");
  return 0;
}
