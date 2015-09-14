#ifndef __PODType__
#define __PODType__

#include "ArrayType.hpp"

// Manages N-dimensional arrays of blocks of opaque data - 
// requires a real type object to provide the type interface
// to the data.  In this case the data type must be a plain
// ole data (POD) type.  I.e., no constructors.  Structs and
// traditional values OK.

namespace FM
{
  struct ThreadContext;

  template <class T>
  class PODType : public ArrayType<T> {
    virtual void* allocateArray(ndx_t size) const {return calloc(size_t(size),sizeof(T));}
    virtual void releaseData(T* dst, ndx_t size) const {memset(dst,0,size_t(sizeof(T)*size));} // No release required
    virtual void freeData(T* ptr, ndx_t size) const {free(ptr);}
    virtual T zeroElement() const {return T();}
  public:
    PODType(ThreadContext* ctxt, const FMString &name) : ArrayType<T>(ctxt,name) {}
    void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {}
    FMString brief(const Object &a) {
      return this->describe(a);
    }
    FMString describe(const Object &a) {
      if (a.isEmpty()) return FMString("[]");
      if (a.isScalar())	
	return Stringify(this->scalarValue(a));
      return(a.dims().toString() + " " + this->name() + " array");
    }
    virtual ~PODType() {}
  };
}

#endif
