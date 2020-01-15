﻿#include <iostream>
#include <thread>
#include "pnlog.h"
#include "file_out_stream.h"
#include "back_end.h"
#include <chrono>

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
  backend->open(option, 2, new pnlog::FileOutStream("pnlogger"));
  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";

  std::vector<std::thread> ths;

  using namespace std::chrono;
  time_point<system_clock, milliseconds> start = time_point_cast<milliseconds>(system_clock::now());
  for (int i = 2; i < 5; ++i) {
    ths.emplace_back([&,i]()->void {
      for (int k = 0; k < 1000000; ++k) {
        capture->log_trace(2, piece(buf, " ", k));
      }
    });
  }
  //backend->reopen(2, new pnlog::FileOutStream("reopenfile.txt"));
  for (auto& each : ths) {
    each.join();
  }
  backend->stop();
  time_point<system_clock,milliseconds> end = time_point_cast<milliseconds>(system_clock::now());
  std::cout << "use : " << (end - start).count() << " milliseconds." << std::endl;
  system("pause");
  return 0;
}
