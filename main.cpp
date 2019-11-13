﻿// log.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <string>
//前端： 
#include <thread>
#include <ctime>
#include "pnlog.h"

int main()
{
  capture.setLevel(CapTure::Level::TRACE);
  for (int i = 2; i < 8; ++i) {
    backend.open(i, new FileOutStream(piece("e://test", i, ".txt")));
  }
  std::vector<std::thread> ths;
  clock_t start, end;
  start = clock();
  char buf[100] = "wo ai ni";
  
  for (int i = 0; i < 6; ++i) {
    ths.emplace_back([&,i]() {
    for (int k = 0; k < 1000000; ++k) {
      capture.log_debug(i + 2 , __LINE__, __FILE__, piece(buf,k));
    }
    capture.log_fatal(2, piece("cut : ", i + 2));
    });
  }
  
  for (int i = 0; i <6; ++i) {
    ths.at(i).join();
  }
  backend.stop();
  end = clock();
  std::cout << (double)(end - start) / CLOCKS_PER_SEC;
  system("pause");
  return 0;
}