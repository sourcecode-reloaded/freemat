#ifndef __DenseTypeProxy_hpp__
#define __DenseTypeProxy_hpp__

#include "Type.hpp"
#include "Complex.hpp"
#include "Object.hpp"

namespace FM {
  typedef Object (Type::*TypeMemFn)(const Object &x, const Object &y);

  struct ThreadContext;
  
  // The regular type system doesn't quite line up with the sparse one.
  // In particular, the sparse types have one class for real and a separate
  // class for complex, while the regular type system overloads the two.
  // In order to provide a rational interface, we create proxy classes
  // to manage the mismatch.
  template <class T>
  class DenseTypeProxy {
    ThreadContext* _ctxt;
  public:
    DenseTypeProxy(ThreadContext *ctxt) : _ctxt(ctxt) {}
    T* rw(Object &a);
    const T* ro(const Object &a);
    Object make(ndx_t rows, ndx_t cols);
    Object applyFunc(TypeMemFn func, const Object &x, const Object &y);
    void setColumn(Object &ret, ndx_t col, const Object &val);
    Object convert(const Object &a);
  };
}

#endif
