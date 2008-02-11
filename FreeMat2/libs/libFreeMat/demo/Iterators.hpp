#ifndef __Iterators_hpp__
#define __Iterators_hpp__

#include "NTuple.hpp"

template <typename T>
class NDimIterator {
public:
  virtual void nextSlice() = 0;
  virtual bool isValid() const = 0;
  virtual const T& get() const = 0;
  virtual void set(const T&) const = 0;
  virtual void next() = 0;
  virtual void prev() = 0;
  virtual index_t size() const = 0;
  virtual ~NDimIterator() {}
};

template <typename T>
class ConstNDimIterator {
public:
  virtual void nextSlice() = 0;
  virtual bool isValid() const = 0;
  virtual const T& get() const = 0;
  virtual void next() = 0;
  virtual void prev() = 0;
  virtual index_t size() const = 0;
  virtual ~ConstNDimIterator() {}
};

template <typename T>
class VecIterator {
public:
  virtual bool isValid() const = 0;
  virtual const T& get() const = 0;
  virtual void set(const T&) const = 0;
  virtual void next() = 0;
  virtual void prev() = 0;
  virtual ~VecIterator() {}
};

template <typename T>
class ConstVecIterator {
public:
  virtual bool isValid() const = 0;
  virtual const T& get() const = 0;
  virtual void next() = 0;
  virtual void prev() = 0;
  virtual ~ConstVecIterator() {}
};

class NDimIteratorHelper {
private:
  index_t m_lineSize;
  index_t m_planeSize;
  index_t m_planeCount;
  index_t m_plane;
  index_t m_line;
public:
  NDimIteratorHelper(const NTuple &dims, int dim) {
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
  inline void advanceSlice() {
    m_line++;
    if (m_line >= m_planeSize) {
      m_line = 1;
      m_plane++;
    }
  }
  inline bool isSliceValid() const {
    return (m_plane<m_planeCount);
  }
};


#endif
