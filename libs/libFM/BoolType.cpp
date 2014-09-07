#include "BoolType.hpp"
#include "BaseTypes.hpp"
#include "ThreadContext.hpp"

using namespace FM;

Object BoolType::asIndexNoBoundsCheck(const Object &a)
{
  const bool *ip = this->readOnlyData(a);
  dim_t len = a.elementCount();
  dim_t trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _ctxt->_index->makeMatrix(trueCount,1);
  ndx_t *op = _ctxt->_index->readWriteData(output);
  trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;  
}

Object BoolType::asIndex(const Object &a, dim_t max)
{
  if (a.dims().elementCount() > max)
    throw Exception("Index out of range");
  const bool *ip = this->readOnlyData(a);
  dim_t len = a.elementCount();
  dim_t trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _ctxt->_index->makeMatrix(trueCount,1);
  ndx_t *op = _ctxt->_index->readWriteData(output);
  trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;
}

bool BoolType::any(const Object &a)
{
  const bool *ip = this->readOnlyData(a);
  dim_t len = a.elementCount();
  for (dim_t i=0;i<len;i++)
    if (ip[i]) return true;
  return false;
}

bool BoolType::all(const Object &a)
{
  const bool *ip = this->readOnlyData(a);
  dim_t len = a.elementCount();
  for (dim_t i=0;i<len;i++)
    if (!ip[i]) return false;
  return true;
}
