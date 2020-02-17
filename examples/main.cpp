#include <iostream>
#include <thread>
#include "../include/pnlog.h"
#include "../include/back_end.h"
#include "../include/file_out_stream.h"
#include <chrono>

using pnlog::backend;

char buf[100] = "Youth is not a time of life; "
                "it is a state of mind. It is not a matter of rosy cheeks.";
/*
 * example 1
 *
 */

int main()
{
  auto capture = backend->get_capture(0);

  for(int i = 0; i < 10; ++i) {
    if(i >= 5) {
      capture->enable_time();
    }
    if(i >= 8) {
      capture->disable_time();
    }
    capture->trace(piece(buf, " ", i));
  }
  return 0;
}
