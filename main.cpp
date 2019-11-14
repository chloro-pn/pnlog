// log.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <string>
//前端： 
#include <thread>
#include <ctime>
#include "pnlog.h"

using pnlog::CapTure;
using pnlog::capture;
using pnlog::backend;

int main()
{
  capture.setLevel(CapTure::Level::PN_TRACE);
  for (int i = 2; i < 20; ++i) {
    backend.open(i, new pnlog::FileOutStream(piece("e://test",i,".txt")));
  }

  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";

  std::vector<std::thread> ths;
  for (int i = 0; i < 3; ++i) {
    ths.emplace_back([&,i]()->void{
      capture.time_stamp(0, piece("thread ", i, " loop begin."));
      for (int k = 0; k < 1000000; ++k) {
        capture.log_debug(rand() % 18 + 2, piece(buf, " ", k,":",i));
        if (k % 100000 == 0) {
          capture.time_record(piece("thread ", i, " k == ", k));
        }
      }
      capture.time_record(piece("thread ", i, " loop over."));
    });
  }
  for (int i = 0; i < 3; ++i) {
    ths.at(i).join();
  }
  system("pause");
  return 0;
}