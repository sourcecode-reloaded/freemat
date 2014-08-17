#include "PODType.hpp"
#include "BaseTypes.hpp"
#include "IntegerType.hpp"

using namespace FM;

template <class T>
static void copyPage(T* op, dim_t &outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize)
{
  for (dim_t j=0;j<pagesize;j++)
    op[outputOffset+j] = ip[input_offset+j];
  outputOffset += pagesize;
  input_offset += pagesize;
}

template <class T>
static void copyPagePromote(Complex<T>* op, dim_t outputOffset, const T* ip, dim_t &input_offset, dim_t pagesize)
{
  for (dim_t j=0;j<pagesize;j++)
    op[outputOffset+j] = Complex<T>(ip[input_offset+j]);
  outputOffset += pagesize;
  input_offset += pagesize;
}

template <class T>
static void moveLoop(T* ap, const Tuple &outdims, const Tuple &adims)
{
  dim_t iterations = 1;
  for (int i=1;i<MAXDIMS;i++)
    iterations *= adims.dimension(i);
  // Calculate where the last sample from a has to move to
  dim_t new_count = outdims.elementCount();
  dim_t old_index = adims.elementCount()-1;
  dim_t new_index = 0;
  dim_t stride = 1;
  for (int i=0;i<MAXDIMS;i++)
    {
      new_index += stride*(adims.dimension(i)-1);
      stride *= outdims.dimension(i);
    }
  assert(new_index >= old_index);
  // Zero out the tail of ap
  for (dim_t n=new_index+1;n<new_count;n++)
    ap[n] = T();  
  dim_t arows = adims.rows();
  dim_t orows = outdims.rows();
  for (dim_t iter = 0;iter < iterations; iter++)
    {
      for (dim_t row = 0;row < arows; row++)
	ap[new_index-row] = ap[old_index-row];
      for (dim_t row = 0;row < orows-arows;row++)
	ap[new_index+1+row] = T();
      new_index -= orows;
      old_index -= arows;
    }
}

template <class T>
static void copyLoop(T* op, const T* ip, const Tuple & outdims, const Tuple & adims)
{
  // Compute the number of iterations required by the RESIZE
  // Excludes the first dimension.  
  dim_t iterations = 1;
  for (int i=1;i<MAXDIMS;i++)
    iterations *= adims.dimension(i);
  // Loop over the iterations
  dim_t arows = adims.rows();
  dim_t orows = outdims.rows();
  for (dim_t iter=0;iter<iterations;iter++) {
    for (dim_t row=0;row<arows;row++)
      op[row] = ip[row];
    ip += arows;
    op += orows;
  }
}

// This algorithm is responsible for the set -- it is a bit tricky (for performance reasons), so here are the details.
//
// op - points to the beginning of the output array (resized to be large enough already)
// argdim - length of argument arrays, i.e., argdim[i] = numel(coords[i])
// adims - the dimensions of the array being modified
// ndims - the number of dimensions for the indexing array, i.e., numel(coords)
// bp - pointer to the beginning of the RHS array
// isScalar - flag that indicates if RHS is a scalar, and so replication is required.

template <class T>
static void setLoop(T* op, const dim_t (&argdim) [MAXDIMS], const Tuple & adims, int ndims, const ndx_t *(&coords)[MAXDIMS], const T* bp, bool isScalar)
{
  dim_t ndx[MAXDIMS];
  dim_t strides[MAXDIMS];
  ndx[0] = 0;
  strides[0] = 1;
  // Compute the number of iterations required by the set
  // Excludes the first dimension.  Also computes the strides
  // and initializes the index array to all zeros
  dim_t iterations = 1;
  for (int i=1;i<ndims;i++) {
    iterations *= argdim[i];
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
	if (ndx[j] >= argdim[j])
	  {
	    ndx[j] = 0;
	    ndx[j+1]++;
	  }
    }
    for (dim_t row=0;row<argdim[0];row++)
      {
	op[start + coords[0][row]] = *bp;
	if (!isScalar) bp++;
      }
  }
}

template <class T>
static void getLoop(T* op, const T* ip, const dim_t (&outdim)[MAXDIMS], const Tuple & adims, int ndims, const ndx_t * (&coords)[MAXDIMS])
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

template <class T, bool _objType>
Object PODType<T,_objType>::getParens(const Object &a, const Object &b) {
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
    Object output = this->zeroArrayOfSize(Tuple::RawTuple(outdim,bsize),a.isComplex());
  if (a.isComplex())
    getLoop<Complex<T> >(this->readWriteDataComplex(output),
			 this->readOnlyDataComplex(a),
			 outdim,adims,bsize,coords);
  else
    getLoop<T>(this->readWriteData(output),this->readOnlyData(a),outdim,adims,bsize,coords);
  return output;
}

template <class T, bool _objType>
void PODType<T,_objType>::resize(Object &a, const Tuple &newsize) {
  if (a.elementCount() == 0)
    {
      if (a.d->capacity > newsize.elementCount())
	{
	  a.dims() = newsize;
	  return;
	}
      a = this->zeroArrayOfSize(newsize,a.isComplex());
      return;
    }
  // Determine if this is a move or a copy
  if (a.d->capacity > newsize.elementCount()) {
    if (a.isComplex())
      moveLoop<Complex<T> >(this->readWriteDataComplex(a),
			    newsize,a.dims());
    else
      moveLoop<T>(this->readWriteData(a),newsize,a.dims());
  } else {
    // Copy
    Object na = this->zeroArrayOfSize(newsize,a.isComplex());
    if (a.isComplex())
      copyLoop<Complex<T> >(this->readWriteDataComplex(na),
			    this->readOnlyDataComplex(a),
			    newsize,a.dims());
    else
      copyLoop<T>(this->readWriteData(na),this->readOnlyData(a),newsize,a.dims());
    a = na;
  }
  a.dims() = newsize;
}

template <class T, bool _objType>
void PODType<T,_objType>::setParens(Object &a, const Object &args, const Object &b) {
  // TODO: Row indexing mode
  //      if (b.size() == 1) return getRowIndexMode(a,b[0].asIndex(a.dims()));
  Tuple & adims(a.dims());
  // TODO: Trim trailing singular dimensions.  Make sure b.size() < a.numDims.
  // TODO: Special case all-scalar indexing case
  Object c[MAXDIMS];
  int argsize = args.elementCount();
  const Object *argsp = args.asType<ListType>()->readOnlyData(args);
  for (int i=0;i<argsize;i++)
    c[i] = argsp[i].asIndexNoBoundsCheck();
  // TODO: Add slice test here (c.f. isSliceIndexCase)
  // Expand colons, and do bounds check
  if (argsize > MAXDIMS) 
    throw Exception(FMString("FreeMat is compiled for maximum ") 
		    + Stringify(MAXDIMS) + FMString(" dimensions.  Indexing expression ")
		    + FMString(" requires too many dimensions."));
  dim_t argdim[MAXDIMS];
  dim_t outdim[MAXDIMS];
  const ndx_t *coords[MAXDIMS];
  bool resize_required = false;
  dim_t outcount = 1;
  for (int i=0;i<argsize;i++)
    {
      IndexType *ip = static_cast<IndexType*>(c[i].type());
      if (ip->isColon(c[i]))
	c[i] = ip->expandColons(adims.dimension(i));
      argdim[i] = c[i].elementCount();
      coords[i] = ip->readOnlyData(c[i]);
      outdim[i] = ip->maxValue(c[i])+1;
      outcount *= (outdim[i]+1);
      resize_required |= (outdim[i] > adims.dimension(i));
    }
  // Check if the output requires resizing of the array
  if (resize_required)
    {
      // StdIOTermIF io;
      // io.output("Before resize:\n");
      // a.type()->print(a,io);
      a.type()->resize(a,Tuple::RawTuple(outdim,argsize));
      // io.output("After resize:\n");
      // a.type()->print(a,io);
      adims = a.dims();
    }      
  // Check for validity of b
  if (!(b.isScalar() || b.elementCount() == outcount))
    throw Exception("Assignment A(...) = b requires b either be a scalar or have the correct number of elements");
  // TODO - handle b is complex, a is real
  if (a.isComplex())
    setLoop<Complex<T> >(this->readWriteDataComplex(a),
			 argdim,adims,argsize,coords,
			 this->readOnlyDataComplex(b),b.isScalar());
  else
    setLoop<T>(this->readWriteData(a),
	       argdim,adims,argsize,coords,
	       this->readOnlyData(b),b.isScalar());
}

template <class T, bool _objType>
void PODType<T,_objType>::printSheet(const Object &a, TermIF &io, const ArrayFormatInfo &format, ndx_t offset) {
  ndx_t columns = a.cols();
  ndx_t rows = a.rows();
  int colsPerPage = (int) floor((io.getTerminalWidth()-1)/((double) format.width + 2));
  colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
  int pageCount = (int) ceil(columns/((double)colsPerPage));
  for (int k=0;k<pageCount;k++) {
    int colsInThisPage;
    colsInThisPage = columns - colsPerPage*k;
    colsInThisPage = (colsInThisPage > colsPerPage) ? 
      colsPerPage : colsInThisPage;
    if ((rows*columns > 1) && (pageCount > 1))
      io.output("\n Columns %d to %d\n\n",k*colsPerPage+1,k*colsPerPage+colsInThisPage);
    for (int i=0;i<rows;i++) {
      for (int j=0;j<colsInThisPage;j++)
	this->printElement(a,io,format,i+(k*colsPerPage+j)*rows+offset);
      io.output("\n");
    }
  }
  io.output("\n");
}

template <class T, bool _objType>
void PODType<T,_objType>::print(const Object &a, TermIF &io)
{
  if (a.isEmpty())
  {
    if (a.dims() == Tuple(0,0)) {
      io.output("  []\n");
    } else {
      io.output("  Empty array ");
      io.output(a.dims().toString());
      io.output("\n");
    }
  }
  ArrayFormatInfo format;
  this->computeArrayFormatInfo(io.getFormatMode(),a,format);
  if (!a.isScalar() && (format.scalefact != 1))
    io.output("\n   %.1e * \n",format.scalefact);
  if (a.isScalar() && !format.floatasint) {
    if (fabs(log10(format.scalefact)) > 3) {
      format.expformat = true;
    } else {
      format.scalefact = 1;
    }
  }
  if (a.is2D())
    this->printSheet(a,io,format,0);
  else {
    /**
     * For N-ary arrays, data slice  -  start with 
     * [1,1,1,...,1].  We keep doing the matrix
     * print, incrementing from the highest dimension,
     * and rolling downwards.
     */
    io.output("\n");
    dim_t offset = 0;
    dim_t page = a.rows()*a.cols();
    dim_t page_count = a.elementCount()/page;
    for (dim_t p=0;p<page_count;p++)
      {
	Tuple w = offset % a.dims();
	io.output("(:,:");
	for (int m=2;m<w.dimensions();m++) 
	  io.output(FMString(",") + Stringify(w.dimension(m)));
	io.output(") = \n");
	this->printSheet(a,io,format,offset);
	offset += page;
      }
  }
}


// Preconditions - all objects must be the same size
// Zero objects are removed
// Mixed real/complex objects are all promoted to 
// Complex case needs to be considered!
template <class T, bool _objType>
Object PODType<T,_objType>::NCat(const Object& p, int dim)
{
  int objectCount = p.elementCount();
  const Object *dp = _base->_list->readOnlyData(p);
  if (objectCount == 0) return this->empty();
  // Compute the size of the output
  Tuple outputSize = dp[0].dims();
  // Sum the components along the given dimension
  dim_t aggregated_size = 0;
  for (int i=0;i<objectCount;i++)
    aggregated_size += dp[i].dims().dimension(dim);
  outputSize.set(dim,aggregated_size);
  std::cout << "Aggregated size: " << aggregated_size << "  " << outputSize.toString() << "\n";
  // Cache the pointers to the objects
  std::vector<const T*> pointers(objectCount);
  // Cache the current indices into each object
  std::vector<dim_t> offsets(objectCount);
  // Cache the page size for each object
  std::vector<dim_t> pagesize(objectCount);
  // Cache the promotion flag for each object
  std::vector<bool> promotionRequired(objectCount);
  // Determine if promotion to complex is required
  // Required if any of the objects is complex, but
  // not all of them.
  bool anyComplex = false;
  bool allComplex = true;
  for (int i=0;i<objectCount;i++)
    {
      anyComplex |= dp[i].isComplex();
      allComplex &= dp[i].isComplex();
    }
  for (int i=0;i<objectCount;i++)
    {
      pointers[i] = this->readOnlyData(dp[i]);
      offsets[i] = 0;
      dim_t pagesze = 1;
      for (int j=0;j<=dim;j++)
	pagesze *= dp[i].dims().dimension(j);
      pagesize[i] = pagesze;
      promotionRequired[i] = anyComplex && (!allComplex) && (!dp[i].isComplex());
    }
  // Allocate the output object
  Object retObject = this->zeroArrayOfSize(outputSize,anyComplex);
  dim_t outputOffset = 0;
  dim_t outputCount = outputSize.elementCount();
  int k = 0;
  T* op = this->readWriteData(retObject);
  while (outputOffset < outputCount)
    {
      if (!anyComplex)
	copyPage<T>(op,outputOffset,pointers[k],offsets[k],pagesize[k]);
      else if (anyComplex && !promotionRequired[k])
	copyPage<Complex<T> >(reinterpret_cast<Complex<T>*>(op),outputOffset,
			      reinterpret_cast<const Complex<T>*>(pointers[k]),offsets[k],pagesize[k]);
      else
	copyPagePromote<T>(reinterpret_cast<Complex<T>*>(op),outputOffset,
			   pointers[k],offsets[k],pagesize[k]);
      k = (k + 1) % objectCount;
    }
  return retObject;
}


template class PODType<char,false>;
template class PODType<float,false>;
template class PODType<double,false>;
template class PODType<Object,true>;
template class PODType<int32_t,false>;
template class PODType<uint32_t,false>;
template class PODType<int64_t,false>;
template class PODType<uint64_t,false>;
template class PODType<bool,false>;
//template class PODType<ndx_t,false>;

