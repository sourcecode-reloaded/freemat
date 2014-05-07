#include "BoolType.hpp"
#include "BaseTypes.hpp"

using namespace FM;

Object BoolType::asIndex(const Object &a, dim_t max)
{
  if (a.dims().elementCount() > max)
    throw Exception("Index out of range");
  const bool *ip = this->readOnlyData(a);
  dim_t len = a.elementCount();
  dim_t trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _base->_index->makeMatrix(trueCount,1);
  ndx_t *op = _base->_index->readWriteData(output);
  trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;
}
