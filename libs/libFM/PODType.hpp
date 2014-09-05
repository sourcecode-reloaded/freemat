#ifndef __PODType__
#define __PODType__

#include "Type.hpp"
#include "Object.hpp"
#include "FixedPool.hpp"
#include "Complex.hpp"
#include "ArrayFormatInfo.hpp"
#include "Transpose.hpp"
#include "Hermitian.hpp"
#include <iostream>

// Manages N-dimensional arrays of blocks of opaque data - 
// requires a real type object to provide the type interface
// to the data.

namespace FM
{
  struct BaseTypes;
  
  const size_t min_capacity = 100;

  template <class T, bool _objectType>
  class PODType : public Type {
    FixedPool<ObjectBase*> *pool;
    FMString _name;
  public:
    PODType(BaseTypes* base, const FMString &name) :
      _name(name) {
      _base = base;
      pool = new FixedPool<ObjectBase*>(1000);
    }
    virtual ~PODType() {
      FixedPool<ObjectBase*> *q = pool;
      pool = 0;
      delete q;
    }
    virtual Type* typeInstance() = 0;
    virtual const FMString& name() const {return _name;}
    virtual Data* duplicateData(const ObjectBase *p, dim_t &reserve) const {
      Data *q = new Data;
      q->refcnt = 1;
      dim_t elem_count = p->dims.elementCount();
      if ((p->flags & OBJECT_COMPLEX_FLAG) != 0) elem_count *= 2;
      reserve = std::max<size_t>(elem_count*2,min_capacity);
      if (!_objectType)
	{
	  q->ptr = calloc(reserve,sizeof(T));
	  const T *pd = static_cast<const T*>(p->data->ptr) + p->offset;
	  memcpy(q->ptr,pd,elem_count*sizeof(T));
	} 
      else
	{
	  T* Tptr = new T[reserve];
	  const T *pd = static_cast<const T*>(p->data->ptr) + p->offset;
	  for (dim_t i=0;i<elem_count;i++)
	    Tptr[i] = pd[i];
	  q->ptr = Tptr;
	}
      return q;
    }
    virtual void destroyObject(ObjectBase* p) {
      if (pool && (p->dims.elementCount() <= min_capacity) && (p->data->refcnt == 1) && pool->push(p)) 
	{
	  if (_objectType)
	    {
	      T* Tptr = static_cast<T*>(p->data->ptr);
	      for (dim_t i=0;i<p->dims.elementCount();i++)
                Tptr[i] = T();
	    }
	  return;
	}
      if (--p->data->refcnt == 0) {
	if (!_objectType)
	  free(p->data->ptr);
	else
	  delete [] static_cast<T *>(p->data->ptr);
	delete p->data;
      }
      delete p;
    }
    ObjectBase * makeObjectBaseOfCapacity(dim_t capacity) {
      Data *q = new Data;
      q->refcnt = 0;
      if (!_objectType)
	q->ptr = calloc(capacity,sizeof(T));
      else
	q->ptr = new T[capacity];
      ObjectBase *p = new ObjectBase(q,this,0,Tuple(),0,capacity);
      p->refcnt = 0;
      return p;
    }
    void fillPool() {
      std::cout << "FILL!\n";
      while (!pool->isFull()) {
	pool->push(makeObjectBaseOfCapacity(min_capacity));
      }
    }
    ObjectBase* getObjectBase(dim_t capacity) {
      if (capacity <= min_capacity)
	{
	  if (pool->isEmpty())
	    fillPool();
	  return pool->pop();
	}
      return makeObjectBaseOfCapacity(capacity);
    }
    Object zeroArrayOfSize(const Tuple & dims, bool isComplex) {
      dim_t elementCount = dims.elementCount();
      if ((elementCount == 1) && !isComplex) return zeroScalar();
      if (isComplex) elementCount *= 2;
      dim_t capacity = (2*elementCount < min_capacity) ? min_capacity : 2*elementCount;
      ObjectBase *p = makeObjectBaseOfCapacity(capacity);
      p->dims = dims;
      if (isComplex) p->flags = OBJECT_COMPLEX_FLAG;
      return Object(p);    
    }
    Object makeMatrix(dim_t rows, dim_t cols, bool isComplex = false) {
      if (rows == 1 && cols == 1 && !isComplex)
	return zeroScalar();
      dim_t elementCount = rows*cols;
      if (isComplex) elementCount *= 2;
      dim_t capacity = (2*elementCount < min_capacity) ? min_capacity : 2*elementCount;
      ObjectBase *p = makeObjectBaseOfCapacity(capacity);
      p->dims.setMatrixSize(rows,cols);
      if (isComplex) p->flags = OBJECT_COMPLEX_FLAG;
      return Object(p);
    }
    Object zeroScalar() {
      ObjectBase* p = getObjectBase(min_capacity);
      p->dims.setScalar();
      static_cast<T*>(p->data->ptr)[0] = T();
      return Object(p);
    }
    Object empty() {
      ObjectBase* p = getObjectBase(min_capacity);
      p->dims.setEmpty();
      p->flags = 0;
      return Object(p);
    }
    Object makeScalar(const T& data) {
      ObjectBase* p = getObjectBase(min_capacity);
      p->dims.setScalar();
      static_cast<T*>(p->data->ptr)[0] = data;
      return Object(p);
    }
    Object makeComplex(const T& real, const T& imag) {
      Object p = makeMatrix(1,1,true);
      T* p_data = readWriteData(p);
      p_data[0] = real;
      p_data[1] = imag;
      return p;
    }
    T scalarValue(const Object &a) {
      assert(a.isScalar());
      return static_cast<const T*>(a.d->data->ptr)[0];
    }
    FMString describe(const Object &a) {
      if (a.isScalar())	
	return Stringify(scalarValue(a));
      return(a.dims().toString() + " " + this->name() + " array");
    }
    bool equals(const Object &a, const Object &b) {
      if (a.type()->code() != b.type()->code()) return false;
      if (!(a.dims() == b.dims())) return false;
      size_t byte_count = a.dims().elementCount()*sizeof(T);
      if ((a.flags() & OBJECT_COMPLEX_FLAG) ^ (b.flags() & OBJECT_COMPLEX_FLAG)) return false;
      if (a.flags() & OBJECT_COMPLEX_FLAG) byte_count *= 2;
      if (!_objectType)
	return (memcmp(readOnlyData(a),readOnlyData(b),byte_count) == 0);
      const T* ap = this->readOnlyData(a);
      const T* bp = this->readOnlyData(b);
      for (dim_t i=0;i<a.dims().elementCount();i++)
	if (!(ap[i] == bp[i])) return false;
      return true;
    }
    inline const T* readOnlyData(const Object &p) const {
      return static_cast<const T *>(static_cast<const T *>(p.d->data->ptr) + p.d->offset);
    }
    inline T* readWriteData(Object &p) const {
      p.detach();
      return static_cast<T*>(p.d->data->ptr);
    }
    inline const Complex<T>* readOnlyDataComplex(const Object &p) const {
      return reinterpret_cast<const Complex<T>*>(readOnlyData(p));
    }
    inline Complex<T>* readWriteDataComplex(Object &p) const {
      return reinterpret_cast<Complex<T>*>(readWriteData(p));
    }
    void print(const Object &a, TermIF &io);
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &a, ArrayFormatInfo &format) = 0;
    virtual void printElement(const Object &a, TermIF &io, const ArrayFormatInfo &format, ndx_t offset) = 0;
    void printSheet(const Object &a, TermIF &io, const ArrayFormatInfo &format, ndx_t offset);
    Object sliceColumn(Object &p, ndx_t col) {
      // Check!
      ObjectBase *q = new ObjectBase(p.d->data,p.d->type,
				     (col-1)*p.d->dims.rows(),
				     Tuple(p.d->dims.rows(),1),
				     p.d->flags,
				     p.d->dims.rows());
      q->refcnt = 0;
      return Object(q);
    }
    Object getRowIndexMode(const Object &a, const Object &b);
    virtual Object getParens(const Object &a, const Object &b);
    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual void resize(Object &a, const Tuple &newsize);
    virtual Object NCat(const Object &p, int dimension);
    virtual Object Transpose(const Object &a) {return MatrixTranspose<T,_objectType>(a);}
    virtual Object Hermitian(const Object &a) {
      if (_objectType) 
	return Transpose(a);
      else
	return MatrixHermitian<T>(a);
    }
  };

}

#endif
