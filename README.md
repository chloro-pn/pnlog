# pnlog
a c++ log library based on c++11 for linux.

# features
* 多日志文件
* 基于日志文件的同步/异步模式
* 基于前端对象的日志等级
* 基于日志文件的按时间间隔回滚
* 日志文件运行时关闭
* 支持输出到stdout
* 支持打开/关闭时间戳记录
* 日志记录，reopen，close接口线程安全

# build
pnlog uses cmake build tool to build the project.
```console
mkdir build && cd build
cmake ..
make
```

# license
MIT License

# how to use
you can research example code in /examples dir to learn how to use pnlog.

## write to stdout.

* Global object backend's type is std::shared_ptr<BackEnd>, and you can only
have one object of the class BackEnd, which means the constructor of class BackEnd
is privated.
* Global object backend is in charge of opening, closing, and reopening various
log files, and each of log files managed by backend is identified by an index.
* stdcout is opened by backend by default and it's index is 0.
* You can call backend's get_capture method to create a std::shared<CapTure> obejct from backend.
The parameter is the index of log file, each log submitd by CapTure object will be appended to
log file identified by index.

```c++
#include <iostream>
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"

using pnlog::backend;

int main() {
  auto capture = backend->get_capture(0);
  for(int i = 0; i < 10; ++i) {
    if(i >= 5) {
      capture->enable_time();
    }
    if(i >= 8) {
      capture->disable_time();
    }
    if(i % 2 == 0) {
      capture->trace(piece("hello world ! ", " ", i));
    }
    else {
      capture->trace(piece("hello world ! ", " ", i), __FILE__, __LINE__);
    }
  }
  return 0;
}
```

## use enable_time() method and disable_time() method to choose weather cap1 log timestamp.
```c++ 
#include "../include/pnlog.h"

using pnlog::backend;
int main() {
  auto cap1 = backend->get_capture(0);
  cap1->enable_time();
  for(int i = 0; i < 10; ++i) {
    if(i > 5) {
      cap1->setLevel(pnlog::CapTure::Level::PN_WARNING);
    }
    cap1->trace(piece("example cap1 ", i));
  }
  return 0;
}
```

## CapTure object can be constructed by invalid index(no log files correspond to it), under the condition ,the log will be abandoned.
* More than one CapTure object can be constructed by the one index, and cap1 and cap2 are different objects :
```c++
auto cap1 = backend->get_capture(0);
auto cap2 = backend->get_capture(0);
```
* each CapTure object has own log_level and log format.
```c++
#include "../include/pnlog.h"

using pnlog::backend;
int main() {
  auto cap1 = backend->get_capture(0);
  auto cap2 = backend->get_capture(0);
  auto cap3 = backend->get_capture(1); // index 1 is invalid.
  cap1->enable_time();
  for(int i = 0; i < 5; ++i) {
    cap1->trace(piece("example cap1 ", i));
    cap2->trace(piece("example cap2 ", i), __FILE__, __LINE__);
    cap3->trace(piece("example cap3 ")); // do nothing.
  }
  return 0;
}
```
## open a log file.
* Use backend->open method to open a log file, and you can use pnlog::BackEnd::options
object to choose the different model.
* each log file can be opened by sync model (which means call out_stream_base->
write() for each log)and async model (store a lot of logs in memory cache and
write to log file once).
```c++
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <cstdio>
#include <iostream>

using pnlog::backend;
int main() {
  pnlog::BackEnd::options op;
  op.asyn = false; // op.asyn = true by default.
  backend->open(op, 1, new pnlog::FileOutStream("example3log.txt"));
  auto cap1 = backend->get_capture(1);
  cap1->enable_time();
  for(int i = 0; i < 5; ++i) {
    cap1->trace(piece("example3 ", i));
  }
  cap1->close();
  std::cout << "syn model \n";
  return 0;
}
```
## capture would not close log file identified by index, you can call close method to do it.
```c++
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <iostream>
#include <cstdio>

using pnlog::backend;
int main() {
  pnlog::BackEnd::options op;
  backend->open(op, 1, new pnlog::FileOutStream("example4log.txt"));
  auto cap1 = backend->get_capture(1);
  cap1->enable_time();
  for(int i = 0; i < 5; ++i) {
    cap1->trace(piece("example cap1 ", i));
  }
  cap1->close();
  std::cout << "the program is running now, but log file has been closed, try to "
               "check out it.\n";
  getchar();
  return 0;
}
```

## log rotate can be supported by set op object. pnlog support duration-based rotate models.
```c++
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <unistd.h>
#include <iostream>

using pnlog::backend;
int main() {
  pnlog::BackEnd::options op;
  //Rotate by duration, and each 3 seconds rotate the log file.
  op.duration_rotating = true;
  op.duration = std::chrono::seconds(3);
  //set rotate file path and name.
  op.path = "example5";
  backend->open(op, 1, new pnlog::FileOutStream("example5.txt"));
  auto cap1 = backend->get_capture(1);
  cap1->enable_time();
  for(int i = 0; i < 10; ++i) {
    cap1->trace(piece("example cap1 ", i));
    sleep(1);
    std::cout << "wait for 9 : "<< i << std::endl;
  }
  return 0;
}
```

## thread-safe reopen method and close method.
```c++
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace pnlog;

int main() {
  BackEnd::options op;
  backend->open(op, 1, new FileOutStream("example6log.txt"));
  backend->open(op, 2, new FileOutStream("example6_closedlog.txt"));
  auto cap = backend->get_capture(1);
  auto cap2 = backend->get_capture(2);
  std::thread th([&]()->void {
                   for(int i = 0; i < 10000; ++i) {
                     cap->trace(piece("writing in back thread. ", i));
                     cap2->trace(piece("waiting for close.", i));
                   }
                 });
  backend->close(2);
  usleep(1e4);
  backend->reopen(1, new FileOutStream("example6_reopenlog.txt"));
  th.join();
  std::cout << "please check example6*.txt file \n";
  return 0;
}
```

## configure log schedule for your class.
```c++
#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <memory>

using pnlog::CapTure;
using pnlog::backend;
using pnlog::BackEnd;

class test {
private:
  int num_;//your data member.
  std::shared_ptr<CapTure> logger;

public:
  test():num_(0), logger(backend->get_capture(0)) {
    logger->enable_time();
    logger->setLevel(CapTure::Level::PN_DEBUG);
  }

  void add() {
    ++num_;
    logger->trace(piece("num change : ", num_), __FILE__, __LINE__);
  }

  void cut_down() {
    if(num_ <= 0) {
      logger->warning(piece("num now : ", num_), __FILE__, __LINE__);
    }
    --num_;
    return;
  }

  ~test() = default;
};

int main() {
  test t;
  t.add();
  t.cut_down();
  t.cut_down();
  return 0;
}
```
