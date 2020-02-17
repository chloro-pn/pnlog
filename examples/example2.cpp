#include "../include/pnlog.h"

/*
* 5. CapTure object can be constructed by invalid index(no log files correspond to it),
* under the condition ,the log will be abandoned.
* 6. More than one CapTure object can be constructed by the one index, like :
* auto cap1 = backend->get_capture(0);
* auto cap2 = backend->get_capture(0);
* cap1 and cap2 are different objects.
* 7. each CapTure object has own log_level and log format.
*/

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
