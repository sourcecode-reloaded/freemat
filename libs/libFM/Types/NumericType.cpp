#include "NumericType.hpp"
#include "IntegerType.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"
#include "StringType.hpp"
#include "ThreadContext.hpp"
#include "Convert.hpp"

using namespace FM;

template <class T>
Object NumericType<T>::asIndexNoBoundsCheck(const Object &a)
{
  if (!a.isComplex() && a.isScalar())
    {
      T aval = this->scalarValue(a);
      if (aval < 1) throw Exception("Index out of range");
      return Type::_ctxt->_index->makeScalar(static_cast<ndx_t>(aval)-1);
    }
  if (a.isComplex())
    {
      // TODO - add message catalog with numbers and translations
      std::cout << "WARNING: Complex part of index ignored\r\n";
    }
  ndx_t len = a.dims().count();
  Object output = Type::_ctxt->_index->makeMatrix(len,1);
  ndx_t *op = Type::_ctxt->_index->rw(output);
  const T *ip = this->ro(a);
  ndx_t incr = (a.isComplex() ? 2 : 1);
  for (ndx_t i=0;i<len;i++)
    {
      op[i] = static_cast<ndx_t>(*ip)-1;
      if (op[i] < 0) throw Exception("Index values must be >= 1");
      ip += incr;
    }
  return output;  
}

template <class T>
Object NumericType<T>::asLogical(const Object &a)
{
  if (this->code() == TypeBool) return a;
  Object output = Type::_ctxt->_bool->zeroArrayOfSize(a.dims());
  bool *op = Type::_ctxt->_bool->rw(output);
  ndx_t len = a.dims().count();
  const T* ip = static_cast<const T*>(this->ro(a));
  for (ndx_t i=0;i<len;i++)
    op[i] = (ip[i] != T(0));
  return output;
}

template <class T>
Object NumericType<T>::asIndex(const Object &a, ndx_t max)
{
  if (a.isScalar())
    {
      ndx_t aval = static_cast<ndx_t>(this->scalarValue(a));
      if ((aval < 1) || (aval > max)) throw Exception("Index out of range");
      return Type::_ctxt->_index->makeScalar(aval-1);
    }
  ndx_t len = a.dims().count();
  Object output = Type::_ctxt->_index->makeMatrix(len,1);
  ndx_t *op = Type::_ctxt->_index->rw(output);
  const T *ip = this->ro(a);
  ndx_t incr = (a.isComplex() ? 2 : 1);
  for (ndx_t i=0;i<len;i++)
    {
      op[i] = static_cast<ndx_t>(*ip)-1;
      if (op[i] < 0) throw Exception("Index values must be >= 1");
      if (op[i] >= max) throw Exception("Out of range");
      ip += incr;
    }
  return output;  
}


/*
template <class T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::asComplex(const Object &a)
{
  if (a.isComplex()) return a;
  if (a.isScalar())
    return this->makeComplex(this->scalarValue(a),0);
  Object ret = this->zeroArrayOfSize(a.dims(),true);
  ndx_t len = a.dims().count();
  Complex<T> *op = this->rwComplex(ret);
  const T*ip = this->ro(a);
  for (ndx_t i=0;i<len;i++)
    op[i] = Complex<T>(ip[i]);
  return ret;
}
*/

template <class T>
Object NumericType<T>::convert(const Object &a)
{
  if (this->code() == a.type()->code()) return a;
  Object ret = this->zeroArrayOfSize(a.dims());
  ndx_t len = a.dims().count();
  T* op = this->rw(ret);
  switch (a.type()->code())
    {
    case TypeString:
      convertLoop<T,FMChar>(Type::_ctxt->_string->ro(a),op,len);
      break;
    case TypeSingle:
      convertLoop<T,float>(Type::_ctxt->_single->ro(a),op,len);
      break;
    case TypeDouble:
      convertLoop<T,double>(Type::_ctxt->_double->ro(a),op,len);
      break;
    case TypeInt8:
      convertLoop<T,int8_t>(Type::_ctxt->t_int8->ro(a),op,len);
      break;
    case TypeUInt8:
      convertLoop<T,uint8_t>(Type::_ctxt->_uint8->ro(a),op,len);
      break;
    case TypeInt16:
      convertLoop<T,int16_t>(Type::_ctxt->t_int16->ro(a),op,len);
      break;
    case TypeUInt16:
      convertLoop<T,uint16_t>(Type::_ctxt->_uint16->ro(a),op,len);
      break;
    case TypeInt32:
      convertLoop<T,int32_t>(Type::_ctxt->t_int32->ro(a),op,len);
      break;
    case TypeUInt32:
      convertLoop<T,uint32_t>(Type::_ctxt->_uint32->ro(a),op,len);
      break;
    case TypeInt64:
      convertLoop<T,int64_t>(Type::_ctxt->t_int64->ro(a),op,len);
      break;
    case TypeUInt64:
      convertLoop<T,uint64_t>(Type::_ctxt->_uint64->ro(a),op,len);
      break;
    default:
      throw Exception("Type conversion from " + a.type()->name() + " to " + this->name() + " is unsupported.");
    }
  return ret;
}

/*
template <typename T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::realPart(const Object &a) {
  if (!a.isComplex()) return a;
  Object ret = this->zeroArrayOfSize(a.dims());
  const Complex<T>* dp = this->roComplex(a);
  T* op = this->rw(ret);
  for (ndx_t i=0;i<a.count();i++)
    op[i] = dp[i].r;
  return ret;
}

template <typename T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::imagPart(const Object &a) {
  if (!a.isComplex()) return a;
  Object ret = this->zeroArrayOfSize(a.dims(),false);
  const Complex<T>* dp = this->roComplex(a);
  T* op = this->rw(ret);
  for (ndx_t i=0;i<a.count();i++)
    op[i] = dp[i].i;
  return ret;
}
*/

template <typename T>
bool NumericType<T>::isNonNegative(const Object &a) {
  if (a.isComplex()) return false;
  const T* dp = this->ro(a);
  for (ndx_t i=0;i<a.count();i++)
    if (dp[i] < 0) return false;
  return true;
}

template <typename T>
bool NumericType<T>::isIntegerValued(const Object &a) {
  if (a.isComplex()) return false;
  const T* dp = this->ro(a);
  for (ndx_t i=0;i<a.count();i++)
    if (dp[i] != int(dp[i])) return false;
  return true;
}

/*
template <typename T, FM::DataCode codeNum>
bool NumericType<T,codeNum>::anyNonzeroImaginary(const Object &a) {
  if (!a.isComplex()) return false;
  const Complex<T>* dp = this->roComplex(a);
  for (ndx_t i=0;i<a.count();i++)
    if (dp[i].i != T(0)) return true;
  return false;
}
*/

template <typename T>
Object NumericType<T>::getDiagonal(const Object &a) {
  if (!a.isSquare()) throw Exception("Cannot get diagonal of non-square matrix");
  ndx_t N = a.rows();
  Object ret = this->makeMatrix(N,1);
  const T *ap = this->ro(a);
  T *rp = this->rw(ret);
  for (auto i=0;i<N;i++)
    rp[i] = ap[i*N+i];
  return ret;
}

template <typename T>
Object NumericType<T>::asDiagonalMatrix(const Object &a) {
  ndx_t N = a.count();
  Object ret = this->makeMatrix(N,N);
  const T *ap = this->ro(a);
  T *rp = this->rw(ret);
  for (int i=0;i<N;i++)
    rp[i*N+i] = ap[i];
  return ret;
}

template <typename T>
void NumericType<T>::fill(Object &a, T val) {
  T* ap = this->rw(a);
  for (auto i=0;i<a.count();i++)
    ap[i] = val;
}

/*
template <typename T, FM::DataCode codeNum>
void NumericType<T,codeNum>::fillComplex(Object &a, Complex<T> val) {
  Complex<T> *ap = this->rwComplex(a);
  for (auto i=0;i<a.count();i++)
    ap[i] = val;
}
*/

template class FM::NumericType<float>;
template class FM::NumericType<double>;
template class FM::NumericType<int8_t>;
template class FM::NumericType<uint8_t>;
template class FM::NumericType<int16_t>;
template class FM::NumericType<uint16_t>;
template class FM::NumericType<int32_t>;
template class FM::NumericType<uint32_t>;
template class FM::NumericType<int64_t>;
template class FM::NumericType<uint64_t>;
template class FM::NumericType<Complex<float> >;
template class FM::NumericType<Complex<double> >;
template class FM::NumericType<Complex<int8_t> >;
template class FM::NumericType<Complex<uint8_t> >;
template class FM::NumericType<Complex<int16_t> >;
template class FM::NumericType<Complex<uint16_t> >;
template class FM::NumericType<Complex<int32_t> >;
template class FM::NumericType<Complex<uint32_t> >;
template class FM::NumericType<Complex<int64_t> >;
template class FM::NumericType<Complex<uint64_t> >;
//template class FM::NumericType<ndx_t>;
template class FM::NumericType<FMChar>;
