#pragma once
#include "back_end.h"
#include "capture.h"
#include <memory>

namespace pnlog {
  extern std::shared_ptr<BackEnd> backend;
  extern std::shared_ptr<CapTure> capture;
}

using pnlog::piece;
