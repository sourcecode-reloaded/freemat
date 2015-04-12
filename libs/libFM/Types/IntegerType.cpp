#include "IntegerType.hpp"
#include "ThreadContext.hpp"
#include "Type.hpp"
#include "BoolType.hpp"

using namespace FM;

const int MSGBUFLEN = 100;

// FIXME - replace with log10?
template <class T>
static int GetNominalWidthInteger(const T* qp, ndx_t len) {
  char buffer[MSGBUFLEN];
  int maxdigit = 0;
  for (ndx_t i=0;i<len;i++) {
    memset(buffer,0,MSGBUFLEN);
    sprintf(buffer,"%lld",(int64_t)qp[i]);
    int j = maxdigit;
    while (buffer[j] && j>=maxdigit)
      j++;
    maxdigit = j;
  }
  return maxdigit;
}

template <class T, FM::DataCode codeNum, class Op>
static inline Object int_cmpop(const Object &a, const Object &b, BoolType *o)
{
  if (a.isScalar() && b.isScalar() && !a.isComplex() && !b.isComplex() && b.type()->code() == a.type()->code()) {
    const T * ap = static_cast<IntegerType<T,codeNum>*>(a.type())->ro(a);
    const T * bp = static_cast<IntegerType<T,codeNum>*>(b.type())->ro(b);
    bool p;
    Op::template func<bool,T,T,T>(&p,ap,bp);
    return o->makeScalar(p);
  }
  switch (b.type()->code())
    {
    case TypeDouble:
      return dispatch_cmpop<bool,T,double,double,Op>(a,b,o);
    case codeNum:
      return dispatch_cmpop<bool,T,T,T,Op>(a,b,o);
    default:
      throw Exception("Unsupported type combination of " + a.type()->name() + " and " + b.type()->name());
    }
}

template<class T, FM::DataCode codeNum>
void IntegerType<T,codeNum>::computeArrayFormatInfo(FMFormatMode, const Object &a, ArrayFormatInfo &format) {
  const T* dp = this->ro(a);
  ndx_t cnt = a.count();
  if (a.isComplex()) cnt *= 2;
  format.width = GetNominalWidthInteger(dp,cnt);
}

template <class T, FM::DataCode codeNum>
void IntegerType<T,codeNum>::printElement(const Object &a, const ArrayFormatInfo &format, ndx_t ndx) {
  if (!a.isComplex())
    {
      const T* dp = this->ro(a);
      Type::_ctxt->_io->output("% *lld",format.width,(int64_t)(dp[ndx]));
    }
  else
    {
      const Complex<T>* dp = this->roComplex(a);
      Type::_ctxt->_io->output("% *lld",format.width,(int64_t)(dp[ndx].r));
      Type::_ctxt->_io->output("% *+lldi",format.width,(int64_t)(dp[ndx].i));
    }
}

template <class T, FM::DataCode codeNum>
Object IntegerType<T,codeNum>::Equals(const Object &a, const Object &b) {
  return int_cmpop<T,codeNum,OpEQ>(a,b,Type::_ctxt->_bool);
}

template <class T, FM::DataCode codeNum>
Object IntegerType<T,codeNum>::NotEquals(const Object &a, const Object &b) {
  return int_cmpop<T,codeNum,OpNE>(a,b,Type::_ctxt->_bool);
}

template <class T, FM::DataCode codeNum>
Object IntegerType<T,codeNum>::LessEquals(const Object &a, const Object &b) {
  return int_cmpop<T,codeNum,OpLE>(a,b,Type::_ctxt->_bool);
}

template <class T, FM::DataCode codeNum>
Object IntegerType<T,codeNum>::LessThan(const Object &a, const Object &b) {
  return int_cmpop<T,codeNum,OpLT>(a,b,Type::_ctxt->_bool);
}

template <class T, FM::DataCode codeNum>
Object IntegerType<T,codeNum>::GreaterEquals(const Object &a, const Object &b) {
  return int_cmpop<T,codeNum,OpGE>(a,b,Type::_ctxt->_bool);
}

template <class T, FM::DataCode codeNum>
Object IntegerType<T,codeNum>::GreaterThan(const Object &a, const Object &b) {
  return int_cmpop<T,codeNum,OpGT>(a,b,Type::_ctxt->_bool);
}


template class FM::IntegerType<FMChar,TypeString>;
template class FM::IntegerType<ndx_t,TypeIndex>;
template class FM::IntegerType<uint8_t,TypeUInt8>;
template class FM::IntegerType<int8_t,TypeInt8>;
template class FM::IntegerType<uint16_t,TypeUInt16>;
template class FM::IntegerType<int16_t,TypeInt16>;
template class FM::IntegerType<uint32_t,TypeUInt32>;
template class FM::IntegerType<int32_t,TypeInt32>;
template class FM::IntegerType<uint64_t,TypeUInt64>;
template class FM::IntegerType<int64_t,TypeInt64>;

