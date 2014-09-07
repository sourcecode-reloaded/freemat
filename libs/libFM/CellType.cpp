#include "CellType.hpp"
#include "ThreadContext.hpp"
#include "TermIF.hpp"

using namespace FM;

void CellType::printElement(const Object &a, const ArrayFormatInfo &info, ndx_t ndx) {
  const Object *t = this->readOnlyData(a);
  _ctxt->_io->output("[" + t[ndx].description() + "]");
}
