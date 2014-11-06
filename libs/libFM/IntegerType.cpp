#include "IntegerType.hpp"
#include "ThreadContext.hpp"
#include "Type.hpp"

using namespace FM;

const int MSGBUFLEN = 100;

// FIXME - replace with log10?
template <class T>
static int GetNominalWidthInteger(const T* qp, dim_t len) {
  char buffer[MSGBUFLEN];
  int maxdigit = 0;
  for (dim_t i=0;i<len;i++) {
    memset(buffer,0,MSGBUFLEN);
    sprintf(buffer,"%lld",(int64_t)qp[i]);
    int j = maxdigit;
    while (buffer[j] && j>=maxdigit)
      j++;
    maxdigit = j;
  }
  return maxdigit;
}

template<class T, FM::DataCode codeNum>
void IntegerType<T,codeNum>::computeArrayFormatInfo(FMFormatMode, const Object &a, ArrayFormatInfo &format) {
  const T* dp = this->ro(a);
  dim_t cnt = a.elementCount();
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

template class FM::IntegerType<bool,TypeBool>;
template class FM::IntegerType<FMChar,TypeString>;
template class FM::IntegerType<ndx_t,TypeIndex>;
template class FM::IntegerType<uint32_t,TypeUInt32>;
template class FM::IntegerType<int32_t,TypeInt32>;
template class FM::IntegerType<uint64_t,TypeUInt64>;
template class FM::IntegerType<int64_t,TypeInt64>;

