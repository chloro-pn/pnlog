#include "pnlog.h"
#include <memory>

namespace pnlog {
  std::shared_ptr<BackEnd> backend = BackEnd::get_instance();
  std::shared_ptr<CapTure> capture = CapTure::get_instance(backend);
}
