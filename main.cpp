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
  backend.open(2, new pnlog::FileOutStream(piece("e://test.txt")));
  clock_t start, end;
  start = clock();

  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";

  for (int k = 0; k < 2000000; ++k) {
    capture.log_debug(2, piece(buf, " ", k));
  }

  backend.stop();
  end = clock();
  std::cout << (double)(end - start) / CLOCKS_PER_SEC;
  system("pause");
  return 0;
}