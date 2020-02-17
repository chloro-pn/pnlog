#include "../include/str_appender.h"
#include <iostream>
#include <string>

using pnlog::piece;

int main() {
  std::string result = piece("hello world");
  std::cout << result << std::endl;
  result = piece("number : ", 2);
  std::cout << result << std::endl;
  result = piece("float : ", 3.4f);
  std::cout << result << std::endl;
  result = piece("str ", "piece");
  std::cout << result << std::endl;
  return 0;
}
