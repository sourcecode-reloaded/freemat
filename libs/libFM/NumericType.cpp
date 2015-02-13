#include "NumericType.hpp"
#include "IntegerType.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"
#include "ThreadContext.hpp"
#include "Convert.hpp"

using namespace FM;

template <class T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::asIndexNoBoundsCheck(const Object &a)
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
  dim_t len = a.dims().count();
  Object output = Type::_ctxt->_index->makeMatrix(len,1);
  ndx_t *op = Type::_ctxt->_index->rw(output);
  const T *ip = this->ro(a);
  dim_t incr = (a.isComplex() ? 2 : 1);
  for (dim_t i=0;i<len;i++)
    {
      op[i] = static_cast<ndx_t>(*ip)-1;
      if (op[i] < 0) throw Exception("Index values must be >= 1");
      ip += incr;
    }
  return output;  
}

template <class T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::asLogical(const Object &a)
{
  if (codeNum == TypeBool) return a;
  Object output = Type::_ctxt->_bool->zeroArrayOfSize(a.dims(),false);
  bool *op = Type::_ctxt->_bool->rw(output);
  dim_t len = a.dims().count();
  if (a.isComplex())
    {
      const FM::Complex<T> *ip = reinterpret_cast<const FM::Complex<T> *>(this->ro(a));
      for (dim_t i=0;i<len;i++)	
	op[i] = ((ip[i].r != 0) || (ip[i].i != 0));
    }
  else
    {
      const T* ip = static_cast<const T*>(this->ro(a));
      for (dim_t i=0;i<len;i++)
	op[i] = (ip[i] != 0);
    }
  return output;
}

template <class T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::asIndex(const Object &a, dim_t max)
{
  if (!a.isComplex() && a.isScalar())
    {
      T aval = this->scalarValue(a);
      if ((aval < 1) || (aval > max)) throw Exception("Index out of range");
      return Type::_ctxt->_index->makeScalar(static_cast<ndx_t>(aval)-1);
    }
  if (a.isComplex())
    {
      // TODO - add message catalog with numbers and translations
      std::cout << "WARNING: Complex part of index ignored\r\n";
    }
  dim_t len = a.dims().count();
  Object output = Type::_ctxt->_index->makeMatrix(len,1);
  ndx_t *op = Type::_ctxt->_index->rw(output);
  const T *ip = this->ro(a);
  dim_t incr = (a.isComplex() ? 2 : 1);
  for (dim_t i=0;i<len;i++)
    {
      op[i] = static_cast<ndx_t>(*ip)-1;
      if (op[i] < 0) throw Exception("Index values must be >= 1");
      if (op[i] >= max) throw Exception("Out of range");
      ip += incr;
    }
  return output;  
}


template <class T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::asComplex(const Object &a)
{
  if (a.isComplex()) return a;
  if (a.isScalar())
    return this->makeComplex(this->scalarValue(a),0);
  Object ret = this->zeroArrayOfSize(a.dims(),true);
  dim_t len = a.dims().count();
  Complex<T> *op = this->rwComplex(ret);
  const T*ip = this->ro(a);
  for (dim_t i=0;i<len;i++)
    op[i] = Complex<T>(ip[i]);
  return ret;
}

template <class T, FM::DataCode codeNum>
Object NumericType<T,codeNum>::convert(const Object &a)
{
  if (this->code() == a.type()->code()) return a;
  Object ret = this->zeroArrayOfSize(a.dims(),a.isComplex());
  dim_t len = a.dims().count();
  T* op = this->rw(ret);
  if (a.isComplex()) len *= 2;
  switch (a.type()->code())
    {
    case TypeSingle:
      convertLoop<T,float>(Type::_ctxt->_single->ro(a),op,len);
      break;
    case TypeDouble:
      convertLoop<T,double>(Type::_ctxt->_double->ro(a),op,len);
      break;
    case TypeInt32:
      convertLoop<T,int32_t>(Type::_ctxt->_int32->ro(a),op,len);
      break;
    case TypeUInt32:
      convertLoop<T,uint32_t>(Type::_ctxt->_uint32->ro(a),op,len);
      break;
    case TypeInt64:
      convertLoop<T,int64_t>(Type::_ctxt->_int64->ro(a),op,len);
      break;
    case TypeUInt64:
      convertLoop<T,uint64_t>(Type::_ctxt->_uint64->ro(a),op,len);
      break;
    default:
      throw Exception("Type conversion from " + a.type()->name() + " to " + this->name() + " is unsupported.");
      // FIXME - Add remaining integer types
    }
  return ret;
}



template class FM::NumericType<float,TypeSingle>;
template class FM::NumericType<double,TypeDouble>;
template class FM::NumericType<int32_t,TypeInt32>;
template class FM::NumericType<uint32_t,TypeUInt32>;
template class FM::NumericType<int64_t,TypeInt64>;
template class FM::NumericType<uint64_t,TypeUInt64>;
template class FM::NumericType<bool,TypeBool>;
template class FM::NumericType<ndx_t,TypeIndex>;
template class FM::NumericType<FMChar,TypeString>;
