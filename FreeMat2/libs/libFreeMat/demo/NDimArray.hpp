#ifndef __NDimArray_hpp__
#define __NDimArray_hpp__

#include "BaseArray.hpp"
#include "Iterators.hpp"
#include "RangeSet.hpp"
#include <QString>
#include <QVector>

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
  inline const NTuple& dimensions() const {return m_dims;}
  inline const T& operator[](const NTuple& pos) const {
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
  inline const T& operator[](index_t pos) const {
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
    ConstNDimIterator<T> source(this,0);
    NDimIterator<T> dest(&retval,0);
    index_t line_size = qMin(source.size(),dest.size());
    while (source.isValid() && dest.isValid()) {
      for (int i=1;i<=line_size;i++)
	dest[i] = source[i];
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
