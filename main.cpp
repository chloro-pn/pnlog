// log.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <thread>
#include "pnlog.h"
#include <map>

using pnlog::CapTure;
using pnlog::capture;
using pnlog::backend;

/*有问题
新思路：
全部注册给线程池去执行，
不需要过度的同步措施。全异步会简单很多
由线程池去处理结束时
swapinback的操作（实际上不需要swapinback）
*/

int main()
{
  capture->setLevel(CapTure::Level::PN_TRACE);
  for (int i = 2; i < 10; ++i) {
    backend->open(i, new pnlog::FileOutStream(piece("e://test", i, ".txt")));
  }
  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";
  std::vector<std::thread> ths;
  for (int i = 0; i < 4; ++i) {
    ths.emplace_back([&, i]()->void {
      capture->time_stamp( 0, piece("thread ", i+2, " loop begin."));
      for (int k = 0; k < 1000000; ++k) {
        capture->log_trace(i + 2, piece(buf, ":", i + 2, ":", k));
      }
      capture->time_record(piece("thread ", i+2, " loop over."));
     // capture->log_fatal(0, piece(i + 2, " killed the process."));
    });
  }
  for (int i = 0; i < 4; ++i) {
    ths.at(i).join();
  }
  backend->close(2);

  system("pause");
  return 0;
}