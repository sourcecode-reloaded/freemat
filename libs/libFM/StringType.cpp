#include "StringType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

void StringType::printElement(const Object &a, const ArrayFormatInfo &format, ndx_t ndx)
{
  const FMChar *t = this->readOnlyData(a);
  _ctxt->_io->output("%c",t[ndx]);
}
