#ifndef __ListType_hpp__
#define __ListType_hpp__

#include "ObjectArrayType.hpp"
#include "Object.hpp"
#include "DoubleType.hpp"

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
      for (ndx_t i=0;i<a.count();i++)
	{
	  ret += ap[i].description();
	  if (i < a.count() - 1) ret += " ";
	}
      ret += ">";
      return ret;
    }
    Object first(const Object &a) {
      if (a.isEmpty()) return _ctxt->_double->empty(); //throw Exception("Attempt to take first element of empty object");
      return *(this->ro(a));
    }
    Object second(const Object &a) {
      if (a.count() < 2) throw Exception("Attempt to take second element of empty object");
      return (this->ro(a))[1];
    }
    Object makePair(const Object &a, const Object &b) {
      Object ret = this->makeMatrix(2,1);
      Object *rp = this->rw(ret);
      rp[0] = a;
      rp[1] = b;
      return ret;
    }
    Object makeTriple(const Object &a, const Object &b, const Object &c) {
      Object ret = this->makeMatrix(3,1);
      Object *rp = this->rw(ret);
      rp[0] = a;
      rp[1] = b;
      rp[2] = c;
      return ret;
    }
    Object last(const Object &a) {
      if (a.isEmpty()) throw Exception("Attempt to take last element of empty object");
      return (this->ro(a))[a.count()-1];
    }
    Object fetch(const Object &a, ndx_t ndx) {
      return this->ro(a)[ndx];
    }
    Object pop(const Object &a) {
      if (a.count() <= 1) return this->empty();
      // TODO - make this faster by using slicing?
      Object r(this->makeMatrix(a.count()-1,1));
      for (auto i=1;i<a.count();i++)
	this->rw(r)[i-1] = this->ro(a)[i];
      return r;
    }
    Object deleteElement(const Object &a, ndx_t element) {
      if (a.count() <= 1) return this->empty();
      Object r(this->makeMatrix(a.count()-1,1));
      ndx_t j = 0;
      for (auto i=0;i<a.count();i++)
	if (i != element) this->rw(r)[j++] = this->ro(a)[i];
      return r;
    }
    void push(Object &a, const Object &b) {
      ndx_t a_size = a.count();
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
	  for (ndx_t i=0;i<a_size;i++)
	    nap[i] = ap[i];
	  nap[a_size] = b;
	  a = na;
	}
    }
    // FIXME - Make this more efficient?
    void merge(Object &a, const Object &b) {
      const Object *bp = this->ro(b);
      for (ndx_t i=0;i<b.count();i++)
	this->push(a,bp[i]);
    }
    Type* anyElementsWithDataCode(const Object &a, DataCode t) {
      const Object *bp = this->ro(a);
      for (ndx_t i=0;i<a.count();i++)
	if (bp[i].type()->code() == t) return bp[i].type();
      return NULL;
    }
    inline ndx_t indexOf(const Object &a, const Object &b) {
      const Object *ap = this->ro(a);
      for (ndx_t i=0;i<a.count();i++)
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
