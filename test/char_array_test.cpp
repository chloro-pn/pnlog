#include "../third_party/catch.hpp"
#include "../include/char_array.h"
#include <string.h>

TEST_CASE("char array test", "[char array]") {
  pnlog::CharArray ca(1024);
  REQUIRE(ca.getIndex() == -1);
  REQUIRE(ca.getSize() == 0);
  REQUIRE(ca.error() == false);
  ca.append("hello world");
  REQUIRE(ca.getSize() == strlen("hello world"));
  char buf[1025] = {0};
  ca.append(buf, 1024);
  REQUIRE(ca.error() == true);
}

