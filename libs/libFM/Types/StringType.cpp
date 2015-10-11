#include "StringType.hpp"
#include "ThreadContext.hpp"
#include "AllTypes.hpp"

using namespace FM;

Object StringType::asIndexNoBoundsCheck(const Object &)
{
  return _ctxt->_index->makeScalar(-1);
}

Object StringType::asIndex(const Object &, ndx_t)
{
  return _ctxt->_index->makeScalar(-1);
}

void StringType::printElement(const Object &a, const ArrayFormatInfo &, ndx_t ndx)
{
  const FMChar *t = this->ro(a);
  _ctxt->_io->output("%c",t[ndx]);
}
