#include "SparseType.hpp"

using namespace FM;

template <class T>
FMString FM::SparseType<T>::describe(const Object &a) {
  const SparseData<T> *csd = this->ro(a);
  return "Sparse matrix of size "+Stringify(a.rows()) + " x " + Stringify(a.cols()) + " class " + this->_classname + " with "
    + Stringify(csd->m_nnz) + " nonzero entries";
}

template <class T>
FMString FM::SparseType<T>::brief(const Object &a) {
  const SparseData<T> *csd = this->ro(a);
  return "Sparse " + Stringify(a.rows()) + "x" + Stringify(a.cols());  
}

Object SparseSparseAdd(const Object &a, const Object &b) {
  if (a.isLogical()) return SparseSparseAdd(_ctxt->_spdouble->convert(a),b);
  if (b.isLogical()) return SparseSparseAdd(a,_ctxt->_spdouble->convert(b));
  if (!a.isComplex() && b.isComplex()) return SparseSparseAdd(_ctxt->_spcomplex->convert(a),b);
  if (a.isComplex() && !b.isComplex()) return SparseSparseAdd(a,_ctxt->_spcomplex->convert(b));
  if (a.isComplex())  return SparseSparseAdd<Complex<double> >(a,b,_ctxt->_spcomplex);
  return SparseSparseAdd<double>(a,b,_ctxt->_double);
}

template <class T>
Object FM::SparseStype<T>::Add(const Object &a, const Object &b) {
  if (a.isSparse() && b.isSparse())
    return SparseSparseAdd(a,b);
  if (a.isSparse() && !b.isSparse()) {
    Object afull = this->asFullMatrix(a);
    return afull.type()->Add(afull,b);
  }
  throw Exception("Unhandled case for SparseMatrix add!");
}


template <>
Object FM::SparseType<double>::asFullMatrix(const Object &a) {
  const SparseData<double> *ad = this->ro(a);
  Object af = _ctxt->_double->makeMatrix(a.rows(),a.cols());
  double *afd = _ctxt->_double->rw(af);
  for (auto column : ad->m_data)
    for (auto row : column->second)
      afd[row->first + column->first*a.rows()] = row->second;
  return af;
}

template <>
Object FM::SparseType<bool>::asFullMatrix(const Object &a) {
  const SparseData<bool> *ad = this->ro(a);
  Object af = _ctxt->_bool->makeMatrix(a.rows(),a.cols());
  bool *afd = _ctxt->_bool->rw(af);
  for (auto column : ad->m_data)
    for (auto row : column->second)
      afd[row->first + column->first*a.rows()] = row->second;
  return af;
}

template <>
Object FM::SparseType<Complex<double> >::asFullMatrix(const Object &a) {
  const SparseData<Complex<double> > *ad = this->ro(a);
  Object af = _ctxt->_double->makeMatrix(a.rows(),a.cols(),true);
  Complex<double> *afd = _ctxt->_double->rwComplex(af);
  for (auto column : ad->m_data)
    for (auto row : column->second)
      afd[row->first + column->first*a.rows()] = row->second;
  return af;
}

template class FM::SparseType<double>;
template class FM::SparseType<bool>;
template class FM::SparseType<Complex<double> >;
