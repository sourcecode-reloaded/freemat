#ifndef __ArrayType_hpp__
#define __ArrayType_hpp__

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
  struct ThreadContext;
  
  const size_t min_capacity = 100;

  template <class T>
  class ArrayType : public Type {
    FixedPool<ObjectBase*> *pool;
    FMString _name;
    ObjectBase *_emptyBase;
    bool _haveEmpty;
    Object getParensRowMode(const Object &a, const Tuple &dims, const Object &b);
    void erase(Object &a, const Object &args);
    void erasePlanes(Object &a, const Object &mask, const Tuple &outdims, int non_colon_index);
    void eraseRowIndexMode(Object &a, const Object &ndx);
    void eraseRows(Object &a, const Object &mask, const Tuple &outdims);
    Object convertArgumentsToIndexingExpressions(const Object &args);
    void resizeSlow(Object &a, const Tuple &newsize);
  protected:
    /* To make a concrete implementation of this class, you have to provide the following */
    virtual void* allocateArray(dim_t size) const = 0;
    virtual void releaseData(T* dst, dim_t size) const = 0;
    virtual void freeData(T* ptr, dim_t size) const = 0;
    virtual T zeroElement() const = 0;
  public:
    ArrayType(ThreadContext* ctxt, const FMString &name) :
      _name(name) {
      _ctxt = ctxt;
      pool = new FixedPool<ObjectBase*>(10000);
      _haveEmpty = false;
    }
    virtual ~ArrayType() {
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
      q->ptr = allocateArray(reserve);
      const T *pd = static_cast<const T*>(p->data->ptr) + p->offset;
      T *Tptr = static_cast<T*>(q->ptr);
      for (dim_t i=0;i<elem_count;i++)
	Tptr[i] = pd[i];
      return q;
    }
    virtual inline void destroyObject(ObjectBase* p) {
      if (pool && (p->dims.elementCount() <= min_capacity) && (p->data->refcnt == 1) && pool->push(p)) 
	{
	  releaseData(static_cast<T*>(p->data->ptr),p->dims.elementCount());
	  return;
	}
      if (--p->data->refcnt == 0) {
	freeData(static_cast<T*>(p->data->ptr),p->dims.elementCount());
	delete p->data;
      }
      delete p;
    }
    // use placement new?
    ObjectBase * makeObjectBaseOfCapacity(dim_t capacity) {
      Data *q = new Data;
      q->refcnt = 0;
      q->ptr = allocateArray(capacity);
      ObjectBase *p = new ObjectBase(q,this,0,Tuple(),0,capacity);
      p->refcnt = 0;
      return p;
    }
    void fillPool() {
      //      std::cout << "FILL! " << this->name() << " \n";
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
      ObjectBase *p = getObjectBase(capacity);
      p->dims = dims;
      p->flags = 0;
      p->offset = 0;
      if (isComplex) p->flags = OBJECT_COMPLEX_FLAG;
      if (capacity < min_capacity)
	{
	  T* dp = static_cast<T*>(p->data->ptr);
	  for (dim_t i=0;i<capacity;i++)
	    dp[i] = zeroElement();
	}
      return Object(p);    
    }
    Object makeMatrix(dim_t rows, dim_t cols, bool isComplex = false) {
      if (rows == 1 && cols == 1 && !isComplex)
	return zeroScalar();
      dim_t elementCount = rows*cols;
      if (isComplex) elementCount *= 2;
      dim_t capacity = (2*elementCount < min_capacity) ? min_capacity : 2*elementCount;
      ObjectBase *p = getObjectBase(capacity);
      p->dims.setMatrixSize(rows,cols);
      p->flags = 0;
      p->offset = 0;
      if (isComplex) p->flags = OBJECT_COMPLEX_FLAG;
      if (capacity < min_capacity)
	{
	  T* dp = static_cast<T*>(p->data->ptr);
	  for (dim_t i=0;i<capacity;i++)
	    dp[i] = zeroElement();
	}
      return Object(p);
    }
    // Could move to NumericType
    Object zeroScalar() {
      ObjectBase* p = getObjectBase(min_capacity);
      p->dims.setScalar();
      p->flags = 0;
      p->offset = 0;
      static_cast<T*>(p->data->ptr)[0] = zeroElement();
      return Object(p);
    }
    ObjectBase* emptyBase() {
      if (!_haveEmpty) {
	Data *q = new Data;
	q->refcnt = 1;
	q->ptr = allocateArray(1);
	_emptyBase = new ObjectBase(q,this,0,Tuple(),0,1);
	_emptyBase->dims.setEmpty();
	_emptyBase->flags = 0;
	_emptyBase->offset = 0;
	_emptyBase->refcnt = 1;
	_haveEmpty = true;
	std::cout << "Made empty for type : " << this->name() << "\n";
      }
      return _emptyBase;
    }
    Object empty() {
      return Object(emptyBase());
    }
    Object makeScalar(const T& data) {
      ObjectBase* p = getObjectBase(min_capacity);
      p->dims.setScalar();
      p->flags = 0;
      p->offset = 0;
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
      return readOnlyData(a)[0];
    }
    FMString describe(const Object &a) {
      return(a.dims().toString() + " " + this->name() + " array");
    }
    bool equals(const Object &a, const Object &b) {
      if (a.type()->code() != b.type()->code()) return false;
      if (!(a.dims() == b.dims())) return false;
      size_t byte_count = a.dims().elementCount()*sizeof(T);
      if ((a.flags() & OBJECT_COMPLEX_FLAG) ^ (b.flags() & OBJECT_COMPLEX_FLAG)) return false;
      if (a.flags() & OBJECT_COMPLEX_FLAG) byte_count *= 2;
      const T* ap = this->readOnlyData(a);
      const T* bp = this->readOnlyData(b);
      for (dim_t i=0;i<a.dims().elementCount();i++)
	if (!(ap[i] == bp[i])) return false;
      return true;
    }
    void promoteComplex(Object &a);
    inline const T* readOnlyData(const Object &p) const {
      // DEBUG ASSERT
      assert(p.type()->code() == this->code());
      return static_cast<const T *>(static_cast<const T *>(p.d->data->ptr) + p.d->offset);
    }
    inline T* readWriteData(Object &p) const {
      // DEBUG ASSERT
      assert(p.type()->code() == this->code());
      p.detach();
      return static_cast<T*>(p.d->data->ptr);
    }
    void print(const Object &a);
    virtual void computeArrayFormatInfo(FMFormatMode mode, const Object &a, ArrayFormatInfo &format) = 0;
    virtual void printElement(const Object &a, const ArrayFormatInfo &format, ndx_t offset) = 0;
    void printSheet(const Object &a, const ArrayFormatInfo &format, ndx_t offset);
    Object sliceColumn(const Object &p, ndx_t col) {
      // TODO - Check!
      ObjectBase *q = new ObjectBase(p.d->data,p.d->type,
				     (col-1)*p.d->dims.rows(),
				     Tuple(p.d->dims.rows(),1),
				     p.d->flags,
				     p.d->dims.rows());
      q->refcnt = 0;
      return Object(q);
    }
    Object getRowIndexMode(const Object &a, const Object &b);
    Object getSliceMode(const Object &a, Object *c, int cnt, int last_colon);
    virtual Object getParens(const Object &a, const Object &b);
    void setParensRowIndexMode(Object &a, const Object &ndx, const Object &b);
    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual void resize(Object &a, const Tuple &newsize) {
      if (a.elementCount() == 0)
	{
	  if (a.d->capacity > newsize.elementCount())
	    {
	      a.dims() = newsize;
	      return;
	    }
	  a = this->zeroArrayOfSize(newsize,a.isComplex());
	  return;
	}
      if (a.isVector() && newsize.isVector() && a.d->capacity > newsize.elementCount())
	{
	  a.dims() = newsize;
	  return;
	}
      resizeSlow(a,newsize);
    }
    virtual Object NCat(const Object &p, int dimension);
    virtual Object Transpose(const Object &a) {return MatrixTranspose<T>(a);}
  };

}



#endif
