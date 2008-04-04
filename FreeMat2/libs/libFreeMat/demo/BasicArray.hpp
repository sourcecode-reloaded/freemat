#ifndef __BasicArray_hpp__
#define __BasicArray_hpp__

#include <QVector>
#include "CommonIterator.hpp"
#include "NTuple.hpp"
#include <QDebug>
#include "Types.hpp"
#include "FastList.hpp"

template <typename T>
class BasicArray;

typedef BasicArray<index_t> IndexArray;
typedef FastList<IndexArray> IndexArrayList;

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
  BasicArray(const NTuple& dim) : 
    m_data(), m_dims(dim), m_offset(0) {
    m_data.resize((int64)(m_dims.count()));
  }
  BasicArray(T val) {
    m_data << val; 
    m_dims = NTuple(1,1); 
    m_offset = 0;
  }
  ~BasicArray() {}
  inline index_t offset() const {return m_offset;}
  inline QVector<T>& data() {return m_data;}
  inline const QVector<T>& data() const {return m_data;}
  inline const NTuple dimensions() const {return m_dims;}
  inline index_t rows() const {return m_dims.rows();}
  inline index_t cols() const {return m_dims.cols();}
  inline index_t length() const {return m_dims.count();}
  inline index_t isScalar() const {return length() == 1;}
  inline bool isEmpty() const {return (length() == 0);}
  inline bool isColumnVector() const {return m_dims.isColumnVector();}
  inline bool isRowVector() const {return m_dims.isRowVector();}
  inline bool isVector() const {return isColumnVector() || isRowVector();}
  inline const T operator[](const NTuple& pos) const {
    if (m_dims.validate(pos))
      return m_data[(int64)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos))
      return m_data[(int64)(m_dims.map(pos)+m_offset-1)];
    throw Exception("out of range");
  }
  inline const T operator[](index_t pos) const {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[(int64)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  inline T& operator[](index_t pos) {
    if ((pos > 0) && (pos <= m_data.size()))
      return m_data[(int64)(pos+m_offset-1)];
    throw Exception("out of range");
  }
  bool operator==(const BasicArray<T>& b) const;
  inline void set(const NTuple& pos, const T& val) {
    if (dimensions() <= pos) resize(pos);
    m_data[(int64)(m_dims.map(pos)+m_offset-1)] = val;
  }
  inline void set(index_t pos, const T& val) {
    if (dimensions().count() <= pos) resize(pos);
    m_data[(int64)(pos+m_offset-1)] = val;
  }
  inline const T get(index_t pos) const {
    return m_data[(int64)(pos+m_offset-1)];
  }
  inline const T get(const NTuple& pos) const {
    return m_data[(int64)(m_dims.map(pos)+m_offset-1)];
  }
  BasicArray<T> get(const IndexArray& index) const;
  BasicArray<T> get(const IndexArrayList& index) const;
  BasicArray<T> getSlice(const IndexArrayList& index) const;
  void setSlice(const IndexArrayList& index, const BasicArray<T>& data);
  void setSlice(const IndexArrayList& index, const T& data);
  void set(const IndexArray& index, const T& data);
  void set(const IndexArray& index, const BasicArray<T>& data);
  void set(const IndexArrayList& index, const T& data);
  void set(const IndexArrayList& index, const BasicArray<T>& data);
  void del(const IndexArrayList& index);
  void del(const IndexArray& index);
  void resize(const NTuple& pos);
  void resize(index_t len);
  void printMe(std::ostream& o) const;
  inline void reshape(const NTuple& pos) {
    if (m_dims.count() == pos.count())
      m_dims = pos;
    else
      throw Exception("Illegal reshape");
  }
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const BasicArray<T>& arg) {
  arg.printMe(o);
  return o;
}

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

bool AllTrue(const BasicArray<bool>& arg);

template <typename T>
bool AnyNotFinite(const BasicArray<T>& arg);

template <typename T>
T MaxValue(const BasicArray<T>& arg);

template <typename S, typename T>
void ConvertBasicArray(const BasicArray<S>& source, BasicArray<T>& dest) {
  if (source.length() != dest.length())
    throw Exception("Mismatch between source and destination arrays in call to Convert BasicArray");
  for (index_t i=1;i<=source.length();i++)
    dest.set(i,(T) source.get(i));
}

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

template <typename T>
class ConstSpinIterator {
  const BasicArray<T> *m_ptr;
  int m_indx;
public:
  ConstSpinIterator(const BasicArray<T> *ptr) : m_ptr(ptr), m_indx(1) {}
  inline T get() const {return m_ptr->get(m_indx);}
  inline void next() {
    m_indx++; m_indx = ((m_indx - 1) % m_ptr->length()) + 1;
  }
};

BasicArray<bool> GetDeletionMap(const BasicArray<index_t>& vec,
				index_t length);

template <typename T>
BasicArray<index_t> Find(const BasicArray<T>& vec) {
  index_t count = 0;
  for (index_t i=1;i<=vec.length();i++)
    if (vec[i] != T()) count++;
  BasicArray<index_t> retvec(NTuple(count,1));
  count = 1;
  for (index_t i=1;i<=vec.length();i++)
    if (vec[i] != T()) {
      retvec.set(count,i);
      count++;
    }
  return retvec;
}
#endif
