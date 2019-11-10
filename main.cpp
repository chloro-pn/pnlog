// log.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
    backend.open(2, new FileOutStream("d://test.txt"));
    clock_t start, end;
    start = clock();
    std::thread th([&]() {
      for (int i = 0; i < 1000; ++i) {
        capture.log_debug(0, __LINE__, __FILE__, piece("hello world!", i));
      };
    });
    std::thread th2([&]() {
      for (int i = 0; i < 1000; ++i) {
        capture.log_debug(0, __LINE__, __FILE__, piece("hello", i));
      }
      capture.log_fatal(2, __LINE__, __FILE__, piece("hello"));
    });
    th.join();
    th2.join();
    end = clock();
    std::cout << (double)(end - start) / CLOCKS_PER_SEC;
    backend.stop();
    system("pause");
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
