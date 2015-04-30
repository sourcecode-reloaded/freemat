#include "SparseType.hpp"
#include "DoubleType.hpp"

using namespace FM;

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

template <>
Object FM::SparseType<double>::convert(const Object &a) {
  // TODO: Handle sparse-sparse conversion here
  Object aconv = _ctxt->_double->convert(a);
  Object ret(this->empty());
  convertFullMatrixToSparse(ret,_ctxt->_double->ro(aconv),a.dims(),this);
  return ret;
}

template <>
Object FM::SparseType<bool>::convert(const Object &a) {
  // TODO: Handle sparse-sparse conversion here
  Object aconv = _ctxt->_bool->convert(a);
  Object ret(this->empty());
  convertFullMatrixToSparse(ret,_ctxt->_bool->ro(aconv),a.dims(),this);
  return ret;
}

template <>
Object FM::SparseType<Complex<double> >::convert(const Object &a) {
  // TODO: Handle sparse-sparse conversion here
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

static bool* createBuffer(Object &a, SparseType<bool> *t, ndx_t len) {
  ThreadContext *ctxt = t->context();
  a = ctxt->_bool->makeMatrix(len,1,false);
  return ctxt->_bool->rw(a);
}

static double* createBuffer(Object &a, SparseType<double> *t, ndx_t len) {
  ThreadContext *ctxt = t->context();
  a = ctxt->_double->makeMatrix(len,1,false);
  return ctxt->_double->rw(a);
}

static Complex<double>* createBuffer(Object &a, SparseType<Complex<double> > *t, ndx_t len) {
  ThreadContext *ctxt = t->context();
  a = ctxt->_double->makeMatrix(len,1,true);
  return ctxt->_double->rwComplex(a);
}

template <class T>
Object SparseScalarAdd(const Object &a, const T& bval, SparseType<T> *t) {
  ThreadContext *ctxt = t->context();
  Object ret(t->empty());
  SparseData<T> *csd = t->rw(ret);
  const SparseData<T> *asd = t->ro(a);
  Object acol(ctxt);
  T* bufptr = createBuffer(acol,t,a.rows());
  for (auto col=0;col<a.cols();col++) {
    memset(bufptr,0,sizeof(T)*a.rows());
    if (asd->m_data.count(col))
      for (auto row : asd->m_data.at(col))
	bufptr[row.first] = row.second;
    for (auto i=0;i<a.rows();i++)
      bufptr[i] = bufptr[i] + bval;
    for (ndx_t i=0;i<a.rows();i++) {
      if (bufptr[i] != T())
	csd->m_data[col][i] = bufptr[i];
    }
  }
  ret.dims() = a.dims();
  return ret;
}

template <class T>
Object SparseSparseAdd(const Object &a, const Object &b, SparseType<T> *t) {
  if (a.isScalar() && b.isScalar())
    return t->makeScalar(t->scalarValue(a)+t->scalarValue(b));
  if (b.isScalar())
    return SparseScalarAdd(a,t->scalarValue(b),t);
  Object ret(t->empty());
  ret.dims() = Tuple::computeDotOpSize(a.dims(),b.dims());
  ConstSparseIterator<T> aiter(a,t);
  ConstSparseIterator<T> biter(b,t);
  SparseData<T> *csd = t->rw(ret);
  while (aiter.valid() || biter.valid()) {
    if (aiter == biter) {
      T temp = *aiter + *biter;
      if (temp != T()) {
	csd->m_data[aiter.col()][aiter.row()] = temp;
	csd->m_nnz++;
      }
      ++aiter;
      ++biter;
    } else if (aiter < biter) {
      csd->m_data[aiter.col()][aiter.row()] = *aiter;
      csd->m_nnz++;
      ++aiter;
    } else {
      csd->m_data[biter.col()][biter.row()] = *biter;
      csd->m_nnz++;
      ++biter;
    }
  }
  return ret;
}

Object SparseSparseAdd(const Object &a, const Object &b, ThreadContext *ctxt) {
  DataCode acode = a.typeCode();
  DataCode bcode = b.typeCode();
  if (acode == TypeSparseLogical) return SparseSparseAdd(ctxt->_spdouble->convert(a),b,ctxt);
  if (bcode == TypeSparseLogical) return SparseSparseAdd(a,ctxt->_spdouble->convert(b),ctxt);
  if (!(acode == TypeSparseComplex) && (bcode == TypeSparseComplex)) return SparseSparseAdd(ctxt->_spcomplex->convert(a),b,ctxt);
  if ((acode == TypeSparseComplex) && !(bcode == TypeSparseComplex)) return SparseSparseAdd(a,ctxt->_spcomplex->convert(b),ctxt);
  if (acode == TypeSparseComplex)  return SparseSparseAdd<Complex<double> >(a,b,ctxt->_spcomplex);
  return SparseSparseAdd<double>(a,b,ctxt->_spdouble);
}

template <class T>
Object FM::SparseType<T>::Add(const Object &a, const Object &b) {
  if (a.isSparse() && b.isSparse())
    return SparseSparseAdd(a,b,Type::_ctxt);
  if (a.isSparse() && !b.isSparse()) {
    Object afull = this->asFullMatrix(a);
    return afull.type()->Add(afull,b);
  }
  throw Exception("Unhandled case for SparseMatrix add!");
}


template class FM::SparseType<double>;
template class FM::SparseType<bool>;
template class FM::SparseType<Complex<double> >;
