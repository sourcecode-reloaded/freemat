#ifndef __Tuple__
#define __Tuple__

#include <stdint.h>
#include "Exception.hpp"

namespace FM {

  const int MAXDIMS = 8;

  class Tuple {
    //! Stubbornly refusing to allow arbitrary dimensions of
    //data.  But it can be changed easily.
    dim_t dims[MAXDIMS];
    //! The number of dimensions actually used.  Performance
    //tests indicate its better to know than to assume its
    //just MAXDIMS
    unsigned ndims;
    //! The element count described by the tuple (the product of the dimensions)
    dim_t elements;
  public:
      Tuple() : ndims(2), elements(0) {dims[0] = dims[1] = 0;}
    Tuple(dim_t rows, dim_t cols) {
      ndims = 2;
      dims[0] = rows;
      dims[1] = cols;
      elements = rows*cols;
    }
   // Tuple(dim_t *ptr, int nd) : ndims(nd) {memcpy(dims,ptr,nd*sizeof(dim_t)); updateElementCount();}
    inline void prepend(dim_t a) {
      for (int i=0;i<ndims;i++)
	dims[ndims-i] = dims[ndims-i-1];
        ndims++;
      dims[0] = a;
      updateElementCount();
    }
    inline dim_t rows() const {
      if (ndims == 0) return 0;
      return dims[0];
    }
    inline dim_t cols() const {
      if (ndims < 2) return 0;
      return dims[1];
    }
    inline int dimensions() const {
      return ndims;
    }
    inline dim_t dimension(int p) const {
      if (p < ndims) return dims[p];
      return 1;
    }
    inline bool is2D() const {
      return (ndims <= 2);
    }
    inline dim_t elementCount() const {
      return elements;
    }
    inline void setRows(dim_t rows) {
      if (ndims == 0) ndims = 1;
      dims[0] = rows;
      updateElementCount();
    }
    inline void setCols(dim_t cols) {
      if (ndims < 2) ndims = 2;
      dims[1] = cols;
      updateElementCount();
    }
    inline void setMatrixSize(dim_t rows, dim_t cols) {
      dims[0] = rows;
      dims[1] = cols;
      ndims = 2;
      elements = rows*cols;
    }
    // set(2,4) --> ndims -> 3
    inline void set(int dim, dim_t val) {
      for (int i=ndims;i<(dim+1);i++)
	dims[i] = 1;
      ndims = (ndims > (dim+1)) ? ndims : (dim+1);
      dims[dim] = val;
      updateElementCount();
    }
    inline void setEmpty() {
      dims[0] = 0;
      dims[1] = 0;
      ndims = 2;
      elements = 0;
    }
    inline void setScalar() {
      dims[0] = 1;
      dims[1] = 1;
      ndims = 2;
      elements = 1;
    }
    inline void updateElementCount() {
      if (ndims == 0) 
	elements = 0;
      else
	{
	  elements = 1;
	  for (unsigned i=0;i<ndims;i++)
	    elements *= dims[i];
	}
    }
    inline FMString toString() const {
      FMString ret;
      ret = Stringify(dims[0]);
      for (int i=1;i<ndims;i++)
	ret += " x " + Stringify(dims[i]);
      return ret;
    }
    inline bool operator==(const Tuple &other) const {
      if (this->elementCount() != other.elementCount()) return false;
      if (this->ndims == other.ndims)
	{
	  for (unsigned i=0;i<ndims;i++)
	    if (this->dims[i] != other.dims[i]) return false;
	  return true;
	}
      return Tuple::canonicalForm(*this) == Tuple::canonicalForm(other);
    }
    static Tuple canonicalForm(const Tuple &other) {
      Tuple ret(other);
      while (ret.ndims > 2 && (ret.dims[ret.ndims-1] <= 1)) ret.ndims--;
      return ret;
    }
      static Tuple RawTuple(dim_t *ptr, int nd)
      {
          Tuple p;
          p.ndims = nd;
          memcpy(p.dims,ptr,nd*sizeof(dim_t));
          p.updateElementCount();
          return p;
      }
    // Potential performance issue!
    static Tuple computeDotOpSize(const Tuple &a, const Tuple &b) {
      if ((a.elementCount() == 1) && (b.elementCount() == 1)) return a;
      if (a.elementCount() == 1) return b;
      if (b.elementCount() == 1) return a;
      if (a == b) return a;
      throw Exception("Incompatible arguments to operation");
    }
  };

  inline Tuple operator%(dim_t vectorIndex, const Tuple & dims) {
    Tuple ret = dims;
    for (int i=0;i<ret.dimensions();i++)
      {
	ret.set(i,vectorIndex % dims.dimension(i));
	vectorIndex /= dims.dimension(i);
      }
    return ret;
  }

}

#endif
