#pragma once
#include <cstdio>
#include <cstdlib>

#define release_assert(x) \
  if(!(x)) { \
    std::abort(); \
  } \
