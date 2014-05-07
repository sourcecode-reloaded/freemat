#include "PODType.hpp"
#include "BaseTypes.hpp"
#include "IntegerType.hpp"

using namespace FM;


template <class T>
static void copyLoop(T* op, const T* ip, const dim_t (&outdim)[MAXDIMS], const Tuple & adims, int ndims, const ndx_t * (&coords)[MAXDIMS])
{
  dim_t ndx[MAXDIMS];
  dim_t strides[MAXDIMS];
  ndx[0] = 0;
  strides[0] = 1;
  // Compute the number of iterations required by the GET
  // Excludes the first dimension.  Also computes the strides
  // and initializes the index array to all zeros
  dim_t iterations = 1;
  for (int i=1;i<ndims;i++) {
    iterations *= outdim[i];
    ndx[i] = 0;
    strides[i] = strides[i-1]*adims.dimension(i-1);
  }
  // Loop over the iterations
  for (dim_t iter=0;iter<iterations;iter++) {
    // Compute the start address
    size_t start = 0;
    for (int i=1;i<ndims;i++) {
      start += coords[i][ndx[i]]*strides[i];
      ndx[i]++;
      for (int j=i;j<ndims;j++)
	if (ndx[j] >= outdim[j])
	  {
	    ndx[j] = 0;
	    ndx[j+1]++;
	  }
    }
    for (dim_t row=0;row<outdim[0];row++)
      op[row] = ip[start + coords[0][row]];
    op += outdim[0];
  }
}

template <class T>
Object PODType<T>::getParens(const Object &a, const Object &b) {
  // TODO: Row indexing mode
  //      if (b.size() == 1) return getRowIndexMode(a,b[0].asIndex(a.dims()));
  const Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object c[MAXDIMS];
  int bsize = b.elementCount();
  const Object *bp = b.asType<ListType>()->readOnlyData(b);
  for (int i=0;i<bsize;i++)
    c[i] = bp[i].asIndex(adims.dimension(i));
  // TODO: Add slice test here (c.f. isSliceIndexCase)
  // Expand colons, and do bounds check
  if (bsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  dim_t outdim[MAXDIMS];
  const ndx_t *coords[MAXDIMS];
  for (int i=0;i<bsize;i++)
    {
      IndexType *ip = static_cast<IndexType*>(c[i].type());
      if (ip->isColon(c[i]))
	c[i] = ip->expandColons(adims.dimension(i));
      outdim[i] = c[i].elementCount();
      coords[i] = ip->readOnlyData(c[i]);
    }
  Object output = this->zeroArrayOfSize(Tuple(outdim,bsize),a.isComplex());
  if (a.isComplex())
    copyLoop<Complex<T> >(reinterpret_cast<Complex<T>*>(this->readWriteData(output)),
			  reinterpret_cast<const Complex<T>*>(this->readOnlyData(a)),
			  outdim,adims,bsize,coords);
  else
    copyLoop<T>(static_cast<T*>(this->readWriteData(output)),
		static_cast<const T*>(this->readOnlyData(a)),
		outdim,adims,bsize,coords);
  return output;
}

template class PODType<char>;
template class PODType<float>;
template class PODType<double>;
template class PODType<Object>;
template class PODType<int32_t>;
template class PODType<uint32_t>;
template class PODType<bool>;
template class PODType<ndx_t>;

