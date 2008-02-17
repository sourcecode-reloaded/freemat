#ifndef __NDimArray_hpp__
#define __NDimArray_hpp__

#include "BaseArray.hpp"
#include "Iterators.hpp"
#include "RangeSet.hpp"
#include <QString>
#include <QVector>

template <typename T>
class NDimArrayIterator;

template <typename T>
class ConstNDimArrayIterator;

// The NDimArray
template <typename T>
class NDimArray : public BaseArray<T> {
  QVector<T> m_data;
  NTuple m_dims;
public:
  NDimArray() : m_data(), m_dims(0,0) { }
  NDimArray(const NTuple& dim) : m_data(), m_dims(dim) {
    m_data.resize(m_dims.count());
  }
  NDimArray(QVector<T> data, NTuple dims) : 
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
    NDimArray<T> retval(pos);
    ConstNDimArrayIterator<T> source(this,0);
    NDimArrayIterator<T> dest(this,0);
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
      (new NDimArrayIterator<T>(this, dim));
  }
  ConstNDimIterator<T>* getConstNDimIterator(int dim) {
    return dynamic_cast<ConstNDimIterator<T>*>
      (new ConstNDimArrayIterator<T>(this, dim));
  }
};

template <typename T>
class NDimArrayIterator : public NDimIterator<T> {
  index_t m_offset;
  index_t m_stride;
  index_t m_indx;
public:
  NDimArrayIterator(NDimArray<T> *ptr, int dim) 
    : NDimIterator<T>(ptr,dim) {
    m_offset = 1;
    m_stride = 1;
    for (int i=0;i<dim;i++) m_stride *= NDimArrayIterator::m_dim[i];
    m_indx = 1;
  }
  void nextSlice() {
    NDimIterator<T>::nextSlice();
    m_indx = NDimArrayIterator::m_dim.map(NDimArrayIterator::m_pos);
  }
  void set(const T& val) {
    (*NDimArrayIterator::m_ptr)[m_indx] = val;
  }
  const T get() const {
    return ((*NDimArrayIterator::m_ptr)[m_indx]);
  }
  void advance() {m_indx += m_stride;}
};

template <typename T>
class ConstNDimArrayIterator : public ConstNDimIterator<T> {
  index_t m_offset;
  index_t m_stride;
  index_t m_indx;
public:
  ConstNDimArrayIterator(const NDimArray<T> *ptr, int dim) 
    : ConstNDimIterator<T>(ptr,dim) {
    m_offset = 1;
    m_stride = 1;
    for (int i=0;i<dim;i++) m_stride *= ConstNDimArrayIterator::m_dim[i];
    m_indx = 1;
  }
  void nextSlice() {
    ConstNDimIterator<T>::nextSlice();
    m_indx = ConstNDimArrayIterator::m_dim.map(ConstNDimArrayIterator::m_pos);
  }
  const T get() const {
    return ((*ConstNDimArrayIterator::m_ptr)[m_indx]);
  }
  void advance() {m_indx += m_stride;}
};


template <typename T>
NDimArray<T> resize(const BaseArray<T> &src, const NTuple& size) {
  NDimArray<T> retval(size);
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
NDimArray<T> resize(const BaseArray<T> &src, index_t len) {
  NDimArray<T> retval(1,len);
  index_t line_size = qMin(src.dimensions().count(),
			   retval.dimensions().count());
  for (int i=1;i<=line_size;i++)
    retval[i] = src[i];
  return retval;
}

#endif
