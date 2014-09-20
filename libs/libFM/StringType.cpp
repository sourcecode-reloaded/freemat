#include "StringType.hpp"
#include "ThreadContext.hpp"
#include "AllTypes.hpp"

using namespace FM;

Object StringType::asIndex(const Object &a, dim_t ndx)
{
  return _ctxt->_index->makeScalar(-1);
}

void StringType::printElement(const Object &a, const ArrayFormatInfo &format, ndx_t ndx)
{
  const FMChar *t = this->readOnlyData(a);
  _ctxt->_io->output("%c",t[ndx]);
}
