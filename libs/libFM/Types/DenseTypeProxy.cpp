#include "DenseTypeProxy.hpp"
#include "AllTypes.hpp"

namespace FM {

  template <>
  bool* DenseTypeProxy<bool>::rw(Object &a) {
    return _ctxt->_bool->rw(a);
  }

  template <>
  double* DenseTypeProxy<double>::rw(Object &a) {
    return _ctxt->_double->rw(a);
  }

  template <>
  Complex<double>* DenseTypeProxy<Complex<double> >::rw(Object &a) {
    return _ctxt->_double->rwComplex(a);
  }

  template <>
  const bool* DenseTypeProxy<bool>::ro(const Object &a) {
    return _ctxt->_bool->ro(a);
  }

  template <>
  const double* DenseTypeProxy<double>::ro(const Object &a) {
    return _ctxt->_double->ro(a);
  }

  template <>
  const Complex<double>* DenseTypeProxy<Complex<double> >::ro(const Object &a) {
    return _ctxt->_double->roComplex(a);
  }

  template <>
  Object DenseTypeProxy<bool>::make(ndx_t rows, ndx_t cols) {
    return _ctxt->_bool->makeMatrix(rows,cols,false);
  }

  template <>
  Object DenseTypeProxy<double>::make(ndx_t rows, ndx_t cols) {
    return _ctxt->_double->makeMatrix(rows,cols,false);
  }

  template <>
  Object DenseTypeProxy<Complex<double> >::make(ndx_t rows, ndx_t cols) {
    return _ctxt->_double->makeMatrix(rows,cols,true);
  }

  template <>
  Object DenseTypeProxy<bool>::applyFunc(TypeMemFn func, const Object &x, const Object &y) {
    return (*(_ctxt->_bool).*func)(x,y);
  }
  
  template <>
  void DenseTypeProxy<bool>::setColumn(Object &ret, ndx_t col, const Object &val) {
    _ctxt->_bool->setColumn(ret,col,val);
  }
  
  template <>
  Object DenseTypeProxy<bool>::convert(const Object &a) {
    return (_ctxt->_bool->convert(a));
  }

  template <>
  Object DenseTypeProxy<double>::applyFunc(TypeMemFn func, const Object &x, const Object &y) {
    return (*(_ctxt->_double).*func)(x,y);
  }
  
  template <>
  void DenseTypeProxy<double>::setColumn(Object &ret, ndx_t col, const Object &val) {
    _ctxt->_double->setColumn(ret,col,val);
  }
  
  template <>
  Object DenseTypeProxy<double>::convert(const Object &a) {
    return (_ctxt->_double->convert(a));
  }

  template <>
  Object DenseTypeProxy<Complex<double> >::applyFunc(TypeMemFn func, const Object &x, const Object &y) {
    return (*(_ctxt->_double).*func)(x,y);
  }
  
  template <>
  void DenseTypeProxy<Complex<double> >::setColumn(Object &ret, ndx_t col, const Object &val) {
    _ctxt->_double->setColumn(ret,col,val);
  }
  
  template <>
  Object DenseTypeProxy<Complex<double> >::convert(const Object &a) {
    return (_ctxt->_double->convert(a));
  }

  template class DenseTypeProxy<double>;
  template class DenseTypeProxy<bool>;
  template class DenseTypeProxy<Complex<double> >;
};
