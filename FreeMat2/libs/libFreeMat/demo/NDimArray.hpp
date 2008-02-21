#ifndef __NDimArray_hpp__
#define __NDimArray_hpp__

#include "BaseArray.hpp"
#include "DenseArray.hpp"
#include "PermArray.hpp"
#include "ShapeArray.hpp"
#include "SliceArray.hpp"
#include "SparseArray.hpp"
#include "Iterators.hpp"

template <typename T>
class NDimArray : public BaseArray<T> {
public:
  enum Type {
    Invalid = 0,
    Dense = 1,
    Perm = 2,
    Slice = 3,
    Shape = 4,
    Sparse = 5
  };
  // Creates a dense NDimArray
  NDimArray(const NTuple &dims) {
    m_p = new DenseArray<T>(dims);
    m_type = Dense;
  }
  // Creates a permuted NDimArray
  NDimArray(NDimArray<T> &ref, const NPerm& perm) {
    m_p = new PermArray<T>(ref,perm);
    m_type = Perm;
  }
  NDimArray(NDimArray<T> &ref, const RangeSet& slice) {
    m_p = new SliceArray<T>(ref,slice);
    m_type = Slice;
  }
  NDimArray(NDimArray<T> &ref, const NTuple& dims) {
    m_p = new ShapeArray<T>(ref,dims);
    m_type = Shape;
  }
  //  NDimArray(SparseMatrix<T>* p) : m_p(p), m_type(Sparse) {}
  NDimArray(const NDimArray<T> &ref) {
    m_type = ref.m_type;
    switch (m_type) {
    case Invalid:
    case Dense:
      m_p = new DenseArray<T>(*(reinterpret_cast<DenseArray<T>*>(ref.m_p)));
      return;
    case Perm:
      m_p = new PermArray<T>(*(reinterpret_cast<PermArray<T>*>(ref.m_p)));
      return;
    case Slice:
      m_p = new SliceArray<T>(*(reinterpret_cast<SliceArray<T>*>(ref.m_p)));
      return;
    case Shape:
      m_p = new ShapeArray<T>(*(reinterpret_cast<ShapeArray<T>*>(ref.m_p)));
      return;
    case Sparse:
      m_p = new SparseMatrix<T>(*(reinterpret_cast<SparseMatrix<T>*>(ref.m_p)));
      return;
    }
  }
  virtual ~NDimArray() {
    switch (m_type) {
    case Invalid:
    case Dense:
      delete reinterpret_cast<DenseArray<T>*>(m_p);
      return;
    case Perm:
      delete reinterpret_cast<PermArray<T>*>(m_p);
      return;
    case Slice:
      delete reinterpret_cast<SliceArray<T>*>(m_p);
      return;
    case Shape:
      delete reinterpret_cast<ShapeArray<T>*>(m_p);
      return;
    case Sparse:
      delete reinterpret_cast<SparseMatrix<T>*>(m_p);
      return;
    }
  }
  // The BaseArray interface
  virtual const NTuple dimensions() const {
    if (m_p) 
      return reinterpret_cast<BaseArray<T>*>(m_p)->dimensions();
    throw Exception("Uninitialized NDimArray");
  }
  virtual const T operator[](const NTuple& pos) const {
    if (m_p)
      return reinterpret_cast<const BaseArray<T>*>(m_p)->operator[](pos);
    throw Exception("Uninitialized NDimArray");
  }
  virtual T& operator[](const NTuple& pos) {
    if (m_p)
      return reinterpret_cast<BaseArray<T>*>(m_p)->operator[](pos);
    throw Exception("Uninitialized NDimArray");
  }
  virtual const T operator[](index_t pos) const {
    if (m_p)
      return reinterpret_cast<const BaseArray<T>*>(m_p)->operator[](pos);
    throw Exception("Uninitialized NDimArray");
  }
  virtual T& operator[](index_t pos) {
    if (m_p)
      return reinterpret_cast<BaseArray<T>*>(m_p)->operator[](pos);
    throw Exception("Uninitialized NDimArray");
  }
  virtual NDimIterator<T>* getNDimIterator(int dim) {
    if (m_p)
      return reinterpret_cast<BaseArray<T>*>(m_p)->getNDimIterator(dim);
    throw Exception("Uninitialized NDimArray");
  }
  virtual ConstNDimIterator<T>* getConstNDimIterator(int dim) {
    if (m_p)
      return reinterpret_cast<BaseArray<T>*>(m_p)->getConstNDimIterator(dim);
    throw Exception("Uninitialized NDimArray");
  }
  Type type() {return m_type;}
private:
  void *m_p;
  Type m_type;
};

#endif
