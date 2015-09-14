#include "BoolType.hpp"
#include "ThreadContext.hpp"
#include "IntegerType.hpp"
#include "Operators.hpp"
#include "Tuple.hpp"

using namespace FM;

template <class Op>
static inline Object bool_op(const Object &a, const Object &b, BoolType *o)
{
  Object ta = a.type()->asLogical(a);
  Object tb = b.type()->asLogical(b);
  int incr_a = ta.isScalar() ? 0 : 1;
  int incr_b = tb.isScalar() ? 0 : 1;
  const bool *pa = o->ro(ta);
  const bool *pb = o->ro(tb);
  ndx_t count = std::max<ndx_t>(ta.count(),tb.count());
  Object tc = o->zeroArrayOfSize(Tuple::computeDotOpSize(ta.dims(),tb.dims()));
  bool *pc = o->rw(tc);
  for (ndx_t i=0;i<count;i++)
    Op::template func<bool,bool,bool,bool>(pc+i,pa+i*incr_a,pb+i*incr_b);
  return tc;
}

Object BoolType::And(const Object &a, const Object &b) {
  return bool_op<OpAnd>(a,b,this);
}

Object BoolType::Or(const Object &a, const Object &b) {
  return bool_op<OpOr>(a,b,this);
}

Object BoolType::Equals(const Object &a, const Object &b) {
  return bool_op<OpEQ>(a,b,this);
}

Object BoolType::NotEquals(const Object &a, const Object &b) {
  return bool_op<OpNE>(a,b,this);
}

Object BoolType::Not(const Object &a)
{
  Object output = _ctxt->_bool->zeroArrayOfSize(a.dims());
  bool *op = this->rw(output);
  const bool *ip = this->ro(a);
  for (ndx_t i=0;i<a.count();i++)
    op[i] = !ip[i];
  return output;
}

Object BoolType::asIndexNoBoundsCheck(const Object &a)
{
  const bool *ip = this->ro(a);
  ndx_t len = a.count();
  ndx_t trueCount = 0;
  for (ndx_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _ctxt->_index->makeMatrix(trueCount,1);
  ndx_t *op = _ctxt->_index->rw(output);
  trueCount = 0;
  for (ndx_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;  
}

Object BoolType::asIndex(const Object &a, ndx_t max)
{
  if (a.dims().count() > max)
    throw Exception("Index out of range");
  const bool *ip = this->ro(a);
  ndx_t len = a.count();
  ndx_t trueCount = 0;
  for (ndx_t i=0;i<len;i++)
    if (ip[i]) trueCount++;
  Object output = _ctxt->_index->makeMatrix(trueCount,1);
  ndx_t *op = _ctxt->_index->rw(output);
  trueCount = 0;
  for (ndx_t i=0;i<len;i++)
    if (ip[i]) op[trueCount++] = i;
  return output;
}

bool BoolType::any(const Object &a)
{
  const bool *ip = this->ro(a);
  ndx_t len = a.count();
  for (ndx_t i=0;i<len;i++)
    if (ip[i]) return true;
  return false;
}

bool BoolType::all(const Object &a)
{
  const bool *ip = this->ro(a);
  ndx_t len = a.count();
  for (ndx_t i=0;i<len;i++)
    if (!ip[i]) return false;
  return true;
}

ndx_t BoolType::countOne(const Object &a)
{
  const bool *ip = this->ro(a);
  ndx_t len = a.count();
  ndx_t count = 0;
  for (ndx_t i=0;i<len;i++)
    if (ip[i]) count++;
  return count;
}

ndx_t BoolType::countZero(const Object &a)
{
  const bool *ip = this->ro(a);
  ndx_t len = a.count();
  ndx_t count = 0;
  for (ndx_t i=0;i<len;i++)
    if (!ip[i]) count++;
  return count;  
}
