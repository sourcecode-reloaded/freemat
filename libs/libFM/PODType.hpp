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
    virtual void* allocateArray(dim_t size) const {return calloc(size,sizeof(T));}
    virtual void releaseData(T* dst, dim_t size) const {} // No release required
    virtual void freeData(T* ptr, dim_t size) const {free(ptr);}
    virtual T zeroElement() const {return T();}
  public:
    PODType(ThreadContext* ctxt, const FMString &name) : ArrayType<T>(ctxt,name) {}
    Complex<T> complexScalarValue(const Object &a) {
      assert(a.isScalar());
      return readOnlyDataComplex(a)[0];
    }
    inline const Complex<T>* readOnlyDataComplex(const Object &p) const {
      return static_cast<const Complex<T>*>(static_cast<const Complex<T>*>(p.d->data->ptr) + p.d->offset);
    }
    inline Complex<T>* readWriteDataComplex(Object &p) const {
      p.detach();
      return static_cast<Complex<T>*>(p.d->data->ptr);
    }
    FMString describe(const Object &a) {
      if (a.isEmpty()) return FMString("[]");
      if (a.isScalar())	
	{
	  if (a.isComplex())
	    return Stringify(complexScalarValue(a));
	  else
	    return Stringify(this->scalarValue(a));
	}
      return(a.dims().toString() + " " + this->name() + " array");
    }
    virtual ~PODType() {}
  };
}

#endif
