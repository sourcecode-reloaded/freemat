#include "IntegerType.hpp"
#include "ThreadContext.hpp"
#include "Type.hpp"
#include "BoolType.hpp"

using namespace FM;

const int MSGBUFLEN = 100;

template <class S>
static void printToStream(TermIF *io, int width, const S& val) {
  io->output("% *lld",width,(int64_t)(val));
}

// FIXME - 
template <class S>
static void printToStream(TermIF *io, int width, const Complex<S> & val) {
  io->output("% *lld +%lld",width,(int64_t)(val.r),(int64_t)(val.i));
}


template <class T, class Op>
static inline Object int_cmpop(const Object &a, const Object &b, BoolType *o)
{
  if (a.isScalar() && b.isScalar() && b.type()->code() == a.type()->code()) {
    const T * ap = static_cast<IntegerType<T>*>(a.type())->ro(a);
    const T * bp = static_cast<IntegerType<T>*>(b.type())->ro(b);
    bool p;
    Op::template func<bool,T,T,T>(&p,*ap,*bp);
    return o->makeScalar(p);
  }
  // TODO - Handle Complex+int combinations
  if (b.type()->code() == TypeDouble)
    return dispatch_cmpop<T,double,double,Op>(a,b,o);
  else if (b.type()->code() == a.type()->code())
    return dispatch_cmpop<T,T,double,Op>(a,b,o);
  throw Exception("Unsupported type combination of " + a.type()->name() + " and " + b.type()->name());
}

// template<class T>
// void IntegerType<T>::computeArrayFormatInfo(FMFormatMode mode, const Object &a, ArrayFormatInfo &format) {
//   GetArrayFormatForPODArray(mode,this->ro(a),a.count(),format);
// }

// template <class T>
// void IntegerType<T>::printElement(const Object &a, const ArrayFormatInfo &format, ndx_t ndx) {
//   PrintFormattedNumberToStream(Type::_ctxt->_io,format,this->ro(a)[offset]);
// }

template <class T>
Object IntegerType<T>::Equals(const Object &a, const Object &b) {
  return int_cmpop<T,OpEQ>(a,b,Type::_ctxt->_bool);
}

template <class T>
Object IntegerType<T>::NotEquals(const Object &a, const Object &b) {
  return int_cmpop<T,OpNE>(a,b,Type::_ctxt->_bool);
}

template <class T>
Object IntegerType<T>::LessEquals(const Object &a, const Object &b) {
  return int_cmpop<T,OpLE>(a,b,Type::_ctxt->_bool);
}

template <class T>
Object IntegerType<T>::LessThan(const Object &a, const Object &b) {
  return int_cmpop<T,OpLT>(a,b,Type::_ctxt->_bool);
}

template <class T>
Object IntegerType<T>::GreaterEquals(const Object &a, const Object &b) {
  return int_cmpop<T,OpGE>(a,b,Type::_ctxt->_bool);
}

template <class T>
Object IntegerType<T>::GreaterThan(const Object &a, const Object &b) {
  return int_cmpop<T,OpGT>(a,b,Type::_ctxt->_bool);
}


template class FM::IntegerType<FMChar>;
template class FM::IntegerType<ndx_t>;
template class FM::IntegerType<uint8_t>;
template class FM::IntegerType<int8_t>;
template class FM::IntegerType<uint16_t>;
template class FM::IntegerType<int16_t>;
template class FM::IntegerType<uint32_t>;
template class FM::IntegerType<int32_t>;
template class FM::IntegerType<uint64_t>;
//template class FM::IntegerType<int64_t>;
template class FM::IntegerType<Complex<uint8_t> >;
template class FM::IntegerType<Complex<int8_t> >;
template class FM::IntegerType<Complex<uint16_t> >;
template class FM::IntegerType<Complex<int16_t> >;
template class FM::IntegerType<Complex<uint32_t> >;
template class FM::IntegerType<Complex<int32_t> >;
template class FM::IntegerType<Complex<uint64_t> >;
template class FM::IntegerType<Complex<int64_t> >;
