#ifndef __FMObject4_hpp__
#define __FMObject4_hpp__

#include "FMPool.hpp"


// What if the type objects are reponsible for both creation
// of the object, _and_ for manipulation of the object itself.
// This was originally difficult because the notion of an n-dimensional
// array didn't seem common to all objects.
//
// It's probably a good idea not to revert back to that model.
// Everything is an array.  But manipulation of the data should
// be left to the type.

typedef unsigned ref_t;
typedef int64_t dim_t;

// The ArrayBase is really a bare, reference counted data chunk.
// But unlike most reference counted objects, it doesn't know
// anything about the type of the data block it has, so it cannot
// be a true object.
struct FMData {
  ref_t refcnt;
  void *ptr;
};

class FMType {				       
public:
  virtual void destroyData(const FMObject4Base &p) = 0;
  virtual FMData* duplicateData(const FMObject4Base &p) = 0;
};

class FMObject4Base {
private:
  FMObject4Base& operator=(const FMObject4Base& t);
  //! The pointer to the actual data.  Must not be NULL.
  FMData *_data;
  //! Pointer to the base metatype object that contains the 
  //method pointers for the object of interest.
  FMType *_type;
  //! Reference count.  The FMObject is reference counted,
  //but also contains a reference counting pointer to the
  //underlying data.  This dual level of reference counting
  //seems inelegant, but is required by the need for slicing.
  //Slicing means that one object has an internal (read-only)
  //view into another objects data.
  ref_t refcnt;
  //! Stubbornly refusing to allow arbitrary dimensions of
  //data.  But it can be changed easily.
  dim_t dims[MAXDIMS];
  //! The number of dimensions actually used.  Performance
  //tests indicate its better to know than to assume its
  //just MAXDIMS
  unsigned ndims;
  //! The number of elements stored in the array. Must be 
  //kept up to date
  size_t length;
  //! The offset into the data vector.  This is 
  //used for slicing.
  size_t offset;
  //! The pool that this object belongs to.  May be
  //NULL if the object doesn't come from a pool but
  //was created on the heap.
  FMPool<FMObject4Base> *pool;
public:
  FMObject4Base() {};  
  ~FMObject4Base() {
    if (--_data->refcnt == 0)
      _type->destroyData(this);
  }
  FMObject4Base(const FMObject4Base& t) {
    _type = t._type;
    _refcnt = 0;
    for (unsigned i=0;i<_ndims;i++) _dims[i] = t._dims[i];
    _ndims = t.ndims;
    _length = t._length;
    _offset = t._offset;
    _data = t._data;
    _data->refcnt++;
    _pool = t._pool;
  }
  friend class FMObject4;
};


class FMObject4
{
  FMObject4Base *d;
public:
  inline FMObject4() : d(0) {}
  inline FMObject4(const FMObject4 &copy) : d(copy.d) {
    if (d) d->refcnt++;
  }
  inline ~FMObject4() {
    if (d && ((--d->refcnt) == 0))
      {
	if (d->pool)
	  d->pool->push(d);
	else
	  delete d;
      }
  }
  FMObject4& operator=(const FMObject4& copy) {
    if (this == &copy) return *this;
    if (d && ((--d->refcnt) == 0))
      {
	if (d->pool)
	  d->pool->push(d);
	else
	  delete d;
      }
    d = copy.d;
    d->refcnt++;
  }
};


template <class T>
class FMPODArrayType : public FMType {
public:
  virtual void destroyData(const FMObject4Base &p) {
    FMData *q = p.dataPtr();
    free(q->ptr);
    delete q;
  }
  virtual FMData* duplicateData(const FMObject4Base &p) {
    FMData *q = new FMData;
    q->refcnt = 0;
    q->ptr = calloc(p.length(),sizeof(T));
    memcpy(q->ptr,p.dataPtr()->ptr,p.length()*sizeof(T));
    return q;
  }
  FMObject4 makeScalar(const T & val) {
    
  }
};




#endif
