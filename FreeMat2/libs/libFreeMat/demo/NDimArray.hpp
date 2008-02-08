#ifndef __NDimArray_hpp__
#define __NDimArray_hpp__

#include <QString>
#include <QSharedData>
#include <QVector>
#include <QSharedDataPointer>

#include <iostream>

typedef qint64 index_t;

const int NDims = 10;

typedef QString Exception;

class NTuple {
  index_t m_data[NDims];
public:
  index_t map(const NTuple& pos) const {
    index_t retval = 1;
    index_t nextCoeff = 1;
    for (int i=0;i<NDims;i++) {
      retval += nextCoeff*(pos[i]-1);
      nextCoeff *= m_data[i];
    }
    return retval;
  }
  inline bool validate(const NTuple& pos) const {
    for (int i=0;i<NDims;i++) 
      if ((pos.m_data[i] <= 0) || 
	  (pos.m_data[i] > m_data[i])) return false;
    return true;
  }
  inline NTuple(index_t rows, index_t cols) {
    m_data[0] = rows;
    m_data[1] = cols;
    for (int i=2;i<NDims;i++) m_data[i] = 1;
  }
  inline NTuple(index_t rows, index_t cols, index_t slices) {
    m_data[0] = rows;
    m_data[1] = cols;
    m_data[2] = slices;
    for (int i=3;i<NDims;i++) m_data[i] = 1;
  }
  inline NTuple() {
    m_data[0] = 0;
    m_data[1] = 0;
    for (int i=2;i<NDims;i++) m_data[i] = 1;
  }
  inline void set(int dim, index_t len) {
    m_data[dim] = len;
  }
  inline index_t& get(int dim) {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  inline index_t get(int dim) const {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  index_t operator[](int dim) const {
    return get(dim);
  }
  index_t& operator[](int dim) {
    return get(dim);
  }
  inline index_t count() const {
    index_t ret = 1;
    for (int i=0;i<NDims;i++) ret *= m_data[i];
    return ret;
  }
};

inline std::ostream& operator<<(std::ostream& o, const NTuple &t) {
  o << "<";
  for (int i=0;i<NDims;i++)
    o << t[i] << " ";
  o << ">";
  return o;
}

// consider 1:2:10 .. 1 3 5 7 9 -- 9/2 = 4.5 + 1 = 5
// consider 1:1:1 .. 1 -- 0/1 = 0 + 1
class RangeSet {
  NTuple m_first;
  NTuple m_step;
  NTuple m_last;
  NTuple m_dims;
public:
  RangeSet(NTuple first, NTuple step, NTuple last) :
    m_first(first), m_step(step), m_last(last) {
    calculateDims();
  }
  RangeSet() {}
  inline const NTuple& first() const {return m_first;}
  inline const NTuple& step() const {return m_step;}
  inline const NTuple& last() const {return m_last;}
  inline const NTuple& dims() const {return m_dims;}
  // Take a vector index, and convert to a sliced index
  inline void remap(index_t pos, NTuple& out) const {
    pos = pos-1;
    index_t pagesize = 1;
    for (int i=0;i<NDims-1;i++) {
      pagesize *= m_dims[i];
      index_t sliced_index = pos % pagesize;
      out[i] = m_first[i]+sliced_index*m_step[i];
      pos = pos / pagesize;
    }
  }
  inline void remap(const NTuple& pos, NTuple& out) const {
    for (int i=0;i<NDims;i++)
      out[i] = m_first[i] + (pos[i]-1)*m_step[i];
  }
  inline void calculateDims() {
    for (int i=0;i<NDims;i++)
      m_dims[i] = (((index_t) (m_last[i] - m_first[i])/(m_step[i])) + 1);
  }
  inline index_t count() {
    return m_dims.count();
  }
};

template <typename T>
class NDimIterator;

template <typename T>
class ConstNDimIterator;

template <typename T>
class VectorIterator;

template <typename T>
class ConstVectorIterator;

template <typename T>
class BaseArray {
public:
  virtual const NTuple& dimensions() const = 0;
  virtual const T& operator[](const NTuple& pos) const = 0;
  virtual T& operator[](const NTuple& pos) = 0;
  virtual const T& operator[](index_t pos) const = 0;
  virtual T& operator[](index_t pos) = 0;
  virtual void resize(const NTuple& pos) = 0;
  virtual void resize(index_t pos) = 0;
  virtual void reshape(const NTuple& pos) = 0;
  virtual ~BaseArray() {}
};

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
    index_t line_size = qMin(source.lineSize(),dest.lineSize());
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
class NDimSlice : public BaseArray<T> {
  NDimArray<T> &m_ref;
  const RangeSet &m_slices;
  NTuple m_dims;
public:
  NDimSlice(NDimArray<T> &ref, const RangeSet& slice) : 
    m_ref(ref), m_slices(slice) { 
    m_dims = m_slices.dims();
  }
  inline const NTuple& dimensions() const {
    return m_dims;
  }
  inline const T& operator[](const NTuple& pos) const {
    if (m_dims.validate(pos)) {
      NTuple tpos;
      m_slices.remap(pos,tpos);
      if (m_ref.dimensions().validate(tpos))
	return m_ref[tpos];
    }
    throw Exception("Out of range");
  }
  inline T& operator[](const NTuple& pos) {
    if (m_dims.validate(pos)) {
      NTuple tpos;
      m_slices.remap(pos,tpos);
      if (m_ref.dimensions().validate(tpos))
	return m_ref[tpos];
    }
    throw Exception("Out of range");
  }
  inline const T& operator[](index_t) const {
    throw Exception("unsupported op");
  }
  inline T& operator[](index_t) {
    throw Exception("unsupported op");
  }
  inline void resize(const NTuple& ) {
    throw Exception("unsupported op");
  }
  inline void resize(index_t) {
    throw Exception("unsupported op");
  }
  inline void reshape(const NTuple& ) {
    throw Exception("unsupported op");
  }
  //  inline void unslice() {
  //    QVector<T> data;
  //    index_t count = m_slices.count();
  //    data.resize(count);
  //    ConstVectorIterator<T> p(this);
  //    for (int i=0;i<count;i++) {
  //      data[i] = *p;
  //      ++p;
  //    }
  //    m_data = data;
  //    m_dims = m_slices.dims();
  //    m_sliced = false;
  //  }
  //  inline T& get(const NTuple& pos) {
  //    if (m_sliced) unslice();
  //    if (m_dims.validate(pos))
  //      return m_data[m_dims.map(pos)-1];
  //    throw Exception("out of range");
  //  }
  //  inline const T & get(index_t pos) const {
  //    if (m_sliced) {
  //      NTuple mapped;
  //      m_slices.remap(pos,mapped);
  //      pos = m_dims.map(mapped);
  //    }
  //    if ((pos > 0) && (pos <= m_data.size()))
  //      return m_data[pos-1];
  //    throw Exception("out of range");
  //  }
  //  inline T& get(index_t pos) {
  //    if (m_sliced) unslice();
  //    if ((pos > 0) && (pos <= m_data.size()))
  //      return m_data[pos-1];
//    throw Exception("out of range");
//  }
//  inline void set(const NTuple& pos, const T& val) {
//    if (m_sliced) unslice();
//    if (m_dims.validate(pos))
//      m_data[m_dims.map(pos)-1] = val;
//    else {
//      resize(pos);
//      m_data[m_dims.map(pos)-1] = val;
//    }
//  }
//  inline void set(index_t pos, const T& val) {
//    if (m_sliced) unslice();
//    if (m_data.inVectorRange(pos))
//      m_data[pos-1] = val;
//    else {
//      resize(pos);
//      m_data[pos-1] = val;
//    }
//  }
//  inline const T& operator[](const NTuple& pos) const {
//    return get(pos);
//  }
//  inline  T& operator[](const NTuple& pos) {
//    return get(pos);
//  }
//  inline const T& operator[](index_t pos) const {
//    return get(pos);
//  }
//  inline T& operator[](index_t pos) {
//    return get(pos);
//  }
//  inline NDimArray<T> resize(const NTuple& pos) const {
//    NDimArray<T> retval(pos);
//    ConstNDimIterator<T> source(this,0);
//    NDimIterator<T> dest(&retval,0);
//    index_t line_size = qMin(source.lineSize(),dest.lineSize());
//    while (source.isValid() && dest.isValid()) {
//      for (int i=1;i<=line_size;i++)
//	dest[i] = source[i];
//      source.nextSlice();
//      dest.nextSlice();
//    } 
//    return retval;
//  }
//  inline void resize(index_t pos) {
//    if (m_sliced) unslice();
//    m_data.resize(pos);
//    m_dims = NTuple(1,pos);
//  }
//  inline void reshape(const NTuple& pos) {
//    if (m_sliced) unslice();
//    if (m_dims.count() == pos.count())
//      m_dims = pos;
//    else
//      throw Exception("Illegal reshape");
//  }
};

class VectorIteratorBase {
private:
  index_t m_index;
public:
  VectorIteratorBase(index_t index) {
    m_index = index;
  }
  const index_t& index() const {return m_index;}
  index_t& index() {return m_index;}
};

template <typename T>
class VectorIterator : public VectorIteratorBase  {
  NDimArray<T> *m_ptr;
public:
  VectorIterator(NDimArray<T> *p) : VectorIteratorBase() { m_ptr = p; }
  T& operator*() {return m_ptr->get(index());}
  const T& operator*() const {return m_ptr->get(index());}
  VectorIterator& operator++() {index(index()+1); return *this;}
};

template <typename T>
class ConstVectorIterator : public VectorIteratorBase  {
  const NDimArray<T> *m_ptr;
public:
  ConstVectorIterator(const NDimArray<T> *p) : VectorIteratorBase() 
  { m_ptr = p; }
  T& operator*() {return m_ptr->get(index());}
  const T& operator*() const {return m_ptr->get(index());}
  ConstVectorIterator& operator++() {index(index()+1); return *this;}
};

class NDimIteratorBase {
private:
  index_t m_lineSize;
  index_t m_planeSize;
  index_t m_planeCount;
  index_t m_plane;
  index_t m_line;
public:
  NDimIteratorBase(const NTuple &dims, int dim) {
    m_planeSize = 1;
    for (int i=0;i<dim;i++)
      m_planeSize *= dims[i];
    m_lineSize = dims[dim];
    m_planeCount = 1;
    for (int i=dim+1;i<NDims;i++) 
      m_planeCount *= dims[i];
    m_plane = 0;
    m_line = 1;
  }
  inline index_t lineSize() const {return m_lineSize;}
  inline index_t planeSize() const {return m_planeSize;}
  inline index_t planeCount() const {return m_planeCount;}
  inline index_t plane() const {return m_plane;}
  inline index_t line() const {return m_line;}
  inline index_t vectorAddress(index_t ndx) const {
    return m_plane*m_planeSize*m_lineSize+m_line+(ndx-1)*m_planeSize;
  }
  inline void nextSlice() {
    m_line++;
    if (m_line >= m_planeSize) {
      m_line = 1;
      m_plane++;
    }
  }
  inline bool isValid() {
    return (m_plane<m_planeCount);
  }
};

template <typename T> 
class NDimIterator : public NDimIteratorBase {
  NDimArray<T> *m_ptr;
public:
  NDimIterator(NDimArray<T> *p, int dim) : NDimIteratorBase(p->dimensions(),dim) {m_ptr = p;}
  inline T& operator[](index_t ndx) {return (*m_ptr)[vectorAddress(ndx)];}
};

template <typename T>
class ConstNDimIterator : public NDimIteratorBase {
  const NDimArray<T> *m_ptr;
public:
  ConstNDimIterator(const NDimArray<T> *p, int dim) : NDimIteratorBase(p->dimensions(),dim) {m_ptr = p;}
  inline T operator[](index_t ndx) const {return (*m_ptr)[vectorAddress(ndx)];}
};

std::ostream& operator<<(std::ostream& o, const NDimIteratorBase &x) {
  o << "<" << x.line() << "," << x.plane() << ">-<" << x.planeSize() << "," << x.planeCount() << ">";
  return o;
}

#endif
