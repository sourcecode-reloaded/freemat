#ifndef __ListType_hpp__
#define __ListType_hpp__

#include "ObjectArrayType.hpp"
#include "Object.hpp"

namespace FM
{
  struct ThreadContext;

  class ListType : public ObjectArrayType {
  public:
    ListType(ThreadContext *ctxt) : ObjectArrayType(ctxt,"list") {}
    virtual DataCode code() const {return TypeListArray;}
    virtual Type* typeInstance() {return this;}
    virtual FMString describe(const Object &a) {
      FMString ret = "<";
      const Object *ap = this->ro(a);
      for (dim_t i=0;i<a.elementCount();i++)
	{
	  ret += ap[i].description();
	  if (i < a.elementCount() - 1) ret += " ";
	}
      ret += ">";
      return ret;
    }
    Object first(const Object &a) {
      if (a.isEmpty()) throw Exception("Attempt to take first element of empty object");
      return *(this->ro(a));
    }
    Object second(const Object &a) {
      if (a.elementCount() < 2) throw Exception("Attempt to take first element of empty object");
      return (this->ro(a))[1];
    }
    Object last(const Object &a) {
      if (a.isEmpty()) throw Exception("Attempt to take last element of empty object");
      return (this->ro(a))[a.elementCount()-1];
    }
    Object fetch(const Object &a, dim_t ndx) {
      return this->ro(a)[ndx];
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
	  a.dims().setMatrixSize(a_size+1,1);
	  Object *ap = this->rw(a);
	  ap[a_size] = b;
	}
      else 
	{
	  // Resize is required.  
	  Object na = this->makeMatrix(a_size+1,1,false);
	  Object *nap = this->rw(na);
	  const Object *ap = this->ro(a);
	  for (dim_t i=0;i<a_size;i++)
	    nap[i] = ap[i];
	  nap[a_size] = b;
	  a = na;
	}
    }
    // FIXME - Make this more efficient?
    void merge(Object &a, const Object &b) {
      const Object *bp = this->ro(b);
      for (dim_t i=0;i<b.elementCount();i++)
	this->push(a,bp[i]);
    }
    Type* anyElementsWithDataCode(const Object &a, DataCode t) {
      const Object *bp = this->ro(a);
      for (dim_t i=0;i<a.elementCount();i++)
	if (bp[i].type()->code() == t) return bp[i].type();
      return NULL;
    }
    inline ndx_t indexOf(const Object &a, const Object &b) {
      const Object *ap = this->ro(a);
      for (dim_t i=0;i<a.elementCount();i++)
	if (ap[i] == b) return i;
      return -1;
    }
    inline bool has(const Object &a, const Object &b) {
      return (this->indexOf(a,b) != -1);
    }
    void computeArrayFormatInfo(FMFormatMode, const Object &a, ArrayFormatInfo &format) {
      format.width = 80;
    }
    void printElement(const Object &a, const ArrayFormatInfo &info, ndx_t ndx);
  };
};



#endif
