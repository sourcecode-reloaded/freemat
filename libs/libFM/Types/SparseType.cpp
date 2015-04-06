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

template class FM::SparseType<double>;
template class FM::SparseType<bool>;
template class FM::SparseType<Complex<double> >;
