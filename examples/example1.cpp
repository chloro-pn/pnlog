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
