#ifndef __ListType_hpp__
#define __ListType_hpp__

#include "PODType.hpp"
#include "Object.hpp"

namespace FM
{
  struct ThreadContext;

  class ListType : public PODType<Object,true> {
  public:
    ListType(ThreadContext *ctxt) : PODType<Object,true>(ctxt,"list") {}
    virtual DataCode code() const {return TypeListArray;}
    virtual Type* typeInstance() {return this;}
    virtual FMString describe(const Object &a) {
      FMString ret = "[";
      const Object *ap = this->readOnlyData(a);
      for (dim_t i=0;i<a.elementCount();i++)
	{
	  ret += ap[i].description();
	  if (i < a.elementCount() - 1) ret += " ";
	}
      ret += "]";
      return ret;
    }
    Object first(const Object &a) {
      if (a.isEmpty()) throw Exception("Attempt to take first element of empty object");
      return *(this->readOnlyData(a));
    }
    Object second(const Object &a) {
      if (a.elementCount() < 2) throw Exception("Attempt to take first element of empty object");
      return (this->readOnlyData(a))[1];
    }
    Object fetch(const Object &a, dim_t ndx) {
      return this->readOnlyData(a)[ndx];
    }
    ndx_t indexOf(const Object &a, const Object &b) {
      dim_t a_size = a.elementCount();
      const Object *ap = this->readOnlyData(a);
      for (dim_t i=0;i<a_size;i++)
	if (ap[i] == b) return i;
      return -1;
    }
    void pop(Object &a) {
      a.detach();
      Tuple & adim = a.dims();
      adim.setRows(adim.rows()-1);
      a.d->offset++;
    }
    void push(Object &a, const Object &b) {
      dim_t a_size = a.elementCount();
      if (a.d->capacity > (a_size+1))
	{
	  a.detach();
	  // Resize is not required. 
	  a.dims().setMatrixSize(a_size+1,1);
	  Object *ap = static_cast<Object*>(a.d->data->ptr);
	  ap[a_size] = b;
	}
      else 
	{
	  // Resize is required.  
	  Object na = this->makeMatrix(a_size+1,1,false);
	  Object *nap = this->readWriteData(na);
	  const Object *ap = this->readOnlyData(a);
	  for (dim_t i=0;i<a_size;i++)
	    nap[i] = ap[i];
	  nap[a_size] = b;
	  a = na;
	}
    }
    // FIXME - Make this more efficient?
    void merge(Object &a, const Object &b) {
      const Object *bp = this->readOnlyData(b);
      for (dim_t i=0;i<b.elementCount();i++)
	this->push(a,bp[i]);
    }
    Type* anyElementsWithDataCode(const Object &a, DataCode t) {
      const Object *bp = this->readOnlyData(a);
      for (dim_t i=0;i<a.elementCount();i++)
	if (bp[i].type()->code() == t) return bp[i].type();
      return NULL;
    }
    void computeArrayFormatInfo(FMFormatMode, const Object &a, ArrayFormatInfo &format) {
      format.width = 80;
    }
    void printElement(const Object &a, const ArrayFormatInfo &info, ndx_t ndx);
  };
};



#endif
