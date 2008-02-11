#ifndef __NDimArray_hpp__
#define __NDimArray_hpp__

#include "BaseArray.hpp"
#include "RangeSet.hpp"
#include <QString>
#include <QVector>

// The NDimArray
template <typename T>
class NDimArray : public BaseArray<T> {
  QVector<T> m_data;
  NTuple m_dims;
public:

  template <typename S>
  class NDimArrayNDimIterator : public NDimIterator<S> {
    NDimArray<S> *m_ptr;
    index_t m_ndx;
    NDimIteratorHelper m_hlp;
  public:
    NDimArrayNDimIterator(NDimArray<S> *ptr, int dim) : 
      m_ptr(ptr), m_ndx(0), m_hlp(ptr->dimensions(), dim) {}
    virtual void nextSlice() { m_hlp.advanceSlice(); m_ndx = 0;} 
    virtual bool isValid() const {return m_hlp.isSliceValid();}
    virtual const S& get() const {return (*m_ptr)[m_hlp.vectorAddress(m_ndx+1)];}
    virtual void set(const S& t) const {(*m_ptr)[m_hlp.vectorAddress(m_ndx+1)] = t;}
    virtual void next() {m_ndx++;}
    virtual void prev() {m_ndx--;}
    virtual index_t size() const {return m_hlp.lineSize();}
    ~NDimArrayNDimIterator() {}
  };
  
  template <typename S>
  class ConstNDimArrayNDimIterator : public ConstNDimIterator<S> {
    const NDimArray<S> *m_ptr;
    index_t m_ndx;
    NDimIteratorHelper m_hlp;
  public:
    ConstNDimArrayNDimIterator(const NDimArray<S> *ptr ,int dim) :
      m_ptr(ptr), m_ndx(0), m_hlp(ptr->dimensions(),dim) {}
    virtual void nextSlice() { m_hlp.advanceSlice(); m_ndx = 0;}
    virtual bool isValid() const {return m_hlp.isSliceValid();}
    virtual const S& get() const {return (*m_ptr)[m_hlp.vectorAddress(m_ndx+1)];}
    virtual void next() {m_ndx++;}
    virtual void prev() {m_ndx--;}
    virtual index_t size() const {return m_hlp.lineSize();}
    ~ConstNDimArrayNDimIterator() {}
  };

  template <typename S>
  class NDimArrayVectorIterator : public VecIterator<S> {
    NDimArray<S> *m_ptr;
    index_t ndx;
  public:
    NDimArrayVectorIterator(NDimArray<S> *ptr) {m_ptr = ptr; ndx = 1;}
    virtual bool isValid() const {return ((ndx>0) && (ndx <= m_ptr->dimensions().count()));}
    virtual const T& get() const {return (*m_ptr)[ndx];}
    virtual void set(const S& p) const {(*m_ptr)[ndx] = p;}
    virtual void next() {ndx++;}
    virtual void prev() {ndx--;}
    virtual ~NDimArrayVectorIterator() {}
  };

  template <typename S>
  class ConstNDimArrayVectorIterator : public ConstVecIterator<S> {
    const NDimArray<S> *m_ptr;
    index_t ndx;
  public:
    ConstNDimArrayVectorIterator(const NDimArray<S> *ptr) {m_ptr = ptr; ndx = 1;}
    virtual bool isValid() const {return ((ndx>0) && (ndx <= m_ptr->dimensions().count()));}
    virtual const T& get() const {return (*m_ptr)[ndx];}
    virtual void next() {ndx++;}
    virtual void prev() {ndx--;}
    virtual ~ConstNDimArrayVectorIterator() {}
  };
  

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
  NDimIterator<T>* getNDimIterator(int dim) {
    return new NDimArrayNDimIterator<T>(this, dim);
  }
  ConstNDimIterator<T>* getConstNDimIterator(int dim) const {
    return new ConstNDimArrayNDimIterator<T>(this, dim);
  }
  VecIterator<T>* getVecIterator() {
    return new NDimArrayVectorIterator<T>(this);
  }
  ConstVecIterator<T>* getConstVecIterator() const {
    return new ConstNDimArrayVectorIterator<T>(this);
  }
  inline void resize(const NTuple& pos) {
    NDimArray<T> retval(pos);
    ConstNDimArrayNDimIterator<T> *source = 
      dynamic_cast<ConstNDimArrayNDimIterator<T>* >(getConstNDimIterator(0));
    NDimArrayNDimIterator<T> *dest = 
      dynamic_cast<NDimArrayNDimIterator<T>* >(retval.getNDimIterator(0));
    index_t line_size = qMin(source->size(),dest->size());
    while (source->isValid() && dest->isValid()) {
      for (int i=0;i<line_size;i++) {
	dest->set(source->get());
	source->next(); dest->next();
      }
      source->nextSlice();
      dest->nextSlice();
    }
    m_data = retval.m_data;
    m_dims = retval.m_dims;
    delete source; 
    delete dest;
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

#endif
