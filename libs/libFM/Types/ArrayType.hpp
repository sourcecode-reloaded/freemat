#ifndef __ArrayType_hpp__
#define __ArrayType_hpp__

#include "Type.hpp"
#include "Object.hpp"
#include "FixedPool.hpp"
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
  
  const ndx_t min_capacity = 100;

  template <class T>
  class ArrayType : public Type {
    FixedPool<ObjectBase*> *pool;
    FMString _name;
    ObjectBase *_emptyBase;
    bool _haveEmpty;
    Object getParensRowMode(const Object &a, const Tuple &dims, const Object &b);
    void erase(Object &a, const Object &args);
    void erasePlanes(Object &a, const Object &mask, const Tuple &outdims, ndx_t non_colon_index);
    void eraseRowIndexMode(Object &a, const Object &ndx);
    void eraseRows(Object &a, const Object &mask, const Tuple &outdims);
    Object convertArgumentsToIndexingExpressions(const Object &args);
    void resizeSlow(Object &a, const Tuple &newsize);
  protected:
    /* To make a concrete implementation of this class, you have to provide the following */
    virtual void* allocateArray(ndx_t size) const = 0;
    virtual void releaseData(T* dst, ndx_t size) const = 0;
    virtual void freeData(T* ptr, ndx_t size) const = 0;
    virtual T zeroElement() const = 0;
  public:
    ArrayType(ThreadContext* ctxt, const FMString &name) : Type(ctxt), _name(name) {
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
    virtual Data* duplicateData(const ObjectBase *p, ndx_t &reserve) const {
      Data *q = new Data;
      q->refcnt = 1;
      ndx_t elem_count = p->dims.count();
      reserve = std::max<ndx_t>(elem_count*2,min_capacity);
      q->ptr = allocateArray(reserve);
      const T *pd = static_cast<const T*>(p->data->ptr) + p->offset;
      T *Tptr = static_cast<T*>(q->ptr);
      for (ndx_t i=0;i<elem_count;i++)
	Tptr[i] = pd[i];
      return q;
    }
    virtual inline void destroyObject(ObjectBase* p) {
      if (pool && (p->dims.count() <= min_capacity) && (p->data->refcnt == 1) && pool->push(p)) 
	{
	  releaseData(static_cast<T*>(p->data->ptr),p->dims.count());
	  return;
	}
      if (--p->data->refcnt == 0) {
	freeData(static_cast<T*>(p->data->ptr),p->dims.count());
	delete p->data;
      }
      delete p;
    }
    // use placement new?
    ObjectBase * makeObjectBaseOfCapacity(ndx_t capacity) {
      Data *q = new Data;
      q->refcnt = 0;
      q->ptr = allocateArray(capacity);
      ObjectBase *p = new ObjectBase(q,this,0,Tuple(),0,capacity);
      p->refcnt = 0;
      return p;
    }
    void fillPool() {
      //      std::cout << "FILL! " << this->name() << " \n";
      std::cout << "FILL!\n";
      while (!pool->isFull()) {
	pool->push(makeObjectBaseOfCapacity(min_capacity));
      }
    }
    ObjectBase* getObjectBase(ndx_t capacity) {
      if (capacity <= min_capacity)
	{
	  if (pool->isEmpty())
	    fillPool();
	  return pool->pop();
	}
      return makeObjectBaseOfCapacity(capacity);
    }
    Object import(const Object &target) {
      // Import the object from the foreign context...
      ArrayType* them = target.asType<ArrayType>();
      if (this == them) return target;
      Object ret = this->zeroArrayOfSize(target.dims());
      T* op = this->rw(ret);
      const T* ip = them->ro(target);
      ndx_t elem_count = ret.count();
      for (ndx_t i=0;i<elem_count;i++)
	op[i] = ip[i];
      return ret;
    }
    Object zeroArrayOfSize(const Tuple & dims) {
      ndx_t count = dims.count();
      if (count == 1) return zeroScalar();
      ndx_t capacity = (2*count < min_capacity) ? min_capacity : 2*count;
      ObjectBase *p = getObjectBase(capacity);
      p->dims = dims;
      p->flags = 0;
      p->offset = 0;
      if (capacity < min_capacity)
	{
	  T* dp = static_cast<T*>(p->data->ptr);
	  for (ndx_t i=0;i<capacity;i++)
	    dp[i] = zeroElement();
	}
      return Object(p);    
    }
    Object makeMatrix(ndx_t rows, ndx_t cols) {
      if (rows == 1 && cols == 1)
	return zeroScalar();
      ndx_t count = rows*cols;
      ndx_t capacity = (2*count < min_capacity) ? min_capacity : 2*count;
      ObjectBase *p = getObjectBase(capacity);
      p->dims.setMatrixSize(rows,cols);
      p->flags = 0;
      p->offset = 0;
      if (capacity < min_capacity)
	{
	  T* dp = static_cast<T*>(p->data->ptr);
	  for (ndx_t i=0;i<capacity;i++)
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
    T scalarValue(const Object &a) {
      assert(a.isScalar());
      return ro(a)[0];
    }
    FMString brief(const Object &a) {
      return(a.dims().toString() + " " + this->name() + " array");      
    }
    FMString describe(const Object &a) {
      return(a.dims().toString() + " " + this->name() + " array");
    }
    bool equals(const Object &a, const Object &b) {
      if (a.type()->code() != b.type()->code()) return false;
      if (!(a.dims() == b.dims())) return false;
      ndx_t element_count = a.dims().count();
      const T* ap = this->ro(a);
      const T* bp = this->ro(b);
      for (ndx_t i=0;i<element_count;i++)
	if (!(ap[i] == bp[i])) return false;
      return true;
    }
    inline const T* null() const {
      return (const T*)(nullptr);
    }
    inline T* null() {
      return (T*)(nullptr);
    }
    inline const T* ro(const Object &p) const {
      // DEBUG ASSERT
      assert(p.type()->code() == this->code());
      return static_cast<const T *>(static_cast<const T *>(p.d->data->ptr) + p.d->offset);
    }
    inline T* rw(Object &p) const {
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
				     size_t(col*p.d->dims.rows()),
				     Tuple(p.d->dims.rows(),1),
				     p.d->flags,
				     p.d->dims.rows());
      q->refcnt = 0;
      return Object(q);
    }
    void setColumn(Object &ret, ndx_t column, const Object &value) {
      // Validate assumptions
      assert(ret.rows() == value.rows());
      assert(value.cols() == 1);
      assert(column < ret.cols());
      assert(ret.typeCode() == value.typeCode());
      ndx_t rowsize = ret.rows();
      T* rp = this->rw(ret);
      const T* vp = this->ro(value);
      for (auto i=0;i<rowsize;i++)
	rp[i+column*rowsize] = vp[i];
    }
    //    Object getRowIndexMode(const Object &a, const Object &b);
    Object getSliceMode(const Object &a, Object *c, ndx_t cnt, ndx_t last_colon);
    virtual Object getParens(const Object &a, const Object &b);
    void setParensRowIndexMode(Object &a, const Object &ndx, const Object &b);
    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual void resize(Object &a, const Tuple &newsize) {
      if (a.count() == 0)
	{
	  if (a.d->capacity > newsize.count())
	    {
	      a.setDims(newsize);
	      return;
	    }
	  a = this->zeroArrayOfSize(newsize);
	  return;
	}
      if (a.isVector() && newsize.isVector() && a.d->capacity > newsize.count())
	{
	  a.setDims(newsize);
	  return;
	}
      resizeSlow(a,newsize);
    }
    virtual Object NCat(const Object &p, ndx_t dimension);
    virtual Object Transpose(const Object &a) {return MatrixTranspose<T>(a);}
  };

}



#endif
