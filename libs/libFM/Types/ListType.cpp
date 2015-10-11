#include "ListType.hpp"
#include "ThreadContext.hpp"
#include "TermIF.hpp"

using namespace FM;

void ListType::printElement(const Object &a, const ArrayFormatInfo &, ndx_t ndx) {
  const Object *t = this->ro(a);
  _ctxt->_io->output(t[ndx].description());
}
