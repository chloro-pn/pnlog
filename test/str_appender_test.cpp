#include "../include/str_appender.h"
#include "../third_party/catch.hpp"

TEST_CASE("str appender test") {
  std::string result = pnlog::piece("hello");
  REQUIRE(result == "hello");
  result = pnlog::piece("number:", 4);
  REQUIRE(result == "number:4");
  result = pnlog::piece("nothing:", " ");
  REQUIRE(result == "nothing: ");
  result = pnlog::piece(std::string("num :"), " ", 4);
  REQUIRE(result == "num : 4");
}

