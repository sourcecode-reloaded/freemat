#ifndef __BasicArray_hpp__
#define __BasicArray_hpp__

#include <QVector>
#include "CommonIterator.hpp"
#include "NTuple.hpp"
#include <QDebug>
#include "Types.hpp"

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
  BasicArray() : m_data(), m_dims(0,0), m_offset(0) {}
  BasicArray(const NTuple& dim) : m_data(), m_dims(dim), m_offset(0) 
  {
    m_data.resize(m_dims.count());
  }
  ~BasicArray() {}
  inline index_t offset() const {return m_offset;}
  inline QVector<T>& data() {return m_data;}
  inline const QVector<T>& data() const {return m_data;}
  inline const NTuple dimensions() const {return m_dims;}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t cols() const {return m_dims.cols();}
  inline index_t length() const {return m_dims.count();}
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
  inline const T get(index_t pos) const {
    return m_data[pos+m_offset-1];
  }
  BasicArray<T> getVectorSubset(const Variant& index);
  BasicArray<T> getNDimSubset(const VariantList& index);
  void set(index_t pos, double val);
  void resize(const NTuple& pos);
  inline void reshape(const NTuple& pos) {
    if (m_dims.count() == pos.count())
      m_dims = pos;
    else
      throw Exception("Illegal reshape");
  }
};

template <typename T>
BasicArray<T> Apply(const BasicArray<T>& arg, T (*func)(T));

template <typename T>
BasicArray<T> Transpose(const BasicArray<T>& arg);

template <typename T>
bool IsPositive(const BasicArray<T>& arg);

template <typename T>
bool IsSymmetric(const BasicArray<T>& arg);

template <typename T>
bool AllZeros(const BasicArray<T>& arg);

template <typename T>
bool AnyNotFinite(const BasicArray<T>& arg);

template <typename T, typename S>
const BasicArray<T>& ConvertType(const BasicArray<S>& arg);

template <typename S>
class BasicIterator : public BaseIterator<BasicArray<S>,S> {
public:
  BasicIterator(BasicArray<S> *ptr, int dim) : 
    BaseIterator<BasicArray<S>,S>(ptr,dim) {}
};

template <typename S>
class ConstBasicIterator : public ConstBaseIterator<BasicArray<S>,S> {
public:
  ConstBasicIterator(const BasicArray<S> *ptr, int dim) : 
    ConstBaseIterator<BasicArray<S>,S>(ptr,dim) {}
};

#include "BasicArrayPrivate.hpp"


#endif
