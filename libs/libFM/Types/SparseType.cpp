#include "SparseType.hpp"
#include "DoubleType.hpp"

using namespace FM;

static const int SPARSE_OP_NORMAL = 0;
static const int SPARSE_OP_IS_BOOL_VALUED = 1;
static const int SPARSE_OP_PRESERVES_SPARSITY = 2;

template <class T>
FMString FM::SparseType<T>::describe(const Object &a) {
  const SparseData<T> *csd = this->ro(a);
  return "Sparse matrix of size "+Stringify(a.rows()) + " x " + Stringify(a.cols()) + " class " + this->_classname + " with "
    + Stringify(csd->m_nnz) + " nonzero entries";
}

template <class T>
FMString FM::SparseType<T>::brief(const Object &a) {
  return "Sparse " + Stringify(a.rows()) + "x" + Stringify(a.cols());  
}

template <class T>
static void convertFullMatrixToSparse(Object &dest, const T* src, const Tuple &dims, SparseType<T> *t) {
  dest.dims() = dims;
  ndx_t M = dims.rows();
  SparseData<T> *csd = t->rw(dest);
  for (ndx_t col = 0;col<dims.cols();col++)
    for (ndx_t row = 0;row<M;row++) 
      if (src[col*M+row] != T()) {
	csd->m_data[col][row] = src[col*M+row];
	csd->m_nnz++;
      }
}

template <class T, class S>
static Object convertSparseMatrixToSparseMatrix(const Object &a, SparseType<T> *t, SparseType<S> *s) {
  Object ret(s->empty());
  ret.dims() = a.dims();
  const SparseData<T> *asd = t->ro(a);
  SparseData<S> *rsd = s->rw(ret);
  for (auto col : asd->m_data)
    for (auto row : col.second)
      OpRangeConvert<S,T>::rangeConvert(rsd->m_data[col.first][row.first],row.second);
  return ret;
}

template <>
Object FM::SparseType<double>::convert(const Object &a) {
  if (a.isSparse()) {
    switch (a.typeCode())
      {
      case TypeSparseLogical:
	return convertSparseMatrixToSparseMatrix(a,_ctxt->_splogical,this);
      case TypeSparseDouble:
	return a;
      case TypeSparseComplex:
	this->context()->_io->warning("Complex part of matrix will be ignored");
	return convertSparseMatrixToSparseMatrix(a,_ctxt->_spcomplex,this);
      default:
	throw Exception("Unhandled sparse matrix conversion case");
      }
  }
  Object aconv = _ctxt->_double->convert(a);
  Object ret(this->empty());
  convertFullMatrixToSparse(ret,_ctxt->_double->ro(aconv),a.dims(),this);
  return ret;
}

template <>
Object FM::SparseType<bool>::convert(const Object &a) {
  if (a.isSparse()) {
    switch (a.typeCode())
      {
      case TypeSparseLogical:
	return a;
      case TypeSparseDouble:
	return convertSparseMatrixToSparseMatrix(a,_ctxt->_spdouble,this);
      case TypeSparseComplex:
	return convertSparseMatrixToSparseMatrix(a,_ctxt->_spcomplex,this);
      default:
	throw Exception("Unhandled sparse matrix conversion case");
      }
  }
  Object aconv = _ctxt->_bool->convert(a);
  Object ret(this->empty());
  convertFullMatrixToSparse(ret,_ctxt->_bool->ro(aconv),a.dims(),this);
  return ret;
}

template <>
Object FM::SparseType<Complex<double> >::convert(const Object &a) {
  if (a.isSparse()) {
    switch (a.typeCode())
      {
      case TypeSparseLogical:
	return convertSparseMatrixToSparseMatrix(a,_ctxt->_splogical,this);
      case TypeSparseDouble:
	return convertSparseMatrixToSparseMatrix(a,_ctxt->_spdouble,this);
      case TypeSparseComplex:
	return a;
      default:
	throw Exception("Unhandled sparse matrix conversion case");
      }
  }
  Object aconv = _ctxt->_double->convert(a);
  Object ret(this->empty());
  convertFullMatrixToSparse(ret,_ctxt->_double->roComplex(aconv),a.dims(),this);
  return ret;
}

template <class T>
static void convertSparseMatrixToFull(T* destp, const Object &src, SparseType<T> *t) {
  const SparseData<T> *ad = t->ro(src);
  for (auto column : ad->m_data)
    for (auto row : column.second)
      destp[row.first + column.first*src.rows()] = row.second;
}

template <>
Object FM::SparseType<double>::asFullMatrix(const Object &a) {
  Object af = _ctxt->_double->makeMatrix(a.rows(),a.cols());
  convertSparseMatrixToFull(_ctxt->_double->rw(af),a,this);
  return af;
}

template <>
Object FM::SparseType<bool>::asFullMatrix(const Object &a) {
  Object af = _ctxt->_bool->makeMatrix(a.rows(),a.cols());
  convertSparseMatrixToFull(_ctxt->_bool->rw(af),a,this);
  return af;
}

template <>
Object FM::SparseType<Complex<double> >::asFullMatrix(const Object &a) {
  Object af = _ctxt->_double->makeMatrix(a.rows(),a.cols(),true);
  convertSparseMatrixToFull(_ctxt->_double->rwComplex(af),a,this);
  return af;
}

template <>
Type* FM::SparseType<bool>::getDenseType() {
  return reinterpret_cast<Type*>(Type::_ctxt->_bool);
}

template <>
Object FM::SparseType<bool>::makeDenseBuffer(ndx_t len) {
  return context()->_bool->makeMatrix(len,1,false);
}

template <>
Object FM::SparseType<double>::makeDenseBuffer(ndx_t len) {
  return context()->_double->makeMatrix(len,1,false);
}

template <>
Object FM::SparseType<Complex<double> >::makeDenseBuffer(ndx_t len) {
  return context()->_double->makeMatrix(len,1,true);
}

template <>
bool* FM::SparseType<bool>::denseBufferPtr(Object &a) {
  return context()->_bool->rw(a);
}

template <>
double* FM::SparseType<double>::denseBufferPtr(Object &a) {
  return context()->_double->rw(a);
}

template <>
Complex<double>* FM::SparseType<Complex<double> >::denseBufferPtr(Object &a) {
  return context()->_double->rwComplex(a);
}

template <>
const bool* FM::SparseType<bool>::denseBufferPtr(const Object &a) {
  return context()->_bool->ro(a);
}

template <>
const double* FM::SparseType<double>::denseBufferPtr(const Object &a) {
  return context()->_double->ro(a);
}

template <>
const Complex<double>* FM::SparseType<Complex<double> >::denseBufferPtr(const Object &a) {
  return context()->_double->roComplex(a);
}

typedef Object (Type::*TypeMemFn)(const Object &x, const Object &y);

class ColumnIterator
{
public:
  virtual Object extractColumn(ndx_t column) = 0;
};

class NullIterator : public ColumnIterator
{
  Object _obj;
public:
  NullIterator(Object obj) : _obj(obj) {}
  Object extractColumn(ndx_t) {
    return _obj;
  }
};

class DenseColumnIterator : public ColumnIterator
{
  const Object &_obj;
public:
  DenseColumnIterator(const Object &obj) : _obj(obj) {}
  Object extractColumn(ndx_t column) {
    return _obj.type()->sliceColumn(_obj,column);
  }
};

template <class T>
class SparseColumnIterator : public ColumnIterator
{
  const Object &_obj;
  Object _buffer;
  SparseType<T> *_type;
public:
  SparseColumnIterator(SparseType<T> *type, const Object &obj) :
    _obj(obj), _buffer(type->context()), _type(type) {
    _buffer = type->makeDenseBuffer(_obj.rows());
  }
  Object extractColumn(ndx_t column) {
    _type->extractDenseColumn(_obj,column,_buffer);
    return _buffer;
  }
};

template <class T>
static ColumnIterator* makeSparseColumnIterator(SparseType<T> *type, const Object &obj) {
  if (obj.isScalar())
    return (ColumnIterator*)(new NullIterator(type->scalarObject(obj)));
  else
    return (ColumnIterator*)(new SparseColumnIterator<T>(type,obj));
}

static ColumnIterator* makeColumnIterator(ThreadContext *ctxt, const Object &obj) {
  if (obj.isSparse()) {
    switch (obj.typeCode()) {
    case TypeSparseLogical:
      return makeSparseColumnIterator(ctxt->_splogical,obj);
    case TypeSparseDouble:
      return makeSparseColumnIterator(ctxt->_spdouble,obj);
    case TypeSparseComplex:
      return makeSparseColumnIterator(ctxt->_spcomplex,obj);
    default:
      throw Exception("Unhandled sparse type");
    }
  } else {
    if (obj.isScalar())
      return (ColumnIterator*)(new NullIterator(obj));
    else
      return (ColumnIterator*)(new DenseColumnIterator(obj));
  }
}

template <class T>
static Object SparseSparseDotOperation(const Object &a, const Object &b, ThreadContext *ctxt,
				       SparseType<T> *return_type, TypeMemFn func) {
  Object ret(return_type->empty());
  ret.dims() = Tuple::computeDotOpSize(a.dims(),b.dims());
  auto aiter(makeColumnIterator(ctxt,a));
  auto biter(makeColumnIterator(ctxt,b));
  for (ndx_t col=0;col<ret.cols();col++) {
    Object acol = aiter->extractColumn(col);
    Object bcol = biter->extractColumn(col);
    Object ccol = (*return_type.*func)(acol,bcol);
    return_type->sparseifyColumn(ret,col,ccol);
  }
  return ret;
}
/*

template <class T, class S>
static Object ScalarSparseDotOperation(const Object &scalara, const Object &b, SparseType<T> *t, SparseType<S> *r, TypeMemFn func) {
  ThreadContext *ctxt = t->context();
  Object ret(r->empty());
  ret.dims() = Tuple::computeDotOpSize(scalara.dims(),b.dims());
  Object column_buffer_b(t->makeDenseBuffer(ret.rows()));
  Type *s = t->getDenseType();
  for (ndx_t col=0;col<ret.cols();col++) {
    t->extractDenseColumn(b,col,column_buffer_b);
    Object column_buffer_c((*s.*func)(scalara,column_buffer_b));
    r->sparseifyColumn(ret,col,column_buffer_c);
  }
  return ret;
}

template <class T, class S>
static Object SparseScalarDotOperation(const Object &a, const Object &scalarb, SparseType<T> *t, SparseType<S> *r, TypeMemFn func) {
  ThreadContext *ctxt = t->context();
  Object ret(r->empty());
  ret.dims() = Tuple::computeDotOpSize(a.dims(),scalarb.dims());
  Object column_buffer_a(t->makeDenseBuffer(ret.rows()));
  Type *s = t->getDenseType();
  for (ndx_t col=0;col<ret.cols();col++) {
    t->extractDenseColumn(a,col,column_buffer_a);
    Object column_buffer_c((*s.*func)(column_buffer_a,scalarb));
    r->sparseifyColumn(ret,col,column_buffer_c);
  }
  return ret;
}

template <class T, class S>
static Object SparseDotOperation(const Object &a, const Object &b, SparseType<T> *t, SparseType<S> *r, TypeMemFn func) {
  ThreadContext *ctxt = t->context();
  Object ret(r->empty());
  ret.dims() = Tuple::computeDotOpSize(a.dims(),b.dims());
  Object column_buffer_a(t->makeDenseBuffer(ret.rows()));
  Object column_buffer_b(t->makeDenseBuffer(ret.rows()));
  Type *s = t->getDenseType();
  for (ndx_t col=0;col<ret.cols();col++) {
    t->extractDenseColumn(a,col,column_buffer_a);
    t->extractDenseColumn(b,col,column_buffer_b);
    Object column_buffer_c((*s.*func)(column_buffer_a,column_buffer_b));
    r->sparseifyColumn(ret,col,column_buffer_c);
  }
  return ret;
}

template <class T>
static Object SparseFullDotOperation(const Object &a, const Object &b, SparseType<T> *t, TypeMemFn func) {
  ThreadContext *ctxt = t->context();
  Object ret(t->empty());
  ret.dims() = Tuple::computeDotOpSize(a.dims(),b.dims());
  Object column_buffer_a(t->makeDenseBuffer(ret.rows()));
  Type *s = t->getDenseType();
  for (ndx_t col=0;col<ret.cols();col++) {
    t->extractDenseColumn(a,col,column_buffer_a);
    Object column_buffer_b(s->sliceColumn(b,col));
    Object column_buffer_c((*s.*func)(column_buffer_a,column_buffer_b));
    t->sparseifyColumn(ret,col,column_buffer_c);
  }
  return ret;
}

template <class T>
static Object SparseSparseDotOperation(const Object &a, const Object &b, SparseType<T> *t, TypeMemFn func, int sparseFlags) {
  if (!(sparseFlags & SPARSE_OP_IS_BOOL_VALUED)) {
    if (a.isScalar())
      return ScalarSparseDotOperation(t->scalarObject(a),b,t,t,func);
    else if (b.isScalar())
      return SparseScalarDotOperation(a,t->scalarObject(b),t,t,func);
    return SparseDotOperation(a,b,t,t,func);
  } else {
    if (a.isScalar())
      return ScalarSparseDotOperation(a,b,t,t->context()->_splogical,func);
    else if (b.isScalar())
      return SparseScalarDotOperation(a,b,t,t->context()->_splogical,func);
    return SparseDotOperation(a,b,t,t->context()->_splogical,func);
  }
}
d
static Object SparseSparseDotOperation(const Object &a, const Object &b, ThreadContext *ctxt, TypeMemFn func, int sparseFlags) {
  DataCode acode = a.typeCode();
  DataCode bcode = b.typeCode();
  if (acode == TypeSparseLogical)
    return SparseSparseDotOperation(ctxt->_spdouble->convert(a),b,ctxt,func,sparseFlags);
  if (bcode == TypeSparseLogical)
    return SparseSparseDotOperation(a,ctxt->_spdouble->convert(b),ctxt,func,sparseFlags);
  if (!(acode == TypeSparseComplex) && (bcode == TypeSparseComplex))
    return SparseSparseDotOperation(ctxt->_spcomplex->convert(a),b,ctxt,func,sparseFlags);
  if ((acode == TypeSparseComplex) && !(bcode == TypeSparseComplex))
    return SparseSparseDotOperation(a,ctxt->_spcomplex->convert(b),ctxt,func,sparseFlags);
  if (acode == TypeSparseComplex)
    return SparseSparseDotOperation<Complex<double> >(a,b,ctxt->_spcomplex,func,sparseFlags);
  return SparseSparseDotOperation<double>(a,b,ctxt->_spdouble,func,sparseFlags);
}

static Object SparseFullDotOperation(const Object &a, const Object &b, ThreadContext *ctxt, TypeMemFn func, int sparseFlags) {
  DataCode acode = a.typeCode();
  DataCode bcode = b.typeCode();
  if ((bcode != TypeDouble) && (bcode != TypeBool)) throw Exception("Sparse-full operations can only operate with double or logical full types.");
  switch (acode) {
  case TypeSparseDouble:
    if (b.isComplex())
      return SparseFullDotOperation(ctxt->_spcomplex->convert(a),ctxt->_double->convert(b),ctxt,func,sparseFlags);
    if (b.isScalar())
      return SparseScalarDotOperation(a,b,ctxt->_spdouble,ctxt->_spdouble,func);
    return SparseFullDotOperation(a,b,ctxt->_spdouble,func);
  case TypeSparseComplex:
    if (!b.isComplex())
      return SparseFullDotOperation(a,ctxt->_double->makeComplex(b.asDouble(),0),ctxt,func,sparseFlags);
    if (b.isScalar())
      return SparseScalarDotOperation(a,b,ctxt->_spcomplex,ctxt->_spcomplex,func);
    return SparseFullDotOperation(a,b,ctxt->_spcomplex,func);
  default:
    throw Exception("Unhanded case in sparse-full operation");
  }
}

template <class T>
static Object SparseSparseDotOperationMain(const Object &a, const Object &b, SparseType<T> *t, TypeMemFn func, int sparseFlags = SPARSE_OP_NORMAL) {
  if (a.isSparse() && b.isSparse())
    return SparseSparseDotOperation(a,b,t->context(),func,sparseFlags);
  if (a.isSparse() && !b.isSparse()) {
    if (!(sparseFlags & SPARSE_OP_PRESERVES_SPARSITY)) {
      Object afull = t->asFullMatrix(a);
      return (*(afull.type()).*func)(afull,b);
    } else {
      return SparseFullDotOperation(a,b,t->context(),func,sparseFlags);
    }
  }
  if (!a.isSparse() && b.isSparse()) {
    if (!(sparseFlags & SPARSE_OP_PRESERVES_SPARSITY)) {
      Object bfull = t->asFullMatrix(b);
      return (*(bfull.type()).*func)(a,bfull);
    } else {
      throw Exception("Unhanded case for SparseMatrix operation!");
      //      return FullSparseDotOperation(a,b,t->context(),func,sparseFlags);
    }
  }
  throw Exception("Unhandled case for SparseMatrix operation!");
}

*/

template <class T>
static Object convertSparseType(const Object &a, SparseType<T> *typ) {
  if (a.isSparse()) return typ->convert(a);
  return typ->getDenseType()->convert(a);
}

template <class via, class result>
static Object DispatchSparseSparse(const Object &a, const Object &b, SparseType<via> *typ,
				   SparseType<result> *res, ThreadContext *ctxt,
				   TypeMemFn func, int sparseFlags) {
  Object ca(convertSparseType(a,typ));
  Object cb(convertSparseType(b,typ));
  return SparseSparseDotOperation(ca,cb,ctxt,res,func);
}

// TODO - handle Complex+Real, mixed sparse, dense cases
// TODO handle SPARSE_OP_IS_SPARSITY_PRESERVING flag

static Object DispatchSparseSparseOperationNumerical(const Object &a, const Object &b, ThreadContext *ctxt,
						     TypeMemFn func, int sparseFlags)
{
  if (a.isComplex() || b.isComplex()) 
    return DispatchSparseSparse(a,b,ctxt->_spcomplex,ctxt->_spcomplex,ctxt,func,sparseFlags);
  return DispatchSparseSparse(a,b,ctxt->_spdouble,ctxt->_spdouble,ctxt,func,sparseFlags);
}

static Object DispatchSparseSparseOperationComparison(const Object &a, const Object &b, ThreadContext *ctxt,
						      TypeMemFn func, int sparseFlags)
{
  if (a.isComplex() || b.isComplex()) 
    return DispatchSparseSparse(a,b,ctxt->_spcomplex,ctxt->_splogical,ctxt,func,sparseFlags);
  return DispatchSparseSparse(a,b,ctxt->_spdouble,ctxt->_splogical,ctxt,func,sparseFlags);
}

//static Object DispatchSparseSparse

template <class T>
Object FM::SparseType<T>::LessEquals(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationComparison(a,b,Type::context(),&Type::LessEquals,SPARSE_OP_IS_BOOL_VALUED);
}

template <class T>
Object FM::SparseType<T>::LessThan(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationComparison(a,b,Type::context(),&Type::LessThan,SPARSE_OP_IS_BOOL_VALUED);
}

template <class T>
Object FM::SparseType<T>::GreaterThan(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationComparison(a,b,Type::context(),&Type::GreaterThan,SPARSE_OP_IS_BOOL_VALUED);
}

template <class T>
Object FM::SparseType<T>::GreaterEquals(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationComparison(a,b,Type::context(),&Type::GreaterEquals,SPARSE_OP_IS_BOOL_VALUED);
}

template <class T>
Object FM::SparseType<T>::NotEquals(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationComparison(a,b,Type::context(),&Type::NotEquals,SPARSE_OP_IS_BOOL_VALUED);
}

template <class T>
Object FM::SparseType<T>::Equals(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationComparison(a,b,Type::context(),&Type::Equals,SPARSE_OP_IS_BOOL_VALUED);
}

template <class T>
Object FM::SparseType<T>::Add(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationNumerical(a,b,Type::context(),&Type::Add,0);
}

template <class T>
Object FM::SparseType<T>::Subtract(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationNumerical(a,b,Type::context(),&Type::Subtract,0);
}

template <class T>
Object FM::SparseType<T>::DotLeftDivide(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationNumerical(a,b,Type::context(),&Type::DotLeftDivide,0);
}

template <class T>
Object FM::SparseType<T>::DotRightDivide(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationNumerical(a,b,Type::context(),&Type::DotRightDivide,0);
}

template <class T>
Object FM::SparseType<T>::DotMultiply(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationNumerical(a,b,Type::context(),&Type::DotMultiply,SPARSE_OP_PRESERVES_SPARSITY);
}

template <class T>
Object FM::SparseType<T>::DotPower(const Object &a, const Object &b) {
  return DispatchSparseSparseOperationNumerical(a,b,Type::context(),&Type::DotPower,0);
}

template <class T>
Object FM::SparseType<T>::Or(const Object &a, const Object &b) {
  return DispatchSparseSparse(a,b,Type::context()->_splogical,Type::context()->_splogical,Type::context(),&Type::Or,0);
}

template <class T>
Object FM::SparseType<T>::And(const Object &a, const Object &b) {
  return DispatchSparseSparse(a,b,Type::context()->_splogical,Type::context()->_splogical,Type::context(),&Type::And,0); // Sparse preserving
}

template class FM::SparseType<double>;
template class FM::SparseType<bool>;
template class FM::SparseType<Complex<double> >;
