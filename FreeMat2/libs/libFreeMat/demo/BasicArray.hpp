#ifndef __BasicArray_hpp__
#define __BasicArray_hpp__

#include <QVector>
#include "NTuple.hpp"
#include <QDebug>

template <typename T>
class BasicIterator;

template <typename T>
class ConstBasicIterator;

template <typename T>
class BasicArray {
  QVector<T> m_data;
  NTuple m_dims;
  index_t m_offset;
public:
  BasicArray() : m_data(), m_dims(0,0), m_offset(0) 
  {
    qDebug() << "Construct empty";
  }
  BasicArray(const NTuple& dim) : m_data(), m_dims(dim), m_offset(0) 
  {
    m_data.resize(m_dims.count());
    qDebug() << "Construct full";
  }
  ~BasicArray() {
    qDebug() << "Destruct";
  }
  inline index_t offset() const {return m_offset;}
  inline QVector<T>& data() {return m_data;}
  inline const QVector<T>& data() const {return m_data;}
  inline const NTuple dimensions() const {return m_dims;}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[m_dims.map(pos)+m_offset-1];
    throw Exception("out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos))
      return m_data[m_dims.map(pos)+m_offset-1];
    throw Exception("out of range");
  }
  inline const T operator[](index_t pos) const {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[pos+m_offset-1];
    throw Exception("out of range");
  }
  inline T& operator[](index_t pos) {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[pos+m_offset-1];
    throw Exception("out of range");
  }
  inline void resize(const NTuple& pos) {
    BasicArray<T> retval(pos);
    ConstBasicIterator<T> source(this,0);
    BasicIterator<T> dest(&retval,0);
    index_t line_size = qMin(source.size(),dest.size());
    while (source.isValid() && dest.isValid()) {
      for (int i=1;i<=line_size;i++)
	dest[i] = source[i];
      source.nextSlice();
      dest.nextSlice();
    }
    m_data = retval.m_data;
    m_dims = retval.m_dims;
    m_offset = 0;
  }
  //   inline void resize(index_t pos) {
  //     m_data.resize(pos);
  //     m_dims = NTuple(1,pos);
  //   }
  inline void reshape(const NTuple& pos) {
    if (m_dims.count() == pos.count())
      m_dims = pos;
    else
      throw Exception("Illegal reshape");
  }
};

template <typename S>
class BasicIterator {
  BasicArray<S> *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
  index_t m_ndx;
public:
  BasicIterator(BasicArray<S> *ptr, int dim) : 
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim),
    m_dim(ptr->dimensions()) {
    m_ndx = m_ptr->offset();
  }
  inline void nextSlice() {
    m_dim.increment(m_pos,m_pinDim);
    m_ndx = m_ptr->dimensions().map(m_pos)+m_ptr->offset()-1;
  }
  inline void advance() {m_dim[m_pinDim]++;}
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
  inline S& operator[](index_t n) {return m_ptr->data()[m_ndx+n];}
  inline const S operator[](index_t n) const {return m_ptr->data()[m_ndx+n];}
  ~BasicIterator() {}
};

template <typename S>
class ConstBasicIterator {
  const BasicArray<S> *m_ptr;
  NTuple m_pos;
  int m_pinDim;
  NTuple m_dim;
  index_t m_ndx;
public:
  ConstBasicIterator(const BasicArray<S> *ptr, int dim) :
    m_ptr(ptr), m_pos(NTuple(1,1)), m_pinDim(dim),
    m_dim(ptr->dimensions()) {
    m_ndx = m_ptr->offset();
  }
  inline void nextSlice() {
    m_dim.increment(m_pos,m_pinDim);
    m_ndx = m_ptr->dimensions().map(m_pos)+m_ptr->offset()-1;
  }
  inline void advance() {m_dim[m_pinDim]++;}
  inline bool isValid() const {return (m_pos <= m_dim);}
  inline index_t size() const {return m_dim[m_pinDim];}
  inline const S operator[](index_t n) const {return m_ptr->data()[m_ndx+n];}
  ~ConstBasicIterator() {}
};

#endif
