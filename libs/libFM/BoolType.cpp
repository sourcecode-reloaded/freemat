#include "BoolType.hpp"
#include "BaseTypes.hpp"
#include "ThreadContext.hpp"

using namespace FM;

Object BoolType::Not(const Object &a)
{
  Object output = _ctxt->_bool->zeroArrayOfSize(a.dims(),false);
  bool *op = this->rw(output);
  const bool *ip = this->ro(a);
  for (dim_t i=0;i<a.count();i++)
    op[i] = !ip[i];
  return output;
}

Object BoolType::asIndexNoBoundsCheck(const Object &a)
{
  const bool *ip = this->ro(a);
  dim_t len = a.count();
  dim_t trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _ctxt->_index->makeMatrix(trueCount,1);
  ndx_t *op = _ctxt->_index->rw(output);
  trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;  
}

Object BoolType::asIndex(const Object &a, dim_t max)
{
  if (a.dims().count() > max)
    throw Exception("Index out of range");
  const bool *ip = this->ro(a);
  dim_t len = a.count();
  dim_t trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _ctxt->_index->makeMatrix(trueCount,1);
  ndx_t *op = _ctxt->_index->rw(output);
  trueCount = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;
}

bool BoolType::any(const Object &a)
{
  const bool *ip = this->ro(a);
  dim_t len = a.count();
  for (dim_t i=0;i<len;i++)
    if (ip[i]) return true;
  return false;
}

bool BoolType::all(const Object &a)
{
  const bool *ip = this->ro(a);
  dim_t len = a.count();
  for (dim_t i=0;i<len;i++)
    if (!ip[i]) return false;
  return true;
}

dim_t BoolType::countOne(const Object &a)
{
  const bool *ip = this->ro(a);
  dim_t len = a.count();
  dim_t count = 0;
  for (dim_t i=0;i<len;i++)
    if (ip[i]) count++;
  return count;
}

dim_t BoolType::countZero(const Object &a)
{
  const bool *ip = this->ro(a);
  dim_t len = a.count();
  dim_t count = 0;
  for (dim_t i=0;i<len;i++)
    if (!ip[i]) count++;
  return count;  
}
