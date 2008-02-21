#ifndef __DenseArray_hpp__
#define __DenseArray_hpp__

#include "BaseArray.hpp"
#include "Iterators.hpp"
#include "RangeSet.hpp"
#include <QString>
#include <QVector>

template <typename T>
class DenseArrayIterator;

template <typename T>
class ConstDenseArrayIterator;

// The DenseArray
template <typename T>
class DenseArray : public BaseArray<T> {
  QVector<T> m_data;
  NTuple m_dims;
public:
  DenseArray() : m_data(), m_dims(0,0) { }
  DenseArray(const NTuple& dim) : m_data(), m_dims(dim) {
    m_data.resize(m_dims.count());
  }
  DenseArray(QVector<T> data, NTuple dims) : 
    m_data(data), m_dims(dims) {}
  inline const NTuple dimensions() const {return m_dims;}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[m_dims.map(pos)-1];
    throw Exception("out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos))
      return m_data[m_dims.map(pos)-1];
    else {
      resize(pos);
      return m_data[m_dims.map(pos)-1];
    }
  }
  inline const T operator[](index_t pos) const {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[pos-1];
    throw Exception("out of range");
  }
  inline T& operator[](index_t pos) {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[pos-1];
    if (pos > m_data.size()) {
      resize(pos);
      return m_data[pos-1];
    }
    throw Exception("out of range");
  }
  inline void resize(const NTuple& pos) {
    DenseArray<T> retval(pos);
    ConstDenseArrayIterator<T> source(this,0);
    DenseArrayIterator<T> dest(this,0);
    index_t line_size = qMin(source.size(),dest.size());
    while (source.isValid() && dest.isValid()) {
      for (int i=1;i<=line_size;i++) {
	dest.set(source.get());
	dest.advance(); source.advance();
      }
      source.nextSlice();
      dest.nextSlice();
    }
    m_data = retval.m_data;
    m_dims = retval.m_dims;
  }
  inline void resize(index_t pos) {
    m_data.resize(pos);
    m_dims = NTuple(1,pos);
  }
  inline void reshape(const NTuple& pos) {
    if (m_dims.count() == pos.count())
      m_dims = pos;
    else
      throw Exception("Illegal reshape");
  }
  NDimIterator<T>* getNDimIterator(int dim) {
    return dynamic_cast<NDimIterator<T>*>
      (new DenseArrayIterator<T>(this, dim));
  }
  ConstNDimIterator<T>* getConstNDimIterator(int dim) {
    return dynamic_cast<ConstNDimIterator<T>*>
      (new ConstDenseArrayIterator<T>(this, dim));
  }
};

template <typename T>
class DenseArrayIterator : public NDimIterator<T> {
  index_t m_offset;
  index_t m_stride;
  index_t m_indx;
public:
  DenseArrayIterator(DenseArray<T> *ptr, int dim) 
    : NDimIterator<T>(ptr,dim) {
    m_offset = 1;
    m_stride = 1;
    for (int i=0;i<dim;i++) m_stride *= DenseArrayIterator::m_dim[i];
    m_indx = 1;
  }
  void nextSlice() {
    NDimIterator<T>::nextSlice();
    m_indx = DenseArrayIterator::m_dim.map(DenseArrayIterator::m_pos);
  }
  void set(const T& val) {
    (*DenseArrayIterator::m_ptr)[m_indx] = val;
  }
  const T get() const {
    return ((*DenseArrayIterator::m_ptr)[m_indx]);
  }
  void advance() {m_indx += m_stride;}
};

template <typename T>
class ConstDenseArrayIterator : public ConstNDimIterator<T> {
  index_t m_offset;
  index_t m_stride;
  index_t m_indx;
public:
  ConstDenseArrayIterator(const DenseArray<T> *ptr, int dim) 
    : ConstNDimIterator<T>(ptr,dim) {
    m_offset = 1;
    m_stride = 1;
    for (int i=0;i<dim;i++) m_stride *= ConstDenseArrayIterator::m_dim[i];
    m_indx = 1;
  }
  void nextSlice() {
    ConstNDimIterator<T>::nextSlice();
    m_indx = ConstDenseArrayIterator::m_dim.map(ConstDenseArrayIterator::m_pos);
  }
  const T get() const {
    return ((*ConstDenseArrayIterator::m_ptr)[m_indx]);
  }
  void advance() {m_indx += m_stride;}
};


template <typename T>
DenseArray<T> resize(const BaseArray<T> &src, const NTuple& size) {
  DenseArray<T> retval(size);
  ConstNDimIterator<T> source(&src,0);
  NDimIterator<T> dest(&retval,0);
  index_t line_size = qMin(source.size(),dest.size());
  while (source.isValid() && dest.isValid()) {
    for (int i=1;i<=line_size;i++)
      dest[i] = source[i];
    source.nextSlice();
    dest.nextSlice();
  }
  return retval;
}

template <typename T>
DenseArray<T> resize(const BaseArray<T> &src, index_t len) {
  DenseArray<T> retval(1,len);
  index_t line_size = qMin(src.dimensions().count(),
			   retval.dimensions().count());
  for (int i=1;i<=line_size;i++)
    retval[i] = src[i];
  return retval;
}

#endif
