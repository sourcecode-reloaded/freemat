#ifndef __FMObject2_hpp__
#define __FMObject2_hpp__

#include "FMRefPtr.hpp"

// Specialize this so that duplicate & release are handled by the parent class.
class FMArrayBase {
public:
  unsigned _refcnt;
  void *_data;
  int _datacount;
  int _datasize;
  static void release(FMArrayBase *q) {
    free(q->_data);
    delete q;
  }
  static FMArrayBase* duplicate(FMArrayBase *q) {
    FMArrayBase *p = new FMArrayBase;
    p->_refcnt = 0;
    p->_data = calloc(1,sizeof(double));
    ((double*)p->_data)[0] = ((double*)q->_data)[0];
    return p;
  }
};

template <class T>
class FMObjectPool
{
  T** _pool;
  unsigned _fill;
  unsigned _size;
  T* (*_maker)(FMObjectPool<T>*);
public:
  FMObjectPool(unsigned cnt, T* (maker)(FMObjectPool<T>*))
  {
    _fill = 0;
    _size = cnt;
    _pool = new T*[cnt];
    _maker = maker;
  }
  T* pop() {
    if (!_fill) fill();
    return _pool[--_fill];
  }
  void push(T* ret) {
    if (_fill == _size) 
      {
	delete ret;
	return;
      }
    _pool[_fill++] = ret;
  }
  void fill() {
    while (_fill < _size)
      push(_maker(this));
  }
};


//Test harness should be calling duplicate somewhere...
class FMObject2Base {
public:
  FMType *_type;
  unsigned _refcnt;
  NTuple _dims;
  unsigned _offset;
  FMRefPtrCOW<FMArrayBase> _data;
  FMObjectPool<FMObject2Base> *_pool;
  static void release(FMObject2Base *p) {
    if (p->_pool)
      p->_pool->push(p);
    else
      delete p;
  }
};

typedef FMRefPtrCOW<FMObject2Base> FMObject2;

FMObject2Base* scalarMaker(FMObjectPool<FMObject2Base> *pool)
{  
  FMObject2Base *p = new FMObject2Base;
  p->_type = 0;
  p->_refcnt = 0;
  p->_dims[0] = 1;
  p->_dims[1] = 1;
  p->_offset = 0;
  p->_pool = pool;
  FMArrayBase *q = new FMArrayBase;
  q->_refcnt = 0;
  q->_data = calloc(1,sizeof(double));
  ((double*)(q->_data))[0] = 0;
  p->_data = FMRefPtrCOW<FMArrayBase>(q);
  return p;
}

FMObject2 MakeScalar2(double t)
{
  static FMObjectPool<FMObject2Base> *scalars = 0;
  if (!scalars)
    scalars = new FMObjectPool<FMObject2Base>(1000,scalarMaker);
  FMObject2Base *p = scalars->pop();
  ((double*)(p->_data->_data))[0] = t;
  return FMObject2(p);
  /*
  FMObject2Base *p = new FMObject2Base;
  p->_type = 0;
  p->_refcnt = 0;
  p->_dims[0] = 1;
  p->_dims[1] = 1;
  p->_offset = 0;
  FMArrayBase *q = new FMArrayBase;
  q->_refcnt = 0;
  q->_data = calloc(1,sizeof(double));
  ((double*)(q->_data))[0] = t;
  p->_data = FMRefPtrCOW<FMArrayBase>(q);
  return FMObject2(p);
  */
}

double ScalarValue(const FMObject2 &p)
{
  if (p->_type) throw "Ugh";
  return ((double*)(p->_data->_data))[0];
}

#endif
