#ifndef __NDimSlice_hpp__
#define __NDimSlice_hpp__

#include "BaseArray.hpp"
#include "RangeSet.hpp"
#include <QString>
#include <QVector>

template <typename T>
class NDimSlice : public BaseArray<T> {
  NDimArray<T> &m_ref;
  const RangeSet &m_slices;
  NTuple m_dims;
public:

  template <typename S>
  class NDimSliceNDimIterator : public NDimIterator<S> {
    NDimSlice<S> *m_ptr;
    index_t m_ndx;
    NDimIteratorHelper m_hlp;
  public:
    NDimSliceNDimIterator(NDimSlice<S> *ptr, int dim) :
      m_ptr(ptr), m_ndx(0), m_hlp(ptr->dimensions(), dim) {}
    virtual void nextSlice() { m_hlp.advanceSlice(); m_ndx = 0;} 
    virtual bool isValid() const {return m_hlp.isSliceValid();}
    virtual const S& get() const {
      NTuple tpos;
      m_ptr->remap(m_hlp.vectorAddress(m_ndx+1),tpos);
      return (*m_ptr)[tpos];
    }
    virtual void set(const S& t) const {
      NTuple tpos;
      m_ptr->remap(m_hlp.vectorAddress(m_ndx+1),tpos);
      (*m_ptr)[tpos] = t;
    }
    virtual void next() {m_ndx++;}
    virtual void prev() {m_ndx--;}
    virtual index_t size() const {return m_hlp.lineSize();}
    ~NDimSliceNDimIterator() {}
  };

  template <typename S>
  class ConstNDimSliceNDimIterator : public ConstNDimIterator<S> {
    const NDimSlice<S> *m_ptr;
    index_t m_ndx;
    NDimIteratorHelper m_hlp;
  public:
    ConstNDimSliceNDimIterator(const NDimSlice<S> *ptr, int dim) :
      m_ptr(ptr), m_ndx(0), m_hlp(ptr->dimensions(), dim) {}
    virtual void nextSlice() { m_hlp.advanceSlice(); m_ndx = 0;} 
    virtual bool isValid() const {return m_hlp.isSliceValid();}
    virtual const S& get() const {
      NTuple tpos;
      m_ptr->remap(m_hlp.vectorAddress(m_ndx+1),tpos);
      return (*m_ptr)[tpos];
    }
    virtual void next() {m_ndx++;}
    virtual void prev() {m_ndx--;}
    virtual index_t size() const {return m_hlp.lineSize();}
    ~ConstNDimSliceNDimIterator() {}
  };

  template <typename S>
  class NDimSliceVectorIterator : public VecIterator<S> {
    NDimSlice<S> *m_ptr;
    index_t m_ndx;
  public:
    NDimSliceVectorIterator(NDimSlice<S> *ptr) {m_ptr = ptr; m_ndx = 1;}
    virtual bool isValid() const {
      return ((m_ndx>0) && (m_ndx <= m_ptr->dimensions().count()));
    }
    virtual const T& get() const {
      NTuple tpos;
      m_ptr->remap(m_ndx,tpos);
      return (*m_ptr)[tpos];
    }
    virtual void set(const S& p) const {
      NTuple tpos;
      m_ptr->remap(m_ndx,tpos);
      (*m_ptr)[tpos] = p;
    }
    virtual void next() {m_ndx++;}
    virtual void prev() {m_ndx--;}
    virtual ~NDimSliceVectorIterator() {}
  };

  template <typename S>
  class ConstNDimSliceVectorIterator : public ConstVecIterator<S> {
    const NDimSlice<S> *m_ptr;
    index_t m_ndx;
  public:
    ConstNDimSliceVectorIterator(const NDimSlice<S> *ptr) {m_ptr = ptr; m_ndx = 1;}
    virtual bool isValid() const {
      return ((m_ndx>0) && (m_ndx <= m_ptr->dimensions().count()));
    }
    virtual const T& get() const {
      NTuple tpos;
      m_ptr->remap(m_ndx,tpos);
      return (*m_ptr)[tpos];
    }
    virtual void next() {m_ndx++;}
    virtual void prev() {m_ndx--;}
    virtual ~ConstNDimSliceVectorIterator() {}
  };
  
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
  inline void remap(index_t pos, NTuple& dest) const {
    m_slices.remap(pos,dest);
  }
  NDimIterator<T>* getNDimIterator(int dim) {
    return new NDimSliceNDimIterator<T>(this, dim);
  }
  ConstNDimIterator<T>* getConstNDimIterator(int dim) const {
    return new ConstNDimSliceNDimIterator<T>(this, dim);
  }
  VecIterator<T>* getVecIterator() {
    return new NDimSliceVectorIterator<T>(this);
  }
  ConstVecIterator<T>* getConstVecIterator() const {
    return new ConstNDimSliceVectorIterator<T>(this);
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

#if 0
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

#endif
