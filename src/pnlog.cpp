#include "../include/pnlog.h"
#include <memory>

namespace pnlog {
  std::shared_ptr<BackEnd> backend = BackEnd::get_instance();
}
