#ifndef __FMObject2_hpp__
#define __FMObject2_hpp__

#include "FMRefPtr.hpp"
#include "FMPool.hpp"

// Specialize this so that duplicate & release are handled by the parent class.
class FMArrayBase {
public:
  unsigned _refcnt;
  void *_data;
};

//Test harness should be calling duplicate somewhere...
class FMObject2Base {
private:
  FMObject2Base(const FMObject2Base& t);
  FMObject2Base& operator=(const FMObject2Base& t);
public:
  FMType *_type;
  unsigned _refcnt;
  NTuple _dims;
  unsigned _count;
  unsigned _elsize;
  unsigned _offset;
  FMArrayBase *_data;
  FMPool<FMObject2Base> *_pool;
  FMObject2Base()
  {
  }
  static void release(FMObject2Base *p) {
    if (p->_pool)
      p->_pool->push(p);
    else
      {
	if ((p->_data) && (--p->_data->_refcnt == 0))
	  free(p->_data);
	delete p;
      }
  }
  static FMObject2Base* duplicate(FMObject2Base *p) {
    FMObject2Base *q = new FMObject2Base;
    q->_type = p->_type;
    q->_refcnt = 0;
    q->_dims = p->_dims;
    q->_offset = p->_offset;
    q->_data = p->_data;
    if (q->_data) ++q->_data->_refcnt;
    return q;
  }
};

// Doesn't work in this case.  
typedef FMRefPtrCOW<FMObject2Base> FMObject2;

FMObject2Base* scalarMaker(FMPool<FMObject2Base> *pool)
{  
  FMObject2Base *p = new FMObject2Base;
  p->_type = 0;
  p->_refcnt = 0;
  p->_dims[0] = 1;
  p->_dims[1] = 1;
  p->_count = 1;
  p->_elsize = sizeof(double);
  p->_offset = 0;
  p->_pool = pool;
  FMArrayBase *q = new FMArrayBase;
  q->_refcnt = 1;
  q->_data = calloc(1,sizeof(double));
  ((double*)(q->_data))[0] = 0;
  p->_data = q;
  return p;
}

FMObject2 MakeScalar2(double t)
{
  static FMPool<FMObject2Base> *scalars = 0;
  if (!scalars)
    scalars = new FMPool<FMObject2Base>(1000,scalarMaker);
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
