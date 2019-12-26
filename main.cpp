// log.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <thread>
#include "pnlog.h"
#include <map>

//新模型基本搞定，下午修改chararray index的问题和其他细节。

using pnlog::CapTure;
using pnlog::capture;
using pnlog::backend;

int main()
{
  capture->setLevel(CapTure::Level::PN_TRACE);
  for (int i = 2; i < 10; ++i) {
    backend->open(i, std::shared_ptr<pnlog::FileOutStream>(new pnlog::FileOutStream(piece("e://test", i, ".txt"))));
  }
  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";
  std::vector<std::thread> ths;
  capture->time_stamp(0, piece("begin."));
  for (int i = 2; i < 5; ++i) {
    ths.emplace_back([&, i]()->void {
      for (int k = 0; k < 1; ++k) {
        capture->log_trace(i, piece(buf, ":", i, ":", k));
      }
      backend->close(i);
      backend->open(i,std::shared_ptr<pnlog::FileOutStream>(new pnlog::FileOutStream(piece("e://test", i, "after.txt"))));
      capture->log_trace(i, piece(buf, ":", i));
    });
  }
  for (int i = 0; i < 3; ++i) {
    ths.at(i).join();
  }
  capture->time_record(piece("over."));
  backend->stop();
  system("pause");
  return 0;
}