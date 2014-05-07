#ifndef __PODType__
#define __PODType__

#include "Type.hpp"
#include "Object.hpp"
#include "FixedPool.hpp"
#include <iostream>

// Manages N-dimensional arrays of blocks of opaque data - 
// requires a real type object to provide the type interface
// to the data.
//
// There is a small (<2%) penalty associated with elsize
// not being available at compile time.

namespace FM
{
  class BaseTypes;

  template <class T>
  class PODType : public Type {
    FixedPool<ObjectBase*> *pool;
    FMString _name;
    bool _objectType;
  public:
    PODType(BaseTypes* base, const FMString &name, bool objectType = false) : 
      _name(name), _objectType(objectType) {
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
    virtual Data* duplicateData(const ObjectBase *p) const {
      Data *q = new Data;
      q->refcnt = 1;
      dim_t elem_count = p->dims.elementCount();
      if ((p->flags & OBJECT_COMPLEX_FLAG) != 0) elem_count *= 2;
      if (!_objectType)
	{
	  q->ptr = calloc(elem_count,sizeof(T));
	  const T *pd = static_cast<const T*>(p->data->ptr) + p->offset;
	  memcpy(q->ptr,pd,elem_count*sizeof(T));
	} 
      else
	{
	  T* Tptr = new T[elem_count];
	  const T *pd = static_cast<const T*>(p->data->ptr) + p->offset;
	  for (dim_t i=0;i<elem_count;i++)
	    Tptr[i] = pd[i];
	  q->ptr = Tptr;
	}
      return q;
    }
    virtual void destroyObject(ObjectBase* p) {
      if (pool && (p->dims.elementCount() == 1) && (p->data->refcnt == 1) && pool->push(p)) return;
      if (--p->data->refcnt == 0) {
	if (!_objectType)
	  free(p->data->ptr);
	else
	  delete [] static_cast<T *>(p->data->ptr);
	delete p->data;
      }
      delete p;
    }
    Object zeroArrayOfSize(const Tuple & dims, bool isComplex) {
      dim_t elementCount = dims.elementCount();
      if ((elementCount == 1) && !isComplex) return zeroScalar();
      Data *q = new Data;
      q->refcnt = 0;
      if (isComplex) elementCount *= 2;
      if (!_objectType)
	q->ptr = calloc(elementCount,sizeof(T));
      else
	q->ptr = new T[elementCount];
      ObjectBase *p = new ObjectBase(q);
      p->type = typeInstance();
      p->refcnt = 0;
      p->dims = dims;
      p->offset = 0;
      if (isComplex) 
	p->flags = OBJECT_COMPLEX_FLAG;
      return Object(p);    
    }
    Object makeMatrix(dim_t rows, dim_t cols, bool isComplex = false) {
      if (rows == 1 && cols == 1 && !isComplex)
	return zeroScalar();
      Data *q = new Data;
      q->refcnt = 0;
      dim_t elementCount = rows*cols;
      if (isComplex) elementCount *= 2;
      if (!_objectType)
	q->ptr = calloc(elementCount,sizeof(T));
      else
	q->ptr = new T[elementCount];
      ObjectBase *p = new ObjectBase(q);
      p->type = typeInstance();
      p->refcnt = 0;
      p->dims.setRows(rows);
      p->dims.setCols(cols);
      p->offset = 0;
      return Object(p);
    }
    ObjectBase *getScalarObject() {
      if (pool->isEmpty()) {
	std::cout << "Fill\n";
	while (!pool->isFull()) {
	  Data *q = new Data;
	  q->refcnt = 0;
	  if (!_objectType)
	    q->ptr = calloc(1,sizeof(T));
	  else
	    q->ptr = new T[1];
	  ObjectBase* p = new ObjectBase(q);
	  p->type = this;
	  p->refcnt = 0;
	  p->dims.setRows(1);
	  p->dims.setCols(1);
	  p->offset = 0;
	  p->flags = OBJECT_SCALAR_FLAG;
	  pool->push(p);
	}
      }
      return pool->pop();
    }
    Object zeroScalar() {
      ObjectBase* p = getScalarObject();
      static_cast<T*>(p->data->ptr)[0] = T();
      return Object(p);
    }
    Object makeScalar(const T& data) {
      ObjectBase* p = getScalarObject();
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
    Object sliceColumn(Object &p, ndx_t col) {
      // Check!
      ObjectBase *q = new ObjectBase(p.d->data);
      q->type = p.d->type;
      q->refcnt = 0;
      q->dims.setRows(p.d->dims.rows());
      q->dims.setCols(1);
      q->offset = (col-1)*p.d->dims.rows();
      return Object(q);
    }
    Object getRowIndexMode(const Object &a, const Object &b);
    virtual Object getParens(const Object &a, const Object &b);
    virtual Object getField(const Object &a, const Object &b) {
      throw Exception("Field indexing not legal for objects of type " + this->name());
    }
  };

}

#endif
