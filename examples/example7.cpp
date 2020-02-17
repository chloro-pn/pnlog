#include "../include/pnlog.h"
#include "../include/file_out_stream.h"
#include <memory>
/*
 * configure log schedule for your class.
 */

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
