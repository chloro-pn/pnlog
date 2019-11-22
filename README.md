# pnlog
* [pnlog是什么？](#what-is-pnlog)

* [pnlog的特点？](#characteristic)

* [pnlog的性能？](#performance)

* [pnlog下一步计划？](#next-step)

* [如何使用pnlog？](#how-to-use-pnlog)
***
### <span id = "what-is-pnlog"> pnlog是什么</span> #
pnlog是一个基于c++11标准的日志记录库。
### <span id = "characteristic"> pnlog的特点</span> #
* 高效

    采用异步模型，日志首先会记录在缓冲buf中，当buf填满之后会由后台线程整块写入文件。
    仅开启1个后台线程处理所有日志落盘（实际上本程序可以支持自定义开启线程数目，但本身日志库是io密集型程序，且当日志文件开启不多时线程越少效率越高）。

* 线程安全

    多个线程向同一文件写入日志是线程安全的（包括标准输出和标准错误）。

* 可打开多个日志文件自由选择写入

    在记录之前可以打开多个日志文件，每个文件占有一索引号，日志记录时可以根据索引号选择对应的日志文件记录。pnlog在开始前默认将0号索引分配给stdout，1号索引分配给stderr，目前支持包括0号和1号在内的最多128个文件索引。
    
* 跨平台

    本日志库基于c11/c++11标准书写，故能够运行在所有支持c++11标准编译器的平台上。
    
* 日志分级

    pnlog将日志分级为trace、debug、error、fatal。当调用fatal记录日志后，会将内存中的日志落盘并调用std::abort()终止程序。

* 时间记录

    pnlog不支持每条日志都记录当前时刻，每个日志文件被打开后第一条日志将会写入本地日历时间，pnlog提供time_stamp函数和time_record函数，可用来记录两函数调用之间的间隔时间。一次调用time_stamp之后可以多次调用time_record函数，每次返回的时间间隔都是当前时刻到调用time_stamp函数时刻的时间间隔，随时可以调用time_stamp刷新时刻开始点。该机制是线程安全并独立的，可以记录不同线程各自的间隔时间。

    
* 支持缓冲、非缓冲模式

    pnlog支持用户选择缓冲或者非缓冲模式打开日志文件。调用open函数是缓冲模式打开，而调用open_syn是非缓冲模式打开。stdout、stderr设置为非缓冲模式打开。

### <span id = "performance">pnlog的性能</span> #
测试用机：

 操作系统 | 处理器 | 磁盘
------| ------ | ------
win10 | Intel(R) Core(TM) i5-7500 CPU 3.40GHz 4核 | WDC WD20EZRZ-22Z5HB0 5400转/分

测试分为三种类型：1.单线程写单文件。2.多线程写单文件。3.多线程写多文件。每条日志长91字节。

1.单线程写单文件
单线程写入200W次，测试10次，耗时0.935~1.121s。

2.多线程写单位件
6线程，每线程写入100W次，测试10次，耗时4.207~5.997s。

3.多线程写多文件
6线程分别写入6文件，每线程写入100W次，测试10次，耗时5.89~7.262s。

### <span id = "next-step">下一步计划</span>

* 自定义日志格式
    
    目前并不支持自定义格式。
    
* 自定义类的日志记录机制
    
    目前仅支持std::to_string可以转化的类型、c式字符串、std::string等类向日志输入，不支持自定义类向日志的输入。
    
### <span id = "how-to-use-pnlog"> 如何使用pnlog</span> #
pnlog库提供两个全局对象：capture和backend，capture对象用来捕获并记录日志，backend对象用来进行日志文件相关的操作。

example:

```
#include <iostream>
#include <thread>
#include "pnlog.h"

using pnlog::CapTure;
using pnlog::capture;
using pnlog::backend;

int main()
{
  capture->setLevel(CapTure::Level::PN_TRACE);
  for (int i = 2; i < 20; ++i) {
    backend->open(i, new pnlog::FileOutStream(piece("e://test",i,".txt")));
  }

  char buf[100] = "Youth is not a time of life; it is a state of mind. It is not a matter of rosy cheeks.";

  std::vector<std::thread> ths;
  for (int i = 0; i < 3; ++i) {
    ths.emplace_back([&,i]()->void{
      capture->time_stamp(0, piece("thread ", i, " loop begin."));
      for (int k = 0; k < 1000000; ++k) {
        capture->log_debug(rand() % 18 + 2, piece(buf, " ", k,":",i));
        if (k % 100000 == 0) {
          capture->time_record(piece("thread ", i, " k == ", k));
        }
      }
      capture->time_record(piece("thread ", i, " loop over."));
    });
  }
  for (int i = 0; i < 3; ++i) {
    ths.at(i).join();
  }
  system("pause");
  return 0;
}

```

当backend对象析构时会自动将内存中的日志落盘，不再需要显示的调用stop函数。

可以线程安全的调用capture.log_fatal(i,...);函数，该函数首先将此条日志写入日志文件i的缓存中，然后将所有当前时刻前产生的日志落盘，
最后调用std::abort()函数结束进程。多个线程调用capture.log_fatal函数，可能有多个fatal日志记录被落盘，但所有日志落盘操作以及std::abort()函数保证只调用一次。

关于piece：

piece是一个函数模板，该模板支持输入多于或等于一个的任意类型的参数，只要该类型能够被std::to_string转化、或是c式字符串，或是std::string类型。piece将输入的参数按序添加入一个std::string对象并返回。
